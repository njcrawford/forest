//#############################################################################
// Forest - a web-based multi-system update manager
//
// Copyright (C) 2011 Nathan Crawford
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

// cerr, cout
#include <iostream>

// string functions
#include <string.h>

using namespace std;

#include "forest-client.h"
#include "version.h"
#include "exitcodes.h"

int main(int argc, char** args)
{
	ForestClient * client = new ForestClient();
	
	for(int argNum = 2; argNum < argc; argNum++)
	{
		if(strcmp(args[argNum], "--cron") == 0)
		{
			client->setQuietMode(true);
		}
		else if(strcmp(args[argNum], "--help") == 0)
		{
			cout << "Command line switches:" << endl;
			cout << "--cron    Suppress all output unless an error occurs" << endl;
			cout << "--help    Show this list" << endl;
			cout << "--verbose Output extra detail about what forest is doing (not yet implemented)" << endl;
			cout << "--version Show forest client version and exit" << endl;
			exit(EXIT_CODE_OK);
		}
		else if(strcmp(args[argNum], "--verbose") == 0)
		{
			cout << "Verbose mode is not yet implemented" << endl;
		}
		else if(strcmp(args[argNum], "--version") == 0)
		{
			cout << "Forest client version " << getForestVersion() << endl;
			exit(EXIT_CODE_OK);
		}
		else
		{
			cout << "Unrecognized switch: " << args[argNum] << endl;
			exit(EXIT_CODE_INVALIDSWITCH);
		}
	}

	return client->run();
}
