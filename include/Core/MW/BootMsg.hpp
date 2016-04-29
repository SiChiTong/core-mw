/* COPYRIGHT (c) 2016 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#pragma once

#include <Core/MW/namespace.hpp>
#include <Core/MW/common.hpp>
#include <Core/MW/Message.hpp>
#include <Core/MW/NamingTraits.hpp>

NAMESPACE_CORE_MW_BEGIN


struct IhexRecord {
   enum {
      MAX_DATA_LENGTH = 16
   };

   enum TypeEnum {
      DATA = 0,
      END_OF_FILE,
      EXTENDED_SEGMENT_ADDRESS,
      START_SEGMENT_ADDRESS,
      EXTENDED_LINEAR_ADDRESS,
      START_LINEAR_ADDRESS,
   };

   uint8_t  count;
   uint16_t offset;
   uint8_t  type;
   uint8_t  data[MAX_DATA_LENGTH];
   uint8_t  checksum;

   uint8_t
   compute_checksum() const;
}

CORE_PACKED;


class BootMsg:
   public Message
{
public:
   enum TypeEnum {
      NACK = 0,
      ACK,

      BEGIN_LOADER,
      END_LOADER,
      LINKING_SETUP,
      LINKING_ADDRESSES,
      LINKING_OUTCOME,
      IHEX_RECORD,
      REMOVE_LAST,
      REMOVE_ALL,

      BEGIN_APPINFO,
      END_APPINFO,
      APPINFO_SUMMARY,

      BEGIN_SETPARAM,
      END_SETPARAM,
      BEGIN_GETPARAM,
      END_GETPARAM,
      PARAM_REQUEST,
      PARAM_CHUNK
   };

   enum {
      MAX_PAYLOAD_LENGTH = 30
   };

   typedef const uint8_t* Address;
   typedef size_t         Length;

   struct ErrorInfo {
      enum ReasonEnum {
         UNKNOWN = 0,
         NO_FREE_MEMORY,
         ZERO_LENGTH,
         OUT_OF_RANGE,
      };

      enum TypeEnum {
         NONE = 0,
         TEXT,
         INTEGRAL,
         UINTEGRAL,
         ADDRESS,
         LENGTH,
         CHUNK
      };

      enum {
         MAX_TEXT_LENGTH = 24
      };

      uint16_t line;
      uint8_t  reason;
      uint8_t  type;
      union {
         char      text[MAX_TEXT_LENGTH];
         intmax_t  integral;
         uintmax_t uintegral;
         Address   address;
         Length    length;
         struct {
            Address address;
            Length  length;
         }

         chunk CORE_PACKED;
      }

      CORE_PACKED;
   }

   CORE_PACKED;

   struct LinkingSetup {
      Length   pgmlen;
      Length   bsslen;
      Length   datalen;
      Length   stacklen;
      char     name[NamingTraits < Node > ::MAX_LENGTH];
      uint16_t flags;
   }

   CORE_PACKED;

   struct LinkingAddresses {
      Address infoadr;
      Address pgmadr;
      Address bssadr;
      Address dataadr;
      Address datapgmadr;
      Address nextadr;
   }

   CORE_PACKED;

   struct LinkingOutcome {
      Address mainadr;
      Address cfgadr;
      Length  cfglen;
      Address ctorsadr;
      Length  ctorslen;
      Address dtorsadr;
      Length  dtorslen;
   }

   CORE_PACKED;

   struct AppInfoSummary {
      Length  numapps;
      Address freeadr;
      Address pgmstartadr;
      Address pgmendadr;
      Address ramstartadr;
      Address ramendadr;
   }

   CORE_PACKED;

   struct ParamRequest {
      Length  offset;
      char    appname[NamingTraits < Node > ::MAX_LENGTH];
      uint8_t length;
   }

   CORE_PACKED;

   struct ParamChunk {
      enum {
         MAX_DATA_LENGTH = 16
      };

      uint8_t data[MAX_DATA_LENGTH];
   }

   CORE_PACKED;

public:
   union {
      uint8_t          raw[MAX_PAYLOAD_LENGTH];
      ErrorInfo        error_info;
      IhexRecord       ihex_record;
      LinkingSetup     linking_setup;
      LinkingAddresses linking_addresses;
      LinkingOutcome   linking_outcome;
      AppInfoSummary   appinfo_summary;
      ParamRequest     param_request;
      ParamChunk       param_chunk;
   }

   CORE_PACKED;

   uint8_t type;
}

CORE_PACKED;


NAMESPACE_CORE_MW_END
