#!/usr/bin/php
<?php
require "www/inc/db.php";

$output_message = "";

// default settings and read config file
$email_to = "root";
$server_url = "http://forest/forest/";
include '/etc/forest-server.conf';

$output_message .= $server_url . "\n";

require "www/inc/version.php";
$output_message .= "Forest version " . $forest_version . "\n\n";

// get systems with updates
// print name and number of updates
$update_query = "select * from (
        select 
            systems.name as system_name, 
            count(updates.package_name) as update_count, 
            last_checkin 
        from systems 
            left join (updates) 
            on (systems.id = updates.system_id) 
        group by system_name 
        order by update_count desc
    ) b 
    where 
        last_checkin >= DATE_SUB(NOW(), INTERVAL 36 HOUR) and 
        update_count > 0";
$update_result = mysql_query($update_query);
if (mysql_num_rows($update_result) > 0)
{
	$output_message .= "Updates available on these systems:\n";
	$update_row = mysql_fetch_assoc($update_result);
	while($update_row)
	{
		$output_message .= $update_row['system_name'] . " (" . $update_row['update_count'] . ")\n";
		$update_row = mysql_fetch_assoc($update_result);
	}
}
else
{
	$output_message .= "No systems need updates\n";
}

$output_message .= "\n";

// get systems that need a reboot
// just print name
$reboot_query = "select name from systems where reboot_required = '1' and last_checkin >= DATE_SUB(NOW(), INTERVAL 36 HOUR)";
$reboot_result = mysql_query($reboot_query);
if (mysql_num_rows($reboot_result) > 0)
{
	$output_message .= "These systems need rebooted:\n";
	$reboot_row = mysql_fetch_assoc($reboot_result);
	while($reboot_row)
	{
		$output_message .= $reboot_row['name'] . "\n";
		$reboot_row = mysql_fetch_assoc($reboot_result);
	}
	$output_message .= "\n";
}

// get systems that haven't checked in for a while
// print name and last check in time
$awal_query = "select name, last_checkin from systems where last_checkin < DATE_SUB(NOW(), INTERVAL 36 HOUR) and ignore_awol = '0'";
$awal_result = mysql_query($awal_query);
if (mysql_num_rows($awal_result) > 0)
{
	$output_message .= "These systems have not checked in for more than 36 hours:\n";
	$awal_row = mysql_fetch_assoc($awal_result);
	while($awal_row)
	{
		$output_message .= $awal_row['name'] . " (" . $awal_row['last_checkin'] . ")\n";
		$awal_row = mysql_fetch_assoc($awal_result);
	}
}

// send email
mail($email_to, "Forest system report", $output_message, "From:forest@localhost");

?>
