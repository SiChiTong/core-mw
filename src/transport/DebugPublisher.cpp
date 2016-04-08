#include <Core/MW/namespace.hpp>
#include <Core/MW/transport/DebugPublisher.hpp>

NAMESPACE_CORE_MW_BEGIN


DebugPublisher::DebugPublisher(
		Transport& transport
)
	:
	RemotePublisher(transport)
{}


DebugPublisher::~DebugPublisher() {}


NAMESPACE_CORE_MW_END
