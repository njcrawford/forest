#!/usr/bin/php
<?php

mysql_connect("localhost", "forest_user", "forest_pass");
mysql_select_db("forest");

// get systems with updates
// print name and number of updates
$update_query = "select 
			systems.name as system_name, 
			count(updates.package_name) as update_count 
		from 
			systems, updates 
		where 
			systems.id = updates.system_id 
		group by system_name 
		order by system_name";
$update_result = mysql_query($update_query);
if (mysql_num_rows($update_result) > 0)
{
	echo "Updates available on these systems:\n";
	$update_row = mysql_fetch_assoc($update_result);
	while($update_row)
	{
		echo $update_row['system_name'] . " (" . $update_row['update_count'] . ")\n";
		$update_row = mysql_fetch_assoc($update_result);
	}
}
else
{
	echo "No systems need updates\n";
}

// get systems that need a reboot
// just print name
$reboot_query = "select name from systems where reboot_required = 1";
$reboot_result = mysql_query($reboot_query);
if (mysql_num_rows($reboot_result) > 0)
{
	echo "These systems need rebooted:\n";
	$reboot_row = mysql_fetch_assoc($reboot_result);
	while($reboot_row)
	{
		echo $reboot_row['name'] . "\n";
		$reboot_row = mysql_fetch_assoc($reboot_result);
	}
}

// get systems that haven't checked in for a while
// print name and last check in time
$awal_query = "select name, last_checkin from systems where last_checkin < DATE_SUB(NOW(), INTERVAL 36 HOUR)";
$awal_result = mysql_query($awal_query);
if (mysql_num_rows($awal_result) > 0)
{
	echo "These systems have not checked in for more than 36 hours:\n";
	$awal_row = mysql_fetch_assoc($awal_result);
	while($awal_row)
	{
		echo $awal_row['name'] . " (" . $awal_row['last_checkin'] . ")\n";
		$awal_row = mysql_fetch_assoc($awal_result);
	}
}

// send email

?>
