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

function set_accepted($system_id, $package_name, $accepted, $die_if_locked)
{
	// make sure this package isn't locked before trying to update it
	$lock_query = "select * from update_locks where system_id = '" . $system_id . "' and package_name = '" . $package_name . "'";
	$lock_result = mysql_query($lock_query);
	if(!$lock_result)
	{
		//die("Mysql error: " . mysql_error());
		return false;
	}
	if(mysql_num_rows($lock_result) == 1)
	{
		if($die_if_locked)
		{
			//die("This package is locked");
			return false;
		}
		else
		{
			// this should be enough to fake a success
			return true;
		}
	}

	// make sure the client can apply updates before trying to update it
	$can_apply_updates_query = "select can_apply_updates from systems where id = '" . $system_id . "'";
	$can_apply_updates_result = mysql_query($can_apply_updates_query);
	$can_apply_updates_row = mysql_fetch_assoc($can_apply_updates_result);
	// treat can_apply_updates the same as a lock for now
	if($can_apply_updates_row['can_apply_updates'] != 1)
	{
		if($die_if_locked)
		{
			//die("This client can not apply updates");
			return false;
		}
		else
		{
			// this should be enough to fake a success
			return true;
		}
	}
	
	$query = "update updates 
		set accepted = '" . $accepted . "' 
		where 
		system_id = '" . $system_id . "' and 
		package_name = '" . $package_name . "'";
	return mysql_query($query);
}

// make sure the basic required POST stuff is here
if(empty($_POST['system_id']) && empty($_POST['package_name']))
{
	//die("No package or system specified");
	echo "0";
}
if(!isset($_POST['action']))
{
	//die("No accepted value");
	echo "0";
}
elseif($_POST['action'] != "accept" && $_POST['action'] != "reject")
{
	//die("Invalid accepted value");
	echo "0";
}
$nice_accepted = ($_POST['action'] == "accept") ? 1 : 0;

if(!empty($_POST['system_id']) && !empty($_POST['package_name']))
{
	// specific system/package combo
	$result = set_accepted(mysql_real_escape_string($_POST['system_id']), mysql_real_escape_string($_POST['package_name']), $nice_accepted, true);
	
}
else
{
	$query = "select * from updates where ";
	if(!empty($_POST['system_id']))
	{
		// all updates for a system
		$query .= "system_id = '" . mysql_real_escape_string($_POST['system_id']) . "'";
	}
	elseif(!empty($_POST['package_name']))
	{
		// all systems for a specific package
		$query .= "package_name = '" . mysql_real_escape_string($_POST['package_name']) . "'";
	}
	$updates_result = mysql_query($query);
	for($updates_row = mysql_fetch_assoc($updates_result); $updates_row; $updates_row = mysql_fetch_assoc($updates_result))
	{
		$result = set_accepted($updates_row['system_id'], $updates_row['package_name'], $nice_accepted, false);
		if(!$result)
		{
			break;
		}
	}
}

$is_ajax = (isset($_POST['ajax']) && $_POST['ajax'] == "true");

if($result)
{
	if($is_ajax)
	{
		echo "1";
	}
	else
	{
		redirect_back();
	}
}
else
{
	if($is_ajax)
	{
		echo "0";
	}
	else
	{
		echo "Error: " . mysql_error();
	}
}
?>
