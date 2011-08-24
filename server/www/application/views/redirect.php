<html>
<head>
<title>Redirecting...</title>
<?php
if(!empty($redirect_location))
{
	$this->load->helper('url');
	//echo "<meta http-equiv='REFRESH' content='0;" . site_url("project/view/" . $redirect_project_id) . $nice_anchor . "'>";
	redirect(site_url($redirect_location), 'location');
}
?>
</head>
<body>
<?php 
echo $result_text;
?>
</body>
</html>