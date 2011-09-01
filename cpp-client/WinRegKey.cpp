// no point in trying to compile this file on anything except windows
#ifdef _WIN32

#include "WinRegKey.h"
#include <windows.h>

rebootState WinRegKey::isRebootNeeded()
{
	// found this on a forum, may be able to use this key for our purposes
	// presence of this key indicates a reboot is needed and keys inside it indicate
	// HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\CurrentVersion\WindowsUpdate\Auto Update\RebootRequired

	HKEY hKey = 0;
	char buf[255] = {0};
	DWORD dwType = 0;
	DWORD dwBufSize = sizeof(buf);
	const char* subkey = "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\WindowsUpdate\\Auto Update\\RebootRequired";
	 
	if( RegOpenKey(HKEY_LOCAL_MACHINE,subkey,&hKey) == ERROR_SUCCESS)
	{
		RegCloseKey(hKey);	
		return yes;
	}
	else
	{
		return no;
	}

}

#endif // #ifdef _WIN32
