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
	$systems_result = mysql_query("select * from systems where name = '" . $_GET['name'] . "'");
	$systems_row = mysql_fetch_assoc($systems_result);

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

	$updates_result = mysql_query("select * from updates where system_id = '" . $systems_row['id'] . "'");
        $updates_row = mysql_fetch_assoc($updates_result);

	echo "Name: " . $systems_row['name'] . "<br />Updates: " . mysql_num_rows($updates_result) . "<br />Reboot Needed: " . $nice_reboot . "<br />Last Check-in: " . $systems_row['last_checkin'] . "<br />";
	echo "<tr><td colspan=4><ul>";
	while($updates_row)
	{
		if($updates_row['accepted'] == 1)
		{
			$nice_accepted_value = "false";
			$nice_button_name = "Reject";
			$nice_checked = "checked=\"checked\"";
		}
		else
		{
			$nice_accepted_value = "true";
			$nice_button_name = "Accept";
			$nice_checked = "";
		}
?>
		<li>
			<input type="checkbox" <? echo $nice_checked ?>>
			<a href="packages.php?name=<? echo $updates_row['package_name'] ?>"><? echo $updates_row['package_name'] ?></a>
			<? echo $updates_row['version'] ?>
	                <form method="post" action="mark-accepted.php">
				<input type="hidden" name="accepted" value="<? echo $nice_accepted_value ?>">
				<input type="hidden" name="system_id" value="<? echo $systems_row['id'] ?>">
				<input type="hidden" name="package" value="<? echo $updates_row['package_name'] ?>">
				<input type="submit" value="<? echo $nice_button_name ?>">
			</form>
		</li>
<?php
		$updates_row = mysql_fetch_assoc($updates_result);
	}
	echo "</ul></td></tr>";

	$systems_row = mysql_fetch_assoc($systems_result);
}
else
{
	echo "No system specified";
}

?>
</table>
</body>
</html>
