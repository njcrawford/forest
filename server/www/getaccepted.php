<?php

$updates_table = "updates";
$post_data_name = "available_updates";

$db_server = "localhost";
$db_user = "forest_user";
$db_password = "forest_pass";
$db_name = "forest";

mysql_connect($db_server, $db_user, $db_password);
mysql_select_db($db_name);

if(!empty($_GET['system']))
{
	$system_result = mysql_query("select * from systems where name = '" . $_GET['system'] . "'");
	$system_row = mysql_fetch_assoc($system_result);

	$updates_result = mysql_query("select * from updates where system_id = '" . $system_row['id'] . "'");
	$updates_row = mysql_fetch_assoc($updates_result);

	while($updates_row)
	{
		echo $updates_row['package_name'] . " ";
		$updates_row = mysql_fetch_assoc($updates_result);
	}
}
else
{
	die("No system specified");
}
?>
