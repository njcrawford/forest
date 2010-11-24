<?php

require "inc/check-login.php";

require_once "inc/redirect.php";

// make sure the basic required POST stuff is here
if(!isset($_POST['name']))
{
	die("No system specified");
}

$nice_ignore_awol = ($_POST['ignore_awol'] == "on") ? "1" : "0";
$query = "update systems set ignore_awol = '" . $nice_ignore_awol . "' where name = '" . $_POST['name'] . "'";

require "inc/db.php";

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
