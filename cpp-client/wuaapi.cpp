#include <stdlib.h>
#include <string>
#include <iostream>

#include "wuaapi.h"
#include "forest-client.h"

void WuaApi::getAvailableUpdates(vector<updateInfo> & outList)
{
	// I don't think there is a command that will do this directly under 
	// windows. 
}

void WuaApi::applyUpdates(vector<string> & list)
{
	// I don't think there is a command that will do this directly under 
	// windows.
}
