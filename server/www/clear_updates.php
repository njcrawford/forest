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
require_once "inc/config-file.php";
require_once "inc/redirect.php";

if(isset($_POST['confirmed']))
{
	if(empty($_POST['system_id']))
	{
		die("No system specified");
	}

	$result = mysql_query("delete from updates where system_id = '" . $_POST['system_id'] . "'");
	if($result)
	{
		redirect($forest_config['server_url'] . "/systems.php?system_id=" . $_POST['system_id']);
	}
	else
	{
		echo "Error: " . mysql_error();
	}
}
else
{	
	if(empty($_GET['system_id']))
	{
		die("No system specified");
	}
	$result = mysql_query("select * from systems where id = '" . $_GET['system_id'] . "'");
	$system_row = mysql_fetch_assoc($result);

	$page_title = "Clear updates?";
	require "inc/header.php";
?>
Are you sure you want to clear all updates for <?php echo $system_row['name'] ?>?<br />
<form action=" " method="post">
<input type="hidden" name="confirmed" value="yes" />
<input type="hidden" name="system_id" value="<?php echo $_GET['system_id'] ?>" />
<input type="submit" value="Clear updates" />
</form>
<a href="systems.php?system_id=<?php echo $_GET['system_id'] ?>">Cancel</a>
<?php
	require "inc/footer.php";
}

?>
