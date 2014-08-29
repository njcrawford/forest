// cerr, cout
#include <iostream>
using namespace std;

#include "Rpcv2.h"
#include "helpers.h"
#include "exitcodes.h"
#include "verboselevels.h"

// curl
#include <curl/curl.h>


Rpcv2::Rpcv2(const string & serverUrl, const string & myHostname) : ForestRpc(serverUrl, myHostname)
{
	rebootAccepted = false;
}

bool Rpcv2::checkForServerSupport()
{
	// TODO: Figure out a way to see if the server actually supports v2
	return true;
}

// CURL callback
size_t write_data(void *buffer, size_t size, size_t nmemb, void *userp)
{
	char * charBuf = (char *)buffer;
	string * data = (string *)userp;
	data->append(charBuf, nmemb);
	return nmemb;
}

void Rpcv2::checkInWithServer(ForestClientCapabilities & caps, vector<updateInfo> & availableUpdates, rebootState rebootNeeded, bool rebootAttempted, int verboseLevel)
{
	string command;

	command = "rpc_version=2&system_name=" + myHostname;

	command += "&client_can_apply_updates=";
	if(caps.canApplyUpdates)
	{
		command += "true";
	}
	else
	{
		command += "false";
	}

	command += "&client_can_apply_reboot=";
	if(caps.canApplyReboot)
	{
		command += "true";
	}
	else
	{
		command += "false";
	}

	if(availableUpdates.size() == 0)
	{
		command += "&no_updates_available=true";
	}
	else
	{
		// this output also needs to be escaped for HTML
		command += "&available_updates=";
		for(size_t i = 0; i < availableUpdates.size(); i++)
		{
			if(i != 0)
			{
				command += ",";
			}
			command += availableUpdates[i].name;
		}
		// also add version info
		command += "&versions=";
		for(size_t i = 0; i < availableUpdates.size(); i++)
		{
			if(i != 0)
			{
				command += "|";
			}
			command += availableUpdates[i].version;
		}
	}

	command += "&reboot_required=";
	if(rebootNeeded == rebootState::yes)
	{
		command += "true";
	}
	else if(rebootNeeded == rebootState::no)
	{
		command += "false";
	}
	else
	{
		command += "unknown";
	}

	command += "&reboot_attempted=";
	if(rebootAttempted)
	{
		command += "true";
	}
	else
	{
		command += "false";
	}

	string collectUrl = serverUrl;
	if(collectUrl[collectUrl.size() - 1] != '/')
	{
		collectUrl += "/";
	}
	collectUrl += "collect.php";

	CURL * curlHandle;
	CURLcode res = CURLE_OK;
	string curlOutput;
	curlHandle = curl_easy_init();
	if(curlHandle)
	{
		curl_easy_setopt(curlHandle, CURLOPT_POSTFIELDS, command.c_str());
		curl_easy_setopt(curlHandle, CURLOPT_URL, collectUrl.c_str());
		curl_easy_setopt(curlHandle, CURLOPT_WRITEFUNCTION, write_data);
		curl_easy_setopt(curlHandle, CURLOPT_WRITEDATA, &curlOutput);
		res = curl_easy_perform(curlHandle);

		//cleanup
		curl_easy_cleanup(curlHandle);
	}

	// exit if curl fails
	if(res != CURLE_OK)
	{
		cerr << "cURL failed: " << curl_easy_strerror(res) << endl << curlOutput << endl;
		exit(EXIT_CODE_CURL);
	}

	if( (verboseLevel >= VERBOSE_EXTRA) || ((verboseLevel == VERBOSE_QUIET) && curlOutput.substr(0, 8) != "data_ok:"))
	{
		cout << curlOutput << endl;
	}
}

vector<string> Rpcv2::getAcceptedUpdates()
{
	vector<string> retval;
	string acceptedUrl;
	string command;

	// build accepted URL
	acceptedUrl = serverUrl;
	if(acceptedUrl[acceptedUrl.size() - 1] != '/')
	{
		acceptedUrl += "/";
	}
	acceptedUrl += "getaccepted.php?rpc_version=2&system=" + myHostname;

	// run curl command
	CURL * curlHandle;
	CURLcode res;
	string curlOutput;
	curlHandle = curl_easy_init();
	if(curlHandle)
	{
		curl_easy_setopt(curlHandle, CURLOPT_URL, acceptedUrl.c_str());
		curl_easy_setopt(curlHandle, CURLOPT_WRITEFUNCTION, write_data);
		curl_easy_setopt(curlHandle, CURLOPT_WRITEDATA, &curlOutput);
		res = curl_easy_perform(curlHandle);

		//cleanup
		curl_easy_cleanup(curlHandle);
	}

	// exit if curl fails
	if(res != CURLE_OK)
	{
		cerr << "cURL failed: " << curl_easy_strerror(res) << endl << curlOutput << endl;
		exit(EXIT_CODE_CURL);
	}

	// make sure the response was something we expected
	if(curlOutput.substr(0, 8) != "data_ok:")
	{
		// lack of data_ok: is ok if this system is new to the server
		if(curlOutput.find("System not found in database") != string::npos)
		{
			retval.clear();
		}
		else
		{
			cerr << "Error getting accepted updates: " << curlOutput << endl;
			exit(EXIT_CODE_RESPONSEDATA);
		}
	}
	else
	{
		string::size_type position;
		string::size_type position2;

		// check for data_ok:
		if(curlOutput.substr(0,8) != "data_ok:")
		{
			cerr << "Error getting accepted updates: " << curlOutput << endl;
			exit(EXIT_CODE_RESPONSEDATA);
		}

		// check for reboot-(true|false)
		position = curlOutput.find(':', 0);
		position2 = curlOutput.find(':', position + 1);
		if(position == string::npos)
		{
			cerr << "Error getting accepted updates: " << curlOutput << endl;
			exit(EXIT_CODE_RESPONSEDATA);
		}
		// don't die if reboot state isn't present
		rebootAccepted = false;
		if(position2 != string::npos)
		{
			string rebootState = trim_string(curlOutput.substr(position + 1, position2 - position - 1));
			if(rebootState == "reboot-true")
			{
				rebootAccepted = true;
			}
		}
		else
		{
			// for trimming output, below
			position2 = position;
		}

		// trim data_ok and reboot state from output
		curlOutput = curlOutput.substr(position2 + 1);

		retval.clear();

		// if there are no updates, the string will be empty (because of trim_string)
		if(trim_string(curlOutput).size() == 0)
		{
			return retval;
		}

		string::size_type startPosition = 0;
		string acceptedUpdate;
		for(position = curlOutput.find(' ', 0); position != string::npos; position = curlOutput.find(' ', position + 1))
		{
			acceptedUpdate = curlOutput.substr(startPosition, position - startPosition);
			if(acceptedUpdate.size() > 0)
			{
				//cerr << "DEBUG: accepted update " << acceptedUpdate << endl;
				retval.push_back(acceptedUpdate);
			}
			startPosition = position + 1;
			//curlOutput[0] = curlOutput[0].substr(position + 1);
		}
		//add the last item (whatever is after the last space) if it's not empty
		acceptedUpdate = curlOutput.substr(startPosition);
		if(trim_string(acceptedUpdate).size() > 0)
		{
			//cerr << "DEBUG: accepted update " << acceptedUpdate << endl;
			retval.push_back(acceptedUpdate);
		}
	}

	return retval;
}

bool Rpcv2::getRebootAccepted()
{
    return rebootAccepted;
}
