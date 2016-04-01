<?php

//recojo variables
if(isset($_GET["nombre"]) && isset($_GET["mensaje"])){
	$nombre = $_GET["nombre"];
	$mensaje = $_GET["mensaje"];
}
else{
	die();
}

//Crear conexión a la Base de Datos
$conexion = mysql_connect("localhost","qvm602","password");
if (!$conexion){
	die("Fallo la conexión a la Base de Datos: " . mysql_error());
}

// Seleccionar la Base de Datos a utilizar
$seleccionar_bd = mysql_select_db("bbdd", $conexion);
if (!$seleccionar_bd) {
	die("Fallo la selección de la Base de Datos: " . mysql_error());
}

// compruebo que la conexion es corecta
if (!$conexion || !$seleccionar_bd) {
	die('Fallo la conexión o la selección de la Base de Datos: ');
}

$query = "INSERT INTO Datos (Fecha, nombre, mensaje) VALUES (CURRENT_TIMESTAMP, {$nombre}, {$mensaje})";
	
$result = mysql_query($query,$conexion);

if (!$result) {
		die("Fallo en la insercion de registro en la Base de Datos: " . mysql_error());
	}
	
mysql_close($conexion);

echo ("GRABADOS");

?>
