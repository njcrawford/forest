<?php
$rpc_version = 1;
$RPC_ERROR_TAG = "error: ";
$RPC_SUCCESS_TAG = "data_ok: ";

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
	die($RPC_ERROR_TAG . "No rpc version specified");
}
if($client_rpc_version != $rpc_version)
{
	die($RPC_ERROR_TAG . "rpc version mismatch, server: " . $rpc_version . ", client: " . $_GET['rpc_version']);
}
?>
