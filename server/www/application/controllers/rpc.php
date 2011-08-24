<?php
class RPC extends Controller {

	function index()
	{
		//default function for this controller
		//should display some kind of message pointing the user to the browser site
	}
	
	function collect()
	{
		require "inc/db.php";
require "inc/rpc-common.php";

if(empty($_POST['system_name']))
{
	echo RPC_ERROR_TAG . "No system specified";
}

$system_id_ok = false;
$update_data_ok = false;
$system_name = mysql_real_escape_string($_POST['system_name']);
$result = mysql_query("select * from systems where name = '" . $system_name . "'");
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
	$use_versions = false;
	// Forget about old updates before adding new ones
	mysql_query("update systems set last_checkin = NOW() where id = '" . $system_id . "'");
	mysql_query("delete from updates where system_id = '" . $system_id . "'");
	$packages = explode(",", $_POST['available_updates']);
	if(!empty($_POST['versions']))
	{
		$versions = explode("|", $_POST['versions']);
		if(count($versions) == count($packages))
		{
			$use_versions = true;
		}
	}
	// build an SQL query to save info for all packages that need updated
	for($i = 0; $i < count($packages); $i++)
	{
		$nice_version = $use_versions ? "'" . mysql_real_escape_string($versions[$i]) . "'" : "null";
		$result = mysql_query("insert into updates (system_id, package_name, version) values
			(
				'" . $system_id . "',
				'" . mysql_real_escape_string($packages[$i]) . "',
				" . $nice_version . "
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
// We want to update the reboot_required flag, even if it is null
// Note: reboot_required is already escaped
$query = "update systems set reboot_required = " . $reboot_required;
// reset the accepted flag if the system no longer needs a reboot, or a reboot was attempted
if($reboot_required != "'1'" || (!empty($_POST['reboot_attempted']) && $_POST['reboot_attempted'] == "true"))
{
	$query .= ", reboot_accepted = '0'";
}
// Collect reported client capabilities, if present
$query .= ", can_apply_updates = ";
if(!empty($_POST['client_can_apply_updates']) && $_POST['client_can_apply_updates'] == "true")
{
	$query .= "'1'";
}
else
{
	$query .= "'0'";
}
$query .= ", can_apply_reboot = ";
if(!empty($_POST['client_can_apply_reboot']) && $_POST['client_can_apply_reboot'] == "true")
{
	$query .= "'1'";
}
else
{
	$query .= "'0'";
}
$query .= " where id = '" . $system_id . "'";

mysql_query($query);
	}
	
	function get_accepted()
	{
	require "inc/db.php";
require "inc/rpc-common.php";

if(empty($_GET['system']))
{
	die(RPC_ERROR_TAG . "No system specified");
}


$system_result = mysql_query("select * from systems where name = '" . mysql_real_escape_string($_GET['system']) . "'");
if(!$system_result)
{
	die(RPC_ERROR_TAG . "Mysql error: " . mysql_error());
}
if(mysql_num_rows($system_result) != 1)
{
	// This error will be thrown if the system is not found, or if more than
	// one system in the database has the same name.
	die(RPC_ERROR_TAG . "System not found in database");
}
$system_row = mysql_fetch_assoc($system_result);

$updates_result = mysql_query("select updates.package_name from updates 
    left outer join (update_locks) 
    on 
    (
        updates.system_id = update_locks.system_id and 
        updates.package_name = update_locks.package_name
    ) 
    where 
        updates.system_id = '" . mysql_real_escape_string($system_row['id']) . "' and 
        update_locks.package_name is null and 
        updates.accepted = '1'"
);
if(!$updates_result)
{
	die(RPC_ERROR_TAG . "Mysql error: " . mysql_error());
}

echo RPC_SUCCESS_TAG;

if($system_row['reboot_accepted'] == 1)
{
	echo "reboot-true: ";
}
else
{
	echo "reboot-false: ";
}
$updates_row = mysql_fetch_assoc($updates_result);
while($updates_row)
{
	echo $updates_row['package_name'] . " ";
	$updates_row = mysql_fetch_assoc($updates_result);
}
	}
	
}
?>
