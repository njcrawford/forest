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

class RPCv2 extends CI_Controller {

	function index()
	{
		redirect("browser");
	}
	
	function collect()
	{
		$this->load->model('forest_db');

		$system_name = $this->input->post('system_name');
		$available_updates = $this->input->post('available_updates');
		$versions = $this->input->post('versions');
		$no_updates_available = $this->input->post('no_updates_available');
		$reboot_required = $this->input->post('reboot_required');
		$reboot_attempted = $this->input->post('reboot_attempted');
		$client_can_apply_updates = $this->input->post('client_can_apply_updates');
		$client_can_apply_reboot = $this->input->post('client_can_apply_reboot');

		if(empty($system_name))
		{
			echo RPC_ERROR_TAG . "No system specified";
			return;
		}

		$system_id_ok = false;
		$update_data_ok = false;

		$system_id = $this->forest_db->get_system_id($system_name);

		if($system_id == 0)
		{
			$system_id = $this->forest_db->add_system($system_name);
			if($system_id == 0)
			{
				die(RPC_ERROR_TAG . "Mysql error: could not add system to database");
			}
		}

		if(!empty($available_updates))
		{
			$data_ok = true;
			$use_versions = false;
			// Forget about old updates before adding new ones
			$this->forest_db->system_checkin($system_id);
			$this->forest_db->clear_updates($system_id);

			$temp_packages = explode(",", $available_updates);
			if(!empty($versions))
			{
				$versions = explode("|", $versions);
				if(count($versions) == count($temp_packages))
				{
					$use_versions = true;
				}
			}

			// build an array to save info for all packages that need updated
			$packages = array();
			for($i = 0; $i < count($temp_packages); $i++)
			{
				$this_package = new stdClass;
				// Trim package names to protect against spaces in package names in the database
				$this_package->package_name = trim($temp_packages[$i]);
				if($use_versions)
				{
					$this_package->version = trim($versions[$i]);
				}
				$packages[] = $this_package;
			}
			$data_ok = $this->forest_db->save_updates($system_id, $packages);

			//send back a message indicating data received (or not)
			if($data_ok)
			{
				echo RPC_SUCCESS_TAG;
				$update_data_ok = TRUE;
			}
			else
			{
				echo RPC_ERROR_TAG . "data error";
			}
		}
		elseif(!empty($no_updates_available))
		{
			// Forget about old updates and save checkin time
			$this->forest_db->system_checkin($system_id);
			$this->forest_db->clear_updates($system_id);
			echo RPC_SUCCESS_TAG;
		}
		else
		{
			die(RPC_ERROR_TAG . "missing both available_updates and no_updates_available");
		}

		// the reboot_required section is optional for rpc v1
		if(!empty($reboot_required))
		{
			switch($reboot_required)
			{
				case "true":
					$reboot_required = TRUE;
					break;
				case "false":
					$reboot_required = FALSE;
					break;
			}
		}
		// force reboot_required to false if a reboot was attempted
		if($reboot_required == TRUE && !empty($reboot_attempted) && $reboot_attempted == "true")
		{
			$reboot_required = FALSE;
		}
		$this->forest_db->save_reboot_required($system_id, $reboot_required);

		// reset the accepted flag if the system no longer needs a reboot, or a reboot was attempted
		if($reboot_required != TRUE || (!empty($reboot_attempted) && $reboot_attempted == "true"))
		{
			$this->forest_db->save_reboot_accepted($system_id, FALSE);
		}
		// Collect reported client capabilities, if present
		if($client_can_apply_updates == "true")
		{
			$client_can_apply_updates = TRUE;
		}
		else
		{
			$client_can_apply_updates = FALSE;
		}
		if($client_can_apply_reboot == "true")
		{
			$client_can_apply_reboot = TRUE;
		}
		else
		{
			$client_can_apply_reboot = FALSE;
		}
		$this->forest_db->save_client_capabilities($system_id, $client_can_apply_updates, $client_can_apply_reboot, "", "");
	}

	function get_accepted()
	{
		$this->load->model('forest_db');

		$system = $this->input->get('system');

		if(empty($system))
		{
			die(RPC_ERROR_TAG . "No system specified");
		}

		$system_id = $this->forest_db->get_system_id($system);
		if($system_id == 0)
		{
			die(RPC_ERROR_TAG . "System not found in database");
		}
		$system_row = $this->forest_db->get_system_info($system_id);

		$updates_result = $this->forest_db->get_accepted_updates_for_system($system_id);
		if($updates_result === FALSE)
		{
			die(RPC_ERROR_TAG . "Mysql error: couldn't get accepted updates");
		}

		echo RPC_SUCCESS_TAG;

		if($system_row->reboot_accepted == 1)
		{
			echo "reboot-true: ";
		}
		else
		{
			echo "reboot-false: ";
		}
		foreach($updates_result as $updates_row)
		{
			echo $updates_row->package_name . " ";
		}
	}
	
}
/* End of Rpcv2.php */
