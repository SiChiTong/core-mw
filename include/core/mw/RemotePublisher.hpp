/* COPYRIGHT (c) 2016-2017 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#pragma once

#include <core/mw/namespace.hpp>
#include <core/common.hpp>
#include <core/mw/BasePublisher.hpp>
#include <core/mw/StaticList.hpp>

NAMESPACE_CORE_MW_BEGIN


class RemotePublisher:
    public BasePublisher
{
    friend class Transport;

private:
    Transport* transportp;

    mutable StaticList<RemotePublisher>::Link by_transport;

public:
    Transport*
    get_transport() const;


protected:
    RemotePublisher(
        Transport& transport
    );
    virtual
    ~RemotePublisher() = 0;
};


inline
Transport*
RemotePublisher::get_transport() const
{
    return transportp;
}

NAMESPACE_CORE_MW_END
