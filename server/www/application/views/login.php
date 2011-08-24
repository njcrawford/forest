<?php 
$this->load->helper('url');
?>
<form action="<?php echo site_url("browser/log_me_in") ?>" method="post">
Username: <input name="username" /><br />
Password: <input name="password" type="password" /><br />
<input type="submit" value="Login" />
</form>