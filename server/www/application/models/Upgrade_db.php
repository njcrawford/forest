<?php

class Forest_DB extends CI_Model {

	function __construct()
	{
		// Call the Model constructor
		parent::__construct();
	}

	function upgrade_1_to_2()
	{
		$result = $this->db->query("update settings set value = '2' where name = 'db_version'");
		if(!$result)
		{
			die("Failed setting new db schema version\n");
		}

		$result = $this->db->query("alter table systems add reboot_accepted tinyint(1) NOT NULL DEFAULT '0'");
		if(!$result)
		{
			die("Failed to alter systems table\n");
		}
		$result = $this->db->query("alter table systems add allow_reboot tinyint(1) NOT NULL DEFAULT '0'");
		if(!$result)
		{
			die("Failed to alter systems table\n");
		}

		$result = mysql_query("CREATE TABLE `update_locks` (`system_id` int(11) DEFAULT NULL, `package_name` text) ENGINE=MyISAM DEFAULT CHARSET=latin1");
		if(!$result)
		{
			die("Failed to add update_locks table\n");
		}
	}

	function upgrade_2_to_3()
	{
		$result = $this->db->query("update settings set value = '3' where name = 'db_version'");
		if(!$result)
		{
			die("Failed setting new db schema version\n");
		}

		$result = $this->db->query("alter table systems add can_apply_updates tinyint(1) NOT NULL DEFAULT '0'");
		if(!$result)
		{
			die("Failed to alter systems table\n");
		}

		$result = $this->db->query("alter table systems add can_apply_reboot tinyint(1) NOT NULL DEFAULT '0'");
		if(!$result)
		{
			die("Failed to alter systems table\n");
		}
	}

	function upgrade_3_to_4()
	{
		die("Not yet implemented");
	}

	function run_upgrade()
	{
		// Use a direct query instead of the equivelent function, because the
		// function may change with new versions.
		$query = $this->db->query("select value from settings where name = 'db_version'");
		$result = $query->result();
		$current_db_version = $result->row()->value;

		// Using a magic number for the highest schema version this script has been updated to
		if(DB_VERSION == 4)
		{
			// Make sure it's something we can update before trying to update it
			if($current_db_version > DB_VERSION)
			{
				die("The database is at schema version '" . $current_db_version . "', but this script expects less than '" . DB_VERSION . "'.\n");
			}
			if($current_db_version == DB_VERSION)
			{
				die("The database is already at schema version '" . DB_VERSION . "', no upgrades are needed.\n");
			}

			// do updates incrementally
			if($current_db_version < 2)
			{
				upgrade_1_to_2();
				echo "DB schema updated to v2\n";
			}
			if($current_db_version < 3)
			{
				upgrade_2_to_3();
				echo "DB schema updated to v3\n";
			}
			if($current_db_version < 4)
			{
				upgrade_3_to_4();
				echo "DB schema updated to v4\n";
			}
		}
		else
		{
			echo "This script has not been updated for database schema version '" . DB_VERSION . "'\n";
		}
	}
}
/* End of Update_db.php */
