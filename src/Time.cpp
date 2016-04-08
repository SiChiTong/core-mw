#include <Core/MW/namespace.hpp>
#include <Core/MW/Time.hpp>

NAMESPACE_CORE_MW_BEGIN


const Time Time::IMMEDIATE(0);
const Time Time::INFINITE(MAX_US + 1);
const Time Time::INFINITEN(MIN_US - 1);


NAMESPACE_CORE_MW_END
