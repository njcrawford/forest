<?php
/*
Forest - a web-based multi-system update manager

Copyright (C) 2014 Nathan Crawford
 
This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
 
You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
02111-1307, USA.

A copy of the full GPL 2 license can be found in the docs directory.
You can contact me at http://www.njcrawford.com/contact
*/
defined('BASEPATH') or exit('No direct script access allowed');

class Upgrade_DB extends CI_Model {

	function __construct()
	{
		// Call the Model constructor
		parent::__construct();
	}

	function upgrade_1_to_2()
	{
		echo "Upgrading schema from 1 to 2\n";
		$result = $this->db->query("alter table systems add reboot_accepted tinyint(1) NOT NULL DEFAULT '0'");
		if(!$result)
		{
			die("Failed to add systems.reboot_accepted\n");
		}
		$result = $this->db->query("alter table systems add allow_reboot tinyint(1) NOT NULL DEFAULT '0'");
		if(!$result)
		{
			die("Failed to add systems.allow_reboot\n");
		}

		$result = mysql_query("CREATE TABLE `update_locks` (`system_id` int(11) DEFAULT NULL, `package_name` text) ENGINE=MyISAM DEFAULT CHARSET=latin1");
		if(!$result)
		{
			die("Failed to add update_locks table\n");
		}

		$result = $this->db->query("update settings set value = '2' where name = 'db_version'");
		if(!$result)
		{
			die("Failed setting new db schema version\n");
		}
	}

	function upgrade_2_to_3()
	{
		echo "Upgrading schema from 2 to 3\n";
		$result = $this->db->query("alter table systems add can_apply_updates tinyint(1) NOT NULL DEFAULT '0'");
		if(!$result)
		{
			die("Failed to add systems.can_apply_updates\n");
		}

		$result = $this->db->query("alter table systems add can_apply_reboot tinyint(1) NOT NULL DEFAULT '0'");
		if(!$result)
		{
			die("Failed to add systems.can_apply_reboot\n");
		}

		$result = $this->db->query("update settings set value = '3' where name = 'db_version'");
		if(!$result)
		{
			die("Failed setting new db schema version\n");
		}
	}

	function upgrade_3_to_4()
	{
		echo "Upgrading schema from 3 to 4\n";

		// Make sure tables are InnoDB for foreign keys
		$result = $this->db->query("alter table settings engine=innodb");
		if(!$result)
		{
			die("Failed to set settings engine=innodb\n");
		}

		$result = $this->db->query("alter table systems engine=innodb");
		if(!$result)
		{
			die("Failed to set systems engine=innodb\n");
		}

		$result = $this->db->query("alter table update_locks engine=innodb");
		if(!$result)
		{
			die("Failed to set update_locks engine=innodb\n");
		}

		$result = $this->db->query("alter table updates engine=innodb");
		if(!$result)
		{
			die("Failed to set updates engine=innodb\n");
		}

		// Settings table
		$result = $this->db->query("alter table settings modify name varchar(255) not null");
		if(!$result)
		{
			die("Failed to alter settings.name\n");
		}

		$result = $this->db->query("alter table settings modify value varchar(255) not null");
		if(!$result)
		{
			die("Failed to alter settings.value\n");
		}

		$result = $this->db->query("update settings set name = 'absent_hours' where name = 'awol_hours'");
		if(!$result)
		{
			die("Failed to update absent_hours setting\n");
		}

		// Systems table
		$result = $this->db->query("alter table systems change ignore_awol report_absent tinyint(1) not null");
		if(!$result)
		{
			die("Failed to alter systems.report_absent\n");
		}

		$result = $this->db->query("update systems set report_absent = !report_absent");
		if(!$result)
		{
			die("Failed to update systems.report_absent values\n");
		}

		$result = $this->db->query("alter table systems add uuid char(36) after can_apply_reboot");
		if(!$result)
		{
			die("Failed to add systems.uuid\n");
		}

		$result = $this->db->query("alter table systems change name name varchar(255) after uuid");
		if(!$result)
		{
			die("Failed to alter systems.name\n");
		}

		$result = $this->db->query("alter table systems add client_version varchar(255) after name");
		if(!$result)
		{
			die("Failed to add systems.client_version\n");
		}

		$result = $this->db->query("alter table systems add os_version varchar(255) after client_version");
		if(!$result)
		{
			die("Failed to add systems.os_version\n");
		}

		// Update locks table
		$result = $this->db->query("alter table update_locks modify package_name varchar(255)");
		if(!$result)
		{
			die("Failed to alter update_locks.package_name\n");
		}

		$result = $this->db->query("delete from update_locks where not system_id = any (select id from systems)");
		if(!$result)
		{
			die("Failed to clear invalid update_locks\n");
		}

		$result = $this->db->query("alter table update_locks add constraint update_locks_system_id foreign key system_id_key (system_id) references systems (id)");
		if(!$result)
		{
			die("Failed to add update_locks.system_id foreign key\n");
		}

		// Updates table
		$result = $this->db->query("alter table updates change package_name package_name varchar(255) not null after accepted");
		if(!$result)
		{
			die("Failed to alter updates.package_name\n");
		}

		$result = $this->db->query("alter table updates change version version varchar(255) after package_name");
		if(!$result)
		{
			die("Failed to alter updates.version\n");
		}

		$result = $this->db->query("alter table updates modify accepted tinyint(1) not null default '0'");
		if(!$result)
		{
			die("Failed to alter updates.accepted\n");
		}

		$result = $this->db->query("delete from updates where not system_id = any (select id from systems)");
		if(!$result)
		{
			die("Failed to clear invalid updates\n");
		}

		$result = $this->db->query("alter table updates add constraint updates_system_id foreign key system_id_key (system_id) references systems (id)");
		if(!$result)
		{
			die("Failed to add updates.system_id foreign key\n");
		}

		// DB schema version
		$result = $this->db->query("update settings set value = '4' where name = 'db_version'");
		if(!$result)
		{
			die("Failed setting new db schema version\n");
		}
	}

	function run_upgrade()
	{
		// Use a direct query instead of the equivelent function, because the
		// function may change with new versions.
		$query = $this->db->query("select value from settings where name = 'db_version'");
		$result = $query->row();
		$current_db_version = $result->value;

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
				$this->upgrade_1_to_2();
				echo "DB schema updated to v2\n";
			}
			if($current_db_version < 3)
			{
				$this->upgrade_2_to_3();
				echo "DB schema updated to v3\n";
			}
			if($current_db_version < 4)
			{
				$this->upgrade_3_to_4();
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
