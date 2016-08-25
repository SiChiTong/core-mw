/* COPYRIGHT (c) 2016 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#pragma once

#include <core/os/namespace.hpp>
#include <core/common.hpp>
#include <limits>

NAMESPACE_CORE_OS_BEGIN


class Time
{
   // TODO: Fix infinite algebra

public:
   using Type = uint32_t;

   static const Type MIN_US = std::numeric_limits<Type>::min();
   static const Type MAX_US = std::numeric_limits<Type>::max() - 1;
   static const Type MIN_MS = MIN_US / 1000;
   static const Type MAX_MS = MAX_US / 1000;
   static const Type MIN_S  = MIN_US / 1000000;
   static const Type MAX_S  = MAX_US / 1000000;

public:
   uint32_t
   ticks() const;

   Type
   us() const;

   Type
   ms() const;

   Type
   s() const;

   float
   to_us() const;

   float
   to_ms() const;

   float
   to_s() const;

   float
   hz() const;

   Time&
   operator=(
      const Time& rhs
   );

   Time&
   operator+=(
      const Time& rhs
   );

   Time&
   operator-=(
      const Time& rhs
   );


public:
   Time();
   template <typename T>
   Time(
      T microseconds
   );
   explicit
   Time(
      float seconds
   );
   Time(
      const Time& rhs
   );

public:
   static Time
   us(
      const Type microseconds
   );

   static Time
   ms(
      const Type milliseconds
   );

   static Time
   s(
      const Type seconds
   );

   static Time
   hz(
      const float hertz
   );

   static Time
   now();


public:
   static const Time IMMEDIATE;
   static const Time INFINITE;
   static const Time INFINITEN;

public:
   Type raw;
};

bool
operator==(
   const Time& lhs,
   const Time& rhs
);

bool
operator!=(
   const Time& lhs,
   const Time& rhs
);

bool
operator>(
   const Time& lhs,
   const Time& rhs
);

bool
operator>=(
   const Time& lhs,
   const Time& rhs
);

bool
operator<(
   const Time& lhs,
   const Time& rhs
);

bool
operator<=(
   const Time& lhs,
   const Time& rhs
);

const Time
operator+(
   const Time& lhs,
   const Time& rhs
);

const Time
operator-(
   const Time& lhs,
   const Time& rhs
);


inline
Time::Type
Time::us() const
{
   return raw;
}

inline
Time::Type
Time::ms() const
{
   return raw / 1000;
}

inline
Time::Type
Time::s() const
{
   return raw / 1000000;
}

inline
float
Time::to_us() const
{
   return raw;
}

inline
float
Time::to_ms() const
{
   return raw / 1000.0f;
}

inline
float
Time::to_s() const
{
   return raw / 1000000.0f;
}

inline
float
Time::hz() const
{
   return 1000000.0f / raw;
}

inline
Time&
Time::operator=(
   const Time& rhs
)
{
   raw = rhs.raw;
   return *this;
}

inline
Time&
Time::operator+=(
   const Time& rhs
)
{
   raw += rhs.raw;
   return *this;
}

inline
Time&
Time::operator-=(
   const Time& rhs
)
{
   raw += rhs.raw;
   return *this;
}

inline
Time::Time() : raw() {}


template <typename T>
inline
Time::Time(
   T microseconds
)
   :
   raw(static_cast<Type>(microseconds))
{}


inline
Time::Time(
   float seconds
)
   :
   raw(static_cast<Type>((seconds + 0.5) / 1000000.0))
{}


inline
Time::Time(
   const Time& rhs
) : raw(rhs.raw) {}


inline
Time
Time::us(
   const Type microseconds
)
{
   return Time(microseconds);
}

inline
Time
Time::ms(
   const Type milliseconds
)
{
   return Time(milliseconds * 1000);
}

inline
Time
Time::s(
   const Type seconds
)
{
   return Time(seconds * 1000000);
}

inline
bool
operator==(
   const Time& lhs,
   const Time& rhs
)
{
   return lhs.raw == rhs.raw;
}

inline
bool
operator!=(
   const Time& lhs,
   const Time& rhs
)
{
   return lhs.raw != rhs.raw;
}

inline
bool
operator>(
   const Time& lhs,
   const Time& rhs
)
{
   return lhs.raw > rhs.raw;
}

inline
bool
operator>=(
   const Time& lhs,
   const Time& rhs
)
{
   return lhs.raw >= rhs.raw;
}

inline
bool
operator<(
   const Time& lhs,
   const Time& rhs
)
{
   return lhs.raw < rhs.raw;
}

inline
bool
operator<=(
   const Time& lhs,
   const Time& rhs
)
{
   return lhs.raw <= rhs.raw;
}

inline
const Time
operator+(
   const Time& lhs,
   const Time& rhs
)
{
   return Time(lhs.raw + rhs.raw);
}

inline
const Time
operator-(
   const Time& lhs,
   const Time& rhs
)
{
   return Time(lhs.raw - rhs.raw);
}

NAMESPACE_CORE_OS_END
