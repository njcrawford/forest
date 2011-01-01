#!/usr/bin/php
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

$db_upgrade_mode = true;
require "www/inc/db.php";

function upgrade_1_to_2()
{
	$result = mysql_query("update settings set value = '2' where name = 'db_version'");
	if(!$result)
	{
		die("Failed setting new db schema version");
	}

	$result = mysql_query("alter table systems add reboot_accepted tinyint(1) NOT NULL DEFAULT '0'");
	if(!$result)
	{
		die("Failed to alter systems table");
	}
	$result = mysql_query("alter table systems add allow_reboot tinyint(1) NOT NULL DEFAULT '0'");
	if(!$result)
	{
		die("Failed to alter systems table");
	}

	$result = mysql_query("CREATE TABLE `update_locks` (`system_id` int(11) DEFAULT NULL, `package_name` text) ENGINE=MyISAM DEFAULT CHARSET=latin1");
	if(!$result)
	{
		die("Failed to add update_locks table");
	}
}

$result = mysql_query("select value from settings where name = 'db_version'");
$row = mysql_fetch_assoc($result);
$current_db_version = $row['value'];

if(DB_VERSION == 2)
{
	if($current_db_version == '1')
	{
		upgrade_1_to_2();
	}
	else
	{
		echo "This script does not know how to update DB schema version '" . $current_db_version . "' to version '" . DB_VERSION . "'";
	}
}
else
{
	echo "This script has not been updated for DB schema version '" . DB_VERSION . "'";
}
?>
