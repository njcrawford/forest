<?php

$page_title = "Edit system";
require "inc/header.php";

require "inc/db.php";

$result = mysql_query("select * from systems where name = '" . $_GET['name'] . "'");
$row = mysql_fetch_assoc($result);

$nice_checked = ($row['ignore_awol'] == 1) ? "checked=checked " : "";

?>
<form>
Name: <?php echo $row['name'] ?><br />
Last Check-in: <?php echo $row['last_checkin'] ?><br />
Ignore AWOL: <input name=ignore_awol type=checkbox <?php echo $nice_checked ?>></input><br />
<input type=submit value=Save>
</form>
<?php
require "footer.php";
?>
