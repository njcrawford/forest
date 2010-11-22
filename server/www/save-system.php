<?php

function redirect_back()
{
	// send the user back to the page they just came from
	header("Location: " . $_SERVER['HTTP_REFERER'], 307);
}

// make sure the basic required POST stuff is here
if(!isset($_POST['name']))
{
	die("No system specified");
}
elseif(!isset($_POST['ignore_awol']))
{
	die("No ignore_awol value");
}

$nice_ignore_awol = ($_POST['ignore_awol'] == "on") ? "1" : "0";
$query = "update systems set ignore_awol = '" . $nice_ignore_awol . "' where name = '" . $_POST['name'] . "'";
echo $query;


require "inc/db.php";

$result = mysql_query($query);
if($result)
{
	//redirect_back();
}
else
{
	//echo "Error: " . mysql_error();
}
?>
