#ifndef _WUAAPI_H
#define _WUAAPI_H

#include <vector>
#include <string>
using namespace std;

#include "PackageManager.h"

class WuaApi : public PackageManager
{
public:
    void getAvailableUpdates(vector<updateInfo> & outlist) override;
    void applyUpdates(vector<string> & list) override;
};

#endif
