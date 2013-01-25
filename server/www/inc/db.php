<?php
/*
Forest - a web-based multi-system update manager

Copyright (C) 2013 Nathan Crawford
 
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

require "config-file.php";

// Defines
define("DB_VERSION", 3);

mysql_connect($config['db_hostname'], $config['db_username'], $config['db_password']) or die("Could not connect to DB");
mysql_select_db($config['db_name']) or die("Could not select DB");

if(!isset($db_upgrade_mode))
{
	$version_result = mysql_query("select value from settings where name = 'db_version'");
	$version_row = mysql_fetch_assoc($version_result);
	if(DB_VERSION > $version_row['value'])
	{
		die("Database schema needs updated");
	}
	else if(DB_VERSION < $version_row['value'])
	{
		die("Database schema version too new! Expected: " . DB_VERSION . ", Found: " . $version_row['value']);
	}
}
?>
