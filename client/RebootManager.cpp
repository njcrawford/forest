#include "RebootManager.h"

bool RebootManager::canApplyReboot()
{
	// override to return true in child class if it can apply reboot
	return false;
}

void RebootManager::applyReboot()
{
	// does nothing by default!
	// override in child class to actually do something
}
