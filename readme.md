## AsyncPing
This is a fully asynchronous Ping library for Espressif's ESP8266 MCUs.
have full ping statistic and hardware MAC address

usage
```
AsyncPing ping;
ping.on(true,[](AsyncPing& host){
  if (host.answer()) {
    Serial.printf("%d bytes from %s: icmp_seq=%d ttl=%d time=%d ms\n",host.size(),host.addr().toString().c_str(),host.seq(),host.ttl(),host.time());
    if (host.mac())
      Serial.printf("detected eth address " MACSTR "\n",MAC2STR(host.mac()->addr));
   } else
    Serial.printf("no answer yet for %s icmp_seq=%d\n",host.addr().toString().c_str(),host.seq());
});
ping.init(WiFi.gatewayIP());
```