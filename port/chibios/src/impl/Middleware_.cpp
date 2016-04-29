/* COPYRIGHT (c) 2016 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#include <Core/MW/namespace.hpp>
#include <Core/MW/Middleware.hpp>

#include "ch.h"
#include "hal.h"

NAMESPACE_CORE_MW_BEGIN


void
Middleware::reboot()
{
   chThdSleep((MS2ST(10) > 0) ? MS2ST(10) : 1);
   chSysDisable();

   // Set the reboot magic number
#if CORE_USE_BOOTLOADER
   rebooted_magic = REBOOTED_MAGIC;
#endif

   // Ensure completion of memory access.
   __DSB();

   /* Generate reset by setting VECTRESETK and SYSRESETREQ, keeping priority
    * group unchanged.
    * If only SYSRESETREQ used, no reset is triggered, discovered while
    * debugging.
    * If only VECTRESETK is used, if you want to read the source of the reset
    * afterwards from (RCC->CSR & RCC_CSR_SFTRSTF), it won't be possible to see
    * that it was a software-triggered reset.*/
   SCB->AIRCR = (0x5FA << SCB_AIRCR_VECTKEY_Pos)
                | (SCB->AIRCR & SCB_AIRCR_PRIGROUP_Msk)
                | SCB_AIRCR_VECTRESET_Msk
                | SCB_AIRCR_SYSRESETREQ_Msk;

   // Ensure completion of memory access.
   __DSB();

   // Wait for reset.
   for (;;) {}
} // Middleware::reboot

NAMESPACE_CORE_MW_END
