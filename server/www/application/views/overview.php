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
		<td class="name"><a href="<?= site_url('browser/view_system/' . $this_system->id) ?>"><?= $this_system->name ?></a></td>
		<td><?= $this_system->available_updates ?></td>
		<td><?= $this_system->accepted_updates ?></td>
		<td><?= $this_system->locked_updates ?></td>
		<td <?= $this_system->reboot_required_class ?>><?= $this_system->reboot_required_text ?></td>
		<td <?= $this_system->awol_class ?>><?= $this_system->last_checkin ?></td>
	</tr>
<?php } ?>
</table>
</center>

