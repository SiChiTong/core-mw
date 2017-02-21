/* COPYRIGHT (c) 2016-2017 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#include <core/mw/namespace.hpp>
#include <core/mw/BaseSubscriber.hpp>
#include <core/mw/Message.hpp>
#include <core/mw/Topic.hpp>

NAMESPACE_CORE_MW_BEGIN


bool
BaseSubscriber::release_unsafe(
    Message& msg
)
{
    CORE_ASSERT(topicp != NULL);

    if (!msg.release_unsafe()) {
        topicp->free_unsafe(msg);
        return false;
    }

    return true;
}

bool
BaseSubscriber::release(
    Message& msg
)
{
    CORE_ASSERT(topicp != NULL);

    if (!msg.release()) {
        topicp->free(msg);
        return false;
    }

    return true;
}

BaseSubscriber::BaseSubscriber()
    :
    topicp(NULL)
{}


BaseSubscriber::~BaseSubscriber() {}


bool
BaseSubscriber::has_topic(
    const BaseSubscriber& sub,
    const char*           namep
)
{
    return Topic::has_name(*sub.topicp, namep);
}

NAMESPACE_CORE_MW_END
