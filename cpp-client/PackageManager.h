#ifndef _PACKAGEMANAGER_H
#define _PACKAGEMANAGER_H

class PackageManager
{
    virtual int applyUpdates(vector<string> * list) = 0;
    virtual int getAvailableUpdates(vector<string> * outList) = 0;
}
#endif
