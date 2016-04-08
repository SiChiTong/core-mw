#pragma once

#include <Core/MW/namespace.hpp>

#include <Core/MW/StaticList.hpp>
#include <Core/MW/CoreNode.hpp>

NAMESPACE_CORE_MW_BEGIN

class CoreNodeManager
{
public:
	CoreNodeManager();
	virtual
	~CoreNodeManager();

	void
	add(
			const CoreNode& node
	);

	bool
	setup();

	bool
	run();

	bool
	stop();

	bool
	teardown();

	bool
	isOk();


private:
	Core::MW::StaticList<CoreNode> _nodes;

	bool
	syncronize(
			ICoreNode::Action action,
			ICoreNode::State  state
	);
};

NAMESPACE_CORE_MW_END
