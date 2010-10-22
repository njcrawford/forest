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

if(isset($_GET['name']))
{
	$result = mysql_query("select * from updates where package_name = '" . $_GET['name'] . "'");
	$row = mysql_fetch_assoc($result);

	$result2 = mysql_query("select name from systems where id = '" . $row['system_id'] . "'");
        $row2 = mysql_fetch_assoc($result2);

	echo "Name: " . $row['package_name'] . "<br />Systems: " . mysql_num_rows($result) . "<br />";
	echo "<tr><td colspan=4><ul>";
	while($row)
	{
		$result2 = mysql_query("select name from systems where id = '" . $row['system_id'] . "'");
	        $row2 = mysql_fetch_assoc($result2);
		echo "<li>" . $row2['name'] . "</li>";
		$row = mysql_fetch_assoc($result);
	}
	echo "</ul></td></tr>";
}
else
{
	echo "No system specified";
}

?>
</table>
</body>
</html>
