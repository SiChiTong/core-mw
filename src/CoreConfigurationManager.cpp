/* COPYRIGHT (c) 2016 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#include <core/mw/namespace.hpp>
#include <core/mw/CoreConfigurationManager.hpp>

NAMESPACE_CORE_MW_BEGIN

CoreConfigurationManager::CoreConfigurationManager()
{}

CoreConfigurationManager::~CoreConfigurationManager()
{}

void
CoreConfigurationManager::add(
   const CoreConfigurableBase& configurableObject
)
{
   _objects.link(configurableObject.link);
}

void
CoreConfigurationManager::dumpTo(
   uint8_t*    storage,
   std::size_t size
)
{
   std::size_t offset = 0;
   std::size_t cnt    = 0;

   for (const CoreConfigurableBase& object : _objects) {
      // For every object...
      if (object.dumpConfigurationTo(storage + sizeof(std::size_t), offset, size - sizeof(std::size_t))) {
         // If the conf has been dumped...
         cnt++; // Update the number of blocks
      }
   }

   memcpy(storage, &cnt, sizeof(std::size_t)); // Number of conf blocks
}

void
CoreConfigurationManager::setFrom(
   uint8_t*    storage,
   std::size_t size
)
{
   std::size_t offset = 0;
   std::size_t cnt    = 0;

   memcpy(&cnt, storage, sizeof(std::size_t)); // Number of conf blocks

   for (std::size_t i = 0; i < cnt; i++) {
      // For every block...
      for (CoreConfigurableBase& object : _objects) {
         // Try with every object...
         if (object.setConfigurationFrom(storage, offset, size - sizeof(std::size_t))) {
            // If the block matches the object...
            break; // Go on with the next block!
         }
      }
   }
}

NAMESPACE_CORE_MW_END
