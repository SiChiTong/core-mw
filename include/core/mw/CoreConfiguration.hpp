/* COPYRIGHT (c) 2016 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#pragma once

#include <core/mw/namespace.hpp>
#include <core/mw/CoreType.hpp>
#include <core/Array.hpp>
#include <core/String.hpp>
#include <core/mw/StaticList.hpp>
#include <core/mw/NamingTraits.hpp>

#include <initializer_list>
#include <type_traits>

NAMESPACE_CORE_MW_BEGIN

/*! \brief Base class for all the configurations
 *
 */
struct CoreConfigurationBase {
   //! Type of the signature
   using Signature = uint32_t;
}

CORE_PACKED_ALIGNED;

/*! \brief Something with a signature
 *
 */
struct CoreConfigurationSignature {
   /*! \brief Get the signature for the object
    *
    * \return Signature of the object
    */
   virtual CoreConfigurationBase::Signature
   getConfigurationSignature() const = 0;
};

/*! \brief CoreConfigurationMap
 *
 */
struct CoreConfigurationMap {
   //! Type of the map key
   using Key = core::String<24>;

   /*! \brief Metadata about a configuration field
    *
    */
   struct FieldMetadata {
      Key                key; //!< key (== name) of the field
      std::ptrdiff_t     offset; //!< offset wrt the beginnig of the configuration struct
      core::mw::CoreType type; //!< type of the field
      std::size_t        size; //!< size of the field (== 1 for scalar types)
   };

   /*! \brief Get the number of fields in the map
    *
    */
   virtual const std::size_t
   getSize() const = 0;


   /*! \brief Get metadata for a field
    *
    * \pre i must be valid, otherwise an assert is fired
    */
   virtual const FieldMetadata&
   getField(
      std::size_t i //!< [in] index of the field in the map
   ) const = 0;


   /*! \brief Get metadata for a field
    *
    * \pre  key must be valid, otherwise an assert is fired
    */
   const FieldMetadata
   getField(
      CoreConfigurationMap::Key key //!< [in] name of the field
   ) const;


   /*! \brief Get index of the field given its key
    *
    * \return index of the field in the map

    * \note If the field does not exist, it returns an invalid index
    */
   const std::size_t
   getFieldIndex(
      CoreConfigurationMap::Key key //!< [in] name of the field
   ) const;


   using iterator = const FieldMetadata *;

   virtual iterator
   begin() const = 0;

   virtual iterator
   end() const = 0;
};

/*! \brief CoreConfigurationMap_
 *
 */
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
   static const core::Array<FieldMetadata, LENGTH> _map;
};

/*! \brief CoreConfigurationStatic_
 *
 */
struct CoreConfigurationStatic_ {
   // SET
   static void
   set(
      CoreConfigurationBase*                     obj,
      const CoreConfigurationMap::FieldMetadata& field,
      const void*                                x
   );


   template <typename T>
   static void
   set(
      CoreConfigurationBase*                     obj,
      const CoreConfigurationMap::FieldMetadata& field,
      const T&                                   x
   );


   template <typename T>
   static void
   set(
      CoreConfigurationBase*                     obj,
      const CoreConfigurationMap::FieldMetadata& field,
      const std::initializer_list<T>&            x
   );

   static void
   set(
      CoreConfigurationBase*                     obj,
      const CoreConfigurationMap::FieldMetadata& field,
      const char*                                x
   );


   //GET
   static void
   get(
      const CoreConfigurationBase*               obj,
      const CoreConfigurationMap::FieldMetadata& field,
      void*                                      x
   );


   template <typename T>
   static void
   get(
      const CoreConfigurationBase*               obj,
      const CoreConfigurationMap::FieldMetadata& field,
      T&                                         x
   );

   static void
   get(
      const CoreConfigurationBase*               obj,
      const CoreConfigurationMap::FieldMetadata& field,
      char*                                      x
   );
}

CORE_PACKED;


/*! \brief Interface for the configuration fields access
 *
 */
struct CoreConfiguration {
   /*! \brief Set a configuration field
    *
    */
   virtual void
   set(
      CoreConfigurationMap::Key key, //!< [in] key (== name) of the field
      const void*               x //!< [in] pointer to the data
   ) = 0;


   /*! \brief Set a configuration field
    *
    * This is specialized for strings. It pads the field with \\0.
    */
   virtual void
   set(
      CoreConfigurationMap::Key key, //!< [in] key (== name) of the field
      const char*               x//!< [in] string
   ) = 0;


   /*! \brief Get a configuration field
    *
    */
   virtual void
   get(
      CoreConfigurationMap::Key key, //!< [in] key (== name) of the field
      void*                     x //!< [in/out] pointer to the data
   ) const = 0;


   /*! \brief Get a configuration field
    *
    * This is specialized for strings. It pads the string x with \\0.
    */
   virtual void
   get(
      CoreConfigurationMap::Key key, //!< [in] key (== name) of the field
      char*                     x//!< [in/out] string
   ) const = 0;
};

/*! \brief CoreConfiguration_
 *
 */
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

CORE_PACKED_ALIGNED;

/*! \brief Base class for all configurable objects
 *
 */
class CoreConfigurableBase
{
public:
   /*! \brief Constructor
    *
    * \pre key must be a valid identifier (a 8 characters string made of a-zA-Z0-9_ that is /\\w{0,8}/ if you know what I mean)
    */
   CoreConfigurableBase(
      const char* key
   ) : _configuration(nullptr), _key(key), link(*this)
   {
      //CORE_ASSERT(is_identifier(key, NamingTraits<Node>::MAX_LENGTH));
	   CORE_ASSERT(strlen(key) <= NamingTraits<CoreConfigurableBase>::MAX_LENGTH);
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

   /*! \brief Check if the object has been configured
    *
    */
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

   /*! \brief Get the signature of the configuration struct
    *
    * \return signature of the configuration struct
    */
   virtual CoreConfigurationBase::Signature
   getConfigurationSignature() const = 0;


   /*! \brief Get the size of the configuration struct
    *
    * \return size of the configuration struct
    */
   virtual std::size_t
   getConfigurationSize() const = 0;


//--- CONFIGURATION OVERRIDE --------------------------------------------------
/*! \brief Create an overriding configuration
 *
 * It allocates (if required) a CoreConfigurationType struct, and copies the current configuration (if any).
 *
 * \post The overriding configuration is a copy of the previous configuration (if any).
 *
 * \return sucess
 * \retval true a new configuration has been allocated
 * \retval false it was not possible to allocate a new configuration
 */
   virtual bool
   overrideConfiguration() = 0;


   /*! \brief Get the overriding configuration
    *
    * \pre The overriding configuration must exist, otherwise an assert is fired
    */
   virtual CoreConfiguration&
   getOverridingConfiguration() = 0;

   virtual CoreConfigurationBase&
   getOverridingConfigurationBase() = 0;


//-----------------------------------------------------------------------------

//--- CONFIGURATION IO --------------------------------------------------------
/*! \brief Sets a configuration from memory
 *
 * \pre size >= offset + NamingTraits<Node>::MAX_LENGTH + sizeof(CoreConfigurationBase::Signature) + getConfigurationSize(), otherwise an assert is fired
 * \pre If a configuration matching the key is found, its signature must be valid, otherwise an assert is fired
 * \post In case of success, offset contains the offset to the first byte after the configuration, otherwise it is unchanged
 *
 * \return success
 * \retval true a valid configuration has been found, and set
 * \retval false no valid configuration has been found at storage + offset
 */
   bool
   setConfigurationFrom(
      const void*  storage, //!< [in] pointer to memory area that holds the configuration
      std::size_t& offset,   //!< [in/out] offset
      std::size_t  size   //!< [in] size of storage
   );


   /*! \brief Overrides a configuration from memory
    *
    * \pre size >= offset + NamingTraits<Node>::MAX_LENGTH + sizeof(CoreConfigurationBase::Signature) + getConfigurationSize(), otherwise an assert is fired
    * \pre If a configuration matching the key is found, its signature must be valid, otherwise an assert is fired
    * \post In case of success, offset contains the offset to the first byte after the configuration, otherwise it is unchanged
    *
    * \return success
    * \retval true a valid configuration has been found, and set
    * \retval false no valid configuration has been found at storage + offset
    */
   bool
   overrideConfigurationFrom(
      const void*  storage, //!< [in] pointer to memory area that holds the configuration
      std::size_t& offset,   //!< [in/out] offset
      std::size_t  size   //!< [in] size of storage
   );


   /*! \brief Dumps the current configuration to memory
    *
    * \pre size >= offset + NamingTraits<Node>::MAX_LENGTH + sizeof(CoreConfigurationBase::Signature) + getConfigurationSize(), otherwise an assert is fired
    * \post offset contains the offset to the first byte after the configuration, otherwise it is unchanged
    *
    * \return success
    * \retval true configuration has been dumped
    * \retval false failure
    */
   bool
   dumpConfigurationTo(
      uint8_t*     storage,//!< [in] pointer to memory area that will hold the configuration
      std::size_t& offset,//!< [in/out] offset
      std::size_t  size//!< [in] size of storage
   ) const;


//-----------------------------------------------------------------------------

protected:
   const CoreConfigurationBase* _configuration;
   const char* _key;

public:
   mutable core::mw::StaticList<CoreConfigurableBase>::Link link;
};

/*! \brief Configurable object
 *
 * \tparam T configuration class (must inherit from CoreConfigurationBase)
 */
template <typename T>
class CoreConfigurable:
   public CoreConfigurableBase
{
public:
   using ConfigurationType     = T; //!< Configuration type
   using ConfigurationMapType  = CoreConfigurationMap_<ConfigurationType>; //!< Configuration map
   using CoreConfigurationType = core::mw::CoreConfiguration_<ConfigurationMapType>; //!< Configuration

   /*! \brief Constructor
    *
    * \pre key must be a valid identifier (a 8 characters string made of a-zA-Z0-9_ that is /\\w{0,8}/ if you know what I mean)
    */
   CoreConfigurable(
      const char* key
   ) : CoreConfigurableBase::CoreConfigurableBase(key), _overriding(nullptr) {}

   /*! \brief Set the configuration
    *
    * This sets an existing configuration struct as the object configuration.
    */
   void
   setConfiguration(
      const ConfigurationType& configuration
   )
   {
//      CORE_ASSERT(configuration.getConfigurationSignature() == ConfigurationType::SIGNATURE);  // Make sure we are doing something good...
      _configuration = &configuration;
   }

   /*! \brief Get the current configuration
    *
    */
   inline const ConfigurationType&
   configuration() const
   {
      return *(reinterpret_cast<const ConfigurationType*>(_configuration));
   }

   /*! \brief Create an overriding configuration
    *
    * It allocates (if required) a CoreConfigurationType struct, and copies the current configuration (if any).
    *
    * \post The overriding configuration is a copy of the previous configuration (if any).
    *
    * \return sucess
    * \retval true a new configuration has been allocated
    * \retval false it was not possible to allocate a new configuration
    */
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

   /*! \brief Get the overriding configuration
    *
    * \pre The overriding configuration must exist, otherwise an assert is fired
    */
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

#if 0
   /*! \brief Get the overriding configuration
    *
    * \pre The overriding configuration must exist, otherwise an assert is fired
    */
   inline CoreConfigurationType&
   overridingConfiguration()
   {
      CORE_ASSERT(_overriding != nullptr)

      return *_overriding;
   }
#endif

   /*! \brief Get the signature of the configuration struct
    *
    * \return signature of the configuration struct
    */
   inline CoreConfigurationBase::Signature
   getConfigurationSignature() const
   {
      return ConfigurationType::SIGNATURE;
   }

   /*! \brief Get the size of the configuration struct
    *
    * \return size of the configuration struct
    */
   inline std::size_t
   getConfigurationSize() const
   {
      return sizeof(ConfigurationType);
   }

private:
   CoreConfigurationType* _overriding;
};

// --------------------------------------------------------------------------------------------------------------------
// IMPLEMENTATION
// --------------------------------------------------------------------------------------------------------------------
template <typename T>
void
CoreConfigurationStatic_::set(
   CoreConfigurationBase*                     obj,
   const CoreConfigurationMap::FieldMetadata& field,
   const T&                                   x
)
{
   std::size_t        s1 = field.size;
   std::size_t        s2 = core::mw::CoreTypeUtils::size(x);
   core::mw::CoreType t1 = field.type;
   core::mw::CoreType t2 = core::mw::CoreTypeUtils::coreType(x);

   CORE_ASSERT(s1 == s2 && t1 == t2);   // make sure we are doing something meaningful...

   const T* src = reinterpret_cast<const T*>(&x);
   T*       dst = reinterpret_cast<T*>(obj + field.offset);

   while (s1--) {
      *dst++ = *src++;
   }
}

template <typename T>
void
CoreConfigurationStatic_::set(
   CoreConfigurationBase*                     obj,
   const CoreConfigurationMap::FieldMetadata& field,
   const std::initializer_list<T>&            x
)
{
   std::size_t        s1 = field.size;
   std::size_t        s2 = x.size();
   core::mw::CoreType t1 = field.type;
   core::mw::CoreType t2 = core::mw::CoreTypeTraitsHelperB<T>::types;

   CORE_ASSERT(s1 == s2 && t1 == t2);   // make sure we are doing something meaningful...

   T* dst = reinterpret_cast<T*>(obj + field.offset);

   for (T tmp : x) {
      *dst++ = tmp;
   }
}

template <typename T>
void
CoreConfigurationStatic_::get(
   const CoreConfigurationBase*               obj,
   const CoreConfigurationMap::FieldMetadata& field,
   T&                                         x
)
{
   std::size_t        s1 = field.size;
   std::size_t        s2 = core::mw::CoreTypeUtils::size(x);
   core::mw::CoreType t1 = field.type;
   core::mw::CoreType t2 = core::mw::CoreTypeUtils::coreType(x);

   CORE_ASSERT(s1 == s2 && t1 == t2);     // make sure we are doing something meaningful...

   const T* src = reinterpret_cast<const T*>(obj + field.offset);
   T*       dst = reinterpret_cast<T*>(&x);

   while (s1--) {
      *dst++ = *src++;
   }
}

NAMESPACE_CORE_MW_END

// --------------------------------------------------------------------------------------------------------------------
// Macros that allow us to keep the python code generator clean
// --------------------------------------------------------------------------------------------------------------------
#define CORE_CONFIGURATION_BEGIN(__name__) \
   class __name__: \
      public core::mw::CoreConfigurationBase { \
public: \
      using Type = __name__; \
      __name__();
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
   } \
   CORE_PACKED_ALIGNED;
#define CORE_CONFIGURATION_CONSTRUCTOR_BEGIN(__name__) \
		__name__::__name__() :
#define CORE_CONFIGURATION_CONSTRUCTOR_FIELD(__name__) \
		__name__(defaults::__name__)
#define CORE_CONFIGURATION_CONSTRUCTOR_FIELD_NONE(__name__) \
		__name__()
#define CORE_CONFIGURATION_CONSTRUCTOR_END(__name__) \
		{}
#define CORE_CONFIGURATION_DEFAULT_BEGIN() \
		namespace defaults {
#define CORE_CONFIGURATION_DEFAULT_FIELD(__name__, __type__, __size__, ...) \
		static const core::mw::CoreTypeTraits<core::mw::CoreType::__type__, __size__>::Type __name__ = {__VA_ARGS__};
#define CORE_CONFIGURATION_DEFAULT_END() \
		}
#define CORE_CONFIGURATION_MAP_BEGIN(__name__) \
   template <> \
   const core::Array<core::mw::CoreConfigurationMap::FieldMetadata, core::mw::CoreConfigurationMap_<__name__>::LENGTH>core::mw::CoreConfigurationMap_<__name__>::_map = {{
#define CORE_CONFIGURATION_MAP_ENTRY(__name__, __field__, __type__, __size__) \
   { # __field__, offsetof(__name__, __field__), core::mw::CoreType::__type__, __size__},
#define CORE_CONFIGURATION_MAP_END() \
   } \
   };
