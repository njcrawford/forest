<?php
$data['page_title'] = $page_title;
$this->load->view('header', $data);
$this->load->helper('url');
?>
<table>
	<tr>
		<th rowspan="2">System Name</th>
		<th colspan="3">Updates</th>
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
	</tr>
<?php } ?>
</table>
<?php 
$this->load->view('footer');
?>