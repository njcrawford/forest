<?php
define("RPC_VERSION", 1);
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
if($client_rpc_version != $forest_versions['rpc'])
{
	die(RPC_ERROR_TAG . "rpc version mismatch, server: " . RPC_VERSION . ", client: " . $_GET['rpc_version']);
}
?>
