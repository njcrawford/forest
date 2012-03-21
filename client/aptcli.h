#ifndef _APTCLI_H
#define _APTCLI_H

#include <vector>
#include <string>
using namespace std;

#include "PackageManager.h"

class AptCli : public PackageManager
{
public:
    void getAvailableUpdates(vector<updateInfo> & outlist);
    void applyUpdates(vector<string> & list);
    bool canApplyUpdates();
};

#endif
