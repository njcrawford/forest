#include "RebootStub.h"

void RebootStub::applyReboot()
{
	// does nothing!
}

rebootState RebootStub::isRebootNeeded()
{
	return unknown;
}
