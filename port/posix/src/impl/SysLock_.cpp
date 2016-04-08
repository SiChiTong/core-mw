#include <Core/MW/namespace.hpp>
#include <Core/MW/impl/SysLock_.hpp>

NAMESPACE_CORE_MW_BEGIN


volatile unsigned SysLock_::exclusion = 0;


NAMESPACE_CORE_MW_END
