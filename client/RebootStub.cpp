#include "RebootStub.h"

RebootState RebootStub::isRebootNeeded()
{
	return RebootState::Unknown;
}
