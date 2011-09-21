#ifndef _MACSU_H
#define _MACSU_H

#include <vector>
#include <string>
using namespace std;

#include "PackageManager.h"

class MacSU : public PackageManager
{
public:
    void getAvailableUpdates(vector<updateInfo> & outlist);
    void applyUpdates(vector<string> & list);
    bool canApplyUpdates();
};

#endif