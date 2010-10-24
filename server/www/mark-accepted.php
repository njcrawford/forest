<?php

function redirect_back()
{
	// send the user back to the page they just came from
	header("Location: " . $_SERVER['HTTP_REFERER'], 307);
}

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

$db_server = "localhost";
$db_user = "forest_user";
$db_password = "forest_pass";
$db_name = "forest";

mysql_connect($db_server, $db_user, $db_password);
mysql_select_db($db_name);

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
