<html>
<head>
<title>Forest</title>
</head>
<body>
<?php

$updates_table = "updates";

$db_server = "localhost";
$db_user = "forest_user";
$db_password = "forest_pass";
$db_name = "forest";

mysql_connect($db_server, $db_user, $db_password);
mysql_select_db($db_name);

?>
<style type="text/css">
table {
	border-width: 1px;
	border-spacing: 0px;
	border-style: outset;
	border-color: gray;
	border-collapse: collapse;
	background-color: white;
}
table th {
	border-width: 1px;
	padding: 1px;
	border-style: inset;
	border-color: gray;
	background-color: white;
	text-align: center;
	padding: 5px;
}
table td {
	border-width: 1px;
	padding: 1px;
	border-style: inset;
	border-color: gray;
	background-color: white;
	text-align: center;
	padding: 5px;
}
table td.name {
	text-align: left;
}
form {
	display: inline;
}
</style>
<?php
$awol_result = mysql_query("select * from systems where last_checkin < DATE_SUB(NOW(), INTERVAL 36 HOUR)");
if(mysql_num_rows($awol_result) > 0)
{
	echo "<br /><h3>Systems that have not checked in for more than 36 hours</h3><br />";
	echo "<table>";
	echo "<tr><th>System name</th><th>Last checkin</th></tr>";
	$awol_row = mysql_fetch_assoc($awol_result);
	while($awol_row)
	{
		echo "<tr><td>" . $awol_row['name'] . "</td><td>" . $awol_row['last_checkin'] . "</td></tr>";
		$awol_row = mysql_fetch_assoc($awol_result);
	}
	echo "</table>";
}

$systems_final = array();

$systems_result = mysql_query("select * from (select systems.id, name, count(package_name) as packages, sum(if(accepted is null, 0, accepted)) as accepted_count, reboot_required, last_checkin from systems left join (updates) on (updates.system_id = systems.id) group by systems.id) b order by packages desc");
if(mysql_num_rows($systems_result) > 0)
{
	# Get systems that have updates, sorted by number of updates
	$systems_row = mysql_fetch_assoc($systems_result);
	while($systems_row)
	{
		$systems_final[$systems_row['id']] = $systems_row;

		# Translate all the reboot_required values
		if($systems_row['reboot_required'] == null)
		{
			$systems_final[$systems_row['id']]['reboot_required'] = "Unknown";
		}
		elseif($systems_row['reboot_required'] == 1)
		{
			$systems_final[$systems_row['id']]['reboot_required'] = "Yes";
		}
		else
		{
			$systems_final[$systems_row['id']]['reboot_required'] = "No";
		}

		$systems_row = mysql_fetch_assoc($systems_result);
	}
?>
<br />
<h3>Updates available by system</h3>
<br />
<table>
<tr><th>System Name</th><th>Updates Available/Accepted</th><th>Reboot Required</th><th>Last Checkin</th></tr>
<?php
	foreach($systems_final as $this_system)
	{
?>
	<tr>
		<td class="name">
			<a href="systems.php?name=<? echo $this_system['name'] ?>"><? echo $this_system['name'] ?></a>
		</td>
		<td><? echo $this_system['packages'] . "/" . $this_system['accepted_count'] ?></td>
		<td><? echo $this_system['reboot_required'] ?></td>
		<td><? echo $this_system['last_checkin'] ?></td>
		<td>
<?php
		if($this_system['packages'] > 0 && $this_system['packages'] != $this_system['accepted_count'])
		{
?>
			<form method="post" action="mark-accepted.php">
				<input type="hidden" name="accepted" value="true">
				<input type="hidden" name="system_id" value="<? echo $this_system['id'] ?>">
				<input type="submit" value="Accept all">
			</form>
<?php
		}
		else
		{
			echo "&nbsp;";
		}
?>
		</td>
	</tr>
<?php
	}
?>
</table>
<?php
}
?>
<br />
<h3>Updates available by package name</h3>
<br />
<table>
<tr><th>Name</th><th>Systems/Accepted</th></tr>
<?php
$systems_result = mysql_query("select package_name, count(system_id) as systems, sum(accepted) as accepted_count from updates group by package_name");
$systems_row = mysql_fetch_assoc($systems_result);
while($systems_row)
{
?>
        <tr>
		<td class="name">
			<a href="packages.php?name=<? echo $systems_row['package_name'] ?>"><? echo $systems_row['package_name'] ?></a>
		</td>
		<td><? echo $systems_row['systems'] . "/" . $systems_row['accepted_count'] ?></td>
		<td>
<?php
	if($systems_row['systems'] != $systems_row['accepted_count'])
	{
?>
			<form method="post" action="mark-accepted.php">
				<input type="hidden" name="accepted" value="true">
				<input type="hidden" name="package" value="<? echo $systems_row['package_name'] ?>">
				<input type="submit" value="Accept all">
			</form>
<?php
	}
	else
	{
		echo "&nbsp;";
	}
?>
		</td>
	</tr>
<?php

        $systems_row = mysql_fetch_assoc($systems_result);
}
?>
</table>
</body>
</html>
