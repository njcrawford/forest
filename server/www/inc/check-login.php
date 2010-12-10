<?php
/*
Forest - a web-based multi-system update manager

Copyright (C) 2010 Nathan Crawford
 
This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
 
You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
02111-1307, USA.

A copy of the full GPL 2 license can be found in the docs directory.
You can contact me at http://www.njcrawford.com/contact
*/

require_once "/etc/forest-server.conf";

// Only check for a valid login if required by config
if($forest_config['login_required'] != false)
{
	// check for login cookie
	if(!isset($_COOKIE['login_name']))
	{
		require_once "/etc/forest-server.conf";
		require_once "inc/redirect.php";
		// no cookie, so redirect to login.php
		redirect($forest_config['server_url'] . "/login.php");
	}
}
?>
