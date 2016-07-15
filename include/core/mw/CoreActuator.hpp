/* COPYRIGHT (c) 2016 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#pragma once

#include <core/mw/namespace.hpp>

NAMESPACE_CORE_MW_BEGIN

template <typename _T>
class CoreActuator
{
public:
   using DataType = _T;

   virtual bool
   init() = 0;

   virtual bool
   configure() = 0;

   virtual bool
   start() = 0;

   virtual bool
   stop() = 0;

   virtual bool
   waitUntilReady() = 0;

   virtual bool
   set(
      DataType& data
   ) = 0;
};

NAMESPACE_CORE_MW_END
