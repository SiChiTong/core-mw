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

/*! \brief Time intervals
 *
 */
class Time
{
public:
   using Type = uint32_t;

   static const Type MIN_US = std::numeric_limits<Type>::min();
   static const Type MAX_US;
   static const Type MIN_MS = MIN_US / 1000;
   static const Type MAX_MS = MAX_US / 1000;
   static const Type MIN_S  = MIN_US / 1000000;
   static const Type MAX_S  = MAX_US / 1000000;

public:
   /*! \brief Get the time in system ticks
    *
    */
   uint32_t
   ticks() const;


   /*! \brief Get the time in us
    *
    */
   Type
   us() const;


   /*! \brief Get the time in whole ms
    *
    */
   Type
   ms() const;


   /*! \brief Get the time in whole s
    *
    */
   Type
   s() const;


   /*! \brief Get the time in us
    *
    */
   float
   to_us() const;


   /*! \brief Get the time in ms
    *
    */
   float
   to_ms() const;


   /*! \brief Get the time in s
    *
    */
   float
   to_s() const;


   /*! \brief Get the frequency corresponding to the interval
    *
    */
   float
   hz() const;


   /*! \brief Assignement
    *
    */
   Time&
   operator=(
      const Time& rhs //!< [in] interval to be assigned
   );


   /*! \brief Increment the time
    *
    */
   Time&
   operator+=(
      const Time& rhs //!< [in] interval to be added
   );


   /*! \brief Decrement the time
    *
    */
   Time&
   operator-=(
      const Time& rhs //!< [in] interval to be subtracted
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
   /*! \brief Returns a time interval
    *
    */
   static Time
   us(
      const Type microseconds //!< [in] inteval in us
   );


   /*! \brief Returns a time interval
    *
    */
   static Time
   ms(
      const Type milliseconds //!< [in] inteval in ms
   );


   /*! \brief Returns a time interval
    *
    */
   static Time
   s(
      const Type seconds //!< [in] inteval in s
   );


   /*! \brief Returns a time interval
    *
    */
   static Time
   hz(
      const float hertz //!< [in] frequency in hz
   );


   /*! \brief Returns the actual time
    *
    */
   static Time
   now();


public:
   static const Time IMMEDIATE; //!< A null time interval
   static const Time INFINITE; //!< An infinite time interval
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
