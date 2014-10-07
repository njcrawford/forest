#ifndef _EXITCODES_H
#define _EXITCODES_H

enum ExitCodes
{
	// Application finished successfully
	EXIT_CODE_OK            = 0,

	// An error occurred while working with cURL
	EXIT_CODE_CURL          = 1,
	EXIT_CODE_WSASTARTUP    = 2,
	EXIT_CODE_SOCKETERROR   = 3,
	EXIT_CODE_RESPONSEDATA  = 4,

	// Couldn't open the config file
	EXIT_CODE_CONFIGFILE    = 5,


	EXIT_CODE_HOSTNAME      = 6,

	// Invalid command line switch
	EXIT_CODE_INVALIDSWITCH = 7,

	// Don't use this any more - use a Windows Update API COM error code, below.
	// I'm leaving this value in the enum for historical reference.
	EXIT_CODE_WUAAPI        = 8,

	// Windows Update API COM error codes
	EXIT_CODE_WUAAPI_CREATE_COM     = 1001,
	EXIT_CODE_WUAAPI_UPDATE_SEARCH  = 1002,
	EXIT_CODE_WUAAPI_UPDATE_LIST    = 1003,
	EXIT_CODE_WUAAPI_GET_COUNT      = 1004,
	EXIT_CODE_WUAAPI_GET_ITEM       = 1005,
	EXIT_CODE_WUAAPI_GET_AUTOSELECT = 1006,
	EXIT_CODE_WUAAPI_GET_ISHIDDEN   = 1007,
	EXIT_CODE_WUAAPI_GET_TITLE      = 1008,
	EXIT_CODE_WUAAPI_GET_KB_LIST    = 1009,
	EXIT_CODE_WUAAPI_GET_KB_COUNT   = 1010,
	EXIT_CODE_WUAAPI_GET_KB_ITEM    = 1011,

};

#endif
