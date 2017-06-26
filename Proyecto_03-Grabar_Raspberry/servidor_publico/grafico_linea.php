<?php 

include("jpgraph/src/jpgraph.php");
include("jpgraph/src/jpgraph_line.php");
include("jpgraph/src/jpgraph_date.php");

function get_val ( $val )
{

	return $_GET[$val];

}

$sonda = get_val ("sensor");

print_grafica($sonda);


function print_grafica($sensor)
{

	$ydata = collect_data($sensor, 'dato');
	$horas  = collect_data($sensor,'date');

	$graph = new Graph(550,350,"auto");
	$graph->SetScale("datint");
	$graph->xgrid->Show();
	$lineplot=new LinePlot($ydata,$horas);
	$lineplot->SetColor("black");
	$graph->img->SetMargin(20,20,20,100);
	$graph->title->Set("Sensor $sensor");
	$graph->yaxis->title->Set("Temperatura");
	$graph->xaxis->SetLabelAngle(45);
	$graph->SetShadow();
	$graph->Add($lineplot);
	$graph->Stroke();
	}

	function collect_data($sonda,$campo)
{

	$datay = array();

	// conexion a la BdD
	$conexion = mysql_connect("qvm602.aprendiendoarduino.com","qvm602","password");
	// selección de la Base de datos
	$seleccionar_bd = mysql_select_db("qvm602", $conexion);
	// compruebo que la conexion es corecta
	if (!$conexion || !$seleccionar_bd) {
		die('Unable to connect or select database!');
	}

	$hoy = date ("Y-n-d H:i:s");
	$ayer = strtotime($hoy)-86400;
	$ayer = date("Y-n-d H:i:s",$ayer);
	$query = "SELECT * FROM Datos WHERE arduino='$sonda' AND date BETWEEN '$ayer' AND '$hoy' ORDER BY date ASC";
	
	$grafic = mysql_query($query,$conexion);

	while ($row_grafic = mysql_fetch_array($grafic,MYSQL_ASSOC)) {

		if ($campo == "date"){
			$dato = $row_grafic[$campo];
			$dato = strtotime($dato);
			$datay[] = $dato;
		}
		if ($campo == "dato") {
			$datay[] = $row_grafic[$campo];
		}
	}

	return $datay;

	mysql_close($conexion);
}

?> 