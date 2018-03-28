#include "IPAddress.h"
#include <functional>
#include "core_version.h"

extern "C" {
  #include <lwip/raw.h>
  #include <netif/etharp.h>
  #include <user_interface.h>
}

class AsyncPingResponse{
  public:
    bool  answer;
    u16_t size;
    u16_t icmp_seq;
    u16_t ttl;
    u32_t time;
    struct eth_addr *mac;
    u16_t total_sent;
    u16_t total_recv;
    u32_t total_time;
    u32_t timeout;
    IPAddress addr;
};
#ifdef ARDUINO_ESP8266_RELEASE_2_3_0
  #define C_IP_ADDR
#else
  #define C_IP_ADDR const
#endif
class AsyncPing{
public:
  typedef std::function< bool (const AsyncPingResponse& ) > THandlerFunction;
  AsyncPing();
  void on(bool mode, THandlerFunction handler);
  bool begin(const IPAddress &addr, u8_t count = 3, u32_t timeout = 1000);
  bool begin(const char *host, u8_t count = 3, u32_t timeout = 1000);
  void cancel();
  ~AsyncPing();
  const AsyncPingResponse& response() { return _response; }

private:
  AsyncPingResponse _response;

  os_timer_t _timer;
  static void _s_timer (void*arg);
  void  timer();

  os_timer_t _timer_recv;
  static void _s_timer_recv (void*arg);

  ip_addr_t ping_target;
  u8_t  count_down;
  struct raw_pcb *ping_pcb;
  u16_t ping_id;
  u32_t ping_start;
  u32_t ping_sent;

  void  done();
  void  send_packet();
  void  ping_send(struct raw_pcb *raw, ip_addr_t *addr);
  void  ping_prepare_echo( struct icmp_echo_hdr *iecho, u16_t len);
  u8_t  ping_recv (raw_pcb*pcb, pbuf*p, C_IP_ADDR ip_addr_t *addr);
  static u8_t _s_ping_recv (void*arg, raw_pcb*tpcb, pbuf*pb, C_IP_ADDR ip_addr_t *addr);
  THandlerFunction _on_recv;
  THandlerFunction _on_sent;
};
