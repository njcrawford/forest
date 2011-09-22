#ifndef _FOREST_CONFIG_H
#define _FOREST_CONFIG_H

#ifdef _WIN32
#define CONFIG_FILE_PATH "forest-client.conf"
#else
#define CONFIG_FILE_PATH "/etc/forest-client.conf"
#endif
#define DEFAULT_SERVER_URL "http://forest/forest"

// for now, define what PM to use at compile time (from list above)
// valid options are _APTGET (apt-get), _YUM (yum) and _WUAAPI (Windows 
// update agent API)
//#define PACKAGE_MANAGER_APTGET
//#define PACKAGE_MANAGER_YUM
//#define PACKAGE_MANAGER_WUAAPI
//#define PACKAGE_MANAGER_MACSU

// If no reboot manager is defined, REBOOTSTUB will be used
//#define REBOOT_MANAGER_FILEPRESENCE
//#define REBOOT_MANAGER_KERNELDIFFERENCE
//#define REBOOT_MANAGER_WINREGKEY
//#define REBOOT_MANAGER_REBOOTSTUB

#endif
