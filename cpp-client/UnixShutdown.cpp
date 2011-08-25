#include "UnixShutdown.h"

void UnixShutdown::applyReboot()
{
	//system("shutdown -r now");
}

bool UnixShutdown::canApplyReboot()
{
	return true;
}
