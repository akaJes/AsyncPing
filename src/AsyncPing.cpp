#include <Esp.h>
#include "AsyncPing.h"

extern "C"{
  #include <lwip/icmp.h>
  #include <lwip/sys.h>
  #include <lwip/inet_chksum.h>
}

#define PING_DATA_SIZE 64 - 8

AsyncPing::AsyncPing() {
  ping_id = random(1 << 31);
  ping_pcb = NULL;
  _on_recv = NULL;
  _on_sent = NULL;
}

AsyncPing::~AsyncPing() {
  done();
}

void AsyncPing::on(bool mode, THandlerFunction fn) {
  if(mode)
    _on_recv=fn;
  else
    _on_sent=fn;
}

bool AsyncPing::init(const IPAddress &addr,u8_t count,u32_t timeout) {
  if(!count)
    return false;
  ping_seq_num = 0;
  ping_total_sent = 0;
  ping_total_recv = 0;
  ping_total_time = 0;
  ping_timeout = timeout;
  count_down = count;
  if (!ping_pcb){
    ping_pcb = raw_new(IP_PROTO_ICMP);
    raw_recv(ping_pcb, _s_ping_recv, reinterpret_cast<void*>(this));
    raw_bind(ping_pcb, IP_ADDR_ANY);
  }
  ping_target.addr = addr;
  ping_sent = sys_now(); // micro? system_get_time();
  send_packet();
  return true;
}

void AsyncPing::send_packet(){
  ping_ack=false;
  ping_send(ping_pcb, &ping_target);
  ping_total_sent++;
  count_down--;
  timer_start();
}

void AsyncPing::cancel(){
  count_down=0;
}

void AsyncPing::timer(){
  if(!ping_ack)
    if(_on_recv)
      _on_recv(*this);
  if(count_down){
    send_packet();
  }else{
    ping_total_time=sys_now()-ping_sent; //micro? system_get_time()
    if(_on_sent)
      _on_sent(*this);
  }
}

void AsyncPing::done() {
  timer_stop();
  if (ping_pcb)
    raw_remove(ping_pcb);
}

void AsyncPing::ping_send(struct raw_pcb *raw, ip_addr_t *addr) {
  struct pbuf *p = NULL;
  struct icmp_echo_hdr *iecho = NULL;
  ping_size = sizeof(struct icmp_echo_hdr) + PING_DATA_SIZE;

  p = pbuf_alloc(PBUF_IP, (u16_t)ping_size, PBUF_RAM);
  if (!p) {
    return;
  }
  if ((p->len == p->tot_len) && (p->next == NULL)) {
    iecho = (struct icmp_echo_hdr *)p->payload;

    ping_prepare_echo(iecho, (u16_t)ping_size);
    err_t err= raw_sendto(raw, p, addr);
    ping_start = sys_now();
  }
  pbuf_free(p);
}

void AsyncPing::ping_prepare_echo( struct icmp_echo_hdr *iecho, u16_t len) {
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

u8_t AsyncPing::ping_recv (raw_pcb*pcb, pbuf*p, ip_addr*addr) {
  struct icmp_echo_hdr *iecho = NULL;
  struct ip_hdr *ip = (struct ip_hdr *)p->payload;

  if (pbuf_header( p, -PBUF_IP_HLEN) == 0) {
    iecho = (struct icmp_echo_hdr *)p->payload;
    if ((iecho->id == ping_id) && (iecho->seqno == htons(ping_seq_num)) && iecho->type == ICMP_ER) {
      ping_time = sys_now() - ping_start;
      ping_ttl = ip->_ttl;
      //ping_size = htons(ip->_len);
      ping_ack = true;
      ping_total_recv++;
      if (_on_recv)
        _on_recv(*this);
      pbuf_free(p);
      return 1; /* eat the packet */
    }
  }
  pbuf_header( p, PBUF_IP_HLEN);
  return 0; /* don't eat the packet */
}

void AsyncPing::timer_start(){
  timer_stop();
  os_timer_setfn(&_timer, reinterpret_cast<ETSTimerFunc*>(_s_timer), reinterpret_cast<void*>(this));
  os_timer_arm(&_timer, ping_timeout, 0); // (repeat)?REPEAT:ONCE); //ONCE=0 REPEAT=1
  timer_started = true;
}

void AsyncPing::timer_stop(){
  if (timer_started){
    timer_started = false;
    os_timer_disarm(&_timer);
  }
}

u8_t AsyncPing::_s_ping_recv (void*arg, raw_pcb*tpcb, pbuf*pb, ip_addr*addr){
  return reinterpret_cast<AsyncPing*>(arg)->ping_recv(tpcb, pb, addr);
}

void AsyncPing::_s_timer (void*arg){
  return reinterpret_cast<AsyncPing*>(arg)->timer();
}
