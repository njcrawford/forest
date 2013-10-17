<script type="text/javascript">
//<!--
function AcceptButton(system_id, package_name, accepted_state, update_div) {
        $.post("<?= site_url('browser/mark_accepted_updates') ?>", {system_id: system_id, package_name: package_name, accepted_state: accepted_state}, function() {
            $('#' + update_div).load("<?= site_url('browser/view_one_update') ?>" + '/' + system_id + '/' + encodeURIComponent(package_name));
        });
        return false;
}
//-->
</script>
<a href="<?= site_url() ?>">Back to summary page</a><br />
<a href="<?= site_url('browser/edit_system_info/' . $system_info->id) ?>">Edit system</a><br />
Name: <?= $system_info->name ?><br />
Updates: <?= count($updates) ?><br />
Reboot Needed: <?= ($system_info->reboot_required == 1) ? "Yes" : "No" ?><br>
Reboot Accepted: <?= ($system_info->reboot_accepted == 1) ? "Yes" : "No" ?>
<?php if($system_info->can_apply_reboot == 1) { ?>
	<form action="<?= site_url('browser/mark_accepted_reboot') ?>" method="post">
		<input type="hidden" name="state" value="<?= ($system_info->reboot_accepted == 1) ? '0' : '1' ?>">
		<input type="hidden" name="system_id" value="<?= $system_info->id ?>">
		<input type="submit" value="<?= ($system_info->reboot_accepted == 1) ? 'Cancel reboot request' : 'Request reboot' ?>">
	</form>
<?php } ?>
<br />
Last Check-in: <?= $system_info->last_checkin ?><br />
Client capabilities:
<ul>
	<li>can_apply_updates: <?= ($system_info->can_apply_updates == 1) ? "Yes" : "No" ?></li>
	<li>can_apply_reboot: <?= ($system_info->can_apply_reboot == 1) ? "Yes" : "No" ?></li>
	<li>client_version: <?= (empty($system_info->client_version)) ? "Unknown" : $system_info->client_version ?></li>
	<li>os_version: <?= (empty($system_info->os_version)) ? "Unknown" : $system_info->os_version ?></li>
</ul>
<a href="<?= site_url('browser/confirm_delete_system/' . $system_info->id) ?>">Delete System</a><br />
<?php if(count($updates) > 0) { ?>
<a href="<?= site_url('browser/confirm_clear_updates/' . $system_info->id) ?>">Clear Updates</a><br />
<?php } ?>
<br />
<?php if(count($updates) > 0) { ?>
Updates:
<ul>
	<li>
		<form action="<?= site_url('browser/mark_accepted_updates') ?>" method="post">
			<input type="hidden" name="system_id" value="<?= $system_info->id ?>" />
			<input type="hidden" name="accepted_state" value="accepted" />
			<input type="hidden" name="redirect_location" value="view_system/<?= $system_info->id ?>" />
			<input type="submit" value="Accept all" />
		</form> | 
		<form action="<?= site_url('browser/mark_accepted_updates') ?>" method="post">
			<input type="hidden" name="system_id" value="<?= $system_info->id ?>" />
			<input type="hidden" name="accepted_state" value="rejected" />
			<input type="hidden" name="redirect_location" value="view_system/<?= $system_info->id ?>" />
			<input type="submit" value="Reject all" />
		</form>
	</li>
	<?php foreach($updates as $this_update) { ?>
	<li>
		<?php if($this_update->is_locked) { ?>
			<?= $this_update->package_name ?> <?= $this_update->version ?> (locked)
		<?php } else { ?>
		<div id="update_<?= $this_update->id ?>">
			<?php
				$update_data['update_info'] = $this_update;
				$update_data['system_info'] = $system_info; 
				$update_data['update_div'] = "update_" . $this_update->id;
				$this->load->view('one_package', $update_data)
			?>
		</div>		
		<?php } ?>
	</li>
	<?php } ?>
</ul>
<?php } ?>

