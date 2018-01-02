/* COPYRIGHT (c) 2016-2018 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#include <core/mw/namespace.hpp>
#include <core/mw/RemoteSubscriber.hpp>

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
