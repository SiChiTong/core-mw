#include <Core/MW/namespace.hpp>
#include <Core/MW/transport/RTCANPublisher.hpp>

NAMESPACE_CORE_MW_BEGIN


RTCANPublisher::RTCANPublisher(
		Transport& transport
)
	:
	RemotePublisher(transport)
{}


RTCANPublisher::~RTCANPublisher() {}


NAMESPACE_CORE_MW_END
