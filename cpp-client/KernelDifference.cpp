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
	string command = "rpm --query kernel";
	int junk;
	mySystem(&command, kernelList, &junk); 
	newestKernel = kernelList[kernelList.size() - 1];

	if(runningKernel.compare(newestKernel) == 0)
	{
		return no;
	}
	else
	{
		return yes;
	}
}
