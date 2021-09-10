#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/netdevice.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/ip.h>
#include <linux/udp.h>
#include <linux/types.h>

static struct nf_hook_ops nfhook;

unsigned int nfhookfunc(
  void *priv,
  struct sk_buff *skb,
  const struct nf_hook_state *hookstate
) {
  // ig allocate statically for better performance
  static struct iphdr *ip_header;
  static struct udphdr *udp_header;
  static void *dns_header;
  
  if(!skb) return NF_ACCEPT;
  ip_header = ip_hdr(skb);   if(!ip_header) return NF_ACCEPT;
  udp_header = udp_hdr(skb); if(!udp_header) return NF_ACCEPT;
  dns_header = (void *)udp_header + 8;
  
  if(!(hookstate && hookstate->in && hookstate->in->name && // interface exists
      strcmp(hookstate->in->name, "lo") && // check interface
    ip_header->protocol == IPPROTO_UDP && // check transport protocol
    ip_header->frag_off == 0 && // check fragment ID
    ntohs((uint16_t)udp_header->source) == 53 && // check port
    (ntohl(*(uint32_t*)dns_header) & 0x800F) == 0x8003 // check QR and RCODE
  ) {
    // change RCODE from 3 to 4
    *(uint32_t*)dns_header = htonl((ntohl(*(uint32_t*)dns_header) & ~0xF) | 4);
  }
  
  return NF_ACCEPT;
}

static int __init init_dns_hack(void) {
  nfhook.hook = nfhookfunc;
  nfhook.hooknum = NF_INET_LOCAL_IN;
  nfhook.pf = PF_INET;
  nfhook.priority = NF_IP_PRI_MANGLE; // tbh idk what priority to use
  nf_register_net_hook(&init_net, &nfhook);
  return 0;    
}

static void __exit cleanup_dns_hack(void) {
  nf_unregister_net_hook(&init_net, &nfhook);
}

module_init(init_dns_hack);
module_exit(cleanup_dns_hack);

MODULE_LICENSE("GPL");
