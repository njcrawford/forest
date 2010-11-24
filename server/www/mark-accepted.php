<?php

require_once "inc/redirect.php";

// make sure the basic required POST stuff is here
if(!isset($_POST['system_id']) && !isset($_POST['package']))
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
$query = "update updates set accepted = '" . $nice_accepted . "' where ";
if(isset($_POST['system_id']) && isset($_POST['package']))
{
	// specific system/package combo
	$query .= "system_id = '" . $_POST['system_id'] . "' and package_name = '" . $_POST['package']. "'";
}
elseif(isset($_POST['system_id']))
{
	// all updates for a system
	$query .= "system_id = '" . $_POST['system_id'] . "'";
}
elseif(isset($_POST['package']))
{
	// all systems for a specific package
	$query .= "package_name = '" . $_POST['package'] . "'";
}

require "inc/db.php";

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
