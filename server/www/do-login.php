<?php

require_once "inc/redirect.php";
require "/etc/forest-server.conf";

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
foreach($forest_config['allowed_users'] as $this_user)
{
	if($_POST['username'] == $this_user['username'] && $_POST['password'] == $this_user['password'])
	{
		$login_success = true;
		// this needs to be changed to a session id or something like that
		setcookie('login_name', $_POST['username'], 0, '/forest/');
		break;
	}
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
