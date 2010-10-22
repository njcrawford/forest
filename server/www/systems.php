<html>
<head>
<title>Forest</title>
</head>
<body>
<a href="./">Back to summary page</a><br />
<table>
<?php

$updates_table = "updates";

$db_server = "localhost";
$db_user = "forest_user";
$db_password = "forest_pass";
$db_name = "forest";

mysql_connect($db_server, $db_user, $db_password);
mysql_select_db($db_name);

if(isset($_GET['name']))
{
	$result = mysql_query("select * from systems where name = '" . $_GET['name'] . "'");
	$row = mysql_fetch_assoc($result);

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

	$result2 = mysql_query("select * from updates where system_id = '" . $row['id'] . "'");
        $row2 = mysql_fetch_assoc($result2);

	echo "Name: " . $row['name'] . "<br />Updates: " . mysql_num_rows($result2) . "<br />Reboot Needed: " . $nice_reboot . "<br />Last Check-in: " . $row['last_checkin'] . "<br />";
	echo "<tr><td colspan=4><ul>";
	while($row2)
	{
		echo "<li><a href='packages.php?name=" . $row2['package_name'] . "'>" . $row2['package_name'] . "</a> " . $row2['version'] . "</li>";
		$row2 = mysql_fetch_assoc($result2);
	}
	echo "</ul></td></tr>";

	$row = mysql_fetch_assoc($result);
}
else
{
	echo "No system specified";
}

?>
</table>
</body>
</html>
