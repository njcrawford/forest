<?php

// set defaults
$forest_config[''] = "";
$forest_config['email_to'] = "root@localhost";
$forest_config['server_url'] = "http://forest/forest";
$forest_config['login_required'] = false;

// now read in any changed values from the config file
include "/etc/forest-server.conf";

?>
