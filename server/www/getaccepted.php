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

require "inc/db.php";
require "inc/rpc-common.php";

if(empty($_GET['system']))
{
	die(RPC_ERROR_TAG . "No system specified");
}


$system_result = mysql_query("select * from systems where name = '" . $_GET['system'] . "'");
$system_row = mysql_fetch_assoc($system_result);

$updates_result = mysql_query("select * from updates left outer join (update_locks) on (updates.system_id = update_locks.system_id and updates.package_name = update_locks.package_name) where updates.system_id = '" . $system_row['id'] . "' and update_locks.package_name is null");

$updates_row = mysql_fetch_assoc($updates_result);

echo RPC_SUCCESS_TAG;

while($updates_row)
{
	echo $updates_row['package_name'] . " ";
	$updates_row = mysql_fetch_assoc($updates_result);
}

?>
