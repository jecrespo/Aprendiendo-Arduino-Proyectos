<?php
$mysqli = mysqli_connect("qvm602.aprendiendoarduino.com", "qvm602", "password", "qvm602");
if (mysqli_connect_errno($mysqli)) {
    echo "Fallo al conectar a MySQL: " . mysqli_connect_error();
}

$resultado = mysqli_query($mysqli, "SELECT id, txtMsg, remoteNum FROM SMS WHERE delivered = 0 ORDER BY date LIMIT 1");

if ($resultado -> num_rows > 0){
	$fila = $resultado->fetch_assoc();
	echo ("|".$fila['txtMsg']."|".$fila['remoteNum']."|");
	//echo $fila['id'];
	$sql_update = "UPDATE SMS SET delivered = 1 WHERE id = ".$fila['id'];
	if (mysqli_query($mysqli, $sql_update)){
		//nothing to do
	}
	else{
		echo ("update error");
	}
}
else echo("No SMS");

$resultado->free();
$mysqli->close();
?>