/* COPYRIGHT (c) 2016 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#include <core/mw/CoreConfiguration.hpp>

NAMESPACE_CORE_MW_BEGIN


CoreParameterBase
&
CoreConfiguration::operator[](
   Key key
)
{
   for (KeyValue x : * this) {
      if (keyMatch(key, x.key)) {
         return this->*(x.ptr);
      }
   }

   CORE_ASSERT(!"Unable to find parameter");

   return *static_cast<CoreParameterBase*>(nullptr);
}

CoreParameterBase::operator const char*() const
{
   char* x = nullptr;

   get(x);

   return x;
}

CoreParameterBase&
CoreParameterBase::operator=(
   const char* x
)
{
   CORE_ASSERT(strlen(x) < getSize() && CoreType::CHAR == getCoreType()); // make sure we are doing something meaningful...

   std::size_t i = 0;
   std::size_t l = strlen(x) + 1;

   while (i < l) {
      set(i, &x[i]);
      i++;
   }

   while (i < getSize()) {
      set(i, "\0");
      i++;
   }

   return *this;
} // =

CoreParameterBase&
CoreParameterBase::operator=(
   const CoreParameterBase& x
)
{
   CORE_ASSERT(x.getSize() == getSize() && x.getCoreType() == getCoreType()); // make sure we are doing something meaningful...

   uint8_t tmp[x.getSize() * 8]; // TODO: fix this crap (avoiding an unnecessary copy)

   x.get(tmp);
   set(tmp);

   return *this;
}

NAMESPACE_CORE_MW_END
