<?php

function redirect_back()
{
	// send the user back to the page they just came from
	header("Location: " . $_SERVER['HTTP_REFERER'], 307);
}

if(isset($_POST['system_id']) && isset($_POST['package']))
{
	// specific system/package combo
	mysql_query("update updates set accepted = true where system_id = '" . $_POST['system_id'] . "' and package_name = '" . $_POST['package']. "'");
	redirect_back();
}
elseif($isset($_POST['system_id']))
{
	// all updates for a system
	mysql_query("update updates set accepted = true where system_id = '" . $_POST['system_id'] . "'");
	redirect_back();
}
elseif($isset($_POST['package']))
{
	// all systems for a specific package
	mysql_query("update updates set accepted = true where package_name = '" . $_POST['package_name'] . "'");
	redirect_back();
}
else
{
	echo "No package or system specified";
}
?>
