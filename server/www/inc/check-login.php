<?php

// check for login cookie
if(!isset($_COOKIE['login_name']))
{
	require_once "/etc/forest-server.conf";
	require_once "inc/redirect.php";
	// no cookie, so redirect to login.php
	redirect($forest_config['server_url'] . "/login.php");
}

?>
