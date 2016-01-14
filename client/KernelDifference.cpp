#include <sys/utsname.h>
#include <string>
#include <vector>

#include "KernelDifference.h"
#include "helpers.h"

using namespace std;

RebootState KernelDifference::isRebootNeeded()
{
	// Get the currently running kernel
	utsname kernelInfo;
	uname(&kernelInfo);
	string runningKernel = kernelInfo.release;

	// On centos 6, uname returns the kernel arch as part of the version
	// but yum doesn't. So, we need to remove the arch if it's present in the
	// version string.
	// NOTE: If a system is configured kernels for more than 1 arch then the kernel 
	// diff method may not be accurate for determining whether a reboot is needed.
	string kernelArch = ".";
	kernelArch += kernelInfo.machine;
	string::size_type archPos = runningKernel.find(kernelArch);
	if(archPos != string::npos)
	{
		// Remove arch from kernel version
		runningKernel = runningKernel.substr(0, archPos);
	}

	// Find the highest installed kernel
	// TODO: make this use the current package manager instead of hard coding rpm
	vector<string> kernelList;
	string command = "rpm --query kernel --queryformat %{VERSION}-%{RELEASE}\\n";
	int rpmRetval = 0;
	mySystem(&command, kernelList, &rpmRetval);

	// If rpm command wasn't successful or returned zero results, we don't
	// know for sure what kernels are installed.
	if(rpmRetval != 0 || kernelList.empty())
	{
		return RebootState::Unknown;
	}

	// Compare the running kernel with the newest kernel.
	// The last one in the list should be the most recent.
	if(runningKernel == kernelList[kernelList.size() - 1])
	{
		return RebootState::No;
	}
	else
	{
		return RebootState::Yes;
	}
}
