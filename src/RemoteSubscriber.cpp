#include <Core/MW/namespace.hpp>
#include <Core/MW/RemoteSubscriber.hpp>

NAMESPACE_CORE_MW_BEGIN


RemoteSubscriber::RemoteSubscriber(
		Transport& transport
)
	:
	BaseSubscriber(),
	transportp(&transport),
	by_transport(*this),
	by_topic(*this)
{}


RemoteSubscriber::~RemoteSubscriber() {}


NAMESPACE_CORE_MW_END
