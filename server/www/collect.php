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

require "inc/db.php";
require "inc/rpc-common.php";

if(empty($_POST['system_name']))
{
	echo RPC_ERROR_TAG . "No system specified";
}

$system_id_ok = false;
$update_data_ok = false;
$system_name = mysql_real_escape_string($_POST['system_name']);
$result = mysql_query("select * from systems where name = '" . $_POST['system_name'] . "'");
if($result)
{
	$row = mysql_fetch_assoc($result);
	$system_id = $row['id'];
	if(!empty($system_id))
	{
		$system_id_ok = true;
	}
}

if(!$system_id_ok)
{
	$result = mysql_query("insert into systems (name) values('" . $system_name . "')");
	if($result)
	{
		$result = mysql_query("select LAST_INSERT_ID() as id");
		$row = mysql_fetch_assoc($result);
		$system_id = $row['id'];
	}
	else
	{
		die(RPC_ERROR_TAG . "Mysql error: " . mysql_error());
	}
}

if(!empty($_POST['available_updates']))
{
	$data_ok = true;
	// Forget about old updates before adding new ones
	mysql_query("update systems set last_checkin = NOW() where id = '" . $system_id . "'");
	mysql_query("delete from updates where system_id = '" . $system_id . "'");
	$packages = explode(",", $_POST['available_updates']);
	// build an SQL query to save info for all packages that need updated
	foreach($packages as $this_package)
	{
		$result = mysql_query("insert into updates (system_id, package_name) values
			(
				'" . $system_id . "',
				'" . mysql_real_escape_string($this_package) . "'
			)"
		);
		if(!$result)
		{
			$data_ok = false;
			break;
		}
	}
	//send back a message indicating data received (or not)
	if($data_ok)
	{
		echo RPC_SUCCESS_TAG;
		$update_data_ok = true;
	}
	else
	{
		echo RPC_ERROR_TAG . "data error";
	}
}
elseif(!empty($_POST['no_updates_available']))
{
	// Forget about old updates and save checkin time
	mysql_query("update systems set last_checkin = NOW() where id = '" . $system_id . "'");
	mysql_query("delete from updates where system_id = '" . $system_id . "'");
	echo RPC_SUCCESS_TAG;
}
else
{
	die(RPC_ERROR_TAG . "missing both available_updates and no_updates_available");
}

// the reboot_required section is optional for rpc v1
$reboot_required = "null";
if(!empty($_POST['reboot_required']))
{
	switch($_POST['reboot_required'])
	{
		case "true":
			$reboot_required = "'1'";
			break;
		case "false":
			$reboot_required = "'0'";
			break;
	}
}
// force reboot_required to 0 if a reboot was attempted
if($reboot_required == "'1'" && !empty($_POST['reboot_attempted']) && $_POST['reboot_attempted'] == "true")
{
	$reboot_required = "'0'";
}
// we want to update the reboot_required flag, even if it is null
// reboot_required is already escaped
$query = "update systems set reboot_required = " . $reboot_required;
// reset the accepted flag if the system no longer needs a reboot, or a reboot was attempted
if($reboot_required != "'1'" || (!empty($_POST['reboot_attempted']) && $_POST['reboot_attempted'] == "true"))
{
	$query .= ", reboot_accepted = '0'";
}
$query .= " where id = '" . $system_id . "'";

mysql_query($query);

?>
