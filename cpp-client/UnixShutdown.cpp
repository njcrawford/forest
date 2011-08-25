#include "UnixShutdown.h"

void UnixShutdown::applyReboot()
{
	//system("shutdown -r now");
	system("echo \"shutdown -r now\" | at now + 10 minutes 2>&1 | grep -v \"warning: commands will be executed using\|job [0-9]* at");
}

bool UnixShutdown::canApplyReboot()
{
	return true;
}
