<?php

$updates_table = "updates";
$post_data_name = "available_updates";

$db_server = "localhost";
$db_user = "forest_user";
$db_password = "forest_pass";
$db_name = "forest";

mysql_connect($db_server, $db_user, $db_password);
mysql_select_db($db_name);

if(!empty($_POST['system_name']))
{
	$system_id_ok = false;
	$system_name = $_POST['system_name'];
	$result = mysql_query("select * from systems where name = '" . $_POST['system_name'] . "'");
	if($result)
	{
		$row = mysql_fetch_assoc($result);
		$system_id = $row['id'];
		if(!empty($system_id))
		{
			$system_id_ok = true;
		}
	}

	if(!$system_id_ok)
	{
		$result = mysql_query("insert into systems values(null, '" . $_POST['system_name'] . "', null)");
		if($result)
		{
			$result = mysql_query("select LAST_INSERT_ID() as id");
			$row = mysql_fetch_assoc($result);
			$system_id = $row['id'];
		}
		else
		{
			die("Mysql error: " . mysql_error());
		}
	}

	if(!empty($_POST[$post_data_name]))
	{
		$data_ok = true;
		// Forget about old updates before adding new ones
		mysql_query("update systems set last_checkin = NOW() where id = '" . $system_id . "'");
		mysql_query("delete from " . $updates_table . " where system_id = '" . $system_id . "'");
		$packages = explode(",", $_POST[$post_data_name]);
		// build an SQL query to save info for all packages that need updated
		foreach($packages as $this_package)
		{
			$result = mysql_query("insert into " . $updates_table . " values
				(
					null,
					'" . $system_id . "',
					'" . $this_package . "'
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
	elseif(!empty($_POST['no_updates_available']))
	{
		// Forget about old updates and save checkin time
		mysql_query("update systems set last_checkin = NOW() where id = '" . $system_id . "'");
		mysql_query("delete from " . $updates_table . " where system_id = '" . $system_id . "'");
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
