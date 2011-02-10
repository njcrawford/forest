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

$page_title = "Updates by package name";
require "inc/header.php";

require "inc/db.php";

?>
<a href="./">Back to summary page</a><br />
<?php

if(isset($_GET['name']))
{
	$updates_result = mysql_query("select updates.system_id, updates.package_name, updates.accepted, if(update_locks.package_name is null, 0, 1) as locked from updates left outer join (update_locks) on (updates.package_name = update_locks.package_name and updates.system_id = update_locks.system_id) where updates.package_name = '" . mysql_real_escape_string($_GET['name']) . "'");

	echo "Name: " . $_GET['name'] . "<br />Systems: " . mysql_num_rows($updates_result) . "<br />";
	echo "<ul>";
	for($updates_row = mysql_fetch_assoc($updates_result); $updates_row; $updates_row = mysql_fetch_assoc($updates_result))
	{
		$systems_result = mysql_query("select name from systems where id = '" . mysql_real_escape_string($updates_row['system_id']) . "'");
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
<?php
		if($updates_row['locked'] == 1)
		{
			echo " (locked)";
		}
		else
		{
?>
				<form method="post" action="mark-accepted-updates.php">
					<input type="hidden" name="accepted" value="<? echo $nice_accepted_value ?>">
					<input type="hidden" name="system_id" value="<? echo $updates_row['system_id'] ?>">
					<input type="hidden" name="package_name" value="<? echo $updates_row['package_name'] ?>">
					<input type="submit" value="<? echo $nice_button_name ?>">
				</form>
<?php
		}
?>
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
