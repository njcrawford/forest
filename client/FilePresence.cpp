#include "FilePresence.h"
#include <unistd.h>

RebootState FilePresence::isRebootNeeded()
{
	if(access( "/var/run/reboot-required", F_OK ) == 0)
	{
		return RebootState::Yes;
	}
	return RebootState::No;
}
