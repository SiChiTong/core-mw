/* COPYRIGHT (c) 2016 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#include <Core/MW/namespace.hpp>
#include <Core/MW/LocalPublisher.hpp>

NAMESPACE_CORE_MW_BEGIN


LocalPublisher::LocalPublisher()
   :
   by_node(*this)
{}


LocalPublisher::~LocalPublisher() {}


NAMESPACE_CORE_MW_END
