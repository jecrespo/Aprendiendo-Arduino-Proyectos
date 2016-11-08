/*
 SMS sender: Sends messages in a database

 Circuit:
 * Arduino Leonardo ETH
 * GSM shield
 * SIM card that can send SMS

 */

// Include the GSM library
#include <GSM.h>
//Include Ethernet2
#include <SPI.h>
#include <Ethernet2.h>

#define PINNUMBER ""

// initialize the library instance
GSM gsmAccess;
GSM_SMS sms;

//Network Configuration
// assign a MAC address for the ethernet controller.
byte mac[] = {
  0x90, 0xA2, 0xDA, 0x10, 0x5D, 0x2D
};
// IP address for manual configuration:
IPAddress ip(10, 22, 72, 30);
// Server to collect data
char server[] = "www.aprendiendoarduino.com";

// initialize the library instance:
EthernetClient client;

void setup() {
  // initialize serial communications and wait for port to open:
  Serial.begin(9600);
  delay(5000); // wait for serial port to connect. Needed for native USB port only
  Ethernet.begin(mac, ip);

  Serial.println("SMS Messages Sender");

  // connection state
  boolean notConnected = true;

  // Start GSM shield
  // If your SIM has PIN, pass it as a parameter of begin() in quotes
  while (notConnected) {
    if (gsmAccess.begin(PINNUMBER) == GSM_READY) {
      notConnected = false;
    } else {
      Serial.println("Not connected");
      delay(1000);
    }
  }
  //Serial.println("GSM initialized");
}

void loop() {
  // Every loop check if SMS are queued to send
  if (client.connect(server, 80)) {
    Serial.println("connected");
    // Make a HTTP request:
    client.println("GET /sendSMS/getsms.php HTTP/1.1");
    client.println("Host: server_name");
    client.println("Connection: close");
    client.println();

    // wait until there are incoming bytes available
    // from the server, read them and print them:

    int wait_time = 0;  //wait time in tenths of second

    while ((!client.available()) && (wait_time < 70)) { //Wait until 7 seconds
      delay(100);
      wait_time++;
    }

    Serial.print("Response time (seg): ");
    Serial.println(wait_time / 10);

    if (client.available()) {
      String response = "";
      while (client.available()) {
        response += (char)client.read();
      }

      Serial.print("Response: ");
      Serial.println(response);

      // if the server's disconnected, stop the client:
      if (!client.connected()) {
        Serial.println();
        Serial.println("disconnecting.");
        client.stop();
      }

      //Process data to send SMS
      if (response.indexOf("No SMS") > 0) {
        Serial.println("Nothing to send.");  //Nothing to do
      }
      else {  //Send SMS. response format: |message|receiver1|receiver2|receiver3|receiver4|...
        int params = 0;  //params is number of parameters or mumber of | in response
        int index[] = {0, 0, 0, 0, 0, 0, 0}; //Maximum 1 message and 5 receivers
        index[0] = response.indexOf('|');
        Serial.println(index[0]);
        
        do {
          params++;
          index[params] = response.indexOf('|', index[params - 1] + 1);
          Serial.println(index[params]);
        } while ((index[params] > 0) && params < 7);  //params -1 es el número de parámetors

        if (params >= 3) { //Params must be >= 3 else response error
          char txtMsg[200];
          response.substring(index[0]+1, index[1]).toCharArray(txtMsg, 200);  // telephone number to send sms
          Serial.print("Message: ");
          Serial.println(txtMsg);
          for (int i = 1; i < params - 1 ; i++) { //Number of receivers params-2
            char remoteNum[20];
            response.substring(index[i]+1, index[i + 1]).toCharArray(remoteNum, 20);
            Serial.print("SENDING to: ");
            Serial.println(remoteNum);
            // send the message
            sms.beginSMS(remoteNum);
            sms.print(txtMsg);
            sms.endSMS();
            Serial.println("COMPLETE!");
            delay(1000);
          }
        }
        else {
          Serial.println("response error, no parmas received");
        }
      }
    }
    else {
      Serial.println();
      Serial.println("No server response in time...");
      client.stop();
    }
  }
  else {
    // If you didn't get a connection to the server:
    Serial.println("connection failed");
  }
}
