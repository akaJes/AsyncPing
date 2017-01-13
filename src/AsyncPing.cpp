#include "AsyncPing.h"
extern "C"{
//#include <lwip/raw.h>
#include <lwip/icmp.h>
#include <lwip/sys.h>
#include <lwip/inet_chksum.h>
//#include <lwip/netif.h>

#include <ping.h>
#include <user_interface.h>
}
#define PING_DATA_SIZE 32
#include <Esp.h>
#include "hexdump.h"
ICACHE_FLASH_ATTR AsyncPing::AsyncPing(){ ping_id=system_get_time(); ping_pcb=NULL;}
ICACHE_FLASH_ATTR AsyncPing::~AsyncPing(){ done();}

bool ICACHE_FLASH_ATTR AsyncPing::init(const IPAddress &addr)
{
  ip_addr_t ping_target;
  if (!ping_pcb)
  ping_pcb = raw_new(IP_PROTO_ICMP);
  LWIP_ASSERT("ping_pcb != NULL", ping_pcb != NULL);
  ping_seq_num=0;
  raw_recv(ping_pcb, _s_ping_recv, this);
  raw_bind(ping_pcb, IP_ADDR_ANY);
  //ping_pcb->remote_ip.addr = addr;
  ping_target.addr = addr;
//  Serial.println(ping_target.addr);
  ping_sent = system_get_time();
  ping_send(ping_pcb, &ping_target);
//hexDump("raw",ping_pcb,sizeof(struct raw_pcb));

  //sys_timeout(PING_TIMEOUT_MS, ping_timeout, this);
  //sys_timeout(pingmsg->coarse_time, ping_coarse_tmr, pingmsg);
  return true;
}
void AsyncPing::done(){
  raw_remove(ping_pcb);
  }
void ICACHE_FLASH_ATTR
AsyncPing::ping_send(struct raw_pcb *raw, ip_addr_t *addr)
{
  struct pbuf *p = NULL;
  struct icmp_echo_hdr *iecho = NULL;
  size_t ping_size = sizeof(struct icmp_echo_hdr) + PING_DATA_SIZE;

  LWIP_DEBUGF( PING_DEBUG, ("ping: send "));
  ip_addr_debug_print(PING_DEBUG, addr);
  LWIP_DEBUGF( PING_DEBUG, ("\n"));
  LWIP_ASSERT("ping_size <= 0xffff", ping_size <= 0xffff);

  p = pbuf_alloc(PBUF_IP, (u16_t)ping_size, PBUF_RAM);
  if (!p) {
    return;
  }
  if ((p->len == p->tot_len) && (p->next == NULL)) {
    iecho = (struct icmp_echo_hdr *)p->payload;

    ping_prepare_echo(iecho, (u16_t)ping_size);
//    hexDump("packet",iecho,ping_size);
    err_t err= raw_sendto(raw, p, addr);
    Serial.print("sent :");
    Serial.println(IPAddress(addr->addr));
//    Serial.println(err);
    ping_time = sys_now();
  }
  pbuf_free(p);
//  netif_poll(null);
}
void ICACHE_FLASH_ATTR
AsyncPing::ping_prepare_echo( struct icmp_echo_hdr *iecho, u16_t len)
{
  size_t i = 0;
  size_t data_len = len - sizeof(struct icmp_echo_hdr);

  ICMPH_TYPE_SET(iecho, ICMP_ECHO);
  ICMPH_CODE_SET(iecho, 0);
  iecho->chksum = 0;
  iecho->id     = ping_id;
  ++ ping_seq_num;
  if (ping_seq_num == 0x7fff)
    ping_seq_num = 0;

  iecho->seqno  = htons(ping_seq_num);

  /* fill the additional data buffer with some data */
  for(i = 0; i < data_len; i++) {
    ((char*)iecho)[sizeof(struct icmp_echo_hdr) + i] = (char)i;
  }

  iecho->chksum = inet_chksum(iecho, len);
}
  u8_t  ICACHE_FLASH_ATTR
AsyncPing::ping_recv (raw_pcb*pcb, pbuf*p, ip_addr*addr){
  struct icmp_echo_hdr *iecho = NULL;
  static u16_t seqno = 0;
  LWIP_UNUSED_ARG(pcb);
  LWIP_UNUSED_ARG(addr);
  Serial.print("recv :");
  Serial.print(IPAddress(addr->addr));
  LWIP_ASSERT("p != NULL", p != NULL);
  if (pbuf_header( p, -PBUF_IP_HLEN)==0) {
    iecho = (struct icmp_echo_hdr *)p->payload;

    if ((iecho->id == ping_id) && (iecho->seqno == htons(ping_seq_num)) && iecho->type == ICMP_ER) {
      Serial.println(" - mine");
      LWIP_DEBUGF( PING_DEBUG, ("ping: recv "));
      ip_addr_debug_print(PING_DEBUG, addr);
      LWIP_DEBUGF( PING_DEBUG, (" %"U32_F" ms\n", (sys_now()-ping_time)));

      //PING_RESULT(1);
      pbuf_free(p);
      return 1; /* eat the packet */
    }
  }
  pbuf_header( p, PBUF_IP_HLEN);
  Serial.println();

  return 0; /* don't eat the packet */
}

unsigned char  ICACHE_FLASH_ATTR
AsyncPing::_s_ping_recv (void*arg, raw_pcb*tpcb, pbuf*pb, ip_addr*addr){
  return reinterpret_cast<AsyncPing*>(arg)->ping_recv(tpcb, pb, addr);
}


