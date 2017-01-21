#include "IPAddress.h"
#include <functional>

extern "C" {
  #include <lwip/raw.h>
  #include <netif/etharp.h>
  #include <user_interface.h>
}

class AsyncPing{
public:
  typedef std::function<void(AsyncPing&)> THandlerFunction;
  AsyncPing();
  void on(bool mode, THandlerFunction handler);
  bool init(const IPAddress &addr, u8_t count = 3, u32_t timeout = 1000);
  bool init(const char *host, u8_t count = 3, u32_t timeout = 1000);
  void cancel();
  ~AsyncPing();

  inline bool answer(){ return ping_ack; }
  inline IPAddress addr(){ return ping_target.addr; }
  inline u32_t time(){ return ping_time; }
  inline u32_t seq(){ return ping_seq_num; }
  inline u32_t ttl(){ return ping_ttl; }
  inline u16_t size(){ return ping_size; }
  inline struct eth_addr *mac(){ return addr_mac; }

  inline u16_t total_sent(){ return ping_total_sent; }
  inline u16_t total_recv(){ return ping_total_recv; }
  inline u16_t total_time(){ return ping_total_time; }
private:
  os_timer_t _timer;
  static void _s_timer (void*arg);
  void  timer();

  os_timer_t _timer_recv;
  static void _s_timer_recv (void*arg);

  u32_t ping_timeout;
  struct eth_addr *addr_mac;
  ip_addr_t ping_target;

  u8_t  count_down;
  struct raw_pcb *ping_pcb;
  u16_t ping_id;
  u32_t ping_start;

  u32_t ping_time;
  u16_t ping_ttl;
  u16_t ping_size;
  u16_t ping_seq_num;
  bool  ping_ack;

  u32_t ping_sent;
  u16_t ping_total_sent;
  u16_t ping_total_recv;
  u32_t ping_total_time;

  void  done();
  void  send_packet();
  void  ping_send(struct raw_pcb *raw, ip_addr_t *addr);
  void  ping_prepare_echo( struct icmp_echo_hdr *iecho, u16_t len);
  u8_t  ping_recv (raw_pcb*pcb, pbuf*p, ip_addr*addr);
  static u8_t _s_ping_recv (void*arg, raw_pcb*tpcb, pbuf*pb, ip_addr*addr);
  THandlerFunction _on_recv;
  THandlerFunction _on_sent;
};
