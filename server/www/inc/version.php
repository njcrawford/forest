<?php
$temp = '$URL$';
$temp = explode("/", $temp);
define("FOREST_VERSION", $temp[count($temp)-5]);

/*$forest_rev = '$Revision$';
$forest_rev = explode(" ", $forest_rev);
$forest_rev = $forest_rev[1];

if($forest_version == "trunk")
{
	$display_version = $forest_version . " r" . $forest_rev;
}
else
{
	$display_version = "v" . $forest_version;
}*/
?>
