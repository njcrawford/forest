<form action="<?= $post_url ?>" method="post">
	<input type="hidden" name="system_id" value="<?= $system_id ?>">
	Are you sure you want to <?= $action ?>?<br>
	<input type="submit" value="Yes">
</form>
<form action="<?= $back_url ?>" method="get">
	<input type="submit" value="No">
</form>
