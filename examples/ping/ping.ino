#include "AsyncPing.h"
#include <ESP8266WiFi.h>
//ESP8266WiFiMulti WiFiMulti;
extern "C" void esp_yield();
AsyncPing ping0;
AsyncPing ping1;
//#include <lwip/timers.h>
void setup() {
    Serial.begin(115200);
    delay(10);
    WiFi.disconnect(true);
    WiFi.mode(WIFI_STA);
    // We start by connecting to a WiFi network
    //WiFiMulti.addAP("champion", "rpk.rv.ua");
    WiFi.begin("champion", "rpk.rv.ua");
    
    Serial.println();
    Serial.println();
    Serial.print("Wait for WiFi... ");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    delay(500);
      pinMode(LED_BUILTIN, OUTPUT); 
      Serial.println(WiFi.gatewayIP());
IPAddress ad;
ad.fromString(("192.168.206.32"));
      ping0.init(ad);
     // esp_yield();
      
}


void loop() {
IPAddress ad;
ad.fromString(("192.168.206.32"));
  ping1.init(ad);
  //sys_check_timeouts();
    const uint16_t port = 80;
    const char * host = "ch.rpk.rv.ua"; // ip or dns
  digitalWrite(LED_BUILTIN, LOW);   // Turn the LED on (Note that LOW is the voltage level
                                    // but actually the LED is on; this is because 
                                    // it is acive low on the ESP-01)
  delay(1000);                      // Wait for a second
  digitalWrite(LED_BUILTIN, HIGH);  // Turn the LED off by making the voltage HIGH
  delay(2000);                      // Wait for two seconds (to demonstrate the active low LED)
  Serial.println(ESP.getFreeHeap());
return;
    
    
    Serial.print("connecting to ");
    Serial.println(host);

    // Use WiFiClient class to create TCP connections
    WiFiClient client;

    if (!client.connect(host, port)) {
        Serial.println("connection failed");
        Serial.println("wait 5 sec...");
        delay(5000);
        return;
    }

    // This will send the request to the server
    client.print("Send this data to server");

    //read back one line from server
    String line = client.readStringUntil('\r');
    client.println(line);

    Serial.println("closing connection");
    client.stop();
    
    Serial.println("wait 5 sec...");
    delay(5000*3);
//    int a=random();
}


//send ICMP_ECHO(8)
//recv ok ICMP_ER(0)
//recv err ICMP_DUR 3     /* destination unreachable */
// ICMP_RD 5      /* redirect */
//  ICMP_TE 11     /* time exceeded */
void setup0() {
  // put your setup code here, to run once:
  /*
  ping.on_sent([](){ ///after over all times
    
  });
  ping.on_recv([](){ ///after response or timeout
    ping.timeout()
    //base
    ping.id()
    ping.ip();
    ping.host()

    //last
    ping.seq(); 
    ping.answer(); // bool
    //https://ru.wikipedia.org/wiki/ICMP
    ping.answertype();// 3,5,11
    ping.answerstatus();// 0-15
    ping.time();
    ping.ttl();

    //total
    ping.transmitted()
    ping.received()
    ping.totaltime()
  });
  ping.ping(host);
  */
}

