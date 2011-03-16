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
if(empty($_POST['system_id']))
{
	die("No system specified");
}

$nice_ignore_awol = (!empty($_POST['ignore_awol']) && $_POST['ignore_awol'] == "on") ? "1" : "0";
$nice_allow_reboot = (!empty($_POST['allow_reboot']) && $_POST['allow_reboot'] == "on") ? "1" : "0";
$query = "update systems set ignore_awol = '" . $nice_ignore_awol . "', allow_reboot = '" . $nice_allow_reboot . "' where id = '" . mysql_real_escape_string($_POST['system_id']) . "'";

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
