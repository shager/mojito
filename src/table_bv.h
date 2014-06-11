#ifndef FLOW_TABLE_BV_H
#define FLOW_TABLE_BV_H 1

#include "bv_types.h"

struct flow_table {
    struct datapath*           dp;
    struct ofl_table_features* features;      /*store table features*/
    struct ofl_table_stats*    stats;         /* structure storing table statistics. */
    
    struct list                match_entries;  /* list of entries in order. */
    struct list                hard_entries;   /* list of entries with hard timeout;
                                                ordered by their timeout times. */
    struct list                idle_entries;   /* unordered list of entries with
                                                idle timeout. */
    Range_borders*             range_borders_ip_src; //IP address source
    Range_borders*             range_borders_ip_dst; //IP address dst
    Range_borders*             range_borders_transport_src; //either TCP or UDP 
    Range_borders*             range_borders_transport_dst; //either TCP or UDP
    Range_borders*             range_borders_transport_type; //determines if TCP or UDP is used
    Range_borders*             range_borders_eth_src; //MAC src address
    Range_borders*             range_borders_eth_dst; //MAC dst address
    Range_borders*             range_borders_eth_type; //Ethernet protocol type
    Range_borders*             range_borders_ip_number; //IP version
    Range_borders*             range_borders_port_number; //physical/virtual ingress port number
};

static ofl_err
flow_table_add(struct flow_table *table, struct ofl_msg_flow_mod *mod, bool check_overlap, bool *match_kept, bool *insts_kept);

ofl_err
flow_table_flow_mod(struct flow_table *table, struct ofl_msg_flow_mod *mod, bool *match_kept, bool *insts_kept);

struct flow_table*
flow_table_create(struct datapath *dp, uint8_t table_id);

#endif /* FLOW_TABLE_BV_H */
