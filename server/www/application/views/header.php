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
$this->load->helper('url');

if(!isset($page_title))
{
	$page_title = "(Page Title)";
}
?>
<html>
<head>
<title><?php echo $page_title ?> - Forest</title>
<link rel="stylesheet" type="text/css" href="<?php echo site_url("css/forest.css") ?>" />
</head>
<body>
<script language="javascript" src="<?php echo site_url("jquery/jquery-1.6.1.min.js") ?>"></script>
