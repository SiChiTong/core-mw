/* COPYRIGHT (c) 2016-2018 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#pragma once

#include <core/mw/namespace.hpp>
#include <core/common.hpp>
#include <core/mw/StaticList.hpp>
#include <core/os/Mutex.hpp>
#include <core/os/ScopedLock.hpp>
#include <core/mw/Middleware.hpp>
#include <core/mw/Publisher.hpp>
#include <core/mw/Subscriber.hpp>
#include <core/ConstString.hpp>

#include <core/mw/RPCMessages.hpp>

#include <functional>

NAMESPACE_CORE_MW_BEGIN
namespace rpc {
struct BaseServ {
    virtual std::size_t
    getRequestSize() const = 0;

    virtual std::size_t
    getResponseSize() const = 0;

    virtual void*
    getRequest() = 0;

    virtual void*
    getResponse() = 0;
};

template <class REQUEST, class RESPONSE>
struct Service_:
    public BaseServ {
public:
    using Request  = REQUEST;
    using Response = RESPONSE;

    static_assert(sizeof(Request) <= RPCMessage::PAYLOAD_SIZE, "sizeof(Request) > RPCMessage::PAYLOAD_SIZE");
    static_assert(sizeof(Response) <= RPCMessage::PAYLOAD_SIZE, "sizeof(Response) > RPCMessage::PAYLOAD_SIZE");

    Request CORE_MEMORY_ALIGNED  request;
    Response CORE_MEMORY_ALIGNED response;

    std::size_t
    getRequestSize() const
    {
        return sizeof(Request);
    }

    std::size_t
    getResponseSize() const
    {
        return sizeof(Response);
    }

    void*
    getRequest()
    {
        return &request;
    }

    void*
    getResponse()
    {
        return &response;
    }
};

class Transaction
{
public:
    Transaction() :  _sequence(0), _timeout(core::os::Time::s(1)), _runner(nullptr), _inbound_message(nullptr), _outbound_message(nullptr), _id(0)
    {
        _lock.initialize();
    }

    core::os::Mutex   _lock;
    uint8_t           _sequence;
    core::os::Time    _timeout;
    core::os::Thread* _runner;
    RPCMessage*       _inbound_message;
    RPCMessage*       _outbound_message;
    uint8_t           _id;
};

class RPCBase;

class ServerBase
{
    friend class RPCBase;
    friend class RPC;

public:
    ServerBase() : _rpc(nullptr), _rpc_name(""), _id(0), _timeout(core::os::Time::INFINITE), _by_rpc(*this)
    {
    }

    ServerBase(
        const char* rpc_name
    ) : _rpc(nullptr), _rpc_name(rpc_name), _id(0), _timeout(core::os::Time::INFINITE), _by_rpc(*this)
    {
    }

    virtual ~ServerBase() {}

    virtual bool
    invoke(
        const void* request,
        void*       response
    ) = 0;


protected:
    RPCBase*       _rpc;
    core::ConstString<RPCName::SIZE> _rpc_name;
    uint8_t        _id;
    core::os::Time _timeout;
//    Transaction    transaction;
    mutable StaticList<ServerBase>::Link _by_rpc;
};

class ClientBase
{
    friend class RPCBase;
    friend class RPC;

public:
    enum State {
        NONE, READY, BUSY
    };

public:
    ClientBase(
        const char* rpc_name
    ) : _rpc(nullptr), _rpc_name(rpc_name), _id(0), _server_id(0), _state(State::NONE), _service(nullptr), _timeout(core::os::Time::s(1)), _private(nullptr), _by_rpc(*this)
    {
        _server_name.clear();
    }

    virtual bool
    invoke() = 0;

    virtual
	bool open() = 0;

    operator bool() {
        return _server_id != 0;
    }

    void
    setPrivateData(
        void* private_data
    )
    {
        _private = private_data;
    }

    void*
    getPrivateData()
    {
        return _private;
    }

    virtual bool
	has_callback() = 0;

protected:
    RPCBase*       _rpc;
    core::ConstString<RPCName::SIZE> _rpc_name;
    uint8_t        _id;
    ModuleName     _server_name;
    uint8_t        _server_id;
    Transaction    transaction;
    State          _state;
    BaseServ*      _service;
    core::os::Time _timeout;
    void*          _private;
    mutable StaticList<ClientBase>::Link _by_rpc;
};

class RPCBase
{
public:
    RPCBase(
        const char* name
    ) : _name(name), _runner(nullptr), _sequence(0), _sub_node("RPCSub", false), _pub_node("RPCPub")
    {
        _lock.initialize();
    }

    RPCBase() : _name(nullptr), _runner(nullptr), _sequence(0), _sub_node("RPCSub", false), _pub_node("RPCPub")
    {}

    void initialize(const char* name) {
        _name = name;
        _lock.initialize();
    }

    bool
    call(
        ClientBase& client,
        BaseServ&   serv,
		void*       private_data = nullptr
    )
    {
        bool success = false;

        {
            core::os::SysLock::Scope lock;

            if (!client) {
                return false;
            }

            if (client._state != ClientBase::State::READY) {
                // We are either not connected or a RPC is already ongoing...
                return false;
            }

            client._state = ClientBase::State::BUSY;
        }

        if (beginClientTransaction(client)) {
        	client.setPrivateData(private_data);
            client._service = &serv;

            RPCMessage* request = client.transaction._outbound_message;

            request->header.type = MessageType::REQUEST;
            request->header.target_module_name = client._server_name;
            request->header.server_session     = client._server_id;

            CORE_ASSERT(serv.getRequestSize() < RPCMessage::PAYLOAD_SIZE);

            std::memcpy(request->payload, serv.getRequest(), serv.getRequestSize());

            if (client.transaction._timeout == core::os::Time::IMMEDIATE) {
                if (executeClientTransaction_async(client)) {
                    success = true;
                }
            } else {
                if (executeClientTransaction(client)) {
                    RPCMessage* response = client.transaction._inbound_message;

                    CORE_ASSERT(serv.getResponseSize() < RPCMessage::PAYLOAD_SIZE);

                    std::memcpy(serv.getResponse(), response->payload, serv.getResponseSize());

                    success = true;
                }

                endClientTransaction(client);

                client._service = nullptr;
                client._state   = ClientBase::State::READY;
            }
        }

        return success;
    } // call

    bool
    discover(
        ClientBase& client
    )
    {
        {
            core::os::SysLock::Scope lock;

            if (client._state != ClientBase::State::NONE) {
                // We are already connected!
                return true;
            }
        }

        client._server_name = "";
        client._server_id   = 0;
        bool success = false;

        if (beginClientTransaction(client)) {
            RPCMessage* request = client.transaction._outbound_message;

            request->header.type = MessageType::DISCOVER_REQUEST;
            request->header.target_module_name.clear();
            request->header.server_session = 0;
            request->discovery_request.client_name = _name;
            request->discovery_request.rpc_name    = client._rpc_name;

            if (executeClientTransaction(client)) {
                RPCMessage* response = client.transaction._inbound_message;

                client._server_name = response->discovery_response.server_name;
                client._server_id   = response->header.server_session;
                success = true;
            }

            endClientTransaction(client);
        }

        if (success) {
            client.transaction._timeout = client._timeout;
            client._state = ClientBase::State::READY;
        } else {
            client._state = ClientBase::State::NONE;
        }

        return success;
    } // discover

protected:
    const char*       _name;
    core::os::Mutex   _lock;
    core::os::Thread* _runner;
    uint8_t           _sequence;
    core::mw::Node    _sub_node;
    core::mw::Node    _pub_node;
    core::mw::Publisher<RPCMessage>     _pub;
    core::mw::Subscriber<RPCMessage, 5> _sub;

protected:
    bool
    beginClientTransaction(
        ClientBase& client
    )
    {
    	core::os::ScopedLock<core::os::Mutex> lock(_lock);

    	client.transaction._outbound_message = nullptr;
        client.transaction._inbound_message  = nullptr;

        _sequence++;
        client.transaction._sequence = _sequence;

        if (_pub.alloc(client.transaction._outbound_message)) {
            return true;
        }

        return false;
    }

    bool
    executeClientTransaction(
        ClientBase& client
    )
    {
        client.transaction._outbound_message->header.sequence       = client.transaction._sequence;
        client.transaction._outbound_message->header.client_session = client._id;

        if (!_pub.publish_loopback(client.transaction._outbound_message)) {
            return false;
        }

        return wait(client);
    }

    bool
    executeClientTransaction_async(
        ClientBase& client
    )
    {
        client.transaction._outbound_message->header.sequence       = client.transaction._sequence;
        client.transaction._outbound_message->header.client_session = client._id;

        if (!_pub.publish_loopback(client.transaction._outbound_message)) {
            return false;
        }

        return true;
    }

    bool
    endClientTransaction(
        ClientBase& client
    )
    {
        if (client.transaction._inbound_message != nullptr) {
            _sub.release(*client.transaction._inbound_message);
        }

        client.transaction._outbound_message = nullptr;
        client.transaction._inbound_message  = nullptr;

        return true;
    }

    bool
    wait(
        ClientBase& client
    )
    {
        core::os::SysLock::Scope lock;

        client.transaction._runner = &core::os::Thread::self();
        core::os::Thread::Return msg = core::os::Thread::sleep_timeout(client.transaction._timeout);

        return msg == 0x1BADCAFE;
    }

    void
    wake(
        ClientBase& client
    )
    {
        core::os::SysLock::Scope lock;

        if (client.transaction._runner != nullptr) {
            core::os::Thread::wake(*(client.transaction._runner), 0x1BADCAFE);
            client.transaction._runner = nullptr;
        }
    }
};

template <class SERVICE>
class Server:
    public ServerBase
{
public:
    using Service      = SERVICE;
    using CallbackType = std::function<void(Service&)>;

    static_assert(std::is_base_of<BaseServ, Service>::value, "Service does not inherit from BaseServ");

public:
    Server(
    ) : ServerBase::ServerBase(), _callback() {}

    Server(
        const char* rpc_name
    ) : ServerBase::ServerBase(rpc_name), _callback() {}

    static bool
    invoke(
        ServerBase& server,
        void*       service
    )
    {
        Server<Service>& ii = static_cast<Server<Service>&>(server);
        Service&         ss = *(reinterpret_cast<Service*>(service));

        if (ii._callback) {
            ii._callback(ss);
            return true;
        }

        return false;
    }

    bool
    invoke(
        const void* request,
        void*       response
    )
    {
        Service ss;

        std::memcpy(ss.getRequest(), request, ss.getRequestSize());

        if (_callback) {
            _callback(ss);

            std::memcpy(response, ss.getResponse(), ss.getResponseSize());

            return true;
        }

        return false;
    }

    void
    callback(
        CallbackType callback
    )
    {
        _callback = callback;
    }

    operator bool() {
        return _id != 0;
    }

private:
    CallbackType _callback;
};


template <class SERVICE>
class Client:
    public ClientBase
{
    friend class RPC;

public:
    using Service      = SERVICE;
    using CallbackType = std::function<void(Service&)>;

public:
    //Client() : ClientBase::ClientBase() {};
    Client(
        const char* rpc_name
    ) : ClientBase::ClientBase(rpc_name), _callback() {}

    Client(
        const char*    rpc_name,
        core::os::Time timeout
    ) : ClientBase::ClientBase(rpc_name), _callback()
    {
        _timeout = timeout;
    }

    bool
    call(
        BaseServ& serv
    )
    {
        CORE_ASSERT(_rpc != nullptr);

        return _rpc->call(*this, serv);
    }

    bool
    open()
    {
        CORE_ASSERT(_rpc != nullptr);

        return _rpc->discover(*this);
    }

    bool
    close()
    {
        _server_id = 0;
        _server_name.clear();

        return true;
    }

    bool
    invoke()
    {
        if (_callback) {
            const void* tmp = &(transaction._inbound_message->payload[0]);
            std::memcpy(_service->getResponse(), tmp, _service->getResponseSize());

            _callback(*reinterpret_cast<Service*>(_service));

            return true;
        }

        return false;
    }

    void
    callback(
        CallbackType callback
    )
    {
        _callback = callback;
    }

    bool
	has_callback()
    {
    	return (bool)_callback;
    }
private:
    CallbackType _callback;
};


class RPC:
    public RPCBase
{
public:
    RPC(
        const char* name
    ) : RPCBase::RPCBase(name), _runner(nullptr), _running(false), _next_client_id(0), _next_server_id(0) {}

    RPC() : RPCBase::RPCBase(), _runner(nullptr), _running(false), _next_client_id(0), _next_server_id(0) {}

    bool
    start(
        std::size_t stack_size
    )
    {
    	core::os::ScopedLock<core::os::Mutex> lock(_lock);

    	if(_runner != nullptr) {
            return false;
        }

        _runner = core::os::Thread::create_heap(nullptr, stack_size, core::os::Thread::PriorityEnum::NORMAL - 2, [](void* arg) {
                reinterpret_cast<core::mw::rpc::RPC*>(arg)->thread();
            }, this, "rpcthd");

        if(_runner == nullptr) {
            return false;
        }

        while (!_running) {
            core::os::Thread::sleep(core::os::Time::ms(500));
        }

        return true;
    }

    bool
    addClient(
        ClientBase& client
    )
    {
    	core::os::ScopedLock<core::os::Mutex> lock(_lock);

        if(client._rpc != nullptr) {
        	return false;
        }

        client._rpc = this;
        _clients.link(client._by_rpc);
        client._id = getNextClientId();
        client._server_name = "";

        return true;
    }

    bool
    removeClient(
        ClientBase& client
    )
    {
    	core::os::ScopedLock<core::os::Mutex> lock(_lock);

        if(client._rpc == nullptr) {
        	return false;
        }

        _clients.unlink(client._by_rpc);

        client._rpc = nullptr;
        client._server_name = "";
        client._server_id   = 0;
        client._id = 0;

        return true;
    }

    bool
    addServer(
        ServerBase& server
    )
    {
    	core::os::ScopedLock<core::os::Mutex> lock(_lock);

        // There can be only one server x rpc name
        for (const ServerBase& srvr : _servers) {
            if (srvr._rpc_name == server._rpc_name) {
                server._rpc = nullptr;
                server._id  = 0;

                return false;
            }
        }

        server._rpc = this;
        _servers.link(server._by_rpc);
        server._id = getNextServerId();

        return true;
    } // addServer

    bool
    addServer(
        const char* rpc,
        ServerBase& server
    )
    {
        server._rpc_name = rpc;
        return addServer(server);
    } // addServer

    bool
    removeServer(
        ServerBase& server
    )
    {
    	core::os::ScopedLock<core::os::Mutex> lock(_lock);

        _servers.unlink(server._by_rpc);

        server._rpc = nullptr;
        server._id  = 0;

        return true;
    }

    bool
    processDiscoverRequest(
        RPCMessage* message
    )
    {
        RPCMessage* request = message;

        for (ServerBase& server : _servers) {
            if (request->discovery_request.rpc_name == server._rpc_name) {
                // We have a server
                RPCMessage* response_message;

                if (_pub.alloc(response_message)) {
                    RPCMessage* response = response_message;
                    response->header.type     = core::mw::rpc::MessageType::DISCOVER_RESPONSE;
                    response->header.sequence = request->header.sequence;
                    response->header.target_module_name      = request->discovery_request.client_name;
                    response->header.client_session          = request->header.client_session;
                    response->header.server_session          = server._id;
                    response->discovery_response.rpc_name    = server._rpc_name;  // TODO: remove - it will not be used
                    response->discovery_response.server_name = _name;

                    if (!_pub.publish_loopback(response_message)) {}
                }
            }
        }

        _sub.release(*message);

        return true;
    } // processDiscoverRequest

    bool
    processDiscoverResponse(
        RPCMessage* message
    )
    {
        for (ClientBase& client : _clients) {
            if (message->header.client_session == client._id) {
                // We have a client
                if (message->header.sequence == client.transaction._sequence) {
                    // We are doing the things in the right sequence
                    if (message->header.target_module_name == _name) {
                        // And, most important, the message is for us
                        client.transaction._inbound_message = message;
                        wake(client);
                        return true;
                    }
                }
            }
        }

        // The message was not for us... release the buffer
        _sub.release(*message);

        return false;
    } // processDiscoverResponse

    bool
    processRequest(
        RPCMessage* message
    )
    {
        RPCMessage* request = message;

        if (request->header.target_module_name == _name) {
            for (ServerBase& server : _servers) {
                if (request->header.server_session == server._id) {
                    RPCMessage* response_message;

                    if (_pub.alloc(response_message)) {
                        RPCMessage* response = response_message;
                        response->header.type               = core::mw::rpc::MessageType::RESPONSE;
                        response->header.sequence           = request->header.sequence;
                        response->header.client_session     = request->header.client_session;
                        response->header.server_session     = request->header.server_session;
                        response->header.target_module_name = _name;

                        server.invoke(request->payload, response->payload);

                        if (!_pub.publish_loopback(response_message)) {}
                    }
                }
            }
        }

        _sub.release(*message);

        return true;
    } // processRequest

    bool
    processResponse(
        RPCMessage* message
    )
    {
        for (ClientBase& client : _clients) {
            if (message->header.client_session == client._id) {
                // We have a client
                if (message->header.sequence == client.transaction._sequence) {
                    // We are doing the things in the right sequence
                    if (message->header.server_session == client._server_id) {
                        if (message->header.target_module_name == client._server_name) {
                            client.transaction._inbound_message = message;

                            if (client.transaction._timeout == core::os::Time::IMMEDIATE) {
                                client.invoke();

                                endClientTransaction(client);

                                client._service = nullptr;
                                client._state   = ClientBase::State::READY;
                            } else {
                                wake(client);
                            }

                            return true;
                        }
                    }
                }
            }
        }

        // The message was not for us... release the buffer
        _sub.release(*message);

        return false;
    } // processDiscoverResponse

    void
    thread()
    {
        _sub_node.subscribe(_sub, RPC_TOPIC_NAME);
        _pub_node.advertise(_pub, RPC_TOPIC_NAME);

        _running = true;
        _sub_node.set_enabled(true);

        while (true) {
            RPCMessage* message = nullptr;

            while (_sub.fetch(message)) {
                switch (message->header.type) {
                  case MessageType::DISCOVER_REQUEST:
                      processDiscoverRequest(message);
                      break;
                  case MessageType::DISCOVER_RESPONSE:
                      processDiscoverResponse(message);
                      break;
                  case MessageType::REQUEST:
                      processRequest(message);
                      break;
                  case MessageType::RESPONSE:
                      processResponse(message);
                      break;
                  default:
                      break;
                }
                core::os::Thread::sleep(core::os::Time::ms(10));
            }

            core::os::Thread::sleep(core::os::Time::ms(100));
        }
    } // thread

    operator bool() {
        return _running;
    }

private:
    core::os::Thread* _runner;
    bool _running;

    uint8_t _next_client_id;
    uint8_t _next_server_id;
    StaticList<ClientBase> _clients;
    StaticList<ServerBase> _servers;

private:
    uint8_t
    getNextClientId()
    {
        // The following must be simple enough
        bool found;

        do {
            found = false;

            while ((++_next_client_id) == 0) {
                /* get next id, avoiding 0 */
            }

            for (const ClientBase& client : _clients) {
                if (client._id == _next_client_id) {
                    found = true;
                    break;
                }
            }
        } while (found);

        return _next_client_id;
    } // getNextClientId

    uint8_t
    getNextServerId()
    {
        // The following must be simple enough
        bool found;

        do {
            found = false;

            while ((++_next_server_id) == 0) {
                /* get next id, avoiding 0 */
            }

            for (const ServerBase& server : _servers) {
                if (server._id == _next_server_id) {
                    found = true;
                    break;
                }
            }
        } while (found);

        return _next_server_id;
    } // getNextServerId
};
}
NAMESPACE_CORE_MW_END
