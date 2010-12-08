<?php

$forest_config['db_server'] = "localhost";
$forest_config['db_user'] = "forest_user";
$forest_config['db_password'] = "forest_pass";
$forest_config['db_name'] = "forest";
define("DB_VERSION", 1);

mysql_connect($forest_config['db_server'], $forest_config['db_user'], $forest_config['db_password']) or die("Could not connect to DB");
mysql_select_db($forest_config['db_name']) or die("Could not select DB");

$version_result = mysql_query("select value from settings where name = 'db_version'");
$version_row = mysql_fetch_assoc($version_result);
if(DB_VERSION > $version_row['value'])
{
	die("Database schema needs updated");
}
else if(DB_VERSION < $version_row['value'])
{
	die("Database schema version too new! Expected: " . DB_VERSION . ", Found: " . $version_row['value']);
}
?>
