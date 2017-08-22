#include <core/mw/StaticList.hpp>
#include <functional>
#include <core/os/Mutex.hpp>
#include <core/mw/Middleware.hpp>
#include <core/mw/Publisher.hpp>
#include <core/mw/Subscriber.hpp>

#include <core/mw/RPCMessages.hpp>

namespace core {
namespace mw {
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

    Request  request;
    Response response;

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

class RPCBase;

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

class ServerBase
{
    friend class RPCBase;
    friend class RPC;

public:
    ServerBase(
        const char* rpc_name
    ) : _rpc(nullptr), _id(0), _timeout(core::os::Time::INFINITE), _by_rpc(*this)
    {
        _rpc_name = rpc_name;
    }

    virtual ~ServerBase() {}

    virtual bool
    invoke(
        const void* request,
        void*       response
    ) = 0;


protected:
    RPCBase*       _rpc;
    RPCName        _rpc_name;
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
    ClientBase(
        const char* rpc_name
    ) : _rpc(nullptr), _id(0), _server_id(0), _by_rpc(*this)
    {
        _rpc_name = rpc_name;
        _server_name.clear();
    }

    operator bool() {
        return _server_id != 0;
    }

protected:
    RPCBase*    _rpc;
    RPCName     _rpc_name;
    uint8_t     _id;
    ModuleName  _server_name;
    uint8_t     _server_id;
    Transaction transaction;
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

    bool
    call(
        ClientBase& client,
        BaseServ&   serv
    )
    {
        bool success = false;

        if (!client) {
            return false;
        }

        if (beginClientTransaction(client)) {
            RPCMessage* request = client.transaction._outbound_message;

            request->header.type = MessageType::REQUEST;
            request->header.target_module_name = client._server_name;
            request->header.server_session     = client._server_id;

            CORE_ASSERT(serv.getRequestSize() < RPCMessage::PAYLOAD_SIZE);

            memcpy(request->payload, serv.getRequest(), serv.getRequestSize());

            if (executeClientTransaction(client)) {
                RPCMessage* response = client.transaction._inbound_message;

                CORE_ASSERT(serv.getResponseSize() < RPCMessage::PAYLOAD_SIZE);

                memcpy(serv.getResponse(), response->payload, serv.getResponseSize());

                success = true;
            }

            endClientTransaction(client);
        }

        return success;
    } // call

    bool
    discover(
        ClientBase& client
    )
    {
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
        //client.transaction._lock.acquire();
        _lock.acquire();
        client.transaction._outbound_message = nullptr;
        client.transaction._inbound_message  = nullptr;

        _sequence++;
        client.transaction._sequence = _sequence;
        _lock.release();

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
    endClientTransaction(
        ClientBase& client
    )
    {
        if (client.transaction._inbound_message != nullptr) {
            _sub.release(*client.transaction._inbound_message);
        }

        client.transaction._outbound_message = nullptr;
        client.transaction._inbound_message  = nullptr;

        //client.transaction._lock.release();

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

        return msg == 0x2BAADCAFE;
    }

    void
    wake(
        ClientBase& client
    )
    {
        core::os::SysLock::Scope lock;

        if (client.transaction._runner != nullptr) {
            core::os::Thread::wake(*(client.transaction._runner), 0x2BAADCAFE);
            client.transaction._runner = nullptr;
        }
    }
};

class RPC;

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

        memcpy(ss.getRequest(), request, ss.getRequestSize());

        if (_callback) {
            _callback(ss);

            memcpy(response, ss.getResponse(), ss.getResponseSize());

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
    using Service = SERVICE;

public:
    //Client() : ClientBase::ClientBase() {};
    Client(
        const char* rpc_name
    ) : ClientBase::ClientBase(rpc_name) {}

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

private:
};


class RPC:
    public RPCBase
{
public:
    RPC(
        const char* name
    ) : RPCBase::RPCBase(name), _running(false), _next_client_id(0), _next_server_id(0) {}

    bool
    addClient(
        ClientBase& client
    )
    {
        _lock.acquire();

        client._rpc = this;
        _clients.link(client._by_rpc);
        client._id = getNextClientId();
        client._server_name = "";

        _lock.release();

        return true;
    }

    bool
    removeClient(
        ClientBase& client
    )
    {
        _lock.acquire();

        _clients.unlink(client._by_rpc);

        client._rpc = nullptr;
        client._server_name = "";
        client._server_id   = 0;
        client._id = 0;

        _lock.release();

        return true;
    }

    bool
    addServer(
        ServerBase& server
    )
    {
        _lock.acquire();

        // There can be only one server x rpc name
        for (const ServerBase& srvr : _servers) {
            if (srvr._rpc_name == server._rpc_name) {
                server._rpc = nullptr;
                server._id  = 0;

                _lock.release();

                return false;
            }
        }

        server._rpc = this;
        _servers.link(server._by_rpc);
        server._id = getNextServerId();

        _lock.release();

        return true;
    } // addServer

    bool
    removeServer(
        ServerBase& server
    )
    {
        _lock.acquire();

        _servers.unlink(server._by_rpc);

        server._rpc = nullptr;
        server._id  = 0;

        _lock.release();

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

                            wake(client);

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
}
}
