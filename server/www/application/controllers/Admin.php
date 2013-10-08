<?php
/*
Forest - a web-based multi-system update manager

Copyright (C) 2013 Nathan Crawford
 
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

class Admin extends CI_Controller {

	public function index()
	{
		$this->load->view('header', array('title' => 'Admin options'));
		$this->load->view('admin_landing');
		$this->load->view('footer');
	}

	public function view_settings()
	{
		$this->load->model('forest_db');

		$data = new stdClass;
		$data->settings = $this->forest_db->get_all_settings();

		$this->load->view('header', array('title' => 'View Settings'));
		$this->load->view('settings', $data);
		$this->load->view('footer');
	}

	public function upgrade_db()
	{
		$this->load->model('upgrade_db');

		$doit = $this->input->post('doit');

		if($doit != "yes")
		{
			// Display confirmation
		}
		else
		{
			// Do DB upgrade
			$this->upgrade_db->run_upgrade();
		}
	}
}
/* End of Admin.php */
