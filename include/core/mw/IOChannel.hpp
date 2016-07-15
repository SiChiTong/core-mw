/* COPYRIGHT (c) 2016 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#pragma once

#include <core/mw/namespace.hpp>
#include <core/common.hpp>
#include <chprintf.h>
#include <hal_channels.h>
#include <stdarg.h>

NAMESPACE_CORE_MW_BEGIN

class IOChannel
{
public:
   using Channel = BaseChannel *;

   enum class Defaultcore::os::Timeout {
      INFINITE, IMMEDIATE
   };

   virtual std::size_t
   put(
      const uint8_t& x,
      core::mw::core::os::Time timeout
   ) = 0;

   virtual std::size_t
   get(
      uint8_t&       x,
      core::mw::core::os::Time timeout
   ) = 0;

   virtual std::size_t
   write(
      const uint8_t* buffer,
      std::size_t    size,
      core::mw::core::os::Time timeout
   ) = 0;

   virtual std::size_t
   read(
      uint8_t*       buffer,
      std::size_t    size,
      core::mw::core::os::Time timeout
   ) = 0;

   virtual int
   printf(
      const char* fmt,
      ...
   ) = 0;

   virtual constexpr
   Channel
   rawChannel() = 0;
};

template <class _SD>
struct SDChannelTraits {
   static constexpr auto channel = (IOChannel::Channel)_SD::driver;
};


template <class _CHANNEL, IOChannel::Defaultcore::os::Timeout _DEFAULT_TIMEOUT>
class IOChannel_:
   public IOChannel
{
public:
   using Channel = _CHANNEL;
   static constexpr core::mw::core::os::Time
   DEFAULT_TIMEOUT()
   {
      return _DEFAULT_TIMEOUT == IOChannel::Defaultcore::os::Timeout::INFINITE ? core::mw::core::os::Time::INFINITE : core::mw::core::os::Time::IMMEDIATE;
   }

   inline std::size_t
   put(
      const uint8_t& x,
      core::mw::core::os::Time timeout = DEFAULT_TIMEOUT()
   )
   {
      return (chnPutcore::os::Timeout(Channel::channel, x, timeout.raw) == Q_OK) ? 1 : 0;
   }

   inline std::size_t
   get(
      uint8_t&       x,
      core::mw::core::os::Time timeout = DEFAULT_TIMEOUT()
   )
   {
      msg_t tmp = chnGetcore::os::Timeout(Channel::channel, timeout.raw);

      if ((tmp & 0xFFFFFF00) == 0) {
         x = (uint8_t)(tmp & 0x000000FF);
         return 1;
      } else {
         return 0;
      }
   }

   inline std::size_t
   write(
      const uint8_t* buffer,
      std::size_t    size,
      core::mw::core::os::Time timeout = DEFAULT_TIMEOUT()
   )
   {
      return chnWritecore::os::Timeout(Channel::channel, buffer, size, timeout.raw);
   }

   inline std::size_t
   read(
      uint8_t*       buffer,
      std::size_t    size,
      core::mw::core::os::Time timeout = DEFAULT_TIMEOUT()
   )
   {
      return chnReadcore::os::Timeout(Channel::channel, buffer, size, timeout.raw);
   }

   inline int
   printf(
      const char* fmt,
      ...
   )
   {
      va_list ap;
      int     formatted_bytes;

      va_start(ap, fmt);
      formatted_bytes = chvprintf(reinterpret_cast<BaseSequentialStream*>(rawChannel()), fmt, ap);
      va_end(ap);

      return formatted_bytes;
   }

   inline constexpr IOChannel::Channel
   rawChannel()
   {
      return Channel::channel;
   }
};

#if 0
class IOStream
{
public:
   using Stream = BaseSequentialStream *;
   virtual int
   printf(
      const char* fmt,
      ...
   ) = 0;

   virtual constexpr
   Stream
   rawStream() = 0;
};

template <class _SD>
struct SDStreamTraits {
   static constexpr auto stream = (IOStream::Stream)_SD::driver;
};

template <class _STREAM>
// TODO:: Must be placed somewhere else, it's a port-specific piece of code!
class IOStream_:
   public core::mw::IOStream
{
public:
   using Stream = _STREAM;

   inline int
   printf(
      const char* fmt,
      ...
   )
   {
      va_list ap;
      int     formatted_bytes;

      va_start(ap, fmt);
      formatted_bytes = chvprintf(rawStream(), fmt, ap);
      va_end(ap);

      return formatted_bytes;
   }

   inline int
   read(
      char*          buffer,
      std::size_t    size,
      core::mw::core::os::Time timeout = core::mw::core::os::Time::INFINITE
   )
   {
      return 0;
   }

   inline constexpr IOStream::Stream
   rawStream()
   {
      return Stream::stream;
   }
};
#endif // if 0

NAMESPACE_CORE_MW_END
