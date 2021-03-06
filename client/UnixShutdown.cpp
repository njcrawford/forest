#include "UnixShutdown.h"
#include <stdlib.h>
#include "config.h"
#include <unistd.h>

void UnixShutdown::applyReboot()
{
	// Initiate a shutdown in 10 minutes
	// The 10 minute delay is to allow other cron jobs to finish
	system("echo \"/sbin/shutdown -r now\" | " AT_COMMAND " now + 10 minutes 2>&1 | grep -v \"warning: commands will be executed using\\|job [0-9]* at\"");
}

bool UnixShutdown::canApplyReboot()
{
	// Check for 'at' presence
	if (access(AT_COMMAND, F_OK) != -1)
	{
		return true;
	}
	else
	{
		return false;
	}
}
