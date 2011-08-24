<?php $this->load->helper('url') ?>
<?php echo $update_info['package_name'] ?>
<form action="<?php echo site_url('browser/mark_accepted_updates') ?>" method="post" onSubmit='return AcceptButton("<?php echo $system_info['id'] ?>", "<?php echo $update_info['package_name'] ?>", "<?php echo $update_info['change_state'] ?>", "<?php echo $update_div ?>")'>
	<input type="hidden" name="package_name" value="<?php echo $update_info['package_name'] ?>" />
	<input type="hidden" name="system_id" value="<?php echo $system_info['id'] ?>" />
	<input type="hidden" name="accepted_state" value="<?php echo $update_info['change_state'] ?>" />
	<input type="hidden" name="redirect_location" value="view_system/<?php echo $system_info['id'] ?>" />
	<input type="submit" value="<?php echo $update_info['change_button'] ?>" />
</form>