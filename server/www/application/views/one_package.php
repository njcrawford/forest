<?= $update_info->package_name ?> <?= $update_info->version ?>
<?php if($system_info->can_apply_updates == 1) { ?>
	<form action="<?= site_url('browser/mark_accepted_updates') ?>" method="post" onSubmit='return AcceptButton("<?= $system_info->id ?>", "<?= $update_info->package_name ?>", "<?= $update_info->change_state ?>", "<?= $update_div ?>")'>
		<input type="hidden" name="package_name" value="<?= urlencode($update_info->package_name) ?>" />
		<input type="hidden" name="system_id" value="<?= $system_info->id ?>" />
		<input type="hidden" name="accepted_state" value="<?= $update_info->change_state ?>" />
		<input type="hidden" name="redirect_location" value="view_system/<?= $system_info->id ?>" />
		<input type="submit" value="<?= $update_info->change_button ?>" />
	</form>
<?php } ?>
