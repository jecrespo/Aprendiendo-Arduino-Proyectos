<?php
$mysqli = mysqli_connect("localhost", "arduino", "password", "SendSMS");
if (mysqli_connect_errno($mysqli)) {
    echo "Fallo al conectar a MySQL: " . mysqli_connect_error();
}

$resultado = mysqli_query($mysqli, "SELECT id, txtMsg, remoteNum FROM messages WHERE delivered = 0 ORDER BY date LIMIT 1");

if ($resultado -> num_rows > 0){
	$fila = $resultado->fetch_assoc();
	echo ("|".$fila['txtMsg']."|".$fila['remoteNum']."|");
	//echo $fila['id'];
	$sql_update = "UPDATE messages SET delivered = 1 WHERE id = ".$fila['id'];
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