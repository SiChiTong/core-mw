/* COPYRIGHT (c) 2016 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#pragma once

#include <core/namespace.hpp>
#include <core/common.hpp>
#include <memory>

NAMESPACE_CORE_BEGIN

template <typename T, std::size_t S>
struct ArrayTraits {
   using Type = T[S];

   static constexpr T&
   ref(
      const Type& t,
      std::size_t n
   )
   {
      return const_cast<T&>(t[n]);
   }
};

template <typename T>
struct ArrayTraits<T, 0>{
   using Type = struct {};

   static constexpr T&
   ref(
      const Type&,
      std::size_t
   )
   {
      return *static_cast<T*>(nullptr);
   }
};

template <typename T, std::size_t S>
struct Array {
   using value_type      = T;
   using pointer         = value_type *;
   using const_pointer   = const value_type *;
   using reference       = value_type &;
   using const_reference = const value_type &;
   using iterator        = value_type *;
   using const_iterator  = const value_type *;
   using size_type       = std::size_t;
   using difference_type = std::ptrdiff_t;

   using Traits = ArrayTraits<T, S>;
   typename Traits::Type _data;

   // Iterators.
   iterator
   begin()
   {
      return iterator(data());
   }

   const_iterator
   begin() const
   {
      return const_iterator(data());
   }

   iterator
   end()
   {
      return iterator(data() + S);
   }

   const_iterator
   end() const
   {
      return const_iterator(data() + S);
   }

   const_iterator
   cbegin() const
   {
      return const_iterator(data());
   }

   const_iterator
   cend() const
   {
      return const_iterator(data() + S);
   }

   // Capacity.
   constexpr size_type
   size() const
   {
      return S;
   }

   constexpr size_type
   max_size() const
   {
      return S;
   }

   constexpr bool
   empty() const
   {
      return size() == 0;
   }

   // Element access.
   reference
   operator[](
      size_type __n
   )
   {
      return Traits::ref(_data, __n);
   }

   constexpr const_reference
   operator[](
      size_type __n
   ) const
   {
      return Traits::ref(_data, __n);
   }

   reference
   at(
      size_type __n
   )
   {
      CORE_ASSERT(__n < S);

      return Traits::ref(_data, __n);
   }

   constexpr const_reference
   at(
      size_type __n
   ) const
   {
      CORE_ASSERT(__n < S);

      return Traits::ref(_data, __n);
   }

   reference
   front()
   {
      return *begin();
   }

   constexpr const_reference
   front() const
   {
      return Traits::ref(_data, 0);
   }

   reference
   back()
   {
      return S ? *(end() - 1) : *end();
   }

   constexpr const_reference
   back() const
   {
      return S ? Traits::ref(_data, S - 1) : Traits::ref(_data, 0);
   }

   pointer
   data()
   {
      return std::__addressof(Traits::ref(_data, 0));
   }

   const_pointer
   data() const
   {
      return std::__addressof(Traits::ref(_data, 0));
   }

   operator pointer() {
      return data();
   }

   operator const_pointer() {
      return data();
   }

   void
   copyTo(
      typename Traits::Type to
   ) const
   {
      for (std::size_t i = 0; i < S; i++) {
         to[i] = _data[i];
      }
   }
};

NAMESPACE_CORE_END
