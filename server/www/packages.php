<html>
<head>
<title>Updates by package name - Forest</title>
<link rel="stylesheet" type="text/css" href="forest.css" />
</head>
<body>
<a href="./">Back to summary page</a><br />
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
	$updates_result = mysql_query("select * from updates where package_name = '" . $_GET['name'] . "'");
	$updates_row = mysql_fetch_assoc($updates_result);

//	$systems_result = mysql_query("select name from systems where id = '" . $row['system_id'] . "'");
//        $systems_row = mysql_fetch_assoc($systems_result);

	echo "Name: " . $updates_row['package_name'] . "<br />Systems: " . mysql_num_rows($updates_result) . "<br />";
	echo "<ul>";
	while($updates_row)
	{
		$systems_result = mysql_query("select name from systems where id = '" . $updates_row['system_id'] . "'");
	        $systems_row = mysql_fetch_assoc($systems_result);
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
			<a href="systems.php?name=<? echo $systems_row['name'] ?>"><? echo $systems_row['name'] ?></a>
			<form method="post" action="mark-accepted.php">
				<input type="hidden" name="accepted" value="<? echo $nice_accepted_value ?>">
				<input type="hidden" name="system_id" value="<? echo $updates_row['system_id'] ?>">
				<input type="hidden" name="package" value="<? echo $updates_row['package_name'] ?>">
				<input type="submit" value="<? echo $nice_button_name ?>">
			</form>
		</li>
<?php
		$updates_row = mysql_fetch_assoc($updates_result);
	}
	echo "</ul>";
}
else
{
	echo "No system specified";
}

?>
</body>
</html>
