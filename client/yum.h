#ifndef _YUM_H
#define _YUM_H

#include <vector>
#include <string>
using namespace std;

#include "PackageManager.h"

class Yum : public PackageManager
{
public:
    void applyUpdates(vector<string> & list) override;
	void parseUpdates(vector<updateInfo> & outlist, vector<string> & input);
    void getAvailableUpdates(vector<updateInfo> & outList) override;
    bool canApplyUpdates() override;
};

#endif
