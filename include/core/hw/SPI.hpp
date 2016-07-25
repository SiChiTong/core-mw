/* COPYRIGHT (c) 2016 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#pragma once

//TODO: hal

#include <core/hw/namespace.hpp>
#include <core/hw/common.hpp>

#include <core/hw/GPIO.hpp>

NAMESPACE_CORE_HW_BEGIN

template <std::size_t S>
struct SPIDriverTraits {};

#if STM32_SPI_USE_SPI1
template <>
struct SPIDriverTraits<1> {
   static constexpr auto driver = &SPID1;
};
#endif

#if STM32_SPI_USE_SPI2
template <>
struct SPIDriverTraits<2> {
   static constexpr auto driver = &SPID2;
};
#endif

#if STM32_SPI_USE_SPI3
template <>
struct SPIDriverTraits<3> {
   static constexpr auto driver = &SPID3;
};
#endif

template <class _SPI>
class SPIMaster
{
public:
   using SPI = _SPI;
   inline static void
   start(
      const SPIConfig& config
   )
   {
      ::spiStart(SPI::driver, &config);
   }

   inline static void
   acquireBus()
   {
      ::spiAcquireBus(SPI::driver);
   }

   inline static void
   releaseBus()
   {
      ::spiReleaseBus(SPI::driver);
   }

   inline static void
   ignore(
      size_t n
   )
   {
      ::spiIgnore(SPI::driver, n);
   }

   inline static void
   exchange(
      size_t      n,
      const void* txbuf,
      void*       rxbuf
   )
   {
      ::spiExchange(SPI::driver, n, txbuf, rxbuf);
   }

   inline static void
   send(
      size_t      n,
      const void* txbuf
   )
   {
      ::spiSend(SPI::driver, n, txbuf);
   }

   inline static void
   receive(
      size_t n,
      void*  rxbuf
   )
   {
      ::spiReceive(SPI::driver, n, rxbuf);
   }
};

class SPIDevice
{
public:
   virtual void
   select() = 0;

   virtual void
   deselect() = 0;

   virtual void
   acquireBus() = 0;

   virtual void
   releaseBus() = 0;

   virtual void
   ignore(
      size_t n
   ) = 0;

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

template <class _SPI, class _CS>
class SPIDevice_:
   public SPIDevice
{
public:
   using SPI = _SPI;
   using CS  = _CS;

   inline void
   select()
   {
      _cs.clear();
   }

   inline void
   deselect()
   {
      _cs.set();
   }

   inline void
   acquireBus()
   {
      ::spiAcquireBus(SPI::driver);
   }

   inline void
   releaseBus()
   {
      ::spiReleaseBus(SPI::driver);
   }

   inline void
   ignore(
      size_t n
   )
   {
      ::spiIgnore(SPI::driver, n);
   }

   inline void
   exchange(
      size_t      n,
      const void* txbuf,
      void*       rxbuf
   )
   {
      ::spiExchange(SPI::driver, n, txbuf, rxbuf);
   }

   inline void
   send(
      size_t      n,
      const void* txbuf
   )
   {
      ::spiSend(SPI::driver, n, txbuf);
   }

   inline void
   receive(
      size_t n,
      void*  rxbuf
   )
   {
      ::spiReceive(SPI::driver, n, rxbuf);
   }

private:
   CS _cs;
};

// --- Aliases -----------------------------------------------------------------

using SPI_1 = SPIDriverTraits<1>;
using SPI_2 = SPIDriverTraits<2>;
using SPI_3 = SPIDriverTraits<3>;

NAMESPACE_CORE_HW_END
