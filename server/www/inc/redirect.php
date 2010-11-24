<?php

function redirect($url)
{
	// send the user back to the specified page
	header("Location: " . $url, 307);
}

function redirect_back()
{
	// send the user back to the page they just came from
	redirect($_SERVER['HTTP_REFERER']);
}

?>
