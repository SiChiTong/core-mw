/* COPYRIGHT (c) 2016-2018 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */
 
#include <core/mw/Middleware.hpp>
#include <core/os/Thread.hpp>
#include <core/mw/BootMsg.hpp>
#include <algorithm>

#ifndef MAX_NUMBER_OF_SLAVES
#define MAX_NUMBER_OF_SLAVES 32
#endif

NAMESPACE_CORE_MW_BEGIN

namespace bootloader {

struct SlaveDescription {
	friend class BootloaderMaster;

	enum class Version {
		NONE, V1, V2, V3
	};

	Version version() const;

	const ModuleType& moduleType() const;

	const ModuleName& moduleName() const;

	uint8_t moduleID() const;

private:
	Version _version;
	union {
		payload::DescribeV1 _v1;
		payload::DescribeV2 _v2;
		payload::DescribeV3 _v3;
	};
};

/* Static size associative map.
 *
 * Warning: naive implementation.
 */
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

        // Search the key
        for (i = 0; i < _cnt; i++) {
            if (_data[i].key == key) {
            	// We already have an entry.
                return _data[i].value;
            }
        }

        // No matches. Add a new entry.
        if (i < N) {
            _cnt++;
            _data[i].key   = key;
            _data[i].value = VALUE();

            return _data[i].value;
        } else {
            CORE_ASSERT(!"Too many entries");
        }

        UNREACHABLE;
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

    void clear()
    {
    	_cnt = 0;
    }

    void sort() {
    	std::sort(_data.begin(), _data.end(), [](const Entry& a,const Entry& b) {
    		return a.key < b.key;
    	});
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

    void
	clear();

    bool
    identifySlave(
        ModuleUID uid
    );

    bool
    selectSlave(
        ModuleUID uid
    );

    bool
    describeV1(
        payload::DescribeV1& description
    );

    bool
    describeV2(
        payload::DescribeV2& description
    );

    bool
    describeV3(
        payload::DescribeV3& description
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
	readTags(char* buffer);

    bool
    readTags(
        size_t offset,
        core::String<17>& buffer
    );

    bool
	resetSlave();

    bool
    deselectSlave();

    void
    deselectAllSlaves();

    inline void
	advertise(bool enable) {
    	_advertise = enable;
    }

    inline void
	bootload(bool enable) {
    	_bootload = enable;
    }

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

    const SlaveDescription&
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

    StaticMap<ModuleUID, SlaveDescription, MAX_NUMBER_OF_SLAVES> _slaves;

    core::mw::Node _node;
    core::mw::Publisher<BootMsg>     _pub;
    core::mw::Subscriber<BootMsg, 2> _sub;

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
    commandUID_NoAck(
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

    bool
    commandUIDAndAddress(
        MessageType type,
        ModuleUID   uid,
        uint32_t    address
    );
};
}
NAMESPACE_CORE_MW_END
