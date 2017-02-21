/* COPYRIGHT (c) 2016-2017 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#include <core/mw/namespace.hpp>
#include <core/mw/Utils.hpp>

NAMESPACE_CORE_MW_BEGIN


bool
is_identifier(
    const char* namep
)
{
    if ((namep != 0) && (*namep != 0)) {
        while ((*namep >= 'a' && *namep <= 'z')
               || (*namep >= 'A' && *namep <= 'Z')
               || (*namep >= '0' && *namep <= '9')
               || *namep == '_') {
            ++namep;
        }

        return *namep == 0;
    }

    return false;
}

bool
is_identifier(
    const char* namep,
    size_t      max_length
)
{
    if ((namep != 0) && (*namep != 0)) {
        while (max_length > 0 && ((*namep >= 'a' && *namep <= 'z')
                                  || (*namep >= 'A' && *namep <= 'Z')
                                  || (*namep >= '0' && *namep <= '9')
                                  || *namep == '_')) {
            ++namep;
            --max_length;
        }

        return max_length == 0 || *namep == 0;
    }

    return false;
}

NAMESPACE_CORE_MW_END
