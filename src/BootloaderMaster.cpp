/* COPYRIGHT (c) 2016-2018 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#include <core/mw/BootloaderMaster.hpp>

NAMESPACE_CORE_MW_BEGIN

namespace bootloader {

SlaveDescription::Version SlaveDescription::version() const {
	return _version;
}
const ModuleType& SlaveDescription::moduleType() const {
	switch(_version) {
		case Version::V1:
			return _v1.moduleType;
			break;
		case Version::V2:
			return _v2.moduleType;
			break;
		case Version::V3:
		default:
			return _v3.moduleType;
			break;
	}
}

const ModuleName& SlaveDescription::moduleName() const {
	switch(_version) {
		case Version::V1:
			return _v1.moduleName;
			break;
		case Version::V2:
			return _v2.moduleName;
			break;
		case Version::V3:
		default:
			return _v3.moduleName;
			break;
	}
}

uint8_t SlaveDescription::moduleID() const {
	switch(_version) {
		case Version::V1:
			return _v1.moduleId;
			break;
		case Version::V2:
			return _v2.moduleId;
			break;
		case Version::V3:
		default:
			return _v3.moduleId;
			break;
	}
}

BootloaderMaster::BootloaderMaster() : _node("bootm", false),
    _masterAdvertiseThread(nullptr),
    _masterAnnounceThread(nullptr),
    _masterThread(nullptr),
    _run(true),
    _advertise(true),
    _bootload(true), _ack_filter(MessageType::NONE), _sequence_id(0), _command(nullptr), _selected(0), _runner(nullptr)
{}

bool
BootloaderMaster::masterAdvertiseNode()
{
    auto thread_code = [](void* args) {
                           BootloaderMaster* _this = reinterpret_cast<BootloaderMaster*>(args);

                           _this->_masterAdvertiseThread = &core::os::Thread::self();

                           core::mw::Node node("bootm_adv");
                           core::mw::Publisher<core::mw::bootloader::BootMasterMsg> pub;
                           core::mw::Publisher<core::mw::bootloader::BootMsg>       pub2;
                           core::mw::bootloader::BootMasterMsg* msgp;
                           core::mw::bootloader::BootMsg*       msgp2;

                           node.advertise(pub, BOOTLOADER_MASTER_TOPIC_NAME);
                           node.advertise(pub2, BOOTLOADER_TOPIC_NAME);

                           while (_this->_run) {
                               if (_this->_advertise && pub.alloc(msgp)) {
                                   msgp->command    = core::mw::bootloader::MessageType::MASTER_ADVERTISE;
                                   msgp->sequenceId = 0;

                                   msgp->data[0] = 0;
                                   msgp->data[1] = 0;
                                   msgp->data[2] = 0;
                                   msgp->data[3] = 0;
                                   msgp->data[4] = 0;
                                   msgp->data[5] = 0;

                                   pub.publish_remotely(*msgp);
                               }

                               if (_this->_bootload && pub2.alloc(msgp2)) {
                                   msgp2->command    = core::mw::bootloader::MessageType::BOOTLOAD;
                                   msgp2->sequenceId = 0;

                                   pub2.publish_remotely(*msgp2);
                               }

                               core::os::Thread::sleep(core::os::Time::ms(250));
                           }
                       };

    auto tmp = core::os::Thread::create_heap(nullptr, 128, core::os::Thread::PriorityEnum::LOWEST, thread_code, this, "bootm_adv");

    if (tmp == nullptr) {
        return false;
    }

    while (_masterAdvertiseThread == nullptr) {
        core::os::Thread::sleep(core::os::Time::ms(10));
    }

    return true;
} // BootloaderMaster::masterAdvertiseNode

std::size_t
BootloaderMaster::slavesCount() const
{
    return _slaves.size();
}

bool
BootloaderMaster::waitForAck(
    core::os::Time timeout
)
{
    _last_ack.status = AcknowledgeStatus::NONE;

    return wait(timeout);
}

void
BootloaderMaster::masterNodeCode()
{
    BootMsg* msgp;

    _node.advertise(_pub, BOOTLOADER_TOPIC_NAME);
    _node.subscribe(_sub, BOOTLOADER_TOPIC_NAME);
    _node.set_enabled(true);

    while (_run) {
        if (_sub.fetch(msgp)) {
            if (msgp->command == MessageType::ACK) {
                const AcknowledgeMsg* tmp = reinterpret_cast<const AcknowledgeMsg*>(msgp);

                if (tmp->type == _ack_filter) {
                    if (tmp->sequenceId == _sequence_id + 1) {
                        memcpy(&_last_ack, tmp, BootMsg::MESSAGE_LENGTH);
                        _sequence_id = tmp->sequenceId + 1;
                        _ack_filter  = MessageType::NONE;
                        wake();
                    }
                }
            }

            _sub.release(*msgp);
        }

        core::os::Thread::sleep(core::os::Time::ms(10));
    }
} // BootloaderMaster::masterNodeCode

bool
BootloaderMaster::masterNode()
{
    auto thread_code = [](void* args) {
                           BootloaderMaster* _this = reinterpret_cast<BootloaderMaster*>(args);

                           _this->_masterThread = &core::os::Thread::self();
                           _this->masterNodeCode();
                       };

    auto tmp = core::os::Thread::create_heap(nullptr, 1024, core::os::Thread::PriorityEnum::NORMAL, thread_code, this, "bootm_sub");

    if (tmp == nullptr) {
        return false;
    }

    while (_masterThread == nullptr) {
        core::os::Thread::sleep(core::os::Time::ms(10));
    }

    return true;
} // BootloaderMaster::masterNode

bool
BootloaderMaster::masterAnnounceNode()
{
    auto thread_code = [](void* args) {
                           BootloaderMaster* _this = reinterpret_cast<BootloaderMaster*>(args);

                           _this->_masterAnnounceThread = &core::os::Thread::self();

                           core::mw::Node node("bootm_sub");
                           core::mw::Subscriber<core::mw::bootloader::BootMasterMsg, 5> sub;
                           core::mw::bootloader::BootMasterMsg* msgp;

                           node.subscribe(sub, BOOTLOADER_MASTER_TOPIC_NAME);

                           while (_this->_run) {
                               while (sub.fetch(msgp)) {
                                   if (msgp->command == core::mw::bootloader::MessageType::REQUEST) {
                                	   CORE_WARNINGS_NO_CAST_ALIGN
                                       core::mw::bootloader::payload::Announce* tmp = reinterpret_cast<core::mw::bootloader::payload::Announce*>(&msgp->data);
                                	   CORE_WARNINGS_RESET

					                    core::os::SysLock::Scope lock;

                                       _this->_slaves[tmp->uid];
                                   }

                                   sub.release(*msgp);
                               }

                               core::os::Thread::sleep(core::os::Time::ms(100));
                           }
                       };

    auto tmp = core::os::Thread::create_heap(nullptr, 256, core::os::Thread::PriorityEnum::NORMAL - 1, thread_code, this, "bootm_sub");

    if (tmp == nullptr) {
        return false;
    }

    while (_masterAnnounceThread == nullptr) {
        core::os::Thread::sleep(core::os::Time::ms(10));
    }

    return true;
}                 // BootloaderMaster::masterAnnounceNode

bool
BootloaderMaster::start()
{
    bool success = true;

    _run = true;

    success &= masterAdvertiseNode();
    success &= masterAnnounceNode();
    success &= masterNode();

    return success;
}

bool
BootloaderMaster::stop()
{
    bool success = true;

    _run = false;

    core::os::Thread::join(*(_masterAdvertiseThread));
    _masterAdvertiseThread = nullptr;
    core::os::Thread::join(*(_masterAnnounceThread));
    _masterAnnounceThread = nullptr;
    core::os::Thread::join(*(_masterThread));
    _masterThread = nullptr;

    clear();

    return success;
}

bool
BootloaderMaster::rebootRemotes()
{
    return core::mw::Middleware::instance().reboot_remote("*", true);
}

bool
BootloaderMaster::rebootRemote(
    ModuleName module
)
{
    return core::mw::Middleware::instance().reboot_remote(module, true);
}

bool
BootloaderMaster::wait(
    core::os::Time timeout
)
{
    core::os::SysLock::Scope lock;

    _runner = &core::os::Thread::self();
    core::os::Thread::Return msg = core::os::Thread::sleep_timeout(timeout);

    return msg == 0x1BADCAFE;
}

void
BootloaderMaster::wake()
{
    core::os::SysLock::Scope lock;

    if (_runner != nullptr) {
        core::os::Thread::wake(*(_runner), 0x1BADCAFE);
        _runner = nullptr;
    }
}

bool
BootloaderMaster::beginCommand(
    MessageType type
)
{
    _last_ack.status = AcknowledgeStatus::NONE; // Make sure we always refer to the last ack
    _ack_filter      = type;

    core::mw::bootloader::BootMsg* msgp;

    if (_pub.alloc(msgp)) {
        _command = msgp;
        _command->command    = type;
        _command->sequenceId = _sequence_id;

        return true;
    } else {
        _command = nullptr;
        return false;

    }
} // BootloaderMaster::beginCommand

bool
BootloaderMaster::endCommand()
{
    if (_pub.publish_remotely(*_command)) {
        _command = nullptr;
        return true;
    }

    _command = nullptr;

    return false;
}

bool
BootloaderMaster::commandUIDAndMaster(
    MessageType type,
    ModuleUID   uid
)
{
    if (beginCommand(type)) {
        commandPayload<payload::UIDAndMaster>()->uid = uid;

        if (endCommand()) {
            if (waitForAck()) {
                return _last_ack.status == AcknowledgeStatus::OK;
            }
        }
    }

    return false;
}

bool
BootloaderMaster::commandUID(
    MessageType type,
    ModuleUID   uid
)
{
    if (beginCommand(type)) {
        commandPayload<payload::UID>()->uid = uid;

        if (endCommand()) {
            if (waitForAck()) {
                return _last_ack.status == AcknowledgeStatus::OK;
            }
        }
    }

    return false;
}

bool
BootloaderMaster::commandUID_NoAck(
    MessageType type,
    ModuleUID   uid
)
{
    if (beginCommand(type)) {
        commandPayload<payload::UID>()->uid = uid;


        if (endCommand()) {
            if (waitForAck()) {
            }

            return true;
        }
    }

    return false;
}

bool
BootloaderMaster::commandEmpty_NoAck(
    MessageType type
)
{
    if (beginCommand(type)) {
        if (endCommand()) {
            if (waitForAck()) {
            }

            return true;
        }
    }

    return false;
}
bool
BootloaderMaster::commandUIDAndName(
    MessageType type,
    ModuleUID   uid,
    ModuleName  name
)
{
    if (beginCommand(type)) {
        commandPayload<payload::UIDAndName>()->uid  = uid;
        commandPayload<payload::UIDAndName>()->name.fill(0);
        commandPayload<payload::UIDAndName>()->name = name.c_str(); // Workaround for "STMIA bug"

        if (endCommand()) {
            if (waitForAck()) {
                return _last_ack.status == AcknowledgeStatus::OK;
            }
        }
    }

    return false;
}

bool
BootloaderMaster::commandUIDAndCRC(
    MessageType type,
    ModuleUID   uid,
    uint32_t    crc
)
{
    if (beginCommand(type)) {
        commandPayload<payload::UIDAndCRC>()->uid = uid;
        commandPayload<payload::UIDAndCRC>()->crc = crc;

        if (endCommand()) {
            if (waitForAck()) {
                return _last_ack.status == AcknowledgeStatus::OK;
            }
        }
    }

    return false;
}

bool
BootloaderMaster::commandUIDAndID(
    MessageType type,
    ModuleUID   uid,
    uint8_t     id
)
{
    if (beginCommand(type)) {
        commandPayload<payload::UIDAndID>()->uid = uid;
        commandPayload<payload::UIDAndID>()->id  = id;

        if (endCommand()) {
            if (waitForAck()) {
                return _last_ack.status == AcknowledgeStatus::OK;
            }
        }
    }

    return false;
}

bool
BootloaderMaster::commandIHex(
    MessageType         type,
    payload::IHex::Type ihex_type,
    const char*         ihex_string
)
{
    if (beginCommand(type)) {
        commandPayload<payload::IHex>()->type = ihex_type;
        memcpy(&commandPayload<payload::IHex>()->string, ihex_string, 44);

        if (endCommand()) {
            if (waitForAck()) {
                return _last_ack.status == AcknowledgeStatus::OK;
            }
        }
    }

    return false;
}

bool
BootloaderMaster::commandUIDAndAddress(
    MessageType type,
    ModuleUID   uid,
    uint32_t    address
) {
    if (beginCommand(type)) {
        commandPayload<payload::UIDAndAddress>()->uid = uid;
        commandPayload<payload::UIDAndAddress>()->address = address;

        if (endCommand()) {
            if (waitForAck()) {
                return _last_ack.status == AcknowledgeStatus::OK;
            }
        }
    }

    return false;
}

bool
BootloaderMaster::selectSlave(
    ModuleUID uid
)
{
    _sequence_id = 0;

    if (commandUIDAndMaster(MessageType::SELECT_SLAVE, uid)) {
        _selected = uid;

        return true;
    }

    return false;
}

bool
BootloaderMaster::ls()
{
    bool success = true;

    std::size_t n = slavesCount();

    while (n--) {
        bool      tmp = true;
        ModuleUID uid = _slaves.key(n);
        tmp &= selectSlave(uid);

        if(!tmp) {
        	_slaves.value(n)._version = SlaveDescription::Version::NONE;
    	} else if(describeV3(_slaves.value(n)._v3)) {
    		_slaves.value(n)._version = SlaveDescription::Version::V3;
        } else if(describeV2(_slaves.value(n)._v2)) {
        	_slaves.value(n)._version = SlaveDescription::Version::V2;
        } else if(describeV1(_slaves.value(n)._v1)) {
        	_slaves.value(n)._version = SlaveDescription::Version::V1;
        } else {
        	_slaves.value(n)._version = SlaveDescription::Version::NONE;
        	tmp = false;
        }

        deselectSlave();

        success &= tmp;
    }

    deselectAllSlaves();

    return success;
} // BootloaderMaster::ls

void
BootloaderMaster::clear()
{
    core::os::SysLock::Scope lock;
	_slaves.clear();
}

bool
BootloaderMaster::describeV1(
    payload::DescribeV1& description
)
{
    if (commandUID(MessageType::DESCRIBE_V1, _selected)) {
        AcknowledgeDescribeV1* tmp = reinterpret_cast<AcknowledgeDescribeV1*>(&_last_ack);
        description = tmp->data;
        return true;
    }

    return false;
}

bool
BootloaderMaster::describeV2(
    payload::DescribeV2& description
)
{
    if (commandUID(MessageType::DESCRIBE_V2, _selected)) {
        AcknowledgeDescribeV2* tmp = reinterpret_cast<AcknowledgeDescribeV2*>(&_last_ack);
        description = tmp->data;
        return true;
    }

    return false;
}

bool
BootloaderMaster::describeV3(
    payload::DescribeV3& description
)
{
    if (commandUID(MessageType::DESCRIBE_V3, _selected)) {
        AcknowledgeDescribeV3* tmp = reinterpret_cast<AcknowledgeDescribeV3*>(&_last_ack);
        description = tmp->data;
        return true;
    }

    return false;
}
bool
BootloaderMaster::identifySlave(
    ModuleUID uid
)
{
    return commandUID(MessageType::IDENTIFY_SLAVE, uid);
}

bool
BootloaderMaster::deselectSlave()
{
    return commandUID(MessageType::DESELECT_SLAVE, _selected);
}

bool
BootloaderMaster::resetSlave() {
	return commandUID_NoAck(MessageType::RESET, _selected);
}

bool
BootloaderMaster::resetAllSlaves()
{
    _sequence_id = 0;
    _selected = 0xFFFFFFFF;

    return commandEmpty_NoAck(MessageType::RESET_ALL);
}

void
BootloaderMaster::deselectAllSlaves()
{
	_selected = 0xFFFFFFFF;
	selectSlave(_selected);
	_selected = 0xFFFFFFFF;
	deselectSlave();
}

bool
BootloaderMaster::eraseProgram()
{
    return commandUID(MessageType::ERASE_PROGRAM, _selected);
}

bool
BootloaderMaster::eraseConfiguration()
{
    return commandUID(MessageType::ERASE_CONFIGURATION, _selected);
}

bool
BootloaderMaster::eraseUserConfiguration()
{
    return commandUID(MessageType::ERASE_USER_CONFIGURATION, _selected);
}

bool
BootloaderMaster::writeModuleName(
    ModuleName name
)
{
    return commandUIDAndName(MessageType::WRITE_MODULE_NAME, _selected, name);
}

bool
BootloaderMaster::writeModuleName(
    const char* name
)
{
    ModuleName tmp;

    tmp = name;
    return commandUIDAndName(MessageType::WRITE_MODULE_NAME, _selected, tmp);
}

bool
BootloaderMaster::writeModuleCanID(
    uint8_t id
)
{
    return commandUIDAndID(MessageType::WRITE_MODULE_CAN_ID, _selected, id);
}

bool
BootloaderMaster::writeProgramCRC(
    uint32_t crc
)
{
    return commandUIDAndCRC(MessageType::WRITE_PROGRAM_CRC, _selected, crc);
}

bool
BootloaderMaster::beginIHex()
{
    return commandIHex(MessageType::IHEX_WRITE, payload::IHex::Type::BEGIN, "");
}

bool
BootloaderMaster::writeIHex(
    const char* ihex_string
)
{
    return commandIHex(MessageType::IHEX_WRITE, payload::IHex::Type::DATA, ihex_string);
}

bool
BootloaderMaster::endIHex()
{
    return commandIHex(MessageType::IHEX_WRITE, payload::IHex::Type::END, "");
}

bool
BootloaderMaster::readTags(char* buffer) {
	if(_slaves[_selected]._version == SlaveDescription::Version::V3) {
		uint32_t size = _slaves[_selected]._v3.tagsFlashSize;
		if(size > 0) {
			uint32_t offset = 0;
			while(offset < size) {
				if (commandUIDAndAddress(MessageType::TAGS_READ, _selected, offset)) {
					AcknowledgeString* tmp = reinterpret_cast<AcknowledgeString*>(&_last_ack);
					::memcpy(buffer, tmp->data, 16);
					buffer += 16;
					offset += 16;
				} else {
					return false;
				}
			}
			return true;
		}
	}
	return false;
}

bool
BootloaderMaster::readTags(size_t offset, core::String<17>& buffer) {
	if(_slaves[_selected]._version == SlaveDescription::Version::V3) {
		uint32_t size = _slaves[_selected]._v3.tagsFlashSize;
		if(size > 0) {
			if(offset < size) {
				if (commandUIDAndAddress(MessageType::TAGS_READ, _selected, offset)) {
					AcknowledgeString* tmp = reinterpret_cast<AcknowledgeString*>(&_last_ack);
					buffer.fill(0);
					::memcpy(buffer.data(), tmp->data, 16);
					return true;
				}
			}
		}
	}
	return false;
}
}
NAMESPACE_CORE_MW_END
