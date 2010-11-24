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
// this login mechanism is only for testing, it needs to be changed to something real asap
if($_POST['username'] == $_POST['password'])
{
	$login_success = true;
	// this needs to be changed to a session id or something like that
	setcookie('login_name', $_POST['username'], 0, '/forest/');
}

if($login_success)
{
	// redirect to summary page
	redirect($server_url);
}
else
{
	// go back to the login page
	redirect_back(); 
}

?>
