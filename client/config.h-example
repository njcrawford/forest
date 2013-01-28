#ifndef _FOREST_CONFIG_H
#define _FOREST_CONFIG_H

#ifdef _WIN32
#define CONFIG_FILE_PATH "forest-client.conf"
#else
#define CONFIG_FILE_PATH "/etc/forest-client.conf"
#endif
#define DEFAULT_SERVER_URL "http://forest/forest"

// for now, define what PM to use at compile time 
// this should be the name of the class to use
//#define PACKAGE_MANAGER AptGet
//#define PACKAGE_MANAGER Yum
//#define PACKAGE_MANAGER WuaApi
//#define PACKAGE_MANAGER MacSU

// RebootStub can be used if there isn't a working manager for this OS
//#define REBOOT_MANAGER FilePresence
//#define REBOOT_MANAGER KernelDifference
//#define REBOOT_MANAGER WinRegKey
#define REBOOT_MANAGER RebootStub

#endif
