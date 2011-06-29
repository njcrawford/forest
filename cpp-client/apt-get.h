#ifndef _APTGET_H
#define _APTGET_H

#include <vector>
#include <string>
using namespace std;

#include "PackageManager.h"

class AptGet : public PackageManager
{
public:
    int getAvailableUpdates(vector<string> & outlist);
    int applyUpdates(vector<string> & list);
};

#endif
