<?php

class Forest_DB extends CI_Model {

	function __construct()
	{
		// Call the Model constructor
		parent::__construct();
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
		$query = "select * from updates where system_id = " . $this->db->escape($system_id) . " where accepted = '1'";
		$result = $this->db->query($query);
		return $result->result();
	}

	function get_system_info($system_id)
	{
		$query = "select * from systems where id = " . $this->db->escape($system_id);
		$result = $this->db->query($query);
		return $result->row();
	}

	function save_system_info($system_id, $ignore_awol, $allow_reboot)
	{
		$query = "update systems set ignore_awol = " . $this->db->escape($ignore_awol) . ", allow_reboot = " . $this->db->escape($allow_reboot) . " where id = " . $this->db->escape($system_id);
		return $this->db->query($query);
	}

	function get_system_id($system_name)
	{
		$retval = 0;
		$query = "select id from systems where name = " . $this->db->escape($system_name);
		$result = $this->db->query($query);
		if($result)
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
		$result = $this->db->query($query);
		return $result->result();
	}

	function delete_system($system_id)
	{
		$this->clear_updates($system_id);
		$query = "delete from systems where id = " . $this->db->escape($system_id);
		$result = $this->db->query($query);
		return $result->result();
	}

	function mark_accepted_updates($system_id, $package_name, $state)
	{
		$query = "update updates set accepted = " . $this->db->escape($state) . " where ";
		if(!empty($system_id) && !empty($package_name))
		{
			$query .= " system_id = " . $this->db->escape($system_id) . " and package_name = " . $this->db->escape($package_name);
		}
		elseif(!empty($system_id))
		{
			$query .= " system_id = " . $this->db->escape($system_id);
		}
		elseif(!empty($package_name))
		{
			$query .= " package_name = " . $this->db->escape($package_name);
		}
		else
		{
			die("Must have system_id, package_name or both for mark accepted updates.");
		}
		return $this->db->query($query);
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
		$result = $this->db->query($query);
		return $result->result();
	}

	function save_updates($system_id, $updates)
	{
		$this->db->trans_start();

		// walk through list of updates
		foreach($updates as $this_update)
		{
			if($transaction_ok)
			{
				$query = "insert into updates
					(system_id, package_name, version)
					values
					('" . $system_id . "', '" . $this_update['package_name'] . "', '" . $this_update['version'] ."' )";
				$data_ok = $this->db->query($query);
			}
			else
			{
				break;
			}
		}

		$this->db->trans_complete();

		// return true/false to indicate success/fail
		return $this->db->trans_status();
	}

	function save_reboot_required($system_id, $reboot_required)
	{
		$query = "update systems set reboot_required = " . $this->db->escape($reboot_required) . " where system_id = " . $this->db->escape($system_id);
		$result = $this->db->query($query);
		return $result->result();
	}

	function save_reboot_accepted($system_id, $reboot_accepted)
	{
		$query = "update systems set reboot_accepted = " . $this->db->escape($reboot_accepted) . " where system_id = " . $this->db->escape($system_id);
		$result = $this->db->query($query);
		return $result->result();
	}

	function save_client_capabilities($system_id, $can_apply_updates, $can_apply_reboot)
	{
		$query = "update systems set can_apply_updates = " . $this->db->escape($can_apply_updates) . ", can_apply_reboot = " . $this->db->escape($can_apply_reboot) . " where " . $this->db->escape($system_id);
		$result = $this->db->query($query);
		return $result->result();
	}

	function get_setting($setting_name)
	{
		//TODO: Finish implementing this stub function
		if($setting_name == 'awol_hours')
		{
			return 36;
		}
	}
}

/* End of forest_db.php */
