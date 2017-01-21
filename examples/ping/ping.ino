#include <ESP8266WiFi.h>
#include "AsyncPing.h"

AsyncPing Pings[3];

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

    const char *ips[]={NULL,"google.com","8.8.8.8"};

    for (int i = 0; i < 3 ; i++){
      IPAddress addr;
      if (ips[i]){
        if (!WiFi.hostByName(ips[i], addr))
          addr.fromString(ips[i]);
      }else
        addr = WiFi.gatewayIP();

      Pings[i].on(true,[](const AsyncPingResponse& response){
        IPAddress addr(response.addr); //to prevent with no const toString() in 2.3.0
        if (response.answer)
          Serial.printf("%d bytes from %s: icmp_seq=%d ttl=%d time=%d ms\n", response.size, addr.toString().c_str(), response.icmp_seq, response.ttl, response.time);
        else
          Serial.printf("no answer yet for %s icmp_seq=%d\n", addr.toString().c_str(), response.icmp_seq);
        return false; //do not stop
      });
      Pings[i].on(false,[](const AsyncPingResponse& response){
        IPAddress addr(response.addr); //to prevent with no const toString() in 2.3.0
        Serial.printf("total answer from %s sent %d recevied %d time %d ms\n",addr.toString().c_str(),response.total_sent,response.total_recv,response.total_time);
        if (response.mac)
          Serial.printf("detected eth address " MACSTR "\n",MAC2STR(response.mac->addr));
        return true;
      });
      Serial.printf("started ping to %s:\n",addr.toString().c_str());
      Pings[i].begin(addr);
    }
}
void loop() {

}