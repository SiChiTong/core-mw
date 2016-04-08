#include <Core/MW/namespace.hpp>
#include <Core/MW/RemotePublisher.hpp>

NAMESPACE_CORE_MW_BEGIN

RemotePublisher::RemotePublisher(
		Transport& transport
)
	:
	BasePublisher(),
	transportp(&transport),
	by_transport(*this)
{}


RemotePublisher::~RemotePublisher() {}


NAMESPACE_CORE_MW_END
