#ifndef _YUM_H
#define _YUM_H

#include <vector>
#include <string>
using namespace std;

#include "PackageManager.h"

class Yum : public PackageManager
{
public:
    void applyUpdates(vector<string> & list);
    void getAvailableUpdates(vector<string> & outList);
};

#endif
