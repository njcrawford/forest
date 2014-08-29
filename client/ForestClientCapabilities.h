#ifndef _FORESTCLIENTCAPABILITIES_H
#define _FORESTCLIENTCAPABILITIES_H

#include <string>
using namespace std;

class ForestClientCapabilities
{
private:

public:
	ForestClientCapabilities();
	bool canApplyReboot;
	bool canApplyUpdates;
	string clientVersion;
	string osVersion;
};

#endif
