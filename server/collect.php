<?php

$updates_table = "updates";
$post_data_name = "available_updates";

$db_server = "localhost";
$db_user = "forest_user";
$db_password = "forest_pass";
$db_name = "forest";

mysql_connect($db_server, $db_user, $db_password);
mysql_select_db($db_name);

//look through post data and see if there's anything to save
if(!empty($_POST[$post_data_name]))
{
	$data_ok = true;
	$system_id = $_POST['system_id'];
#	echo "system_id: " . $system_id . "<br />";
	$packages = explode(",", $_POST[$post_data_name]);
	foreach($packages as $this_package)
	{
		//if there is, build an SQL query and run it
#		echo $this_package . "<br />";
		$result = mysql_query("insert into " . $updates_table . " values
			(
				null,
				'" . $system_id . "',
				'" . $this_package . "',
				NOW()
			)");
		if(!$result)
		{
			$data_ok = false;
			break;
		}
	}
	//send back a message indicating data received (or not)
	if($data_ok)
	{
		echo "data ok";
	}
	else
	{
		echo "data error";
	}
}
else
{
	echo "no data";
}
?>
