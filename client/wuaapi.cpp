#ifdef _WIN32

#include <stdlib.h>
#include <string>
#include <iostream>

// windows update api
#include <wuapi.h>
#include <wuerror.h>
#include <comdef.h>

#include "wuaapi.h"
#include "forest-client.h"
#include "exitcodes.h"

void WuaApi::getAvailableUpdates(vector<updateInfo> & outList)
{
	CoInitialize(NULL); // absolutely essential: initialize the COM subsystem

	IUpdateSession * uSession;
	BSTR searchString = BSTR("IsInstalled=0 and Type='Software'");
    IUpdateSearcher * uSearcher;
	ISearchResult * uResult;
	IUpdateCollection * uUpdates;
	LONG updateCount;
	LONG KBCount;
	HRESULT result;
	//                                                   CLSCTX_ALL?
	result = CoCreateInstance(CLSID_UpdateSession, NULL, CLSCTX_INPROC_SERVER, IID_IUpdateSession, (LPVOID*)&uSession);
	if(result != S_OK)
	{
		cerr << "Failed to create IUpdateSession COM object, error " << result << endl;
		cerr << "Exiting..." << endl;
		exit(EXIT_CODE_WUAAPI_CREATE_COM);
	}
	uSession->CreateUpdateSearcher(&uSearcher);
	result = uSearcher->Search(searchString, &uResult);
	if(result != S_OK)
	{
		cerr << "Failed to get results from IUpdateSearcher COM object, error " << result << endl;
		cerr << "Exiting..." << endl;
		exit(EXIT_CODE_WUAAPI_UPDATE_SEARCH);
	}
	
	result = uResult->get_Updates(&uUpdates);
	if(result != S_OK)
	{
		cerr << "Failed to get list of updates from ISearchResult COM object, error " << result << endl;
		cerr << "Exiting..." << endl;
		exit(EXIT_CODE_WUAAPI_UPDATE_LIST);
	}
	
	result = uUpdates->get_Count(&updateCount);
	if(result != S_OK)
	{
		cerr << "Failed to get update count from IUpdateCollection COM object, error " << result << endl;
		cerr << "Exiting..." << endl;
		exit(EXIT_CODE_WUAAPI_GET_COUNT);
	}
	for (long i = 0; i < updateCount; i++)
	{
		IUpdate * thisUpdate;

		result = uUpdates->get_Item(i, &thisUpdate);
		if(result != S_OK)
		{
			cerr << "Failed to get an update from IUpdateCollection COM object, error " << result << endl;
			cerr << "Exiting..." << endl;
			exit(EXIT_CODE_WUAAPI_GET_ITEM);
		}

		// Query string above doesn't seem to have any effect, so we'll check
		// each update ourselves.
		VARIANT_BOOL autoSelect;
		VARIANT_BOOL hidden;
		thisUpdate->get_AutoSelectOnWebSites(&autoSelect);
		if(result != S_OK)
		{
			cerr << "Failed to get AutoSelectOnWebSites from IUpdate COM object, error " << result << endl;
			cerr << "Exiting..." << endl;
			exit(EXIT_CODE_WUAAPI_GET_AUTOSELECT);
		}
		thisUpdate->get_IsHidden(&hidden);
		if(result != S_OK)
		{
			cerr << "Failed to get IsHidden from IUpdate COM object, error " << result << endl;
			cerr << "Exiting..." << endl;
			exit(EXIT_CODE_WUAAPI_GET_ISHIDDEN);
		}

		if(!hidden && autoSelect)
		{
			updateInfo info;
			IStringCollection * KBList;
			BSTR name;
			BSTR desc;

			result = thisUpdate->get_Title(&desc);
			if(result != S_OK)
			{
				cerr << "Failed to get KB title from IStringCollection COM object, error " << result << endl;
				cerr << "Exiting..." << endl;
				exit(EXIT_CODE_WUAAPI_GET_TITLE);
			}
			info.version = _bstr_t(desc);

			result = thisUpdate->get_KBArticleIDs(&KBList);
			if(result != S_OK)
			{
				cerr << "Failed to get list of KB articles from IUpdate COM object, error " << result << endl;
				cerr << "Exiting..." << endl;
				exit(EXIT_CODE_WUAAPI_GET_KB_LIST);
			}

			result = KBList->get_Count(&KBCount);
			if(result != S_OK)
			{
				cerr << "Failed to get KB count from IStringCollection COM object, error " << result << endl;
				cerr << "Exiting..." << endl;
				exit(EXIT_CODE_WUAAPI_GET_KB_COUNT);
			}
			if(KBCount > 0)
			{
				result = KBList->get_Item(0, &name);
				if(result != S_OK)
				{
					cerr << "Failed to get KB name from IStringCollection COM object, error " << result << endl;
					cerr << "Exiting..." << endl;
					exit(EXIT_CODE_WUAAPI_GET_KB_ITEM);
				}
				info.name = "KB";
				info.name += _bstr_t(name);
			}
			else
			{
				info.name = "(No KB Available)";
			}

			// add it to the list
			outList.push_back(info);

			// release COM objects
			KBList->Release();
		}

		// release COM objects
		thisUpdate->Release();
	}

	// release COM objects
	uUpdates->Release();
	uResult->Release();
	uSearcher->Release();
	uSession->Release();

	CoUninitialize(); // cleanup COM after you're done using its services
}

void WuaApi::applyUpdates(vector<string> & list)
{
	//should be able to modify the code used to check for updates, it looks like it should be just a different query
}

#endif // _WIN32
