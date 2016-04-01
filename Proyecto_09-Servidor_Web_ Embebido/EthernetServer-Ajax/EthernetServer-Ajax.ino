#include <SPI.h>
#include <Ethernet.h>

// network configuration.  gateway and subnet are optional.

// the media access control (ethernet hardware) address for the shield:
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
//the IP address for the shield:
byte ip[] = { 192, 168, 1, 152 };
// the router's gateway address:
byte gateway[] = { 192, 168, 1, 1 };
// the subnet:
byte subnet[] = { 255, 255, 255, 0 };

String ip_String = "192.168.1.152";

EthernetServer server = EthernetServer(80);

void setup()
{
  Serial.begin(9600);
  // initialize the ethernet device
  Ethernet.begin(mac, ip, gateway, subnet);
  // start listening for clients
  server.begin();
}

void loop()
{
  EthernetClient client = server.available();
  if (client > 0) {
    String peticion_web = "";
    boolean muestra_web = true;
    while (server.available() > 0) {
      char inChar = client.read();
      peticion_web += inChar;
      Serial.print(inChar);
      if (peticion_web == "GET /ajax") {
        Serial.println();
        client.println(analogRead(A0));
        client.stop();
        muestra_web = false;
        break;
      }
    }
    if (muestra_web) {
      client.println(F("HTTP/1.1 200 OK"));
      client.println(F("Content-Type: text/html"));
      client.println();
      client.println(F("<!DOCTYPE html>"));
      client.println(F("<html>"));
      client.println(F("<head><title>EJEMPLO AJAX</title>"));
      client.println(F("<script type=\"text/javascript\">"));
      client.println(F("var READY_STATE_UNINITIALIZED = 0;"));
      client.println(F("var READY_STATE_LOADING = 1;"));
      client.println(F("var READY_STATE_LOADED = 2;"));
      client.println(F("var READY_STATE_INTERACTIVE = 3;"));
      client.println(F("var READY_STATE_COMPLETE = 4;"));
      client.println(F("var peticion_http;"));
      client.println(F("var peticion_http_events;"));
      client.println(F("function load(url, metodo, funcion) {"));
      client.println(F("peticion_http = inicializa_xhr();"));
      client.println(F("if(peticion_http) {"));
      client.println(F("peticion_http.onreadystatechange = funcion;"));
      client.println(F("peticion_http.open(metodo, url, true);"));
      client.println(F("peticion_http.send(null);"));
      client.println(F("}"));
      client.println(F("}"));
      client.println(F("function inicializa_xhr() {"));
      client.println(F("if(window.XMLHttpRequest) {"));
      client.println(F("return new XMLHttpRequest();"));
      client.println(F("}"));
      client.println(F("else if(window.ActiveXObject) {"));
      client.println(F("return new ActiveXObject(\"Microsoft.XMLHTTP\");"));
      client.println(F("}"));
      client.println(F("}"));
      client.println(F("function show_content() {"));
      client.println(F("if(peticion_http.readyState == READY_STATE_COMPLETE) {"));
      client.println(F("if(peticion_http.status == 200) {"));
      client.println(F("var respuesta = peticion_http.responseText;"));
      client.println(F("document.getElementById(\"valor\").innerHTML=respuesta"));
      client.println(F("var texto_historico = respuesta+\"<br>\"+document.getElementById(\"historico\").innerHTML;"));
      client.println(F("document.getElementById(\"historico\").innerHTML=texto_historico;"));
      client.println(F("}"));
      client.println(F("}"));
      client.println(F("}"));
      client.println(F("function update() {"));
      client.print(F("load(\"http://"));
      client.print(ip_String);
      client.println(F("/ajax\", \"GET\", show_content);"));
      client.println(F("}"));
      client.println(F("setInterval('update()',1000);"));
      client.println(F("</script>"));
      client.println(F("</head>"));
      client.println(F("<style>div {border: 1px solid black; border-collapse: collapse; color: #006666; font-weight:bold; width: 100px; text-align:center; }</style>"));
      client.println(F("<body bgcolor=\"D7DBDA\">"));
      client.print("He leido por el puerto analogico: ");
      client.println(F("<div id=\"valor\">"));
      client.println(F("</div>"));
      client.print("Historico de Valores Leidos: ");
      client.println(F("<div id=\"historico\">"));
      client.println(F("</div>"));
      client.println(F("</body>"));
      client.println(F("</html>"));
      client.println();
      client.stop();
    }
  }
}
