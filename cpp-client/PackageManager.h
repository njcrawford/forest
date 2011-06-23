#ifndef _PACKAGEMANAGER_H
#define _PACKAGEMANAGER_H

#include <vector>
#include <string>
using namespace std;

class PackageManager
{
public:
    virtual int applyUpdates(vector<string> * list) = 0;
    virtual int getAvailableUpdates(vector<string> * outList) = 0;
};

#endif
