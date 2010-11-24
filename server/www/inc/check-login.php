<?php

// check for login cookie
if(!isset($_COOKIE['login_name']))
{
	require_once "/etc/forest-server.conf";
	require_once "inc/redirect.php";
	// if no cookie, redirect to login.php
	redirect($server_url . "/login.php");
}

?>
