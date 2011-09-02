#include <stdlib.h>
#include <string>
#include <iostream>

// windows update api
#include <wuapi.h>
#include <wuerror.h>
#include <comdef.h>

#include "wuaapi.h"
#include "forest-client.h"

void WuaApi::getAvailableUpdates(vector<updateInfo> & outList)
{
	CoInitialize(NULL); // absolutely essential: initialize the COM subsystem

	IUpdateSession * uSession;
	BSTR searchString = BSTR("IsInstalled=0 and Type='Software'");
    IUpdateSearcher * uSearcher;// = uSession.CreateUpdateSearcher();
	ISearchResult * uResult;
	IUpdateCollection * uUpdates;
	LONG updateCount;
	HRESULT result;
	//                                                   CLSCTX_ALL?
	result = CoCreateInstance(CLSID_UpdateSession, NULL, CLSCTX_INPROC_SERVER, IID_IUpdateSession, (LPVOID*)&uSession);
	if(result != S_OK)
	{
		cerr << "Failed to create IUpdateSession COM object, error " << result << endl;
		cerr << "Exiting..." << endl;
		exit(1);
	}
	uSession->CreateUpdateSearcher(&uSearcher);
	uSearcher->Search(searchString, &uResult);
    //ISearchResult uResult = uSearcher.Search("IsInstalled=0 and Type='Software'");
	
	uResult->get_Updates(&uUpdates);
	
	uUpdates->get_Count(&updateCount);
    for (long i = 0; i < updateCount; i++)
    {
		IUpdate * thisUpdate;
		uUpdates->get_Item(i, &thisUpdate);
		VARIANT_BOOL autoSelect;
		thisUpdate->get_AutoSelectOnWebSites(&autoSelect);
        if (autoSelect)
        {
			updateInfo info;
			IStringCollection * KBList;
			BSTR name;
			BSTR desc;

			info.name = "KB";
			thisUpdate->get_KBArticleIDs(&KBList);
			KBList->get_Item(0, &name);
			info.name += _bstr_t(name);
			thisUpdate->get_Title(&desc);
			info.version = _bstr_t(desc);

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
	// I don't think there is a command that will do this directly under 
	// windows.
}
