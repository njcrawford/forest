<?php
$forest_version = '$URL$';
$forest_version = explode("/");
$forest_version = $forest_version[count($forest_version)-4];

$forest_rev = '$Revision$';
$forest_rev = explode(" ", $forest_rev);
$forest_rev = $forest_rev[1];

if($forest_version == "trunk")
{
	$display_version = $forest_version . " r" . $forest_rev;
}
else
{
	$display_version = "v" . $forest_version;
}
?>
<br />
Forest <?php echo $display_version ?>
</body>
</html>
