REM Run this script to configure the appliation for the current platform.
REM The batch file version is currently for Windows only.

set config=config.h

echo // This file is automatically generated by the configure script. > %config%
echo // Any changes made here will be overwritten next time the script is run. >> %config%
echo // See config.h-example for comments about options. >> %config%

echo #ifndef _FOREST_CONFIG_H >> %config%
echo #define _FOREST_CONFIG_H >> %config%
echo #define CONFIG_FILE_PATH "forest-client.conf" >> %config%
echo #define DEFAULT_SERVER_URL "http://forest/forest" >> %config%

echo #define PACKAGE_MANAGER WuaApi >> %config%
echo #define REBOOT_MANAGER WinRegKey >> %config%

echo #endif >> %config%

