#include <sys/utsname.h>
#include <string>
#include <vector>

#include "KernelDifference.h"
#include "forest-client.h"

using namespace std;

rebootState KernelDifference::isRebootNeeded()
{
	// get the currently running kernel
	utsname kernelInfo;
	uname(&kernelInfo);
	//vector<string> kernelList;
	//mySystem("uname -r", kernelList);
	string runningKernel = kernelInfo.release;
	// On centos 6, uname returns the kernel arch but yum doesn't.
	// If a system is configured kernels for more than 1 arch then the kernel 
	// diff method may not be accurate for determining whether a reboot is needed.
	//kernelList.clear();
	//mySystem("uname -m", kernelList);
	string kernelArch = ".";
	kernelArch += kernelInfo.machine;
	string::size_type archPos = runningKernel.find(kernelArch);
	if(archPos != string::npos)
	{
		runningKernel = runningKernel.substr(0, archPos - 1);
	}

	// find the highest installed kernel
	// TODO: make this use the current package manager instead of hard coding yum
	vector<string> kernelList;
	string newestKernel = "";
	kernelList.clear();
	string command = "yum -q list installed kernel 2>&1";
	int junk;
	mySystem(&command, kernelList, &junk); 
	for(int i = kernelList.size() - 1; i >= 0; i--)
	{
		// grep -v "Installed Packages"
		if(kernelList[i].find("Installed Packages") != string::npos)
		{
			continue;
		}

		// cut out version number
		string::size_type start = kernelList[i].find(' ');
		start = kernelList[i].find('.', start);
		start = kernelList[i].rfind(' ', start) + 1;
		string::size_type end = kernelList[i].find(' ', start) - 1;
		string kernelTest = kernelList[i].substr(start, end - start);

		// compare this to the highest found so far
		if(newestKernel.compare(kernelTest) > 0)
		{
			newestKernel = kernelTest;
		}
	}

	if(runningKernel.compare(newestKernel) == 0)
	{
		return yes;
	}
	else
	{
		return no;
	}
}
