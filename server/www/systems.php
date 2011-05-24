<?php
/*
Forest - a web-based multi-system update manager

Copyright (C) 2011 Nathan Crawford
 
This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
 
You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
02111-1307, USA.

A copy of the full GPL 2 license can be found in the docs directory.
You can contact me at http://www.njcrawford.com/contact
*/

require "inc/check-login.php";
require "inc/db.php";

$systems_result = mysql_query("select * from systems where id = '" . mysql_real_escape_string($_GET['system_id']) . "'");
$systems_row = mysql_fetch_assoc($systems_result);

$systems_result = mysql_query("select 
	updates.system_id, 
	count(updates.package_name) as packages, 
	sum(updates.accepted) as accepted_count, 
	sum(if(update_locks.package_name is null, 0, 1)) as locked_count
	from updates 
	left outer join (update_locks) on 
	(
		updates.package_name = update_locks.package_name and 
		updates.system_id = update_locks.system_id
	) where updates.system_id = '" . $systems_row['id'] . "'");
$systems_row2 = mysql_fetch_assoc($systems_result);
$systems_row['packages'] = $systems_row2['packages'];
$systems_row['accepted_count'] = $systems_row2['accepted_count'];
$systems_row['locked_count'] = $systems_row2['locked_count'];

$page_title = "Updates for " . $systems_row['name'];
require "inc/header.php";

?>
<a href="./">Back to summary page</a><br />
<?php
if(isset($_GET['system_id']))
{
?>
<a href="edit-system.php?system_id=<?php echo $_GET['system_id'] ?>">Edit system</a><br />
<?php
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
	<a href="clear_updates.php?system_id=<?php echo $systems_row['id'] ?>">Clear Updates</a><br />
	<a href="delete_system.php?system_id=<?php echo $systems_row['id'] ?>">Delete system</a><br />
<?php
	if(($systems_row['packages'] - $systems_row['accepted_count'] - $systems_row['locked_count']) > 0 && (($systems_row['packages'] - $systems_row['locked_count']) != $systems_row['accepted_count']))
	{
?>
		<form method="post" action="mark-accepted-updates.php">
			<input type="hidden" name="action" value="accept">
			<input type="hidden" name="system_id" value="<?php echo $systems_row['id'] ?>">
			<input type="submit" value="Accept all">
		</form>
<?php
	}
?>
	<ul>
<?php
	$updates_result = mysql_query("select updates.package_name, updates.version, if(update_locks.package_name is null, 0, 1) as locked, updates.accepted from updates left outer join (update_locks) on (updates.system_id = update_locks.system_id and updates.package_name = update_locks.package_name) where updates.system_id = '" . $systems_row['id'] . "' order by updates.package_name");
	for($updates_row = mysql_fetch_assoc($updates_result); $updates_row; $updates_row = mysql_fetch_assoc($updates_result))
	{
		if($updates_row['accepted'] == 1)
		{
			$nice_action = "reject";
			$nice_button_name = "Reject";
			$nice_checked = "checked=\"checked\"";
		}
		else
		{
			$nice_action = "accept";
			$nice_button_name = "Accept";
			$nice_checked = "";
		}
?>
		<li>
			<input type="checkbox" <?php echo $nice_checked ?>>
			<a href="packages.php?name=<?php echo $updates_row['package_name'] ?>"><?php echo $updates_row['package_name'] ?></a>
			<?php echo $updates_row['version'] ?>
<?php
		if($updates_row['locked'] == 0)
		{
?>
	                <form method="post" action="mark-accepted-updates.php">
				<input type="hidden" name="action" value="<?php echo $nice_action ?>">
				<input type="hidden" name="system_id" value="<?php echo $systems_row['id'] ?>">
				<input type="hidden" name="package_name" value="<?php echo $updates_row['package_name'] ?>">
				<input type="submit" value="<?php echo $nice_button_name ?>" class="acceptupdates">
			</form>
<?php
		}
		else
		{
?>
			(locked)
<?php
		}
?>
			<?php echo $updates_row['version'] ?>
		</li>
<?php
	}
	echo "</ul>";
}
else
{
	echo "No system specified";
}

require "inc/footer.php";
?>
