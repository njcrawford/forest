#ifndef _RPCV2_H
#define _RPCV2_H

#include "ForestRpc.h"

class Rpcv2 : public ForestRpc
{
private:
    bool rebootAccepted;

public:
	Rpcv2(const string & serverUrl, const string & myHostname);
	bool checkForServerSupport() override;
	void checkInWithServer(ForestClientCapabilities & caps, vector<updateInfo> & availableUpdates, rebootState rebootNeeded, bool rebootAttempted, int verboseLevel) override;
	vector<string> getAcceptedUpdates() override;
	bool getRebootAccepted() override;
};

#endif
