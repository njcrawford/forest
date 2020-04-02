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

class Forest_DB extends CI_Model {

	function __construct()
	{
		// Call the Model constructor
		parent::__construct();
		
		// Check DB schema version
		$query = "select value from settings where name = 'db_version'";
		$result = $this->db->query($query);
		$row = $result->row();
		$db_version = $row->value;
		if($db_version != DB_VERSION)
		{
			die("Database schema is version " . $db_version . ", but application requires version " . DB_VERSION);
		}
	}

	function get_systems()
	{
		$query = "select * from systems";
		$result = $this->db->query($query);
		return $result->result();
	}

	function get_updates_for_system($system_id)
	{
		$query = "select * from updates where system_id = " . $this->db->escape($system_id) . " order by package_name";
		$result = $this->db->query($query);
		return $result->result();
	}
	
	function get_one_update($system_id, $package_name)
	{
		$query = "select * from updates where system_id = " . $this->db->escape($system_id) . " and package_name = " . $this->db->escape($package_name);
		$result = $this->db->query($query);
		return $result->row();
	}
	
	function get_update_locks($system_id)
	{
		$query = "select * from update_locks where system_id = " . $this->db->escape($system_id);
		$result = $this->db->query($query);
		return $result->result();
	}

	function get_accepted_updates_for_system($system_id)
	{
		$query = "select * from updates where system_id = " . $this->db->escape($system_id) . " and accepted = '1'";
		$result = $this->db->query($query);
		return $result->result();
	}

	function get_system_info($system_id)
	{
		$query = "select * from systems where id = " . $this->db->escape($system_id);
		$result = $this->db->query($query);
		return $result->row();
	}

	function save_system_info($system_id, $report_absent, $allow_reboot)
	{
		$query = "update systems set report_absent = " . $this->db->escape($report_absent) . ", allow_reboot = " . $this->db->escape($allow_reboot);
		// If disabling reboot requests, cancel potential pending request
		if($allow_reboot == 0)
		{
			$query .= ", reboot_accepted = 0";
		}

		$query .= " where id = " . $this->db->escape($system_id);
		return $this->db->query($query);
	}

	function get_system_id($system_name)
	{
		$retval = 0;
		$query = "select id from systems where name = " . $this->db->escape($system_name);
		$result = $this->db->query($query);
		if(!empty($result) && $result->num_rows() == 1)
		{
			$retval = $result->row()->id;
		}
		return $retval;
	}

	function add_update_lock($system_id, $package_name)
	{
		$query = "insert into update_locks (system_id, package_name) values (" . $this->db->escape($system_id) . ", " . $this->db->escape($package_name) . ")";
		return $this->db->query($query);
	}

	function remove_update_lock($system_id, $package_name)
	{
		$query = "delete from update_locks where system_id = " . $this->db->escape($system_id) . " and package_name = " . $this->db->escape($package_name);
		return $this->db->query($query);
	}

	function clear_updates($system_id)
	{
		$query = "delete from updates where system_id = " . $this->db->escape($system_id);
		return $this->db->query($query);
	}

    function clear_update_locks($system_id)
    {
        $query = "delete from update_locks where system_id = ?";
        return $this->db->query($query, array($system_id));
    }

	function delete_system($system_id)
	{
		$this->clear_updates($system_id);
        $this->clear_update_locks($system_id);
		$query = "delete from systems where id = " . $this->db->escape($system_id);
		return $this->db->query($query);
	}

	function mark_accepted_updates($system_id, $package_name, $state)
	{
		if(empty($system_id))
		{
			die("Must have system_id for mark accepted updates.");
		}

		$query = "UPDATE updates SET accepted = ? WHERE system_id = ?";
        $subs = array($state, $system_id);
		if(!empty($package_name))
		{
			// Specify a single update for this system to accept/reject
			$query .= " AND package_name = ?";
            $subs[] = $package_name;
		}

		// Limit accepting updates to those that are not in the update lock list
		$query .= " AND package_name NOT IN (SELECT package_name FROM update_locks WHERE system_id = ?)";
        $subs[] = $system_id;

		return $this->db->query($query, $subs);
	}

	function add_system($system_name)
	{
		$retval = 0;

		$query = "insert into systems (name) values(" . $this->db->escape($system_name) . ")";
		$result = $this->db->query($query);

		if($result)
		{
			$retval = $this->db->insert_id();
		}

		return $retval;
	}

	function system_checkin($system_id)
	{
		$query = "update systems set last_checkin = NOW() where id = " . $this->db->escape($system_id);
		return $this->db->query($query);
	}

	function save_updates($system_id, $updates)
	{
		$this->db->trans_start();

		$data_ok = TRUE;
		// walk through list of updates
		foreach($updates as $this_update)
		{
			if(!$data_ok)
			{
				break;
			}
			$query = "insert into updates
				(system_id, package_name, version)
				values
				(" . 
					$this->db->escape($system_id) . ", " . 
					$this->db->escape($this_update->package_name) . ", " . 
					$this->db->escape($this_update->version) . 
				")";
			$data_ok = $this->db->query($query);
		}

		$this->db->trans_complete();

		// return true/false to indicate success/fail
		return $this->db->trans_status();
	}

	function save_reboot_required($system_id, $reboot_required)
	{
		$query = "update systems set reboot_required = " . $this->db->escape($reboot_required) . " where id = " . $this->db->escape($system_id);
		return $this->db->query($query);
	}

	function save_reboot_accepted($system_id, $reboot_accepted)
	{
		$query = "update systems set reboot_accepted = " . $this->db->escape($reboot_accepted) . " where id = " . $this->db->escape($system_id);
		return $this->db->query($query);
	}

	function save_client_capabilities($system_id, $can_apply_updates, $can_apply_reboot, $client_version, $os_version)
	{
		$query = "update systems set
			can_apply_updates = " . $this->db->escape($can_apply_updates) . ",
			can_apply_reboot = " . $this->db->escape($can_apply_reboot) . ",
			client_version = " . $this->db->escape($client_version) . ",
			os_version = " . $this->db->escape($os_version) . "
			where id = " . $this->db->escape($system_id);
		return $this->db->query($query);
	}

	// Returns the value of one setting
	function get_setting($setting_name)
	{
		$query = $this->db->query("select * from settings where name = " . $this->db->escape($setting_name));
		return $query->row()->value;
	}

	// Returns all settings
	function get_all_settings()
	{
		$query = $this->db->query("select * from settings");
		return $query->result();
	}

	// Used with usort() to sort systems with the most updates to the start
	// of the array.
	function sort_by_updates_helper($a, $b)
	{
		if(count($a->updates) < count($b->updates))
		{
			return 1;
		}
		elseif(count($a->updates) > count($b->updates))
		{
			return -1;
		}
		else
		{
			return 0;
		}
	}
}

/* End of forest_db.php */
