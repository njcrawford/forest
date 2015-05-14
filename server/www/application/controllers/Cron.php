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

class Cron extends CI_Controller {

	public function index()
	{
		die("This function can only be run from CLI!");
	}

	public function summary_email()
	{
		if(!$this->input->is_cli_request())
		{
			die("This function can only be run from CLI!");
		}

		$this->load->model('forest_db');
		$this->load->library('email');

		$output_message = base_url() . "\n";
		$output_message .= "Forest version " . FOREST_VERSION . "\n\n";

		$update_message = "";
		$reboot_message = "";
		$absent_message = "";
		$absent_hours = $this->forest_db->get_setting('absent_hours');

		$systems = $this->forest_db->get_systems();
		if(count($systems) > 0)
		{
			foreach($systems as &$this_system_ref)
			{
				$this_system_ref->updates = $this->forest_db->get_updates_for_system($this_system_ref->id);
			}

			usort($systems, array($this->forest_db, "sort_by_updates_helper"));

			foreach($systems as $this_system)
			{
				if(strtotime($this_system->last_checkin) >= (time() - (60 * 60 * $absent_hours)))
				{
					if(count($this_system->updates) > 0)
					{
						$update_message .= $this_system->name . " (" . count($this_system->updates) . ")\n";
					}

					if($this_system->reboot_required == 1)
					{
						$reboot_message .= $this_system->name . "\n";
					}
				}
				elseif($this_system->report_absent == 1)
				{
					$absent_message .= $this_system->name . " (" . $this_system->last_checkin . ")\n";
				}
			}
		}
		else
		{
			$output_message .= "No systems registered\n";
		}
		
		if(empty($update_message))
		{
			$output_message .= "No systems have available updates\n";
		}
		else
		{
			$output_message .= "Updates available on these systems:\n";
			$output_message .= $update_message;
		}

		if(!empty($reboot_message))
		{
			$output_message .= "\n";
			$output_message .= "These systems need a reboot to finish installing updates:\n";
			$output_message .= $reboot_message;
		}

		if(!empty($absent_message))
		{
			$output_message .= "\n";
			$output_message .= "These systems have not checked in for more than " . $absent_hours . " hours:\n";
			$output_message .= $absent_message;
		}


		$this->email->from($this->config->item('email_from'), $this->config->item('email_name_from'));
		$this->email->to($this->config->item('email_to'), $this->config->item('email_name_to'));

		$this->email->subject($this->config->item('email_subject'));
		$this->email->message($output_message);

		if(!$this->email->send())
		{
			die("Failed to send email!\n" . $this->email->print_debugger());
		}
	}

}
/* End of Cron.php */
