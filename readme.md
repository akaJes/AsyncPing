## AsyncPing
This is a fully asynchronous Ping library for Espressif's ESP8266 MCUs.
have full ping statistic and hardware MAC address

usage
```
AsyncPing ping;
ping.on(true,[](const AsyncPingResponse& response){
  IPAddress addr(response.addr); //to prevent with no const toString() in 2.3.0
  if (response.answer)
    Serial.printf("%d bytes from %s: icmp_seq=%d ttl=%d time=%d ms\n", response.size, addr.toString().c_str(), response.icmp_seq, response.ttl, response.time);
  else
    Serial.printf("no answer yet for %s icmp_seq=%d\n", addr.toString().c_str(), response.icmp_seq);
  return false; //do not stop
});
ping.on(false,[](const AsyncPingResponse& response){
  IPAddress addr(response.addr); //to prevent with no const toString() in 2.3.0
  Serial.printf("total answer from %s sent %d recevied %d time %d ms\n",addr.toString().c_str(),response.total_sent,response.total_recv,response.total_time);
  if (response.mac)
    Serial.printf("detected eth address " MACSTR "\n",MAC2STR(response.mac->addr));
  return true;
});

ping.begin(WiFi.gatewayIP());
```