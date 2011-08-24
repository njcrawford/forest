<?php
/*
Forest - a web-based multi-system update manager

Copyright (C) 2011 Nathan Crawford
 
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
class Browser extends CI_Controller {

	function index()
	{
		if(!$this->_is_logged_in())
		{
			$this->load->helper('url');
			redirect("browser/login");
			return;
		}
		$this->load->model('forest_db');
		// this will get all info about all systems
		$data['systems'] = $this->forest_db->get_systems();
		foreach($data['systems'] as &$this_system)
		{
			$locked_updates_result = $this->forest_db->get_locked_updates_for_system($this_system['id']);
			$locked_updates = array();
			foreach($locked_updates_result as $this_lock)
			{
				$locked_updates[] = $this_lock['package_name'];
			}
			$this_system['updates'] = $this->forest_db->get_updates_for_system($this_system['id']);
			$this_system['available_updates'] = 0;
			$this_system['accepted_updates'] = 0;
			$this_system['locked_updates'] = 0;
			foreach($this_system['updates'] as $this_update)
			{
				if($this_update['accepted'] == 1)
				{
					$this_system['accepted_updates']++;
				}
				elseif(in_array($this_update['package_name'], $locked_updates))
				{
					$this_system['locked_updates']++;
				}
				else
				{
					$this_system['available_updates']++;
				}
			}
			
			// translate reboot_required value
			$this_system['reboot_required_class'] = "";
			if($this_system['reboot_required'] == null)
			{
				$this_system['reboot_required_text'] = "Unknown";
			}
			elseif($this_system['reboot_required'] == 1)
			{
				if($this_system['reboot_accepted'] == 1)
				{
					$this_system['reboot_required_text'] = "Accepted";
				}
				else
				{
					$this_system['reboot_required_text'] = "Yes";
					$this_system['reboot_required_class'] = "class=\"reboot\"";
				}
			}
			else
			{
				$this_system['reboot_required_text'] = "No";
			}
			
			// use CSS class "awal" for display of check-in time of awal systems
			if(strtotime("-3 days") > strtotime($this_system['last_checkin']))
			{
				$this_system['awol_class'] = "class=\"awol\"";
			}
			else
			{
				$this_system['awol_class'] = "";
			}
		}
		$data['page_title'] = "Summary";
		$this->load->view('overview', $data);
	}

	function view_system($system_id)
	{
		if(!$this->_is_logged_in())
		{
			$this->load->helper('url');
			redirect("browser/login");
			return;
		}
		$this->load->model('forest_db');
		// this will get all info about one system
		$data['system_info'] = $this->forest_db->get_system_info($system_id);
		$locked_updates_result = $this->forest_db->get_locked_updates_for_system($system_id);
		$locked_updates = array();
		foreach($locked_updates_result as $this_lock)
		{
			$locked_updates[] = $this_lock['package_name'];
		} 
		$data['updates'] = $this->forest_db->get_updates_for_system($system_id);
		foreach($data['updates'] as &$this_update)
		{
			// these are for html controls - they should be opposite of whatever accepted is now
			if($this_update['accepted'] == '1')
			{
				$this_update['change_state'] = "rejected"; 
				$this_update['change_button'] = "Reject";
			}
			else
			{
				$this_update['change_state'] = "accepted"; 
				$this_update['change_button'] = "Accept";
			}
			
			if(in_array($this_update['package_name'], $locked_updates))
			{
				$this_update['is_locked'] = true;
			}
			else
			{
				$this_update['is_locked'] = false;
			}
		}
		$data['page_title'] = "Details for " . $data['system_info']['name'];
		$this->load->view('system', $data);
	}
	
	function view_one_update($system_id, $package_name)
	{
		if(!$this->_is_logged_in())
		{
			return;
		}
		$this->load->model('forest_db');
		
		$data['system_info'] = $this->forest_db->get_system_info($system_id);
		
		$data['update_info'] = $this->forest_db->get_one_update($system_id, $package_name);
		$data['update_info'] = $data['update_info'][0];
		if($data['update_info']['accepted'] == '1')
		{
			$data['update_info']['change_state'] = "rejected"; 
			$data['update_info']['change_button'] = "Reject";
		}
		else
		{
			$data['update_info']['change_state'] = "accepted"; 
			$data['update_info']['change_button'] = "Accept";
		}
		
		$data['update_div'] = "update_" . $data['update_info']['id'];
		
		$this->load->view('one_package', $data);
	}

	function add_update_lock($system_id, $package_name)
	{
		if(!$this->_is_logged_in())
		{
			return;
		}
		$this->load->model('forest_db');
		$this->forest_db->do_something();
		redirect();
	}

	function remove_update_lock($system_id, $package_name)
	{
		if(!$this->_is_logged_in())
		{
			return;
		}
		$this->load->model('forest_db');
		$this->forest_db->do_something();
		redirect();
	}

	function clear_updates($system_id)
	{
		if(!$this->_is_logged_in())
		{
			return;
		}
		$this->load->model('forest_db');
		$this->forest_db->do_something();
		redirect();
	}

	function delete_system($system_id)
	{
		if(!$this->_is_logged_in())
		{
			return;
		}
		$this->load->model('forest_db');
		$this->forest_db->do_something();
		redirect();
	}

	function mark_accepted_reboot($system_id)
	{
		if(!$this->_is_logged_in())
		{
			return;
		}
		$this->load->model('forest_db');
		$this->forest_db->do_something();
		redirect();
	}

	function mark_accepted_updates()
	{
		if(!$this->_is_logged_in())
		{
			return;
		}
		$system_id = $this->input->post('system_id');
		$package_name = $this->input->post('package_name');
		$accepted_state = $this->input->post('accepted_state');
		$redirect_location = $this->input->post('redirect_location');
		
		// must have system_id and/or package name, and accepted state
		if(empty($system_id) && empty($package_name) && empty($accepted_state))
		{
			// show an error
			$data['page_title'] = "Error";
			$this->load->view('header', $data);
			$data['error_message'] = "System ID and/or package name along with accepted state must be specified.";
			$this->load->view('error', $data);
			$this->load->view('footer');
			return;
		}
		
		try
		{
			$this->load->model('forest_db');
			
			$locked_updates_result = $this->forest_db->get_locked_updates_for_system($system_id);
			$locked_updates = array();
			foreach($locked_updates_result as $this_lock)
			{
				if($package_name == $this_lock['package_name'])
				{
					// show an error
					$data['page_title'] = "Error";
					$this->load->view('header', $data);
					$data['error_message'] = "Package is locked";
					$this->load->view('error', $data);
					$this->load->view('footer');
					return;
				}
			} 
			
			$accepted_state = ($accepted_state == "accepted");
			$data['result_text'] = $this->forest_db->mark_accepted_updates($system_id, $package_name, $accepted_state);
			$data['redirect_location'] = "browser";
			if(!empty($redirect_location))
			{
				$data['redirect_location'] .= "/" . $redirect_location;
			} 
			$this->load->view('redirect', $data);
		}
		catch(Exception $e)
		{
			$data['page_title'] = "Error";
			$this->load->view('header', $data);
			$data['error_message'] = $e->getMessage();
			$this->load->view('error', $data);
			$this->load->view('footer');
		}
	}

	function save_system_info($system_id)
	{
		if(!$this->_is_logged_in())
		{
			return;
		}
		$this->load->model('forest_db');
		$this->forest_db->do_something();
		$this->load->view('redirect');
	}
	
	private function _is_logged_in()
	{
		if(session_id() == "")
		{
			session_start();
		}
		return !empty($_SESSION['login_name']);
	}
	
	private function _get_user_logged_in()
	{
		if(session_id() == "")
		{
			session_start();
		}
		
		if($this->_is_logged_in())
		{
			return $_SESSION['login_name'];
		}
		else
		{
			return "not logged in";
		}
	}
	
	public function login()
	{
		$data['page_title'] = "Login page";
		$this->load->view('header', $data);
		$this->load->view('login');
		$this->load->view('footer');
	}
	
	private function _set_logged_in_token($token)
	{
		if(session_id() == "")
		{
			session_start();
		}
		$_SESSION['login_name'] = $token;
	}
	
	public function log_me_in()
	{
		$username = $this->input->post('username');
		$password = $this->input->post('password');
		
		if(empty($username) || empty($password))
		{
			die('Username or password not provided');
		}
		
		$login_success = false;
		// do the actual login
		if($this->config->item('login_source') == "config_file")
		{
			foreach($this->config->item('allowed_users') as $this_user)
			{
				if($username == $this_user['username'] && $password == $this_user['password'])
				{
					$login_success = true;
					$this->_set_logged_in_token($username);
					break;
				}
			}
		}
		elseif($this->config->item('login_source') == "ldap")
		{
			$ds=ldap_connect($forest_config['ldap_server']);
			if (!$ds)
			{
				die('Cannot connect to LDAP server.');
			}
			$dn = $this->config->item('ldap_auth_var') . "=" . $username . ", " . $this->config->item('ldap_base');
		
			$result=@ldap_bind($ds,$dn,$password);
			
			$ldap_allowed_users = $this->config->item('ldap_allowed_users');
			if (!$result)
			{
				// Couldn't bind to LDAP server, see 
				$lderr = ldap_error($ds);
				if($lderr != "Invalid credentials")
				{
					echo "Error: " . ldap_error($ds) . "<br /><br />";
					die('Check your user name and password and try again.');
				}
			}
			elseif(!empty($ldap_allowed_users))
			{
				// make sure this user is allowed by config file
				foreach($ldap_allowed_users as $this_user)
				{
					if($username == $this_user)
					{
						$login_success = true;
						$this->_set_logged_in_token($username);
						break;
					}
				}
			}
			else
			{
				$login_success = true;
				$this->_set_logged_in_token($username);
			}
		}
		else
		{
			die("Invalid login_source in config file");
		}
		
		if($login_success)
		{
			// redirect to summary page
			$this->load->helper('url');
			redirect();
		}
		else
		{
			die("Invalid login information");
		}
	}
}
?>
