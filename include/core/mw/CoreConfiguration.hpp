/* COPYRIGHT (c) 2016 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#pragma once

#include <core/mw/namespace.hpp>
#include <core/mw/CoreType.hpp>
#include <core/Array.hpp>
#include <core/mw/StaticList.hpp>
#include <core/mw/NamingTraits.hpp>

#include <initializer_list>
#include <type_traits>

/* Macros that allow us to keep the python code generator clean */
#define CORE_CONFIGURATION_BEGIN(__name__) \
   class __name__: \
      public core::mw::CoreConfigurationBase { \
public: \
      using Type = __name__;
#define CORE_CONFIGURATION_BEGIN_FULL(__name__, __l__, __s__) \
   class __name__: \
      public core::mw::CoreConfigurationBase { \
public: \
      using Type = __name__; \
public: \
      static const core::mw::CoreConfigurationBase::Signature SIGNATURE = __s__; \
      core::mw::CoreConfigurationBase::Signature getConfigurationSignature() const { \
         return __s__; \
      } \
      static const std::size_t LENGTH = __l__;
#define CORE_CONFIGURATION_FIELD(__name__, __type__, __size__) \
public: \
   core::mw::CoreTypeTraits<core::mw::CoreType::__type__, __size__>::Type __name__;
#define CORE_CONFIGURATION_SIGNATURE(__s__) \
public: \
   static const core::mw::CoreConfigurationBase::Signature SIGNATURE = __s__; \
   core::mw::CoreConfigurationBase::Signature getConfigurationSignature() const { \
      return __s__; \
   }
#define CORE_CONFIGURATION_LENGTH(__l__) \
   static const std::size_t LENGTH = __l__;
#define CORE_CONFIGURATION_END() \
   };

#define CORE_CONFIGURATION_MAP_BEGIN(__name__) \
   template <> \
   const core::Array<core::mw::CoreConfigurationMap::FieldMetadata, core::mw::CoreConfigurationMap_<__name__>::LENGTH>core::mw::CoreConfigurationMap_<__name__>::_map = {{
#define CORE_CONFIGURATION_MAP_ENTRY(__name__, __field__, __type__, __size__) \
   { # __field__, offsetof(__name__, __field__), core::mw::CoreType::__type__, __size__},
#define CORE_CONFIGURATION_MAP_END() \
   } \
   };

NAMESPACE_CORE_MW_BEGIN

struct CoreConfigurationBase {
   using Signature = uint32_t;
};

struct CoreConfigurationSignature {
   virtual CoreConfigurationBase::Signature
   getConfigurationSignature() const = 0;
};


struct CoreConfigurationMap {
   using Key = const char*;

   struct FieldMetadata {
      Key key;
      std::ptrdiff_t offset;
      core::mw::CoreType type;
      std::size_t size;
   };

   virtual const FieldMetadata&
   getField(
      std::size_t i
   ) const = 0;

   virtual const std::size_t
   getSize() const = 0;


   const FieldMetadata
   getField(
      CoreConfigurationMap::Key key
   ) const
   {
      // Returning a copy of the field... Not a reference (that would be to a local)
      for (FieldMetadata f : * this) {
         if (keyMatch(key, f.key)) {
            return f;
         }
      }
   }

   const std::size_t
   getFieldIndex(
      CoreConfigurationMap::Key key
   ) const
   {
      std::size_t i = 0;

      for (FieldMetadata f : * this) {
         if (keyMatch(key, f.key)) {
            return i;
         }

         i++;
      }

      return i; // this will be gt the last index. It will fire an assert in getField
   }

   using iterator = const FieldMetadata *;

   virtual iterator
   begin() const = 0;

   virtual iterator
   end() const = 0;


protected:
   inline bool
   keyMatch(
      CoreConfigurationMap::Key a,
      CoreConfigurationMap::Key b
   ) const
   {
      return strcmp(a, b) == 0;
   }
}

CORE_PACKED;

template <class C>
struct CoreConfigurationMap_:
   public C,
   public core::mw::CoreConfigurationMap {
   using Type = C;
   static const std::size_t LENGTH = Type::LENGTH;
   const FieldMetadata&
   getField(
      std::size_t i
   ) const
   {
      return _map.at(i);
   }

   const std::size_t
   getSize() const
   {
      return _map.size();
   }

   iterator
   begin() const
   {
      return _map.begin();
   }

   iterator
   end() const
   {
      return _map.end();
   }

private:
   static const core::Array<FieldMetadata, LENGTH>_map;
}

CORE_PACKED;

struct CoreConfigurationStatic_ {
   // SET
   static void
   set(
      CoreConfigurationBase*                     obj,
      const CoreConfigurationMap::FieldMetadata& field,
      const void*                                x
   )
   {
      std::size_t s1        = field.size;
      core::mw::CoreType t1 = field.type;

      std::size_t len = s1 * core::mw::CoreTypeUtils::coreTypeSize(t1);

      const uint8_t* src = reinterpret_cast<const uint8_t*>(x);
      uint8_t* dst       = reinterpret_cast<uint8_t*>(obj + field.offset);

      while (len--) {
         * dst++ = *src++;
      }
   }

   template <typename T>
   static void
   set(
      CoreConfigurationBase*                     obj,
      const CoreConfigurationMap::FieldMetadata& field,
      const T&                                   x
   )
   {
      std::size_t s1        = field.size;
      std::size_t s2        = core::mw::CoreTypeUtils::size(x);
      core::mw::CoreType t1 = field.type;
      core::mw::CoreType t2 = core::mw::CoreTypeUtils::coreType(x);

      CORE_ASSERT(s1 == s2 && t1 == t2);   // make sure we are doing something meaningful...

      const T* src = reinterpret_cast<const T*>(&x);
      T* dst       = reinterpret_cast<T*>(obj + field.offset);

      while (s1--) {
         * dst++ = *src++;
      }
   }

   template <typename T>
   static void
   set(
      CoreConfigurationBase*                     obj,
      const CoreConfigurationMap::FieldMetadata& field,
      const std::initializer_list<T>&            x
   )
   {
      std::size_t s1        = field.size;
      std::size_t s2        = x.size();
      core::mw::CoreType t1 = field.type;
      core::mw::CoreType t2 = core::mw::CoreTypeTraitsHelperB<T>::types;

      CORE_ASSERT(s1 == s2 && t1 == t2);   // make sure we are doing something meaningful...

      T* dst = reinterpret_cast<T*>(obj + field.offset);

      for (T tmp : x) {
         * dst++ = tmp;
      }
   }

   static void
   set(
      CoreConfigurationBase*                     obj,
      const CoreConfigurationMap::FieldMetadata& field,
      const char*                                x
   )
   {
      std::size_t s1 = field.size;
      std::size_t s2 = strlen(x);

      core::mw::CoreType t1 = field.type;
      core::mw::CoreType t2 = core::mw::CoreType::CHAR;

      CORE_ASSERT(s1 >= s2 && t1 == t2);   // make sure we are doing something meaningful...

      const char* src = reinterpret_cast<const char*>(x);
      char* dst       = reinterpret_cast<char*>(obj + field.offset);

      std::size_t i = 0;

      while (i < s2) {
         // copy
         * dst++ = *src++;
         i++;
      }

      while (i < s1) {
         // pad
         * dst = 0;
         i++;
      }
   } // set

   //GET
   static void
   get(
      const CoreConfigurationBase*               obj,
      const CoreConfigurationMap::FieldMetadata& field,
      void*                                      x
   )
   {
      std::size_t s1        = field.size;
      core::mw::CoreType t1 = field.type;

      std::size_t len = s1 * core::mw::CoreTypeUtils::coreTypeSize(t1);

      const uint8_t* src = reinterpret_cast<const uint8_t*>(obj + field.offset);
      uint8_t* dst       = reinterpret_cast<uint8_t*>(x);

      while (len--) {
         * dst++ = *src++;
      }
   }

   template <typename T>
   static void
   get(
      const CoreConfigurationBase*               obj,
      const CoreConfigurationMap::FieldMetadata& field,
      T&                                         x
   )
   {
      std::size_t s1        = field.size;
      std::size_t s2        = core::mw::CoreTypeUtils::size(x);
      core::mw::CoreType t1 = field.type;
      core::mw::CoreType t2 = core::mw::CoreTypeUtils::coreType(x);

      CORE_ASSERT(s1 == s2 && t1 == t2);     // make sure we are doing something meaningful...

      const T* src = reinterpret_cast<const T*>(obj + field.offset);
      T* dst       = reinterpret_cast<T*>(&x);

      while (s1--) {
         * dst++ = *src++;
      }
   }

   static void
   get(
      const CoreConfigurationBase*               obj,
      const CoreConfigurationMap::FieldMetadata& field,
      char*                                      x
   )
   {
      std::size_t s1 = field.size;
      std::size_t s2 = strlen(x);

      core::mw::CoreType t1 = field.type;
      core::mw::CoreType t2 = core::mw::CoreType::CHAR;

      CORE_ASSERT(s1 >= s2 && t1 == t2);     // make sure we are doing something meaningful...

      const char* src = reinterpret_cast<const char*>(obj + field.offset);
      char* dst       = reinterpret_cast<char*>(x);

      std::size_t i = 0;

      while (i < s2) {
         // copy
         * dst++ = *src++;
         i++;
      }

      while (i < s1) {
         // pad
         * dst = 0;
         i++;
      }
   } // get
}

CORE_PACKED;

struct CoreConfiguration {
   virtual void
   set(
      CoreConfigurationMap::Key key,
      const void*               x
   ) = 0;

   virtual void
   set(
      CoreConfigurationMap::Key key,
      const char*               x
   ) = 0;

   virtual void
   get(
      CoreConfigurationMap::Key key,
      void*                     x
   ) const = 0;

   virtual void
   get(
      CoreConfigurationMap::Key key,
      char*                     x
   ) const = 0;
};

template <typename C>
struct CoreConfiguration_:
   public C,
   public CoreConfiguration,
   private CoreConfigurationStatic_ {
   void
   setAt(
      std::size_t i,
      const void* x
   )
   {
      CoreConfigurationBase* obj = static_cast<CoreConfigurationBase*>(this);
      CoreConfigurationStatic_::set(obj, this->getField(i), x);
   }

   template <typename T>
   void
   setAt(
      std::size_t i,
      const T&    x
   )
   {
      CoreConfigurationBase* obj = static_cast<CoreConfigurationBase*>(this);
      CoreConfigurationStatic_::set(obj, this->getField(i), x);
   }

   template <typename T>
   void
   setAt(
      std::size_t                     i,
      const std::initializer_list<T>& x
   )
   {
      CoreConfigurationBase* obj = static_cast<CoreConfigurationBase*>(this);
      CoreConfigurationStatic_::set(obj, this->getField(i), x);
   }

   void
   setAt(
      std::size_t i,
      const char* x
   )
   {
      CoreConfigurationBase* obj = static_cast<CoreConfigurationBase*>(this);
      CoreConfigurationStatic_::set(obj, this->getField(i), x);
   }

   void
   set(
      CoreConfigurationMap::Key key,
      const void*               x
   )
   {
      setAt(this->getFieldIndex(key), x);
   }

   template <typename T>
   void
   set(
      CoreConfigurationMap::Key key,
      const T&                  x
   )
   {
      setAt(this->getFieldIndex(key), x);
   }

   template <typename T>
   void
   set(
      CoreConfigurationMap::Key       key,
      const std::initializer_list<T>& x
   )
   {
      setAt(this->getFieldIndex(key), x);
   }

   void
   set(
      CoreConfigurationMap::Key key,
      const char*               x
   )
   {
      setAt(this->getFieldIndex(key), x);
   }

   void
   getAt(
      std::size_t i,
      void*       x
   ) const
   {
      const CoreConfigurationBase* obj = static_cast<const CoreConfigurationBase*>(this);
      CoreConfigurationStatic_::get(obj, this->getField(i), x);
   }

   template <typename T>
   void
   getAt(
      std::size_t i,
      T&          x
   ) const
   {
      const CoreConfigurationBase* obj = static_cast<const CoreConfigurationBase*>(this);
      CoreConfigurationStatic_::get(obj, this->getField(i), x);
   }

   void
   getAt(
      std::size_t i,
      char*       x
   ) const
   {
      const CoreConfigurationBase* obj = static_cast<const CoreConfigurationBase*>(this);
      CoreConfigurationStatic_::get(obj, this->getField(i), x);
   }

   void
   get(
      CoreConfigurationMap::Key key,
      void*                     x
   ) const
   {
      getAt(this->getFieldIndex(key), x);
   }

   template <typename T>
   void
   get(
      CoreConfigurationMap::Key key,
      T&                        x
   ) const
   {
      getAt(this->getFieldIndex(key), x);
   }

   void
   get(
      CoreConfigurationMap::Key key,
      char*                     x
   ) const
   {
      getAt(this->getFieldIndex(key), x);
   }
}

CORE_PACKED;

class CoreConfigurableBase
{
public:
   CoreConfigurableBase(
      const char* key
   ) : _configuration(nullptr), _key(key), link(*this)
   {
      CORE_ASSERT(is_identifier(key, NamingTraits<Node>::MAX_LENGTH));
   }

   void
   setConfigurationBase(
      const CoreConfigurationBase& configuration
   )
   {
	   core::os::SysLock::acquire();
      _configuration = &configuration;
      core::os::SysLock::release();
   }

   const CoreConfigurationBase&
   getConfigurationBase() const
   {
      CORE_ASSERT(_configuration != nullptr);

      return *_configuration;
   }

   inline bool
   isConfigured() const
   {
      return _configuration != nullptr;
   }

   inline const char*
   getKey() const
   {
      return _key;
   }

   virtual CoreConfigurationBase::Signature
   getConfigurationSignature() const = 0;

   virtual std::size_t
   getConfigurationSize() const = 0;


//--- CONFIGURATION OVERRIDE --------------------------------------------------
   virtual bool
   overrideConfiguration() = 0;

   virtual CoreConfiguration&
   getOverridingConfiguration() = 0;

   virtual CoreConfigurationBase&
   getOverridingConfigurationBase() = 0;


//-----------------------------------------------------------------------------

//--- CONFIGURATION IO --------------------------------------------------------
bool
   setConfigurationFrom(
      const uint8_t* storage,
	  std::size_t& offset,
      std::size_t    size
   )
   {
      CORE_ASSERT(size >= offset + NamingTraits<Node>::MAX_LENGTH + sizeof(CoreConfigurationBase::Signature) + getConfigurationSize());

      if(strncmp(_key, reinterpret_cast<const char*>(storage + offset), NamingTraits<Node>::MAX_LENGTH ) != 0) { // This was not for us!
    	  return false;
      }

      offset += NamingTraits<Node>::MAX_LENGTH;

      CoreConfigurationBase::Signature signature;
      memcpy(&signature, storage + offset, sizeof(CoreConfigurationBase::Signature));
      offset += sizeof(CoreConfigurationBase::Signature);

      CORE_ASSERT(signature == getConfigurationSignature());   // Avoid messing up the configuration

      setConfigurationBase(*reinterpret_cast<const CoreConfigurationBase*>(storage + offset));

      std::size_t dataSize = getConfigurationSize();
      offset += dataSize;

      return true;
   } // setConfigurationFrom

   bool
   overrideConfigurationFrom(
      const uint8_t* storage,
	  std::size_t& offset,
      std::size_t    size
   )
   {
      CORE_ASSERT(size >= offset + NamingTraits<Node>::MAX_LENGTH + sizeof(CoreConfigurationBase::Signature) + getConfigurationSize());

      if(strncmp(_key, reinterpret_cast<const char*>(storage + offset), NamingTraits<Node>::MAX_LENGTH ) != 0) { // This was not for us!
    	  return false;
      }

      offset += NamingTraits<Node>::MAX_LENGTH;

      CoreConfigurationBase::Signature signature;
      memcpy(&signature, storage + offset, sizeof(CoreConfigurationBase::Signature));
      offset += sizeof(CoreConfigurationBase::Signature);

      CORE_ASSERT(signature == getConfigurationSignature());   // Avoid messing up the configuration

      overrideConfiguration();

      std::size_t dataSize = getConfigurationSize();
      memcpy(&getOverridingConfigurationBase(), storage + offset, dataSize);

      offset += dataSize;

      return true;
   } // overrideConfigurationFrom

   bool
   dumpConfigurationTo(
      uint8_t*    storage,
	  std::size_t& offset,
      std::size_t size
   ) const
   {
      CORE_ASSERT(size >= offset + NamingTraits<Node>::MAX_LENGTH + sizeof(CoreConfigurationBase::Signature) + getConfigurationSize());

      memcpy(storage + offset, _key, NamingTraits<Node>::MAX_LENGTH);
      offset += NamingTraits<Node>::MAX_LENGTH;

      CoreConfigurationBase::Signature signature = getConfigurationSignature();

      memcpy(storage + offset, &signature, sizeof(CoreConfigurationBase::Signature));
      offset += sizeof(CoreConfigurationBase::Signature);

      const CoreConfigurationBase* data = static_cast<const CoreConfigurationBase*>(_configuration);
      std::size_t dataSize = getConfigurationSize();

      memcpy(storage + offset, data, dataSize);
      offset += dataSize;

      return offset;
   } // dumpTo

//-----------------------------------------------------------------------------

protected:
   const CoreConfigurationBase* _configuration;
   const char* _key;

public:
   mutable core::mw::StaticList<CoreConfigurableBase>::Link link;
};

template <typename T>
class CoreConfigurable:
   public CoreConfigurableBase
{
public:
   using ConfigurationType     = T;
   using ConfigurationMapType  = CoreConfigurationMap_<ConfigurationType>;
   using CoreConfigurationType = core::mw::CoreConfiguration_<ConfigurationMapType>;

   CoreConfigurable(
      const char* key
   ) : CoreConfigurableBase::CoreConfigurableBase(key), _overriding(nullptr) {}

   void
   setConfiguration(
      const ConfigurationType& configuration
   )
   {
//      CORE_ASSERT(configuration.getConfigurationSignature() == ConfigurationType::SIGNATURE);  // Make sure we are doing something good...
      _configuration = &configuration;
   }

   inline const ConfigurationType&
   configuration() const
   {
      return *(reinterpret_cast<const ConfigurationType*>(_configuration));
   }

   bool
   overrideConfiguration()
   {
      if (_overriding == nullptr) {
         _overriding = new CoreConfigurationType(); // It will live forever!!!
      }

      if (_overriding == nullptr) {
         return false;
      }

      if (_configuration != nullptr) {
         // Copy the old configuration into the overriding one
    	  memcpy(static_cast<CoreConfigurationBase*>(_overriding), _configuration, getConfigurationSize());
      }

      setConfiguration(*_overriding);

      return true;
   } // overrideConfiguration

   inline CoreConfiguration&
   getOverridingConfiguration()
   {
      CORE_ASSERT(_overriding != nullptr)

      return *_overriding;
   }

   inline CoreConfigurationBase&
   getOverridingConfigurationBase()
   {
      CORE_ASSERT(_overriding != nullptr)

      return *_overriding;
   }

   inline CoreConfigurationType&
   overridingConfiguration()
   {
      CORE_ASSERT(_overriding != nullptr)

      return *_overriding;
   }

   inline CoreConfigurationBase::Signature
   getConfigurationSignature() const
   {
      return ConfigurationType::SIGNATURE;
   }

   inline std::size_t
   getConfigurationSize() const
   {
      return sizeof(ConfigurationType);
   }

private:
   CoreConfigurationType* _overriding;
};

NAMESPACE_CORE_MW_END
