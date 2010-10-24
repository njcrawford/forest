<html>
<head>
<title>Forest</title>
</head>
<body>
Updates available by system
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
	$result2 = mysql_query("select * from updates where system_id = '" . $row['id'] . "'");
	$row2 = mysql_fetch_assoc($result2);
	
	if($row['reboot_required'] == null)
	{
		$nice_reboot = "Unknown";
	}
	elseif($row['reboot_required'] == 1)
	{
		$nice_reboot = "Yes";
	}
	else
	{
		$nice_reboot = "No";
	}

	$result2 = mysql_query("select count(package_name) as packages from updates where system_id = '" . $row['id'] . "'");
        $row2 = mysql_fetch_assoc($result2);

?>
	<tr>
		<td>
			<a href="systems.php?name=<? echo $row['name'] ?>"><? echo $row['name'] ?></a>
		</td>
		<td><? echo $row2['packages'] ?></td>
		<td><? echo $nice_reboot ?></td>
		<td><? echo $row['last_checkin'] ?></td>
		<td>
			<form method="post" action="mark-accepted.php">
				<input type="hidden" name="accepted" value="true">
				<input type="hidden" name="system_id" value="<? echo $row['id'] ?>">
				<input type="submit" value="Accept all">
			</form>
		</td>
	</tr>
<?php

	$row = mysql_fetch_assoc($result);
}

?>
</table>
<br />
Updates available by package name
<table>
<tr><td>Name</td><td>Systems</td></tr>
<?php
$result = mysql_query("select package_name, count(system_id) as systems from updates group by package_name");

$row = mysql_fetch_assoc($result);
while($row)
{
?>
        <tr>
		<td>
			<a href="packages.php?name=<? echo $row['package_name'] ?>"><? echo $row['package_name'] ?></a>
		</td>
		<td><? echo $row['systems'] ?></td>
		<td>
			<form method="post" action="mark-accepted.php">
				<input type="hidden" name="accepted" value="true">
				<input type="hidden" name="package" value="<? echo $row['package_name'] ?>">
				<input type="submit" value="Accept all">
			</form>
		</td>
	</tr>
<?php

        $row = mysql_fetch_assoc($result);
}
?>
</table>
</body>
</html>
