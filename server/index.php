<html>
<head>
<title>Forest</title>
</head>
<body>
<table>
<?php

$updates_table = "updates";

$db_server = "localhost";
$db_user = "forest_user";
$db_password = "forest_pass";
$db_name = "forest";

mysql_connect($db_server, $db_user, $db_password);
mysql_select_db($db_name);

$result = mysql_query("select system_id, count(package_name) as packages from " . $updates_table . " group by system_id");

$row = mysql_fetch_assoc($result);
while($row)
{
	echo "<tr><td>" . $row['system_id'] . "</td><td>" . $row['packages'] . "</td></tr>";

	$row = mysql_fetch_assoc($result);
}

?>
</table>
</body>
</html>
