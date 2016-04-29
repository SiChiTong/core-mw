/* COPYRIGHT (c) 2016 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#pragma once

#include <Core/MW/namespace.hpp>
#include <Core/MW/Array.hpp>
#include <Core/MW/Time.hpp>

NAMESPACE_CORE_MW_BEGIN

enum class CoreType {
   VOID, CHAR, INT8, UINT8, INT16, UINT16, INT32, UINT32, INT64, UINT64, FLOAT32, FLOAT64, TIMESTAMP
};

template <CoreType T>
struct CoreTypeTraitsHelperF;

template <>
struct CoreTypeTraitsHelperF<CoreType::VOID>{
   typedef void Type;
   static const std::size_t sizeOfType = 0;
};

template <>
struct CoreTypeTraitsHelperF<CoreType::CHAR>{
   typedef char Type;
   static const std::size_t sizeOfType = sizeof(Type);
};

template <>
struct CoreTypeTraitsHelperF<CoreType::INT8>{
   typedef int8_t Type;
   static const std::size_t sizeOfType = sizeof(Type);
};

template <>
struct CoreTypeTraitsHelperF<CoreType::UINT8>{
   typedef uint8_t Type;
   static const std::size_t sizeOfType = sizeof(Type);
};

template <>
struct CoreTypeTraitsHelperF<CoreType::INT16>{
   typedef int16_t Type;
   static const std::size_t sizeOfType = sizeof(Type);
};

template <>
struct CoreTypeTraitsHelperF<CoreType::UINT16>{
   typedef uint16_t Type;
   static const std::size_t sizeOfType = sizeof(Type);
};

template <>
struct CoreTypeTraitsHelperF<CoreType::INT32>{
   typedef int32_t Type;
   static const std::size_t sizeOfType = sizeof(Type);
};

template <>
struct CoreTypeTraitsHelperF<CoreType::UINT32>{
   typedef uint32_t Type;
   static const std::size_t sizeOfType = sizeof(Type);
};

template <>
struct CoreTypeTraitsHelperF<CoreType::INT64>{
   typedef int64_t Type;
   static const std::size_t sizeOfType = sizeof(Type);
};

template <>
struct CoreTypeTraitsHelperF<CoreType::UINT64>{
   typedef uint64_t Type;
   static const std::size_t sizeOfType = sizeof(Type);
};

template <>
struct CoreTypeTraitsHelperF<CoreType::TIMESTAMP>{
   typedef struct {
      uint32_t sec;
      uint32_t nsec;
   } Type;
//		typedef Core::MW::Time Type;
   static const std::size_t sizeOfType = sizeof(Type);
};

template <>
struct CoreTypeTraitsHelperF<CoreType::FLOAT32>{
   typedef float Type;
   static const std::size_t sizeOfType = sizeof(Type);
};

template <>
struct CoreTypeTraitsHelperF<CoreType::FLOAT64>{
   typedef double Type;
   static const std::size_t sizeOfType = sizeof(Type);
};

template <typename T>
struct CoreTypeTraitsHelperB;

template <>
struct CoreTypeTraitsHelperB<char>{
   static const CoreType types = CoreType::CHAR;
};

template <>
struct CoreTypeTraitsHelperB<int8_t>{
   static const CoreType types = CoreType::INT8;
};

template <>
struct CoreTypeTraitsHelperB<uint8_t>{
   static const CoreType types = CoreType::UINT8;
};

template <>
struct CoreTypeTraitsHelperB<int16_t>{
   static const CoreType types = CoreType::INT16;
};

template <>
struct CoreTypeTraitsHelperB<uint16_t>{
   static const CoreType types = CoreType::UINT16;
};

template <>
struct CoreTypeTraitsHelperB<int>{
   static const CoreType types = CoreType::INT32;
};

template <>
struct CoreTypeTraitsHelperB<int32_t>{
   static const CoreType types = CoreType::INT32;
};

template <>
struct CoreTypeTraitsHelperB<uint32_t>{
   static const CoreType types = CoreType::UINT32;
};

template <>
struct CoreTypeTraitsHelperB<int64_t>{
   static const CoreType types = CoreType::INT64;
};

template <>
struct CoreTypeTraitsHelperB<uint64_t>{
   static const CoreType types = CoreType::UINT64;
};

template <>
struct CoreTypeTraitsHelperB<float>{
   static const CoreType types = CoreType::FLOAT32;
};

template <>
struct CoreTypeTraitsHelperB<double>{
   static const CoreType types = CoreType::FLOAT64;
};

template <>
struct CoreTypeTraitsHelperB<CoreTypeTraitsHelperF<CoreType::TIMESTAMP> >{
   static const CoreType types = CoreType::TIMESTAMP;
};

template <CoreType T, std::size_t S>
struct CoreTypeTraits {
   static const CoreType    types       = T;
   static const std::size_t sizeOfTypes = CoreTypeTraitsHelperF<T>::sizeOfType;
   static const std::size_t size        = S;

   using BaseType = typename CoreTypeTraitsHelperF<T>::Type;
   using Type     = Array<BaseType, S>;
};

template <CoreType T>
struct CoreTypeTraits<T, 1>{
   static const CoreType    types       = T;
   static const std::size_t sizeOfTypes = CoreTypeTraitsHelperF<T>::sizeOfType;
   static const std::size_t size        = 1;

   using BaseType = typename CoreTypeTraitsHelperF<T>::Type;
   using Type     = BaseType;
};

template <CoreType T>
struct CoreTypeTraits<T, 0>{
   static const CoreType    types       = CoreType::VOID;
   static const std::size_t sizeOfTypes = CoreTypeTraitsHelperF<T>::sizeOfType;
   static const std::size_t size        = 0;

   using BaseType = typename CoreTypeTraitsHelperF<T>::Type;
   using Type     = BaseType;
};

namespace CoreTypeUtils {
   template <typename T, std::size_t S>
   inline std::size_t
   size(
      const T(&v)[S]
   )
   {
      return S;
   }

   template <typename T, std::size_t S>
   inline std::size_t
   size(
      const Array<T, S> (&v)
   )
   {
      return S;
   }

   template <typename T>
   inline std::size_t
   size(
      const T(&v)
   )
   {
      return 1;
   }

   template <typename T, std::size_t S>
   inline CoreType
   coreType(
      const T(&v)[S]
   )
   {
      return CoreTypeTraitsHelperB<T>::types;
   }

   template <typename T, std::size_t S>
   inline CoreType
   coreType(
      const Array<T, S> (&v)
   )
   {
      return CoreTypeTraitsHelperB<T>::types;
   }

   template <typename T>
   inline CoreType
   coreType(
      const T(&v)
   )
   {
      return CoreTypeTraitsHelperB<T>::types;
   }
}

NAMESPACE_CORE_MW_END

#define CORETYPE_UDL_INT(__s__, __t__) constexpr Core::MW::CoreTypeTraits<Core::MW::CoreType::__t__, 1>::Type operator"" _ ## __s__(unsigned long long x) { \
      return static_cast<Core::MW::CoreTypeTraits<Core::MW::CoreType::__t__, 1>::Type>(x); \
}
#define CORETYPE_UDL_FLOAT(__s__, __t__) constexpr Core::MW::CoreTypeTraits<Core::MW::CoreType::__t__, 1>::Type operator"" _ ## __s__(long double x) { \
      return static_cast<Core::MW::CoreTypeTraits<Core::MW::CoreType::__t__, 1>::Type>(x); \
}

CORETYPE_UDL_INT(
   s8,
   INT8
)
CORETYPE_UDL_INT(u8, UINT8)
CORETYPE_UDL_INT(s16, INT16)
CORETYPE_UDL_INT(u16, UINT16)
CORETYPE_UDL_INT(s32, INT32)
CORETYPE_UDL_INT(u32, UINT32)
CORETYPE_UDL_INT(s64, INT64)
CORETYPE_UDL_INT(u64, UINT64)
CORETYPE_UDL_FLOAT(f32, FLOAT32)
CORETYPE_UDL_FLOAT(f64, FLOAT64)
