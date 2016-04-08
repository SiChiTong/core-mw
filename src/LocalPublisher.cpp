#include <Core/MW/namespace.hpp>
#include <Core/MW/LocalPublisher.hpp>

NAMESPACE_CORE_MW_BEGIN


LocalPublisher::LocalPublisher()
	:
	by_node(*this)
{}


LocalPublisher::~LocalPublisher() {}


NAMESPACE_CORE_MW_END
