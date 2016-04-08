// Led::toggle and Led::write are to be implemented in derived Board

#pragma once

#include <Core/MW/namespace.hpp>
#include <Core/MW/common.hpp>
#include <Core/MW/CoreNodeManager.hpp>

NAMESPACE_CORE_MW_BEGIN

class CoreModule:
	public CoreNodeManager
{
public:
	class Led
	{
public:
		void
		toggle();

		void
		write(
				unsigned on
		);
	};

	static Led& led;


	bool
	initialize();

	static const void
	halt(
			const char* message
	);


	CoreModule() {}

	virtual ~CoreModule() {}
};

NAMESPACE_CORE_MW_END
