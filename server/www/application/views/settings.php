<table>
	<?php foreach($settings as $this_setting) { ?>
		<tr>
			<td><?= $this_setting->name ?></td>
			<td><?= $this_setting->value ?></td>
		</tr>
	<?php } ?>
</table>
