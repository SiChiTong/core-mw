/* COPYRIGHT (c) 2016 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#pragma once

//TODO: hal

#include <Core/HW/namespace.hpp>
#include <Core/HW/common.hpp>

#include <functional>

NAMESPACE_CORE_HW_BEGIN

template <std::size_t E>
struct EXTDriverTraits {};

template <>
struct EXTDriverTraits<1> {
   static constexpr auto driver = &EXTD1;
};

class EXTChannel
{
public:
   virtual void
   enable() = 0;

   virtual void
   disable() = 0;

   virtual void
   setCallback(
      std::function<void()>callback
   ) = 0;
};

template <class _EXT, std::size_t _CH, uint32_t _MODE>
class EXTChannel_:
   public EXTChannel
{
public:
   using EXT = _EXT;

public:
   static std::function<void()> callback_impl;

public:
   inline void
   enable()
   {
      ::extChannelEnable(EXT::driver, _CH);
   }

   inline void
   disable()
   {
      ::extChannelDisable(EXT::driver, _CH);
   }

   inline void
   setCallback(
      std::function<void()>callback
   )
   {
      callback_impl = callback;

      EXTChannelConfig tmp;

      tmp.mode = _MODE;
      tmp.cb   = _callback;

      extSetChannelMode(EXT::driver, _CH, &tmp);
   }

private:
   static void
   _callback(
      EXTDriver*   extp,
      expchannel_t channel
   )
   {
      callback_impl();
   }
};

template <class _EXT, std::size_t _CH, uint32_t _MODE>
std::function<void()>EXTChannel_<_EXT, _CH, _MODE>::callback_impl;

// --- Aliases -----------------------------------------------------------------

using EXT_1 = EXTDriverTraits<1>;

NAMESPACE_CORE_HW_END
