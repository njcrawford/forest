<html>
<head>
<title>Forest</title>
</head>
<body>
<table>
<tr><td>System ID</td><td>Updates Available</td><td>Reboot Required</td><td>Last Checkin</td></tr>
<?php

$updates_table = "updates";

$db_server = "localhost";
$db_user = "forest_user";
$db_password = "forest_pass";
$db_name = "forest";

mysql_connect($db_server, $db_user, $db_password);
mysql_select_db($db_name);

$result = mysql_query("select * from systems");

$row = mysql_fetch_assoc($result);
while($row)
{
	$result2 = mysql_query("select count(package_name) as packages from updates where system_id = '" . $row['id'] . "'");
	$row2 = mysql_fetch_assoc($result2);
	
	echo "<tr><td>" . $row['name'] . "</td><td>" . $row2['packages'] . "</td><td>" . $row['reboot_required'] . "</td><td>" . $row['last_checkin'] . "</td></tr>";

	$row = mysql_fetch_assoc($result);
}

?>
</table>
</body>
</html>
