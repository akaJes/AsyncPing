extern "C" {
#include <lwip/raw.h>
}
#include "IPAddress.h"

//#define PING_DATA_SIZE 32
class AsyncPing{
  struct raw_pcb *ping_pcb;
  u32_t ping_sent;
  u32_t ping_time;
  u16_t ping_id;
  u16_t ping_seq_num;
  //on_recv
  //on_sent
  void ICACHE_FLASH_ATTR done();
  void ICACHE_FLASH_ATTR ping_send(struct raw_pcb *raw, ip_addr_t *addr);
  void ICACHE_FLASH_ATTR ping_prepare_echo( struct icmp_echo_hdr *iecho, u16_t len);
  u8_t ICACHE_FLASH_ATTR ping_recv (raw_pcb*pcb, pbuf*p, ip_addr*addr);
  static u8_t ICACHE_FLASH_ATTR _s_ping_recv (void*arg, raw_pcb*tpcb, pbuf*pb, ip_addr*addr);
public:
  AsyncPing();
  ~AsyncPing();
  bool ICACHE_FLASH_ATTR init(const IPAddress &addr);
};


