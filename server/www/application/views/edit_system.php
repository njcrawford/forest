System info<br>
<form action="<?= site_url('browser/save_system_info') ?>" method="post">
	<input type="hidden" name="system_id" value="<?= $system_info->id ?>">
	Ignore AWOL: <?= form_checkbox('ignore_awol', '1', ($system_info->ignore_awol == 1)) ?><br>
	Allow reboot: <?= form_checkbox('allow_reboot', '1', ($system_info->allow_reboot == 1)) ?><br>
	<input type="submit" value="Save">
</form>
<br>
<br>
<form action="<?= site_url('browser/add_update_lock') ?>" method="post">
	<input type="hidden" name="system_id" value="<?= $system_info->id ?>">
	New update lock: <input name="package_name">
	<input type="submit" value="Add">
</form>
<ul>
	<?php foreach($update_locks as $this_lock) { ?>
		<li>
			<?= $this_lock->package_name ?>
			<form action="<?= site_url('browser/remove_update_lock') ?>" method="post">
				<input type="hidden" name="system_id" value="<?= $system_info->id ?>">
				<input type="hidden" name="package_name" value="<?= $this_lock->package_name ?>">
				<input type="submit" value="Remove">
			</form>
		</li>
	<?php } ?>
</ul>
