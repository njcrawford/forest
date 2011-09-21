#include "FilePresence.h"
#include <unistd.h>

rebootState FilePresence::isRebootNeeded()
{
	if(access( "/var/run/reboot-required", F_OK ) == 0)
	{
		return yes;
	}
	return no;
}
