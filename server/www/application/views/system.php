<?php
$data['page_title'] = $page_title;
$this->load->view('header', $data);
$this->load->helper('url');
?>
<script type="text/javascript">
//<!--
function AcceptButton(system_id, package_name, accepted_state, update_div) {
        $.post("<?php echo site_url('browser/mark_accepted_updates') ?>", {system_id: system_id, package_name: package_name, accepted_state: accepted_state}, function() {
            $('#' + update_div).load("<?php echo site_url('browser/view_one_update') ?>" +'/' + system_id + '/' + package_name);
        });
        return false;
}
//-->
</script>

Name: <?php echo $system_info['name'] ?><br />
Updates: <?php echo count($updates) ?><br />
Reboot Needed: <?php echo $system_info['reboot_required'] ?><br />
Last checkin: <?php echo $system_info['last_checkin'] ?><br />
Client capabilities:
<ul>
	<li>can_apply_updates: <?php echo $system_info['can_apply_updates'] ?></li>
	<li>can_apply_reboot: <?php echo $system_info['can_apply_reboot'] ?></li>
</ul>
<a href="<?php echo site_url('browser/confirm/delete_system/' . $system_info['id']) ?>">Delete System</a><br />
<?php if(count($updates) > 0) { ?>
<a href="<?php echo site_url('browser/confirm/clear_updates/' . $system_info['id']) ?>">Clear Updates</a><br />
<?php } ?>
<br />
<?php if(count($updates) > 0) { ?>
Updates:
<ul>
	<li>
		<form action="<?php echo site_url('browser/mark_accepted_updates') ?>" method="post">
			<input type="hidden" name="system_id" value="<?php echo $system_info['id'] ?>" />
			<input type="hidden" name="accepted_state" value="accepted" />
			<input type="hidden" name="redirect_location" value="view_system/<?php echo $system_info['id'] ?>" />
			<input type="submit" value="Accept all" />
		</form> | 
		<form action="<?php echo site_url('browser/mark_accepted_updates') ?>" method="post">
			<input type="hidden" name="system_id" value="<?php echo $system_info['id'] ?>" />
			<input type="hidden" name="accepted_state" value="rejected" />
			<input type="hidden" name="redirect_location" value="view_system/<?php echo $system_info['id'] ?>" />
			<input type="submit" value="Reject all" />
		</form>
	</li>
	<?php foreach($updates as $this_update) { ?>
	<li>
		<?php if($this_update['is_locked']) { ?>
			<?php echo $this_update['package_name'] ?> (locked)
		<?php } else { ?>
		<div id="update_<?php echo $this_update['id'] ?>">
			<?php
				$update_data['update_info'] = $this_update;
				$update_data['system_info'] = $system_info; 
				$update_data['update_div'] = "update_" . $this_update['id'];
				$this->load->view('one_package', $update_data)
			?>
		</div>		
		<?php } ?>
	</li>
	<?php } ?>
</ul>
<?php } ?>
<?php 
$this->load->view('footer');
?>
