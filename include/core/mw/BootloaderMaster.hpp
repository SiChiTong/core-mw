#include <core/mw/Middleware.hpp>
#include <core/os/Thread.hpp>
#include <core/mw/BootMsg.hpp>

#ifndef MAX_NUMBER_OF_SLAVES
#define MAX_NUMBER_OF_SLAVES 32
#endif

NAMESPACE_CORE_MW_BEGIN

namespace bootloader {
template <typename KEY, typename VALUE, std::size_t N>
class StaticMap
{
private:
    struct Entry {
        KEY   key;
        VALUE value;
    };

    core::Array<Entry, N> _data;
    std::size_t           _cnt;

public:
    StaticMap() : _cnt(0) {}

    VALUE&
    operator[](
        const KEY& key
    )
    {
        std::size_t i;

        for (i = 0; i < _cnt; i++) {
            if (_data[i].key == key) {
                return _data[i].value;
            }
        }

        if (i < N) {
            _cnt++;
            _data[i].key   = key;
            _data[i].value = VALUE();
        } else {
            CORE_ASSERT(!"Too many entries");
        }

        return _data[i].value;
    } // []

/*
    const VALUE&
    operator[](
        const KEY& key
    ) const
    {
        std::size_t i;

        for (i = 0; i < _cnt; i++) {
            if (_data[i].key == key) {
                return _data[i].value;
            }
        }

        CORE_ASSERT(!"Element not found");
    }
*/
    const KEY&
    key(
        std::size_t i
    ) const
    {
        CORE_ASSERT(i < _cnt);
        return _data[i].key;
    }

    const VALUE&
    value(
        std::size_t i
    ) const
    {
        CORE_ASSERT(i < _cnt);
        return _data[i].value;
    }

    VALUE&
    value(
        std::size_t i
    )
    {
        CORE_ASSERT(i < _cnt);
        return _data[i].value;
    }

/*
    Entry&
    entry(
        std::size_t i
    )
    {
        CORE_ASSERT(i < _cnt);
        return _data[i];
    }
*/
    std::size_t
    size() const
    {
        return _cnt;
    }

    std::size_t
    max_size() const
    {
        return N;
    }
};

class BootloaderMaster
{
public:
    BootloaderMaster();

    bool
    start();

    bool
    stop();

    bool
    ls();

    bool
    identifySlave(
        ModuleUID uid
    );

    bool
    selectSlave(
        ModuleUID uid
    );

    bool
    describe(
        payload::Describe& description
    );

    bool
    eraseProgram();

    bool
    eraseConfiguration();

    bool
    eraseUserConfiguration();

    bool
    writeModuleName(
        ModuleName name
    );

    bool
    writeModuleName(
        const char* name
    );

    bool
    writeModuleCanID(
        uint8_t id
    );

    bool
    writeProgramCRC(
        uint32_t crc
    );

    bool
    beginIHex();

    bool
    writeIHex(
        const char* ihex_string
    );

    bool
    endIHex();

    bool
    deselectSlave();


    bool
    rebootRemote(
        ModuleName module
    );

    bool
    rebootRemotes();

    std::size_t
    slavesCount() const;


    ModuleUID
    slaveID(
        std::size_t i
    ) const
    {
        return _slaves.key(i);
    }

    const payload::Describe&
    slaveDescription(
        std::size_t i
    ) const
    {
        return _slaves.value(i);
    }

private:
    bool
    masterAdvertiseNode();

    bool
    masterAnnounceNode();

    bool
    masterNode();

    void
    masterNodeCode();


    StaticMap<ModuleUID, payload::Describe, MAX_NUMBER_OF_SLAVES> _slaves;

    core::mw::Node _node;
    core::mw::Publisher<BootMsg>     _pub;
    core::mw::Subscriber<BootMsg, 5> _sub;

    core::os::Thread* _masterAdvertiseThread;
    core::os::Thread* _masterAnnounceThread;
    core::os::Thread* _masterThread;
    bool _run;

    bool           _advertise;
    bool           _bootload;
    MessageType    _ack_filter;
    uint8_t        _sequence_id;
    BootMsg*       _command;
    AcknowledgeMsg _last_ack;
    ModuleUID      _selected;

    core::os::Thread* _runner;

    bool
    wait(
        core::os::Time timeout
    );

    void
    wake();

    bool
    beginCommand(
        MessageType type
    );


    template <typename T>
    inline T*
    commandPayload()
    {
    	CORE_WARNINGS_NO_CAST_ALIGN
        return reinterpret_cast<T*>(&_command->data);
    	CORE_WARNINGS_RESET
    }

    bool
    endCommand();

    bool
    waitForAck(
        core::os::Time timeout = core::os::Time::s(5)
    );


    /* Command types */

    bool
    commandUIDAndMaster(
        MessageType type,
        ModuleUID   uid
    );

    bool
    commandUID(
        MessageType type,
        ModuleUID   uid
    );

    bool
    commandUIDAndName(
        MessageType type,
        ModuleUID   uid,
        ModuleName  name
    );

    bool
    commandUIDAndID(
        MessageType type,
        ModuleUID   uid,
        uint8_t     id
    );

    bool
    commandUIDAndCRC(
        MessageType type,
        ModuleUID   uid,
        uint32_t    crc
    );

    bool
    commandIHex(
        MessageType         type,
        payload::IHex::Type ihex_type,
        const char*         ihex_string
    );
};
}
NAMESPACE_CORE_MW_END
