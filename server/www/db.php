<?php

$updates_table = "updates";

$db_server = "localhost";
$db_user = "forest_user";
$db_password = "forest_pass";
$db_name = "forest";
$built_for_db_version = '1';

mysql_connect($db_server, $db_user, $db_password) or die("Could not connect to DB");
mysql_select_db($db_name) or die("Could not select DB");

$version_result = mysql_query("select value from settings where name = 'db_version'");
$version_row = mysql_fetch_assoc($version_result);
if($built_for_db_version > $version_row['value'])
{
	die("Database schema needs updated");
}
else if($built_for_db_version < $version_row['value'])
{
	die("Database schema is newer than application");
}
?>
