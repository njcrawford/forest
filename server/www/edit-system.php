<?php

$page_title = "Edit system";
require "inc/header.php";

require "inc/db.php";

$result = mysql_query("select * from systems where name = '" . $_GET['name'] . "'");
$row = mysql_fetch_assoc($result);

?>
Name: <?php echo $row['name'] ?><br />
Last Check-in: <?php echo $systems_row['last_checkin'] ?><br />
Ignore AWOL: <?php echo $systems_row['ignore_awol'] ?><br />

<?php
require "footer.php";
?>
