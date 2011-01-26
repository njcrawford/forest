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

require_once "config-file.php";

function is_login_token_set()
{
	// this needs to be beefed up
	return isset($_SESSION['login_name']);
}

function set_logged_in_token($token)
{
	// this needs to be changed to a session id or something like that
	//setcookie('login_name', $_POST['username'], 0, '/forest/');
	if(!is_login_token_set())
	{
		session_start();
		$_SESSION['login_name'] = $token;
	}
}

?>
