//#############################################################################
// Forest - a web-based multi-system update manager
//
// Copyright (C) 2013 Nathan Crawford
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
// 02111-1307, USA.
//
// A copy of the full GPL 2 license can be found in the docs directory.
// You can contact me at http://www.njcrawford.com/contact
//#############################################################################

// exit, needed on linux only?
#include <cstdlib>

// cerr, cout
#include <iostream>

// string functions
#include <string.h>

#include "forest-client.h"
#include "version.h"
#include "exitcodes.h"
#include "verboselevels.h"

using namespace std;

int main(int argc, char** args)
{
	ForestClient * client = new ForestClient();
	
	for(int argNum = 1; argNum < argc; argNum++)
	{
		if(strcmp(args[argNum], "--cron") == 0)
		{
			client->setVerboseLevel(VERBOSE_QUIET);
		}
		else if(strcmp(args[argNum], "--help") == 0)
		{
			cout << "Command line switches:" << endl;
			cout << "--cron    Suppress all output unless an error occurs" << endl;
			cout << "--help    Show this list" << endl;
			cout << "--verbose Output extra detail about actions" << endl;
			cout << "--version Show forest client version and exit" << endl;
			cout << "--wait    Wait for enter keypress before terminating" << endl;
			exit(EXIT_CODE_OK);
		}
		else if(strcmp(args[argNum], "--verbose") == 0)
		{
			client->setVerboseLevel(VERBOSE_EXTRA);
		}
		else if(strcmp(args[argNum], "--version") == 0)
		{
			cout << "Forest client version " << FOREST_VERSION << endl;
			exit(EXIT_CODE_OK);
		}
		else if(strcmp(args[argNum], "--wait") == 0)
		{
			client->setWaitMode(true);
		}
		else
		{
			cout << "Unrecognized switch: " << args[argNum] << endl;
			exit(EXIT_CODE_INVALIDSWITCH);
		}
	}

	return client->run();
}
