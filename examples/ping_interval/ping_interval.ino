#include <ESP8266WiFi.h>
#include "AsyncPing.h"
#include "Ticker.h"

Ticker timer;

AsyncPing Pings[3];
IPAddress addrs[3];

const char *ips[]={NULL,"google.com","8.8.8.8"};

void setup() {
    Serial.begin(115200);
    delay(10);
    WiFi.disconnect(true);
    WiFi.mode(WIFI_STA);
    // We start by connecting to a WiFi network
    WiFi.begin("SIID", "password");

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
    Serial.println("gateway IP address: ");
    Serial.println(WiFi.gatewayIP());

    for (int i = 0; i < 3 ; i++){
      if (ips[i]){
        if (!WiFi.hostByName(ips[i], addrs[i]))
          addrs[i].fromString(ips[i]);
      }else
        addrs[i] = WiFi.gatewayIP();

      Pings[i].on(true,[](AsyncPing& host){
        if (host.answer())
          Serial.printf("%d bytes from %s: icmp_seq=%d ttl=%d time=%d ms\n",host.size(),host.addr().toString().c_str(),host.seq(),host.ttl(),host.time());
        else
          Serial.printf("no answer yet for %s icmp_seq=%d\n",host.addr().toString().c_str(),host.seq());
      });
      Pings[i].on(false,[](AsyncPing& host){
        Serial.printf("total answer from %s sent %d recevied %d time %d ms\n",host.addr().toString().c_str(),host.total_sent(),host.total_recv(),host.total_time());
        if (host.mac())
          Serial.printf("detected eth address " MACSTR "\n",MAC2STR(host.mac()->addr));
       });
    }
    ping();
    timer.attach(10,ping);
}
void ping(){
  for (int i = 0; i < 3 ; i++){
    Serial.printf("started ping to %s:\n",addrs[i].toString().c_str());
    Pings[i].init(addrs[i]);
  }
}
void loop() {

}