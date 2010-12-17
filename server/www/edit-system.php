<?php
/*
Forest - a web-based multi-system update manager

Copyright (C) 2010 Nathan Crawford
 
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

$page_title = "Edit system";
require "inc/header.php";

if(!isset($_GET['name']))
{
	die("No system specified");
}

require "inc/db.php";

$result = mysql_query("select * from systems where name = '" . $_GET['name'] . "'");
$row = mysql_fetch_assoc($result);

$nice_checked = ($row['ignore_awol'] == 1) ? "checked=checked " : "";

?>
<a href="./">Back to summary page</a><br />
<a href="systems.php?name=<?php echo $_GET['name'] ?>">Back to updates for <?php echo $_GET['name'] ?></a><br />
<br />
<form action="save-system.php" method="post">
<input type="hidden" name="name" value="<?php echo $_GET['name'] ?>">
Name: <?php echo $row['name'] ?><br />
Last Check-in: <?php echo $row['last_checkin'] ?><br />
Ignore AWOL: <input name="ignore_awol" type=checkbox <?php echo $nice_checked ?>></input><br />
<input type=submit value=Save>
</form>
<br />
<form action="add-update-lock.php" method="post">
<input type="hidden" name="system_name" value="<?php echo $_GET['name'] ?>">
New update lock<input name="package_name"></input>
<input type=submit value="Add lock">
</form>
<ul>
<?php
$result2 = mysql_query("select * from update_locks where system_id = '" . $row['id'] . "'");
$row2 = mysql_fetch_assoc($result2);
while($row2 != null)
{
?>
<li><?php echo $row2['package_name'] ?></li>
<?php
$row2 = mysql_fetch_assoc($result2);
}
?>
</ul>
<?php
require "inc/footer.php";
?>
