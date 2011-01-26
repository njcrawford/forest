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

require_once "inc/redirect.php";
require_once "inc/config_file.php";

function set_logged_in_token()
{
	// this needs to be changed to a session id or something like that
	//setcookie('login_name', $_POST['username'], 0, '/forest/');
	session_start();
	$_SESSION['login_name'] = $_POST['username'];
}

// check inputs
if(!isset($_POST['username']))
{
	die("No username specified");
}
if(!isset($_POST['password']))
{
	die("No password specified");
}

$login_success = false;
// do the actual login
if($forest_config['login_source'] == "config_file")
{
	foreach($forest_config['allowed_users'] as $this_user)
	{
		if($_POST['username'] == $this_user['username'] && $_POST['password'] == $this_user['password'])
		{
			$login_success = true;
			set_logged_in_token();
			break;
		}
	}
}
elseif($forest_config['login_source'] == "ldap")
{
	$ds=ldap_connect($forest_config['ldap_server']);
	if (!$ds)
	{
		die('Cannot connect to LDAP server.');
	}
	$dn = $forest_config['ldap_auth_var'] . "=" . $_POST['username'] . ", " . $forest_config['ldap_base'];

	$result=@ldap_bind($ds,$dn,$_POST['password']);
	if (!$result)
	{
		// Couldn't bind to LDAP server, see 
		$lderr = ldap_error($ds);
		if($lderr != "Invalid credentials")
		{
			echo "Error: " . ldap_error($ds) . "<br /><br />";
			die('Check your user name and password and try again.');
		}
	}
	elseif(!empty($forest_config['ldap_allowed_users']))
	{
		foreach($forest_config['ldap_allowed_users'] as $this_user)
		{
			if($_POST['username'] == $this_user)
			{
				$login_success = true;
				set_logged_in_token();
				break;
			}
		}
	}
	else
	{
		$login_success = true;
		set_logged_in_token();
	}
}
else
{
	die("Invalid login_source in config file");
}

if($login_success)
{
	// redirect to summary page
	redirect($forest_config['server_url']);
}
else
{
	// go back to the login page
	//redirect_back(); 
	die("Invalid login information");
}

?>
