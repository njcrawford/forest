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

define("RPC_VERSION", 2);
define("RPC_ERROR_TAG", "error: ");
define("RPC_SUCCESS_TAG", "data_ok: ");

if(!empty($_GET['rpc_version']))
{
	$client_rpc_version = $_GET['rpc_version'];
}
else if(!empty($_POST['rpc_version']))
{
	$client_rpc_version = $_POST['rpc_version'];
}
else
{
	die(RPC_ERROR_TAG . "No rpc version specified");
}
if($client_rpc_version != RPC_VERSION)
{
	die(RPC_ERROR_TAG . "rpc version mismatch, server: " . RPC_VERSION . ", client: " . $_GET['rpc_version']);
}
?>
