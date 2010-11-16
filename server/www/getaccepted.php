<?php

require "inc/db.php";
require "inc/rpc-common.php";

if(empty($_GET['system']))
{
	die($RPC_ERROR_TAG ."No system specified");
}


$system_result = mysql_query("select * from systems where name = '" . $_GET['system'] . "'");
$system_row = mysql_fetch_assoc($system_result);

$updates_result = mysql_query("select * from updates where system_id = '" . $system_row['id'] . "' and accepted = '1'");
$updates_row = mysql_fetch_assoc($updates_result);

echo $RPC_SUCCESS_TAG;

while($updates_row)
{
	echo $updates_row['package_name'] . " ";
	$updates_row = mysql_fetch_assoc($updates_result);
}

?>
