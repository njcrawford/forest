#include <stdlib.h>
#include <string>
#include <iostream>

#include "wuaapi.h"
#include "forest-client.h"

int WuaApi::getAvailableUpdates(vector<string> * outList)
{
	// I don't think there is a command that will do this directly under 
	// windows. 
}

int WuaApi::applyUpdates(vector<string> * list)
{
	// I don't think there is a command that will do this under windows.
	// This will probably need to be broken out into a seperate function.
}
