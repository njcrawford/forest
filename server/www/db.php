<?php

$updates_table = "updates";

$db_server = "localhost";
$db_user = "forest_user";
$db_password = "forest_pass";
$db_name = "forest";

mysql_connect($db_server, $db_user, $db_password) or die("Could not connect to DB");
mysql_select_db($db_name) or die("Could not select DB");
?>
