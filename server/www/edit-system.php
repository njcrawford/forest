<?php

require "inc/check-login.php";

$page_title = "Edit system";
require "inc/header.php";

if(!isset($_GET['name']))
{
	die("No system specified");
}

require "inc/db.php";

$result = mysql_query("select * from systems where name = '" . $_GET['name'] . "'");
$row = mysql_fetch_assoc($result);

$nice_checked = ($row['ignore_awol'] == 1) ? "checked=checked " : "";

?>
<a href="./">Back to summary page</a><br />
<a href="systems.php?name=<?php echo $_GET['name'] ?>">Back to updates for <?php echo $_GET['name'] ?></a><br />
<br />
<form action="save-system.php" method="post">
<input type="hidden" name="name" value="<?php echo $_GET['name'] ?>">
Name: <?php echo $row['name'] ?><br />
Last Check-in: <?php echo $row['last_checkin'] ?><br />
Ignore AWOL: <input name="ignore_awol" type=checkbox <?php echo $nice_checked ?>></input><br />
<input type=submit value=Save>
</form>
<?php
require "inc/footer.php";
?>
