#ifndef _FORESTRPC_H
#define _FORESTRPC_H

#include <string>
#include <vector>
using namespace std;
#include "foresttypes.h"
#include "ForestClientCapabilities.h"

class ForestRpc
{
protected:
	string serverUrl;
	string myHostname;

public:
	ForestRpc(const string & serverUrl, const string & myHostname);
	virtual bool checkForServerSupport() = 0;
	virtual void checkInWithServer(ForestClientCapabilities & caps, vector<updateInfo> & availableUpdates, rebootState rebootNeeded, bool rebootAttempted, int verboseLevel) = 0;
	virtual vector<string> getAcceptedUpdates() = 0;
	virtual bool getRebootAccepted() = 0;
};

#endif
