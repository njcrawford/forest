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
require_once "inc/redirect.php";
require "inc/db.php";

// make sure the basic required POST stuff is here
if(!isset($_POST['system_name']) && !isset($_POST['package_name']))
{
	die("No package or system specified");
}
$query = "select id from systems where name = '" . $_POST['system_name'] . "'";
$result = mysql_query($query);
$row = mysql_fetch_assoc($result);
$query = "delete from update_locks where system_id = '" . $row['id'] . "' and package_name = '" . $_POST['package_name'] . "'";

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
