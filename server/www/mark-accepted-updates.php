<?php
/*
Forest - a web-based multi-system update manager

Copyright (C) 2011 Nathan Crawford
 
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

require "inc/check-login.php";
require "inc/db.php";
require_once "inc/redirect.php";

// make sure the basic required POST stuff is here
if(empty($_POST['system_id']) && empty($_POST['package_name']))
{
	die("No package or system specified");
}
if(!isset($_POST['accepted']))
{
	die("No accepted value");
}
elseif($_POST['accepted'] != "true" && $_POST['accepted'] != "false")
{
	die("Invalid accepted value");
}
$nice_accepted = ($_POST['accepted'] == "true") ? 1 : 0;
$query = "update updates left join (update_locks) 
	on (updates.package_name = update_locks.package_name) 
	set updates.accepted = '" . $nice_accepted . "' where update_locks.package_name is null and ";
if(!empty($_POST['system_id']) && !empty($_POST['package_name']))
{
	// specific system/package combo
	$query .= "updates.system_id = '" . mysql_real_escape_string($_POST['system_id']) . "' and updates.package_name = '" . mysql_real_escape_string($_POST['package_name']) . "'";
}
elseif(!empty($_POST['system_id']))
{
	// all updates for a system
	$query .= "updates.system_id = '" . mysql_real_escape_string($_POST['system_id']) . "'";
}
elseif(!empty($_POST['package']))
{
	// all systems for a specific package
	$query .= "updates.package_name = '" . mysql_real_escape_string($_POST['package_name']) . "'";
}

$result = mysql_query($query);
if($result)
{
	redirect_back();
}
else
{
	echo "Error: " . mysql_error();
}
?>
