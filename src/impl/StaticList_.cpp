/* COPYRIGHT (c) 2016 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#include <core/mw/namespace.hpp>
#include <core/mw/impl/StaticList_.hpp>

NAMESPACE_CORE_MW_BEGIN


size_t
StaticList_::count_unsafe() const
{
   size_t count = 0;

   for (const Link* linkp = headp; linkp != NULL; linkp = linkp->nextp) {
      ++count;
   }

   return count;
}

bool
StaticList_::unlink_unsafe(
   Link& link
)
{
   for (Link* curp = headp, * prevp = NULL;
        curp != NULL;
        prevp = curp, curp = curp->nextp) {
      if (curp == &link) {
         if (prevp != NULL) {
            prevp->nextp = curp->nextp;
         } else {
            headp = curp->nextp;
         }

         curp->nextp = NULL;
         return true;
      }
   }

   return false;
} // StaticList_::unlink_unsafe

int
StaticList_::index_of_unsafe(
   const void* itemp
) const
{
   if (itemp == NULL) {
      return -1;
   }

   int i = 0;

   for (const Link* linkp = headp; linkp != NULL; ++i, linkp = linkp->nextp) {
      if (linkp->itemp == itemp) {
         return i;
      }
   }

   return -1;
}

bool
StaticList_::contains_unsafe(
   const void* itemp
) const
{
   for (const Link* linkp = headp; linkp != NULL; linkp = linkp->nextp) {
      if (linkp->itemp == itemp) {
         return true;
      }
   }

   return false;
}

void*
StaticList_::find_first_unsafe(
   Predicate pred_func
) const
{
   for (const Link* linkp = headp; linkp != NULL; linkp = linkp->nextp) {
      if (pred_func(linkp->itemp)) {
         return linkp->itemp;
      }
   }

   return NULL;
}

void*
StaticList_::find_first_unsafe(
   Matches     match_func,
   const void* featuresp
) const
{
   CORE_ASSERT(featuresp != NULL);

   for (const Link* linkp = headp; linkp != NULL; linkp = linkp->nextp) {
      if (match_func(linkp->itemp, featuresp)) {
         return linkp->itemp;
      }
   }

   return NULL;
}

const StaticList_::Link*
StaticList_::get_head() const
{
   core::os::SysLock::acquire();
   const Link* headp = get_head_unsafe();
   core::os::SysLock::release();

   return headp;
}

bool
StaticList_::is_empty() const
{
   core::os::SysLock::acquire();
   bool success = is_empty_unsafe();
   core::os::SysLock::release();

   return success;
}

size_t
StaticList_::count() const
{
   size_t count = 0;
   core::os::SysLock::acquire();

   for (const Link* linkp = headp; linkp != NULL; linkp = linkp->nextp) {
      core::os::SysLock::release();
      ++count;
      core::os::SysLock::acquire();
   }

   core::os::SysLock::release();
   return count;
}

void
StaticList_::link(
   Link& link
)
{
   core::os::SysLock::acquire();

   link_unsafe(link);
   core::os::SysLock::release();
}

bool
StaticList_::unlink(
   Link& link
)
{
   core::os::SysLock::acquire();
   bool success = unlink_unsafe(link);
   core::os::SysLock::release();

   return success;
}

int
StaticList_::index_of(
   const void* itemp
) const
{
   if (itemp == NULL) {
      return -1;
   }

   int i = 0;
   core::os::SysLock::acquire();

   for (const Link* linkp = headp; linkp != NULL; ++i, linkp = linkp->nextp) {
      core::os::SysLock::release();

      if (linkp->itemp == itemp) {
         return i;
      }

      core::os::SysLock::acquire();
   }

   core::os::SysLock::release();
   return -1;
} // StaticList_::index_of

bool
StaticList_::contains(
   const void* itemp
) const
{
   core::os::SysLock::acquire();

   for (const Link* linkp = headp; linkp != NULL; linkp = linkp->nextp) {
      if (linkp->itemp == itemp) {
         core::os::SysLock::release();
         return true;
      }

      core::os::SysLock::release();
      core::os::SysLock::acquire();
   }

   core::os::SysLock::release();
   return false;
}

void*
StaticList_::find_first(
   Predicate pred_func
) const
{
   core::os::SysLock::acquire();

   for (const Link* linkp = headp; linkp != NULL; linkp = linkp->nextp) {
      core::os::SysLock::release();

      if (pred_func(linkp->itemp)) {
         return linkp->itemp;
      }

      core::os::SysLock::acquire();
   }

   core::os::SysLock::release();
   return NULL;
}

void*
StaticList_::find_first(
   Matches     match_func,
   const void* featuresp
) const
{
   CORE_ASSERT(featuresp != NULL);

   core::os::SysLock::acquire();

   for (const Link* linkp = headp; linkp != NULL; linkp = linkp->nextp) {
      core::os::SysLock::release();

      if (match_func(linkp->itemp, featuresp)) {
         return linkp->itemp;
      }

      core::os::SysLock::acquire();
   }

   core::os::SysLock::release();
   return NULL;
} // StaticList_::find_first

StaticList_::StaticList_()
   :
   headp(NULL)
{}


NAMESPACE_CORE_MW_END
