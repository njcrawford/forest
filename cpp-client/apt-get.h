#ifndef _APTGET_H
#define _APTGET_H

#include <vector>
#include <string>
using namespace std;

#include "PackageManager.h"

class AptGet : public PackageManager
{
public:
    void getAvailableUpdates(vector<string> & outlist);
    void applyUpdates(vector<string> & list);
};

#endif
