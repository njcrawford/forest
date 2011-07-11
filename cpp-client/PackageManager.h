#ifndef _PACKAGEMANAGER_H
#define _PACKAGEMANAGER_H

#include <vector>
#include <string>
using namespace std;

class PackageManager
{
public:
    virtual void applyUpdates(vector<string> & list) = 0;
    virtual void getAvailableUpdates(vector<updateInfo> & outList) = 0;
    bool canApplyUpdates();
};

#endif
