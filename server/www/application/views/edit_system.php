System info<br>
<form action="<?= site_url('browser/save_system_info') ?>" method="post">
<input type="hidden" name="system_id" value="<?= $system_info->id ?>">
Ignore AWOL: <?= form_checkbox('ignore_awol', '1', ($system_info->ignore_awol == 1)) ?><br>
Allow reboot: <?= form_checkbox('allow_reboot', '1', ($system_info->allow_reboot == 1)) ?><br>
<input type="submit" value="Save">
</form>
