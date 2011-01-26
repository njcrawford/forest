<?php

// set defaults
$forest_config['email_to'] = "root@localhost";
$forest_config['server_url'] = "http://forest/forest";
$forest_config['login_required'] = false;
$forest_config['db_server'] = "localhost";
$forest_config['db_user'] = "forest_user";
$forest_config['db_password'] = "forest_pass";
$forest_config['db_name'] = "forest";

// now read in any changed values from the config file
include "/etc/forest-server.conf";

?>
