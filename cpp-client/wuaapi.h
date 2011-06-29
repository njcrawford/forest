#ifndef _WUAAPI_H
#define _WUAAPI_H

#include <vector>
#include <string>
using namespace std;

#include "PackageManager.h"

class WuaApi : public PackageManager
{
public:
    int getAvailableUpdates(vector<string> & outlist);
    int applyUpdates(vector<string> & list);
};

#endif
