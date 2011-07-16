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

require_once "inc/redirect.php";

require "inc/db.php";

// make sure the basic required POST stuff is here
if(!isset($_POST['system_id']))
{
	die("No system specified");
}
if(!isset($_POST['accepted']))
{
	die("No accepted value");
}
elseif($_POST['accepted'] != "true" && $_POST['accepted'] != "false")
{
	die("Invalid accepted value");
}

$checks_query = "select allow_reboot, can_apply_reboot from systems where id = '" . mysql_real_escape_string($_POST['system_id']) . "'";
$checks_result = mysql_query($checks_query);
$checks_row = mysql_fetch_assoc($checks_result);

// make sure this client is allowed to reboot
if($checks_row['allow_reboot'] != 1)
{
	die("System is not allowed to reboot");
}
// make sure this client is able to reboot
if($checks_row['can_apply_reboot'] != 1)
{
	die("Client is not capable of rebooting");
}

$nice_accepted = ($_POST['accepted'] == "true") ? 1 : 0;
$query = "update systems set reboot_accepted = '" . $nice_accepted . "' where id = '" . mysql_real_escape_string($_POST['system_id']) . "'";

$result = mysql_query($query);
if($result)
{
	redirect_back();
}
else
{
	echo "Error: " . mysql_error();
}
?>
