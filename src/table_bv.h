#ifndef FLOW_TABLE_BV_H
#define FLOW_TABLE_BV_H 1

#include <stdio.h>

static ofl_err
flow_table_add(struct flow_table *table, struct ofl_msg_flow_mod *mod, bool check_overlap, bool *match_kept, bool *insts_kept);

ofl_err
flow_table_flow_mod(struct flow_table *table, struct ofl_msg_flow_mod *mod, bool *match_kept, bool *insts_kept);

#endif /* FLOW_TABLE_BV_H */
