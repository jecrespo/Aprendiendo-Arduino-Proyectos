<?php
function get_val ( $val )
{

	return $_GET[$val];

}

$sonda = get_val ("sensor");

$hoy = date ("Y-n-d H:i:s");
$ayer = strtotime($hoy)-86400;
$ayer = date("Y-n-d H:i:s",$ayer);
$query = "SELECT * FROM Datos WHERE arduino='$sonda' AND date BETWEEN '$ayer' AND '$hoy' ORDER BY date DESC";


imprime($query);

function imprime($query)
{
	
	// conexion a la BdD
	$conexion = mysql_connect("qvm602.aprendiendoarduino.com","qvm602","password");
	// selección de la Base de datos
	$seleccionar_bd = mysql_select_db("qvm602", $conexion);
	// compruebo que la conexion es corecta
	if (!$conexion || !$seleccionar_bd) {
		die('Unable to connect or select database!');
	}

	// lanzo la consulta
	
	$result = mysql_query($query,$conexion);
	
	if (!$result) {
		die("Fallo en la insercion de registro en la Base de Datos: " . mysql_error());
	}
	
	// funcion para imprimir consultas 
	
	echo "<table border='2'>";
	echo "<tr>";
	// nº de campos y filas de la consulta
	echo ("<td> Numero Campos Consulta </td>");
	echo ("<td> Numero Filas Consulta </td>");
	echo "</tr>";
	
	echo "<tr>";
	$num = mysql_num_fields($result);
	$num_row = mysql_num_rows ($result);
	echo ("<td> $num </td>");
	echo ("<td> $num_row </td>");
	echo "</tr>";
	
	echo "</table> <br>";
	
	// Pinto los campos de la consulta
	echo "<table border='2'>";
	echo "<tr>";
	while ($campo = mysql_fetch_field($result)){
		echo ("<td> <b> $campo->name </b> </td>");
	}
	echo "</tr>";
	
	// recorro todas las filas y saco el NI y el comentario
	while ($row = mysql_fetch_array($result,MYSQL_ASSOC)) {
		echo "<tr>";
		foreach ($row as $item){
			if (!$item){
				$item="-";
			}
			
			echo ("<td>$item</td>");
		}
		echo "</tr>";
	}
	echo "</table>";
	
	mysql_close($conexion);	
}

?>