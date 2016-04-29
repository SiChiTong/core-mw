/* COPYRIGHT (c) 2016 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */
 
#pragma once

#include <Core/MW/namespace.hpp>
#include <Core/MW/common.hpp>
#include <Core/MW/BaseSubscriber.hpp>

NAMESPACE_CORE_MW_BEGIN

class Transport;


class RemoteSubscriber:
	public BaseSubscriber
{
	friend class Transport;
	friend class Topic;

private:
	Transport* transportp;

	mutable StaticList<RemoteSubscriber>::Link by_transport;
	mutable StaticList<RemoteSubscriber>::Link by_topic;

public:
	Transport*
	get_transport() const;


protected:
	RemoteSubscriber(
			Transport& transport
	);
	virtual
	~RemoteSubscriber() = 0;
};


inline
Transport*
RemoteSubscriber::get_transport() const
{
	return transportp;
}

NAMESPACE_CORE_MW_END
