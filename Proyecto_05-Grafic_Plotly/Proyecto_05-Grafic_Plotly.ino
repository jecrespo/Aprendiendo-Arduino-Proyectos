#include <SPI.h>
#include <Ethernet.h>
#include "plotly_streaming_ethernet.h"

#define tmp36sensor_pin A0
#define tmp36sensor_pin2 A1

// Sign up to plotly here: https://plot.ly
// View your API key and streamtokens here: https://plot.ly/settings
#define nTraces 2
// View your tokens here: https://plot.ly/settings
// Supply as many tokens as data traces
// e.g. if you want to ploty A0 and A1 vs time, supply two tokens
char *tokens[nTraces] = {"04xink8yr3","v40mn2t5xz"};
// arguments: username, api key, streaming token, filename
plotly graph("aprendiendoarduino", "m8be1nghkc", tokens, "Proyecto_5-Grafic_Plotly", nTraces);

byte mac[] = {
  0x90, 0xA2, 0xDA, 0x0F, 0xE6, 0x29
};
byte ip[] = {
  192, 168, 1, 152
};
byte DNS[] = {
  8, 8, 8, 8
};
byte gateway[] = {
  192, 168, 1, 1
};
byte subnet[] = {
  255, 255, 255, 0
};

void startEthernet(){
    Serial.println("... Initializing ethernet");
	
    if(Ethernet.begin(mac) == 0){
        Serial.println("... Failed to configure Ethernet using DHCP");
        // no point in carrying on, so do nothing forevermore:
        // try to congifure using IP address instead of DHCP:
        Ethernet.begin(mac, ip, DNS, gateway, subnet);
    }
    Serial.println("... Done initializing ethernet");
    delay(1000);
}


void setup() {
  graph.maxpoints = 500;
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }

  startEthernet();

  bool success;
  success = graph.init();
  if(!success){while(true){}}
  graph.openStream();
}

void loop() {
  //getting the voltage reading from the temperature sensor
  int reading = analogRead(tmp36sensor_pin);
  int reading2 = analogRead(tmp36sensor_pin2);
  // converting that reading to voltage, for 3.3v arduino use 3.3
  float voltage = reading * 5.0;
  voltage /= 1024.0;
  float voltage2 = reading2 * 5.0;
  voltage2 /= 1024.0;
  //converting from 10 mv per degree wit 500 mV offset
  //to degrees ((voltage - 500mV) times 100)
  float temperatureC = (voltage - 0.5) * 100;
  float temperatureC2 = (voltage2 - 0.5) * 100;
  // now convert to Fahrenheit
  float temperatureF = (temperatureC * 9.0 / 5.0) + 32.0;
  graph.plot(millis(), voltage, tokens[0]);
  graph.plot(millis(), voltage2, tokens[1]);
  delay(500);
}
