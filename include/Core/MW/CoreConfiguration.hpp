/* COPYRIGHT (c) 2016 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#pragma once

#include <Core/MW/namespace.hpp>
#include <Core/MW/CoreType.hpp>
#include <Core/MW/Array.hpp>
#include <initializer_list>
#include <type_traits>

/* Macros that allow us to keep the python code generator clean */
#define CORE_CONFIGURATION_BEGIN(__name__) class __name__: \
   public Core::MW::CoreConfiguration {
#define CORE_CONFIGURATION_FIELD(__name__, __type__, __size__) \
public: \
   Core::MW::CoreParameter<Core::MW::CoreType::__type__, __size__>__name__;
#define CORE_CONFIGURATION_MAP_BEGIN(__n__) \
private: \
   const Core::MW::Array<KeyValue, __n__>_map = {{
#define CORE_CONFIGURATION_MAP_ENTRY(__name__, __field__) { # __field__, Core::MW::pointer_to_helper<__name__, decltype(__name__::__field__), & __name__::__field__>::pointer},
#define CORE_CONFIGURATION_MAP_END() } \
   };
#define CORE_CONFIGURATION_END() \
private: \
   iterator begin() { \
      return _map.begin(); \
   } \
private: \
   iterator end() { \
      return _map.end(); \
   } \
   };

NAMESPACE_CORE_MW_BEGIN

struct CoreParameterBase {
   virtual CoreType
   getCoreType() const = 0;

   virtual std::size_t
   getSize() const = 0;

   virtual void
   set(
      const void* data
   ) = 0;

   virtual void
   set(
      std::size_t i,
      const void* data
   ) = 0;

   virtual void
   get(
      void* data
   ) const = 0;


   template <typename T>
   CoreParameterBase&
   operator=(
      const T& x
   );


   template <typename T>
   CoreParameterBase&
   operator=(
      const std::initializer_list<T>& x
   );

   CoreParameterBase&
   operator=(
      const CoreParameterBase& x
   );


   CoreParameterBase&
   operator=(
      const char* x
   );


   template <typename T>
   operator T() const;


   template <typename T, std::size_t S>
   operator Array<T, S>() const;

   operator const char*() const;
};

template <typename T>
CoreParameterBase&
CoreParameterBase::operator=(
   const T& x
)
{
   CORE_ASSERT(CoreTypeUtils::size(x) == getSize() && CoreTypeUtils::coreType(x) == getCoreType()); // make sure we are doing something meaningful...

   set(&x);

   return *this;
}

template <typename T>
CoreParameterBase&
CoreParameterBase::operator=(
   const std::initializer_list<T>& x
)
{
   CORE_ASSERT(x.size() == getSize() && CoreTypeTraitsHelperB<T>::types == getCoreType()); // make sure we are doing something meaningful...

   std::size_t i = 0;

   for (T v : x) {
      set(i, &v);
      i++;
   }

   return *this;
}

template <typename T>
CoreParameterBase::operator T() const
{
   T x;

   CORE_ASSERT(CoreTypeUtils::size(x) == getSize() && CoreTypeUtils::coreType(x) == getCoreType()); // make sure we are doing something meaningful...

   get(&x);

   return x;
}

template <typename T, std::size_t S>
CoreParameterBase::operator Array<T, S>() const {
   CORE_ASSERT(S == getSize() && CoreTypeTraitsHelperB<T>::types == getCoreType()); // make sure we are doing something meaningful...

   Array<T, S>tmp;

   get(tmp.begin());

   return tmp;
}

template <CoreType T, std::size_t S>
struct CoreParameter:
   CoreParameterBase {
   using Traits   = CoreTypeTraits<T, S>;
   using BaseType = typename Traits::BaseType;
   using Type     = typename Traits::Type;

   constexpr std::size_t
   getSize() const
   {
      return Traits::size;
   }

   constexpr
   CoreType
   getCoreType() const
   {
      return Traits::types;
   }

   CoreParameter()
   {}

   CoreParameter(
      const std::initializer_list<BaseType>& x
   )
   {
      CORE_ASSERT(x.size() == S);
      std::size_t i = 0;

      for (BaseType v : x) {
         storage[i++] = v;
      }
   }

   void
   set(
      const void* data
   )
   {
      const BaseType* tmp = static_cast<const BaseType*>(data); // array = pointer to...
      std::size_t i       = Traits::size;

      while (i--) {
         BaseType t = tmp[i];
         storage[i] = t;
      }
   }

   void
   set(
      std::size_t i,
      const void* data
   )
   {
      const BaseType* tmp = static_cast<const BaseType*>(data); // array = pointer to...

      storage[i] = *tmp;
   }

   void
   get(
      void* data
   ) const
   {
      BaseType* tmp = static_cast<BaseType*>(data);
      std::size_t i = Traits::size;

      while (i--) {
         tmp[i] = storage[i];
      }
   }

   BaseType&
   operator[](
      size_t i
   )
   {
      return const_cast<BaseType&>(storage[i]);
   }

   const BaseType&
   operator[](
      size_t i
   ) const
   {
      return const_cast<BaseType&>(storage[i]);
   }

   void
   copyStorageTo(
      Type& x
   )
   {
      std::size_t i = Traits::size;

      while (i--) {
         x[i] = storage[i];
      }
   }

   const Type*
   raw()
   {
      return storage;
   }

   operator Type() const {
      return storage;
   }

   CoreParameter<T, S>&
   operator=(
      Type& x
   )
   {
      std::size_t i = Traits::size;

      while (i--) {
         storage[i] = x[i];
      }

      return *this;
   }

   CoreParameter<T, S>&
   operator=(
      const CoreParameter<T, S>& x
   )
   {
      std::size_t i = Traits::size;

      while (i--) {
         storage[i] = x.storage[i];
      }

      return *this;
   }

   CoreParameter<T, S>&
   operator=(
      const std::initializer_list<BaseType>& x
   )
   {
      CORE_ASSERT(x.size() == Traits::size);

      std::size_t i = 0;

      for (BaseType v : x) {
         storage[i++] = v;
      }

      return *this;
   }

   static inline CoreParameter<T, S>&
   reference(
      CoreParameterBase& x
   )
   {
      return static_cast<CoreParameter<T, S>&>(x);
   }

private:
   Type storage;
};

template <std::size_t S>
struct CoreParameter<CoreType::CHAR, S> :
   CoreParameterBase {
   using Traits   = CoreTypeTraits<CoreType::CHAR, S>;
   using BaseType = typename Traits::BaseType;
   using Type     = typename Traits::Type;

   constexpr std::size_t
   getSize() const
   {
      return Traits::size;
   }

   constexpr
   CoreType
   getCoreType() const
   {
      return Traits::types;
   }

   CoreParameter()
   {}

   CoreParameter(
      BaseType& v
   )
   {
      storage = v;
   }

   void
   set(
      const void* data
   )
   {
      const BaseType* tmp = static_cast<const BaseType*>(data); // array = pointer to...
      std::size_t i       = Traits::size;

      while (i--) {
         BaseType t = tmp[i];
         storage[i] = t;
      }
   }

   void
   set(
      std::size_t i,
      const void* data
   )
   {
      const BaseType* tmp = static_cast<const BaseType*>(data); // array = pointer to...

      storage[i] = *tmp;
   }

   void
   get(
      void* data
   ) const
   {
      BaseType* tmp = static_cast<BaseType*>(data);
      std::size_t i = Traits::size;

      while (i--) {
         tmp[i] = storage[i];
      }
   }

   BaseType&
   operator[](
      size_t i
   )
   {
      return const_cast<BaseType&>(storage[i]);
   }

   const BaseType&
   operator[](
      size_t i
   ) const
   {
      return const_cast<BaseType&>(storage[i]);
   }

   void
   copyStorageTo(
      Type& x
   )
   {
      std::size_t i = Traits::size;

      while (i--) {
         x[i] = storage[i];
      }
   }

   const Type*
   raw()
   {
      return storage;
   }

   operator Type() const {
      return storage;
   }

   CoreParameter<CoreType::CHAR, S>&
   operator=(
      Type& x
   )
   {
      std::size_t i = Traits::size;

      while (i--) {
         storage[i] = x[i];
      }

      return *this;
   }

   CoreParameter<CoreType::CHAR, S>&
   operator=(
      const CoreParameter<CoreType::CHAR, S>& x
   )
   {
      std::size_t i = Traits::size;

      while (i--) {
         storage[i] = x.storage[i];
      }

      return *this;
   }

   static inline CoreParameter<CoreType::CHAR, S>&
   reference(
      CoreParameterBase& x
   )
   {
      return static_cast<CoreParameter<CoreType::CHAR, S>&>(x);
   }

   CoreParameter<CoreType::CHAR, S>
   operator=(
      const char* x
   )
   {
      CORE_ASSERT(strlen(x) < Traits::size);

      std::size_t i = 0;
      std::size_t l = strlen(x) + 1;

      while (i < l) {
         storage[i] = x[i];
         i++;
      }

      while (i < Traits::size) {
         storage[i] = 0;
         i++;
      }

      return *this;
   } // =

   operator const char*() const
   {
      return storage.begin();
   }

private:
   Type storage;
};

template <CoreType T>
struct CoreParameter<T, 1> :
   CoreParameterBase {
   using Traits   = CoreTypeTraits<T, 1>;
   using BaseType = typename Traits::BaseType;
   using Type     = typename Traits::Type;

   constexpr std::size_t
   getSize() const
   {
      return Traits::size;
   }

   constexpr
   CoreType
   getCoreType() const
   {
      return Traits::types;
   }

   CoreParameter()
   {}

   CoreParameter(
      BaseType& v
   )
   {
      storage = v;
   }

   void
   set(
      const void* data
   )
   {
      const BaseType* tmp = static_cast<const BaseType*>(data);

      storage = *tmp;
   }

   void
   set(
      std::size_t i,
      const void* data
   )
   {
      const BaseType* tmp = static_cast<const BaseType*>(data); // array = pointer to...

      storage = *tmp;
   }

   void
   get(
      void* data
   ) const
   {
      BaseType* tmp = static_cast<BaseType*>(data);

      *tmp = storage;
   }

   operator Type() const
   {
      return storage;
   }

   CoreParameter<T, 1>&
   operator=(
      const Type& x
   )
   {
      storage = x;
      return *this;
   }

   CoreParameter<T, 1>&
   operator=(
      const CoreParameter<T, 1>& x
   )
   {
      storage = x.storage;
      return *this;
   }

   static inline CoreParameter<T, 1>&
   reference(
      CoreParameterBase& x
   )
   {
      return static_cast<CoreParameter<T, 1>&>(x);
   }

private:
   Type storage;
};

template <CoreType T>
struct CoreParameter<T, 0> :
   CoreParameterBase {
   using Traits   = CoreTypeTraits<T, 0>;
   using BaseType = typename Traits::BaseType;
   using Type     = typename Traits::Type;

   constexpr std::size_t
   getSize() const
   {
      return Traits::size;
   }

   constexpr
   CoreType
   getCoreType() const
   {
      return Traits::types;
   }

   void
   set(
      const void* data
   )
   {
      CORE_ASSERT(!"VOID");
   }

   void
   set(
      std::size_t i,
      const void* data
   )
   {
      CORE_ASSERT(!"VOID");
   }

   void
   get(
      void* data
   ) const
   {
      CORE_ASSERT(!"VOID");
   }
};

class CoreConfiguration
{
public:
   typedef const char* Key;

   struct KeyValue {
      Key key;
      Core::MW::CoreParameterBase Core::MW::CoreConfiguration::* ptr;
   };

public:
   CoreParameterBase&
   operator[](
      Key key
   );


   inline bool
   keyMatch(
      Key a,
      Key b
   )
   {
      return strcmp(a, b) == 0;
   }

protected:
   typedef const KeyValue* iterator;

   virtual iterator
   begin() = 0;

   virtual iterator
   end() = 0;
};

template <typename C, typename T, T C::* m>
struct pointer_to_helper {
   static constexpr Core::MW::CoreParameterBase Core::MW::CoreConfiguration::* pointer
      = static_cast<Core::MW::CoreParameterBase Core::MW::CoreConfiguration::*>(reinterpret_cast<Core::MW::CoreParameterBase C::*>(m));
};


#define p__(__class__, __member__) { # __member__, Core::MW::pointer_to_helper<__class__, decltype(__class__::__member__), & __class__::__member__>::pointer}

NAMESPACE_CORE_MW_END
