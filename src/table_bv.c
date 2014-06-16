#include "table_bv.h"
#include "oflib/ofl.h"
#include "flow_table.h"
#include "oflib/ofl-messages.h"
#include "bv_types.h"

/* Handle OpenFlow ADD message */
static ofl_err
flow_table_add(struct flow_table *table, struct ofl_msg_flow_mod *mod, bool check_overlap, bool *match_kept, bool *insts_kept) {
    struct flow_entry* entry;
    table->range_borders->insert_element(table->range_borders, entry->match_node);

    // NOTE: old from here
    // Note: new entries will be placed behind those with equal priority
    struct flow_entry *entry, *new_entry;

    LIST_FOR_EACH (entry, struct flow_entry, match_node, &table->match_entries) {
        if (check_overlap && flow_entry_overlaps(entry, mod)) {
            return ofl_error(OFPET_FLOW_MOD_FAILED, OFPFMFC_OVERLAP);
        }

        /* if the entry equals, replace the old one */
        if (flow_entry_matches(entry, mod, true/*strict*/, false/*check_cookie*/)) {
            new_entry = flow_entry_create(table->dp, table, mod);
            *match_kept = true;
            *insts_kept = true;

            /* NOTE: no flow removed message should be generated according to spec. */
            list_replace(&new_entry->match_node, &entry->match_node);
            list_remove(&entry->hard_node);
            list_remove(&entry->idle_node);
            flow_entry_destroy(entry);
            add_to_timeout_lists(table, new_entry);
            return 0;
        }

        if (mod->priority > entry->stats->priority) {
            break;
        }
    }

    if (table->stats->active_count == FLOW_TABLE_MAX_ENTRIES) {
        return ofl_error(OFPET_FLOW_MOD_FAILED, OFPFMFC_TABLE_FULL);
    }
    table->stats->active_count++;

    new_entry = flow_entry_create(table->dp, table, mod);
    *match_kept = true;
    *insts_kept = true;

    list_insert(&entry->match_node, &new_entry->match_node);
    add_to_timeout_lists(table, new_entry);

    return 0;
}

ofl_err
flow_table_flow_mod(struct flow_table *table, struct ofl_msg_flow_mod *mod, bool *match_kept, bool *insts_kept) {

}

struct flow_table *
flow_table_create(struct datapath *dp, uint8_t table_id) {
    struct flow_table *table;
    struct ds string = DS_EMPTY_INITIALIZER;

    ds_put_format(&string, "table_%u", table_id);

    table = xmalloc(sizeof(struct flow_table));
    table->dp = dp; 
    
    /*Init table stats */
    table->stats = xmalloc(sizeof(struct ofl_table_stats));
    table->stats->table_id      = table_id;
    table->stats->active_count  = 0;
    table->stats->lookup_count  = 0;
    table->stats->matched_count = 0;

    /* Init Table features */
    table->features = xmalloc(sizeof(struct ofl_table_features));
    table->features->table_id = table_id;
    table->features->name          = ds_cstr(&string);
    table->features->metadata_match = 0xffffffffffffffff; 
    table->features->metadata_write = 0xffffffffffffffff;
    table->features->config        = OFPTC_TABLE_MISS_CONTROLLER;
    table->features->max_entries   = FLOW_TABLE_MAX_ENTRIES;
    table->features->properties_num = flow_table_features(table->features);

    list_init(&table->match_entries);
    list_init(&table->hard_entries);
    list_init(&table->idle_entries);

    table->range_borders_ip_src = range_borders_ctor();
    table->range_borders_ip_dst = range_borders_ctor();
    table->range_borders_eth_src = range_borders_ctor();
    table->range_borders_eth_dst = range_borders_ctor();
    table->range_borders_transport_src = range_borders_ctor();
    table->range_borders_transport_dst = range_borders_ctor();
    table->range_borders_transport_type = range_borders_ctor();
    table->range_borders_port_number = range_borders_ctor();
    table->range_borders_eth_type = range_borders_ctor();
    table->range_borders_ip_number = range_borders_ctor();

    return table;
}
