#ifndef _YUM_H
#define _YUM_H

#include <vector>
#include <string>
using namespace std;

#include "PackageManager.h"

class Yum : public PackageManager
{
public:
    int applyUpdates(vector<string> * list);
    int getAvailableUpdates(vector<string> * outList);
};

#endif
