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

$systems_result = mysql_query("select * from systems");

$systems_row = mysql_fetch_assoc($systems_result);
while($systems_row)
{
	$updates_result = mysql_query("select * from updates where system_id = '" . $systems_row['id'] . "'");
	$updates_row = mysql_fetch_assoc($updates_result);
	
	if($systems_row['reboot_required'] == null)
	{
		$nice_reboot = "Unknown";
	}
	elseif($systems_row['reboot_required'] == 1)
	{
		$nice_reboot = "Yes";
	}
	else
	{
		$nice_reboot = "No";
	}

	$updates_result = mysql_query("select count(package_name) as packages from updates where system_id = '" . $systems_row['id'] . "'");
        $updates_row = mysql_fetch_assoc($updates_result);

?>
	<tr>
		<td>
			<a href="systems.php?name=<? echo $systems_row['name'] ?>"><? echo $systems_row['name'] ?></a>
		</td>
		<td><? echo $updates_row['packages'] ?></td>
		<td><? echo $nice_reboot ?></td>
		<td><? echo $systems_row['last_checkin'] ?></td>
<?php
		if($updates_row['packages'] > 0)
		{
?>
		<td>
			<form method="post" action="mark-accepted.php">
				<input type="hidden" name="accepted" value="true">
				<input type="hidden" name="system_id" value="<? echo $systems_row['id'] ?>">
				<input type="submit" value="Accept all">
			</form>
		</td>
<?php
		}
?>
	</tr>
<?php

	$systems_row = mysql_fetch_assoc($systems_result);
}

?>
</table>
<br />
Updates available by package name
<table>
<tr><td>Name</td><td>Systems</td></tr>
<?php
$systems_result = mysql_query("select package_name, count(system_id) as systems from updates group by package_name");

$systems_row = mysql_fetch_assoc($systems_result);
while($systems_row)
{
?>
        <tr>
		<td>
			<a href="packages.php?name=<? echo $systems_row['package_name'] ?>"><? echo $systems_row['package_name'] ?></a>
		</td>
		<td><? echo $systems_row['systems'] ?></td>
		<td>
			<form method="post" action="mark-accepted.php">
				<input type="hidden" name="accepted" value="true">
				<input type="hidden" name="package" value="<? echo $systems_row['package_name'] ?>">
				<input type="submit" value="Accept all">
			</form>
		</td>
	</tr>
<?php

        $systems_row = mysql_fetch_assoc($systems_result);
}
?>
</table>
</body>
</html>
