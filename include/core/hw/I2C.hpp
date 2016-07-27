/* COPYRIGHT (c) 2016 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#pragma once



#include <core/hw/namespace.hpp>
#include <core/hw/common.hpp>

#include <core/hw/GPIO.hpp>

#include "hal.h"

NAMESPACE_CORE_HW_BEGIN

template <std::size_t S>
struct I2CDriverTraits {};

#if STM32_I2C_USE_I2C1
template <>
struct I2CDriverTraits<1> {
   static constexpr auto driver = &I2CD1;
};
#endif

#if STM32_I2C_USE_I2C2
template <>
struct I2CDriverTraits<2> {
   static constexpr auto driver = &I2CD2;
};
#endif

#if STM32_I2C_USE_I2C3
template <>
struct I2CDriverTraits<3> {
   static constexpr auto driver = &I2CD3;
};
#endif


class I2CMaster
{
public:
   using Address = i2caddr_t;
   using Flags = i2cflags_t;

   virtual void
   start(
      const I2CConfig& config
   ) = 0;

   virtual void
   stop() = 0;

   virtual void
   acquireBus() = 0;

   virtual void
   releaseBus() = 0;

   virtual bool
   exchange(
	Address address,
      size_t      n_tx,
      const void* txbuf,
	  size_t      n_rx,
      void*       rxbuf,
	  core::os::Time timeout = core::os::Time::INFINITE,
	  msg_t* msg = nullptr
   ) = 0;

   virtual bool
   send(
	Address address,
      size_t      n_tx,
      const void* txbuf,
	  core::os::Time timeout = core::os::Time::INFINITE,
	  msg_t* msg = nullptr
   ) = 0;

   virtual bool
   receive(
	Address address,
      size_t      n_rx,
      void* rxbuf,
	  core::os::Time timeout = core::os::Time::INFINITE,
	  msg_t* msg = nullptr
   ) = 0;

   virtual Flags
   getErrors() = 0;
};


template <class _I2C>
class I2CMaster_ : public I2CMaster
{
public:
   using I2C = _I2C;

   inline void
   start(
      const I2CConfig& config
   )
   {
      ::i2cStart(I2C::driver, &config);
   }

   inline void
   stop()
   {
      ::i2cStop(I2C::driver);
   }

   inline void
   acquireBus()
   {
      ::i2cAcquireBus(I2C::driver);
      ::i2cStart(I2C::driver, I2C::driver->config);
   }

   inline void
   releaseBus()
   {
      ::i2cStop(I2C::driver);
      ::i2cReleaseBus(I2C::driver);
   }

   inline bool
   exchange(
	Address address,
      size_t      n_tx,
      const void* txbuf,
	  size_t      n_rx,
      void*       rxbuf,
	  core::os::Time timeout = core::os::Time::INFINITE,
	  msg_t* msg = nullptr
   )
   {
	   address &= 0x7F;

	   msg_t status = ::i2cMasterTransmitTimeout (I2C::driver, address, reinterpret_cast<const uint8_t*>(txbuf), n_tx, reinterpret_cast<uint8_t*>(rxbuf), n_rx, timeout.raw);

	   if(msg != nullptr) {
		   *msg = status;
	   }

	   return status == MSG_OK;
   }

   inline bool
   send(
	Address address,
      size_t      n_tx,
      const void* txbuf,
	  core::os::Time timeout = core::os::Time::INFINITE,
	  msg_t* msg = nullptr
   )
   {
	   address &= 0x7F;

	   msg_t status = ::i2cMasterTransmitTimeout (I2C::driver, address, reinterpret_cast<const uint8_t*>(txbuf), n_tx, nullptr, 0, timeout.raw);

	   if(msg != nullptr) {
		   *msg = status;
	   }

	   return status == MSG_OK;
   }

   inline bool
   receive(
	Address address,
      size_t      n_rx,
      void* rxbuf,
	  core::os::Time timeout = core::os::Time::INFINITE,
	  msg_t* msg = nullptr
   )
   {
	   address &= 0x7F;

	   msg_t status = ::i2cMasterReceiveTimeout (I2C::driver, address, reinterpret_cast<uint8_t*>(rxbuf), n_rx, timeout.raw);

	   if(msg != nullptr) {
		   *msg = status;
	   }

	   return status == MSG_OK;
   }

   inline Flags
   getErrors() {
	   return ::i2cGetErrors(I2C::driver);
   }
};

#if 0 // NE VALE LA PENA???
class I2CDevice
{
public:
   virtual void
   acquireBus() = 0;

   virtual void
   releaseBus() = 0;

   virtual void
   exchange(
      size_t      n,
      const void* txbuf,
      void*       rxbuf
   ) = 0;

   virtual void
   send(
      size_t      n,
      const void* txbuf
   ) = 0;

   virtual void
   receive(
      size_t n,
      void*  rxbuf
   ) = 0;
};

template <class _I2C, uint8_t _ADDRESS>
class I2CDevice_:
   public I2CDevice
{
public:
   using I2C = _I2C;
   static const I2CMaster<I2C>::Address ADDRESS  = _ADDRESS;

   inline void
   acquireBus()
   {
	      ::i2cAcquireBus(I2C::driver);

   }

   inline void
   releaseBus()
   {
	   ::i2cReleaseBus(I2C::driver);
   }

   inline bool
   exchange(
      size_t      n_tx,
      const void* txbuf,
	  size_t      n_rx,
      void*       rxbuf,
	  core::os::Time timeout = core::os::Time::INFINITE,
	  msg_t* msg = nullptr
   )
   {
	   address &= 0x7F;

	   msg_t status = ::i2cMasterTransmitTimeout (I2C::driver, ADDRESS, txbuf, n_tx, rxbuf, n_rx, timeout.raw);

	   if(msg != nullptr) {
		   *msg = status;
	   }

	   return status == MSG_OK;
   }

   inline bool
   send(
      size_t      n_tx,
      const void* txbuf,
	  core::os::Time timeout = core::os::Time::INFINITE,
	  msg_t* msg = nullptr
   )
   {
	   address &= 0x7F;

	   msg_t status = ::i2cMasterTransmitTimeout (I2C::driver, ADDRESS, txbuf, n_tx, nullptr, 0, timeout.raw);

	   if(msg != nullptr) {
		   *msg = status;
	   }

	   return status == MSG_OK;
   }

   inline bool
   receive(
      size_t      n_rx,
      const void* rxbuf,
	  core::os::Time timeout = core::os::Time::INFINITE,
	  msg_t* msg = nullptr
   )
   {
	   address &= 0x7F;

	   msg_t status = ::i2cMasterReceiveTimeout (I2C::driver, ADDRESS, rxbuf, n_rx, timeout.raw);

	   if(msg != nullptr) {
		   *msg = status;
	   }

	   return status == MSG_OK;
   }

private:
   CS _cs;
};
#endif
// --- Aliases -----------------------------------------------------------------

using I2C_1 = I2CDriverTraits<1>;
using I2C_2 = I2CDriverTraits<2>;
using I2C_3 = I2CDriverTraits<3>;

NAMESPACE_CORE_HW_END
