<?php

$page_title = "Updates by system";
require "inc/header.php";

require "inc/db.php";

?>
<a href="./">Back to summary page</a><br />
<?php

if(isset($_GET['name']))
{
	$systems_result = mysql_query("select 
			systems.id, 
			systems.name, 
			systems.reboot_required, 
			systems.last_checkin, 
			count(updates.package_name) as packages, 
			sum(if(accepted is null, 0, accepted)) as accepted_count 
		from systems left join (updates) on (updates.system_id = systems.id)
		where name = '" . $_GET['name'] . "' group by systems.id"
	);
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
?>
	Name: <?php echo $systems_row['name'] ?><br />
	Updates: <?php echo $systems_row['packages'] ?><br />
	Reboot Needed: <?php echo $nice_reboot ?><br />
	Last Check-in: <?php echo $systems_row['last_checkin'] ?><br />
<?php
	if($systems_row['packages'] > 0 && ($systems_row['packages'] != $systems_row['accepted_count']))
	{
?>
		<form method="post" action="mark-accepted.php">
			<input type="hidden" name="accepted" value="true">
			<input type="hidden" name="system_id" value="<?php echo $systems_row['id'] ?>">
			<input type="submit" value="Accept all">
		</form>
<?php
	}
?>
	<ul>
<?php
	$updates_result = mysql_query("select * from updates where system_id = '" . $systems_row['id'] . "'");
        $updates_row = mysql_fetch_assoc($updates_result);
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
	echo "</ul>";

	$systems_row = mysql_fetch_assoc($systems_result);
}
else
{
	echo "No system specified";
}

require "footer.php";
?>
