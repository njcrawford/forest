#ifndef _FOREST_CLIENT_H
#define _FOREST_CLIENT_H

#include <vector>
#include <string>
// to_string and trim_string
#include <sstream>
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

	void getAcceptedUpdates(vector<string> & outList, bool * rebootAccepted);
	void reportAvailableUpdates(vector<updateInfo> & list, bool rebootAttempted);
	void readConfigFile();
	void getHostname();
public:
	ForestClient();
	void setVerboseLevel(unsigned int level);
	void setWaitMode(bool enabled);
	int run();
};

// Works like system(), but returns lines of stdout output in outList and the 
// command's return value in returnVal.
void mySystem(string * command, vector<string> & outList, int * returnVal);

string flattenStringList(vector<string> & list, char delimiter);

template <class T>
inline std::string to_string (const T& t)
{
	std::stringstream ss;
	ss << t;
	return ss.str();
}

inline string trim_string(const string& s)
{
	std::stringstream trimmer;
	trimmer << s;
	string retval;
	trimmer >> retval;
	return retval;
}


#endif
