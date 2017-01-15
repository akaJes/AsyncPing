## AsyncPing
This is a fully asynchronous Ping library for Espressif's ESP8266 MCUs.

usage
```
AsyncPing ping;
ping.on(true,[](AsyncPing& host){
  if (host.answer())
    Serial.printf("%d bytes from %s: icmp_seq=%d ttl=%d time=%d ms\n",host.size(),host.addr().toString().c_str(),host.seq(),host.ttl(),host.time());
  else
    Serial.printf("no answer yet for %s icmp_seq=%d\n",host.addr().toString().c_str(),host.seq());
});
ping.init(WiFi.gatewayIP());
```