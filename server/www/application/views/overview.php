<?php
$data['page_title'] = $page_title;
$this->load->view('header', $data);
$this->load->helper('url');
?>
<center>
<h3>Updates available by system</h3>
<table>
	<tr>
		<th rowspan="2">System Name</th>
		<th colspan="3">Updates</th>
		<th rowspan="2">Reboot Required</th>
		<th rowspan="2">Last Checkin</th>
	</tr>
	<tr>
		<th>Available</th>
		<th>Accepted</th>
		<th>Locked</th>
	</tr>
<?php foreach($systems as $this_system) { ?>
	<tr>
		<td><a href="<?php echo site_url("browser/view_system/" . $this_system['id']) ?>"><?php echo $this_system['name'] ?></a></td>
		<td><?php echo $this_system['available_updates'] ?></td>
		<td><?php echo $this_system['accepted_updates'] ?></td>
		<td><?php echo $this_system['locked_updates'] ?></td>
		<td><?php echo $this_system['reboot_required'] ?></td>
		<td><?php echo $this_system['last_checkin'] ?></td>
	</tr>
<?php } ?>
</table>
</center>
<?php 
$this->load->view('footer');
?>