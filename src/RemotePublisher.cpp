/* COPYRIGHT (c) 2016 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */
 
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
