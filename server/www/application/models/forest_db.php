<?php

class Forest_DB extends CI_Model {

	function __construct()
	{
		// Call the Model constructor
		parent::__construct();
	}

	function connect_to_db()
	{
		mysql_connect($this->config->item('db_hostname'), 
			$this->config->item('db_username'), 
			$this->config->item('db_password'));
		mysql_select_db($this->config->item('db_name'));
	}

	function run_db_query($query)
	{
		$result = mysql_query($query);
		$retval = array();
		for($row = mysql_fetch_assoc($result); $row; $row = mysql_fetch_assoc($result))
		{
			$retval[] = $row;
		}
		return $retval;
	}

	function get_systems()
	{
		$this->connect_to_db();
		$query = "select * from systems";
		return $this->run_db_query($query);
	}

	function get_updates_for_system($system_id)
	{
		$this->connect_to_db();
		$query = "select * from updates where system_id = '" . $system_id . "'";
		return $this->run_db_query($query);
	}
	
	function get_one_update($system_id, $package_name)
	{
		$this->connect_to_db();
		$query = "select * from updates where system_id = '" . $system_id . "' and package_name = '" . $package_name . "'";
		return $this->run_db_query($query);
	}
	
	function get_locked_updates_for_system($system_id)
	{
		$this->connect_to_db();
		$query = "select * from update_locks where system_id = '" . $system_id . "'";
		return $this->run_db_query($query);
	}

	function get_system_info($system_id)
	{
		$this->connect_to_db();
		$query = "select * from systems where id = '" . $system_id . "'";
		$result = $this->run_db_query($query);
		return $result[0];
	}

	function add_update_lock($system_id, $package_name)
	{
		$this->connect_to_db();
		$query = "";
		return $this->run_db_query($query);
	}

	function remove_update_lock($system_id, $package_name)
	{
		$this->connect_to_db();
		$query = "";
		return $this->run_db_query($query);
	}

	function clear_updates($system_id)
	{
		$this->connect_to_db();
		$query = "";
		return $this->run_db_query($query);
	}

	function delete_system($system_id)
	{
		$this->connect_to_db();
		$query = "";
		return $this->run_db_query($query);
	}

	function mark_accepted_reboot($system_id)
	{
		$this->connect_to_db();
		$query = "";
		return $this->run_db_query($query);
	}

	function mark_accepted_updates($system_id, $package_name, $state)
	{
		$this->connect_to_db();
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
		return $this->run_db_query($query);
	}

	function save_system_info($system_id, $package_name)
	{
		$this->connect_to_db();
		$query = "";
		return $this->run_db_query($query);
	}
}
?>
