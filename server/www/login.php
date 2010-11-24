<?php
require "inc/header.php";
?>
<form method="post" action="do-login.php">
Username: <input name="username"></input><br />
Password: <input type="password" name="password"></input><br />
<input type="submit" value="Login">
</form>
<?php
require "inc/footer.php";
?>
