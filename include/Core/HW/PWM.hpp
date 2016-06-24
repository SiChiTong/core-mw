/* COPYRIGHT (c) 2016 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#pragma once

#include "hal.h"

#include <Core/HW/namespace.hpp>
#include <Core/HW/common.hpp>
#include <Core/HW/GPIO.hpp>

NAMESPACE_CORE_HW_BEGIN

template <std::size_t E>
struct PWMDriverTraits {};

template <>
struct PWMDriverTraits<1> {
   static constexpr auto driver = &PWMD1;
};

class PWMMaster
{
   virtual void
   setCallback(
      std::function<void()>callback
   ) = 0;

   virtual void
   resetCallback() = 0;
};

template <class _PWM>
class PWMMaster_:
   PWMMaster
{
   using PWM = _PWM;

public:
   static std::function<void()> callback_impl;

   inline void
   setCallback(
      std::function<void()>callback
   )
   {
      callback_impl = callback;

      const_cast<PWMConfig*>(PWM::driver->config)->callback = _callback;
   }

   inline void
   resetCallback()
   {
      const_cast<PWMConfig*>(PWM::driver->config)->callback = nullptr;
   }

private:
   static inline void
   _callback(
      PWMDriver* pwmp
   )
   {
      callback_impl();
   }
};

class PWMChannel
{
public:
   using CountDataType = pwmcnt_t;

   virtual void
   stop() = 0;

   virtual void
   enable() = 0;

   virtual void
   disable() = 0;

   virtual bool
   set(
      CountDataType value
   ) = 0;
};

template <class _PWM, std::size_t _CHANNEL>
class PWMChannel_:
   public PWMChannel
{
public:
   using PWM = _PWM;
   const int CHANNEL = _CHANNEL;

public:
   static std::function<void()> callback_impl;

public:
   inline void
   enable()
   {
      ::pwmEnableChannel(PWM::driver, CHANNEL, 0);
   }

   inline void
   disable()
   {
      ::pwmDisableChannel(PWM::driver, CHANNEL);
   }

   inline void
   stop()
   {
      ::pwmStop(PWM::driver);
   }

   inline bool
   set(
      CountDataType value
   )
   {
      ::pwmEnableChannel(PWM::driver, CHANNEL, value);
      return true;
   }
};

template <class _PWM>
std::function<void()>PWMMaster_<_PWM>::callback_impl;

// --- Aliases -----------------------------------------------------------------

using PWM_1 = PWMDriverTraits<1>;

NAMESPACE_CORE_HW_END
