#include <lwip/raw.h>
//#include <lwip/icmp.h>
//#include <lwip/sys.h>
//#include <lwip/inet_chksum.h>

//#include <ping.h>
//#include <user_interface.h>
//#define PING_DATA_SIZE 32
class AsyncPing{
  struct raw_pcb *ping_pcb;
  u32_t ping_sent;
  u32_t ping_time;
  u16_t ping_id;
  u16_t ping_seq_num;
  //on_recv
  //on_sent
  bool ICACHE_FLASH_ATTR init(struct ping_option *ping_opt);
  void ICACHE_FLASH_ATTR done();
  void ICACHE_FLASH_ATTR ping_send(struct raw_pcb *raw, ip_addr_t *addr);
  void ICACHE_FLASH_ATTR ping_prepare_echo( struct icmp_echo_hdr *iecho, u16_t len);
  u8_t ICACHE_FLASH_ATTR ping_recv (raw_pcb*pcb, pbuf*p, ip_addr*addr);
  static u8_t ICACHE_FLASH_ATTR _s_ping_recv (void*arg, raw_pcb*tpcb, pbuf*pb, ip_addr*addr);
};


