#ifndef _FOREST_CLIENT_H
#define _FOREST_CLIENT_H

#include <vector>
#include <string>

using namespace std;

#include "PackageManager.h"
#include "RebootManager.h"
#include "foresttypes.h"

class ForestClient
{
private:
	unsigned int verboseLevel;
	bool waitMode;
	string serverUrl;
	string myHostname;
	PackageManager * packageManager;
	RebootManager * rebootManager;

	void getAcceptedUpdatesRpcv2(vector<string> & outList, bool * rebootAccepted);
	void reportAvailableUpdatesRpcv2(vector<updateInfo> & list, bool rebootAttempted);
	void readConfigFile();
	void getHostname();
public:
	ForestClient();
	void setVerboseLevel(unsigned int level);
	void setWaitMode(bool enabled);
	int run();
};

#endif
