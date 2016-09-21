/* COPYRIGHT (c) 2016 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#pragma once

#include <core/mw/namespace.hpp>

#include <core/mw/StaticList.hpp>
#include <core/mw/CoreConfiguration.hpp>

NAMESPACE_CORE_MW_BEGIN

class CoreConfigurationManager
{
public:
   CoreConfigurationManager();
   virtual
   ~CoreConfigurationManager();

   void
   add(
      const CoreConfigurableBase& configurableObject
   );

   void
   dumpTo(
      uint8_t*    storage,
      std::size_t size
   );
   void
   setFrom(
      uint8_t*    storage,
      std::size_t size
   );

private:
   core::mw::StaticList<CoreConfigurableBase> _objects;
};

NAMESPACE_CORE_MW_END
