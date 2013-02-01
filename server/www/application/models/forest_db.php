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
		$query = "select * from updates where system_id = '" . $system_id . "' order by package_name";
		$result = $this->db->query($query);
		return $result->result();
	}
	
	function get_one_update($system_id, $package_name)
	{
		$query = "select * from updates where system_id = '" . $system_id . "' and package_name = '" . $package_name . "'";
		$result = $this->db->query($query);
		return $result->result();
	}
	
	function get_locked_updates_for_system($system_id)
	{
		$query = "select * from update_locks where system_id = '" . $system_id . "'";
		$result = $this->db->query($query);
		return $result->result();
	}

	function get_accepted_updates_for_system($system_id)
	{
		$query = "select * from updates where system_id = '" . $system_id . "' where accepted = '1'";
		$result = $this->db->query($query);
		return $result->result();
	}

	function get_system_info($system_id)
	{
		$query = "select * from systems where id = '" . $system_id . "'";
		$result = $this->db->query($query);
		return $result->row();
	}

	function get_system_id($system_name)
	{
		$retval = 0;
		$query = "select id from systems where name = '" . $system_name . "'";
		$result = $this->db->query($query);
		if($result)
		{
			$retval = $result->row()->id;
		}
		return $retval;
	}

	function add_update_lock($system_id, $package_name)
	{
		$query = "insert into update_locks (system_id, package_name) values ('" . $system_id . "', '" . $package_name . "')";
		$result = $this->db->query($query);
		return $result->result();
	}

	function remove_update_lock($system_id, $package_name)
	{
		$query = "delete from update_locks where system_id = '" . $system_id . "' and package_name = '" . $package_name . "'";
		$result = $this->db->query($query);
		return $result->result();
	}

	function clear_updates($system_id)
	{
		$query = "delete from updates where system_id = '" . $system_id . "'";
		$result = $this->db->query($query);
		return $result->result();
	}

	function delete_system($system_id)
	{
		$this->clear_updates($system_id);
		$query = "delete from systems where id = '" . $system_id . "'";
		$result = $this->db->query($query);
		return $result->result();
	}

	function mark_accepted_reboot($system_id)
	{
		$query = "update systems set reboot_accepted = '1' where system_id = '" . $system_id . "'";
		$result = $this->db->query($query);
		return $result->result();
	}

	function mark_accepted_updates($system_id, $package_name, $state)
	{
		$query = "update updates set accepted = '" . $state . "' where ";
		if(!empty($system_id) && !empty($package_name))
		{
			$query .= " system_id = '" . $system_id . "' and package_name = '" . $package_name . "'";
		}
		elseif(!empty($system_id))
		{
			$query .= " system_id = '" . $system_id . "'";
		}
		elseif(!empty($package_name))
		{
			$query .= " package_name = '" . $package_name . "'";
		}
		$result = $this->db->query($query);
		return $result->result();
	}

	function add_system($system_name)
	{
		$retval = 0;

		$query = "insert into systems (name) values('" . $system_name . "')";
		$result = $this->db->query($query);

		if($result)
		{
			$query = "select LAST_INSERT_ID() as id";
			$result = $this->db->query($query);
			$retval = $result->row()->id;
		}

		return $retval;
	}

	function system_checkin($system_id)
	{
		$query = "update systems set last_checkin = NOW() where id = '" . $system_id . "'";
		$result = $this->db->query($query);
		return $result->result();
	}

	function save_updates($system_id, $updates)
	{
		$transaction_ok = true;

		// start transaction
		$query = "START TRANSACTION";
		$transaction_ok = $this->db->query($query);

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

		// if success, commit transation
		if($transaction_ok)
		{
			$query = "COMMIT";
			$transaction_ok = $this->db->query($query);
		}
		// if fail, rollback transaction
		else
		{
			$query = "ROLLBACK";
			$this->db->query($query);
		}

		// return true/false to indicate success/fail
		return $transaction_ok;
	}

	function save_reboot_required($system_id, $reboot_required)
	{
		$query = "update systems set reboot_required = '" . $reboot_required . "' where system_id = '" . $system_id . "'";
		$result = $this->db->query($query);
		return $result->result();
	}

	function save_reboot_accepted($system_id, $reboot_accepted)
	{
		$query = "update systems set reboot_accepted = '" . $reboot_accepted . "' where '" . $system_id . "'";
		$result = $this->db->query($query);
		return $result->result();
	}

	function save_client_capabilities($system_id, $can_apply_updates, $can_apply_reboot)
	{
		$query = "update systems set can_apply_updates = '" . $can_apply_updates . "', can_apply_reboot = '" . $can_apply_reboot . "' where '" . $system_id . "'";
		$result = $this->db->query($query);
		return $result->result();
	}
}

/* End of forest_db.php */
