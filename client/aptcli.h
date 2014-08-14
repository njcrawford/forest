#ifndef _APTCLI_H
#define _APTCLI_H

#include <vector>
#include <string>
using namespace std;

#include "PackageManager.h"

class AptCli : public PackageManager
{
public:
    void getAvailableUpdates(vector<updateInfo> & outlist) override;
	void parseUpdates(vector<updateInfo> & outlist, vector<string> & input);
    void applyUpdates(vector<string> & list) override;
    bool canApplyUpdates() override;
};

#endif
