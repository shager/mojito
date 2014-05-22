#include "flow_entry_bv.h"

struct flow_entry_bv *
flow_entry_bv_create(struct datapath *dp, struct flow_table_bv *table, struct ofl_msg_flow_mod *mod) {
    struct flow_entry_bv *entry;
    uint64_t now = time_msec();

    entry = xmalloc(sizeof(struct flow_entry_bv));
    entry->dp    = dp;
    entry->table = table;
 
    entry->stats = xmalloc(sizeof(struct ofl_flow_stats));
 
    entry->stats->table_id         = mod->table_id;
    entry->stats->duration_sec     = 0;
    entry->stats->duration_nsec    = 0;
    entry->stats->priority         = mod->priority;
    entry->stats->idle_timeout     = mod->idle_timeout;
    entry->stats->hard_timeout     = mod->hard_timeout;
    entry->stats->cookie           = mod->cookie;
    entry->no_pkt_count = ((mod->flags & OFPFF_NO_PKT_COUNTS) != 0 );
    entry->no_byt_count = ((mod->flags & OFPFF_NO_BYT_COUNTS) != 0 );
    if (entry->no_pkt_count)
        entry->stats->packet_count     = 0xffffffffffffffff;
    else
        entry->stats->packet_count     = 0;
    if (entry->no_byt_count)
        entry->stats->byte_count       = 0xffffffffffffffff;
    else
        entry->stats->byte_count       = 0;

    entry->stats->match            = mod->match;
    entry->stats->instructions_num = mod->instructions_num;
    entry->stats->instructions     = mod->instructions;

    entry->match = mod->match; /* TODO: MOD MATCH? */

    entry->created      = now;
    entry->remove_at    = mod->hard_timeout == 0 ? 0
                                  : now + mod->hard_timeout * 1000;
    entry->last_used    = now;
    entry->send_removed = ((mod->flags & OFPFF_SEND_FLOW_REM) != 0);
    list_init(&entry->match_node);
    list_init(&entry->idle_node);
    list_init(&entry->hard_node);

    list_init(&entry->group_refs);
    init_group_refs(entry);

    list_init(&entry->meter_refs);
    init_meter_refs(entry);

    return entry;
}
