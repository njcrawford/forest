<?php
/*
Forest - a web-based multi-system update manager

Copyright (C) 2011 Nathan Crawford
 
This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
 
You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
02111-1307, USA.

A copy of the full GPL 2 license can be found in the docs directory.
You can contact me at http://www.njcrawford.com/contact
*/

require "inc/check-login.php";

$page_title = "Summary";
require "inc/header.php";

require_once "inc/db.php";

//init systems array to avoid errors later
$systems = array();

// Get data from mysql
$systems_result = mysql_query(
"select * from (
        select 
        systems.id, 
        name,
        count(c.package_name) as packages, 
        sum(if(accepted is null, 0, accepted)) as accepted_count,
        reboot_required, 
        last_checkin,
        if(last_checkin < DATE_SUB(NOW(), INTERVAL 36 HOUR), 1, 0) as awol,
        if((last_checkin < DATE_SUB(NOW(), INTERVAL 36 HOUR)) and ignore_awol = 0, 1, 0) as important_awol,
        ignore_awol,
        reboot_accepted,
        allow_reboot,
        sum(locked) as locked_count
    from systems 
        left join 
        (
            (
                select 
                    updates.system_id, 
                    updates.package_name, 
                    updates.accepted, 
                    if(update_locks.package_name is null, 0, 1) as locked 
                from updates 
                left outer join (update_locks) on 
                (
                    updates.package_name = update_locks.package_name and 
                    updates.system_id = update_locks.system_id
                )
            ) 
            as c
        ) 
        on 
        (
            c.system_id = systems.id
        ) 
    group by systems.id
) b 
order by
    important_awol desc,
    awol, 
    packages desc, 
    reboot_required desc, 
    last_checkin"
);
$systems_row = mysql_fetch_assoc($systems_result);
while($systems_row)
{
	// Copy this row into systems array, and translate variables as needed
	$systems[$systems_row['id']] = $systems_row;

	// Translate reboot_required values
	if($systems[$systems_row['id']]['reboot_required'] == null)
	{
		$systems[$systems_row['id']]['reboot_required_text'] = "Unknown";
	}
	elseif($systems[$systems_row['id']]['reboot_required'] == 1)
	{
		if($systems[$systems_row['id']]['reboot_accepted'] == 1)
		{
			$systems[$systems_row['id']]['reboot_required_text'] = "Accepted";
		}
		else
		{
			$systems[$systems_row['id']]['reboot_required_text'] = "Yes";
		}
	}
	else
	{
		$systems[$systems_row['id']]['reboot_required_text'] = "No";
	}

	if($systems[$systems_row['id']]['locked_count'] == null)
	{
		$systems[$systems_row['id']]['locked_count'] = 0;
	}

	$accepted_temp = isset($systems[$systems_row['id']]['accepted_count']) ? $systems[$systems_row['id']]['accepted_count'] : 0;
	$systems[$systems_row['id']]['packages'] -= $systems[$systems_row['id']]['locked_count'] + $accepted_temp;

	$systems_row = mysql_fetch_assoc($systems_result);
}

if(count($systems) > 0)
{
?>
<br />
<h3>Updates available by system</h3>
<br />
<table>
<tr><th rowspan="2">System Name</th><th colspan="3">Updates       </th><th rowspan="2">Reboot<br />Required</th><th rowspan="2">Last Checkin</th></tr>
<tr>                                <th>Available</th><th>Accepted</th><th>Locked</th></tr>
<?php
	foreach($systems as $this_system)
	{
		$nice_checkin_class = "";
		if( ($this_system['awol'] == 1) && ($this_system['ignore_awol'] == 0) )
		{
			$nice_checkin_class = " class=\"awol\"";
		}
		$nice_reboot_class = (($this_system['reboot_required_text'] == "Yes") && ($this_system['awol'] == 0)) ? " class=\"reboot\"" : "";
?>
	<tr>
		<td class="name">
			<a href="systems.php?name=<?php echo $this_system['name'] ?>"><?php echo $this_system['name'] ?></a>
		</td>
		<td>
			<?php echo $this_system['packages'] ?>
<?php
		if($this_system['packages'] > 0 && ($this_system['packages'] != $this_system['accepted_count']))
		{
?>
			<form method="post" action="mark-accepted-updates.php">
				<input type="hidden" name="accepted" value="true">
				<input type="hidden" name="system_id" value="<?php echo $this_system['id'] ?>">
				<input type="submit" value="Accept all">
			</form>
<?php
		}
?>
		</td>
		<td><?php echo $this_system['accepted_count'] ?></td>
		<td><?php echo $this_system['locked_count'] ?></td>
		<td<?php echo $nice_reboot_class ?>>
			<?php echo $this_system['reboot_required_text'] ?>
<?php
		if($this_system['reboot_required'] == 1 && $this_system['allow_reboot'] == 1)
		{
			if($this_system['reboot_accepted'] != 1)
			{
?>
			<form method="post" action="mark-accepted-reboot.php">
				<input type="hidden" name="accepted" value="true">
				<input type="hidden" name="system_id" value="<?php echo $this_system['id'] ?>">
				<input type="submit" value="Accept">
			</form>
<?php
			}
			else
			{
?>
			<form method="post" action="mark-accepted-reboot.php">
				<input type="hidden" name="accepted" value="false">
				<input type="hidden" name="system_id" value="<?php echo $this_system['id'] ?>">
				<input type="submit" value="Reject">
			</form>
<?php
			}
		}
?>
		</td>
		<td<?php echo $nice_checkin_class ?>><?php echo $this_system['last_checkin'] ?></td>
	</tr>
<?php
	}
?>
</table>
<?php
}
?>
<br />
<h3>Updates available by package name</h3>
<br />
<table>
<tr><th rowspan="2">Name</th><th colspan="3">Systems       </th><th rowspan="2" style="width:4em">&nbsp;</th></tr>
<tr>                         <th>Available</th><th>Accepted</th><th>Locked</th></tr>
<?php
$systems_result = mysql_query("select updates.package_name, 
count(updates.system_id) as systems, 
sum(accepted) as accepted_count, 
sum(if(update_locks.package_name is not null, 1, 0)) as locked_count 
from updates left join (update_locks) 
on (updates.package_name = update_locks.package_name and updates.system_id = update_locks.system_id) 
group by updates.package_name");
$systems_row = mysql_fetch_assoc($systems_result);
while($systems_row)
{
?>
        <tr>
		<td class="name">
			<a href="packages.php?name=<?php echo $systems_row['package_name'] ?>"><?php echo $systems_row['package_name'] ?></a>
		</td>
		<td><?php echo ($systems_row['systems'] - $systems_row['accepted_count'] - $systems_row['locked_count']) ?></td>
		<td><?php echo $systems_row['accepted_count'] ?></td>
		<td><?php echo $systems_row['locked_count'] ?></td>
		<td>
<?php
	if(($systems_row['systems'] - $systems_row['accepted_count'] - $systems_row['locked_count']) > 0 && $systems_row['systems'] != $systems_row['accepted_count'])
	{
?>
			<form method="post" action="mark-accepted.php">
				<input type="hidden" name="accepted" value="true">
				<input type="hidden" name="package" value="<?php echo $systems_row['package_name'] ?>">
				<input type="submit" value="Accept all">
			</form>
<?php
	}
	else
	{
		echo "&nbsp;";
	}
?>
		</td>
	</tr>
<?php

        $systems_row = mysql_fetch_assoc($systems_result);
}
?>
</table>
<?php
require "inc/footer.php";
?>
