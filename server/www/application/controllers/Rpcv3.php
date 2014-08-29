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

class RPCv3 extends CI_Controller {

	function index()
	{
		// Tell the client that RPCv3 is supported
		echo "RPCv3";
	}

	function get_accepted_updates()
	{
		// Get data for this client from DB
		// TODO: Actually get data from DB
		$reboot_requested = "false";
		$accepted_updates = array(
			array(
				'package_name' => 'test-update',
				'version' => 'test-version'
			)
		);

		// Send XML response back to client

		// Start with a basic structure
		$server_xml = simplexml_load_string('<response forest_rpc="3"></response>');

		// Reboot request
		$server_xml->addChild('reboot_requested', $reboot_requested);

		// Accepted updates (list)
		$update_list_node = $server_xml->addChild('accepted_updates');
		foreach($accepted_updates as $this_update)
		{
			$update_node = $update_list_node->addChild('update');
			$update_node['name'] = $this_update['package_name'];
			$update_node['version'] = $this_update['version'];
		}

		echo $server_xml->asXML();
	}

	function check_in()
	{
		// Parse XML from client
		$client_xml_string = $this->input->post('client_xml');
		$client_xml = simplexml_load_string($client_xml_string);
		if($client_xml === FALSE)
		{
			die("Invalid XML recieved");
		}

		// Verify that XML is client data
		if($client_xml['forest_rpc'] != 3)
		{
			die("XML is not a Forest RPC v3 request");
		}

		// Client capabilities

		// Apply updates
		$client_caps['apply_updates'] = (string)$client_xml->capabilities->apply_updates;
		if($client_caps['apply_updates'] != "true" && $client_caps['apply_updates'] != "false")
		{
			$client_caps['apply_updates'] = "unknown";
		}
		// Apply reboot
		$client_caps['apply_reboot'] = (string)$client_xml->capabilities->apply_reboot;
		if($client_caps['apply_reboot'] != "true" && $client_caps['apply_reboot'] != "false")
		{
			$client_caps['apply_reboot'] = "unknown";
		}
		// Client version
		$client_caps['client_version'] = (string)$client_xml->capabilities->client_version;
		if(empty($client_caps['client_version']))
		{
			$client_caps['client_version'] = "unknown";
		}
		// OS version
		$client_caps['os_version'] = (string)$client_xml->capabilities->os_version;
		if(empty($client_caps['os_version']))
		{
			$client_caps['os_version'] = "unknown";
		}
			
		// System information

		// System name
		$system_name = $client_xml->system_name;
		if(empty($system_name))
		{
			die("Forest RPCv3 requires system_name");
		}

		// Available update names and versions (list)
		$available_updates = array();
		foreach($client_xml->available_updates as $this_update)
		{
			$update_name = (string)$this_update['name'];
			if(empty($update_name))
			{
				die("Forest RPCv3 requires all updates to have a name");
			}

			$update_version = (string)$this_update['version'];
			if(empty($update_version))
			{
				die("Forest RPCv3 requires all updates to have a version");
			}

			$available_updates[] = array('name' => $update_name, 'version' => $update_version);
		}

		// Reboot needed to complete update
		$reboot_needed = (string)$client_xml->reboot_needed;
		if($reboot_needed != "true" && $reboot_needed != "false")
		{
			$reboot_needed = "unknown";
		}

		// Reboot request was applied
		$reboot_applied = (string)$client_xml->reboot_applied;
		if($reboot_applied != "true")
		{
			$reboot_needed = "false";
		}

		// Process client data
		// TODO: implement this section
	}
}
/* End of Rpcv3.php */
