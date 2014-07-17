/* Copyright (c) 2008 The Board of Trustees of The Leland Stanford
 * Junior University
 * 
 * We are making the OpenFlow specification and associated documentation
 * (Software) available for public use and benefit with the expectation
 * that others will use, modify and enhance the Software and contribute
 * those enhancements back to the community. However, since we would
 * like to make the Software available for broadest use, with as few
 * restrictions as possible permission is hereby granted, free of
 * charge, to any person obtaining a copy of this Software to deal in
 * the Software under the copyrights without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 * 
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT.  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 * 
 * The name and trademarks of copyright holder(s) may NOT be used in
 * advertising or publicity pertaining to the Software or any
 * derivatives without specific, written prior permission.
 */

#include <config.h>
#include "table.h"
#include <stdlib.h>
#include "flow.h"
#include "list.h"
#include "openflow/openflow.h"
#include "openflow/nicira-ext.h"
#include "switch-flow.h"
#include "datapath.h"
#include "bv_types.h"

struct sw_table_jit {
    struct sw_table swt;

    unsigned int max_flows;
    unsigned int n_flows;
    
    Range_borders* range_borders_ip_src; //IP address source
    Range_borders* range_borders_ip_dst; //IP address dst
    Range_borders* range_borders_port_number; //physical/virtual ingress port number
    Range_borders* range_borders_vlan_id; //VLAN ID
    Range_borders* range_borders_eth_type; //Ethernet frame type
    Range_borders* range_borders_transport_src; //either TCP or UDP src port
    Range_borders* range_borders_transport_dst; //either TCP or UDP dst port
    Range_borders* range_borders_eth_src; //MAC src address
    Range_borders* range_borders_eth_dst; //MAC dst address
    Range_borders* range_borders_vlan_prio; //Input VLAN priority
    Range_borders* range_borders_ip_dcsp; //IPv4 DCSP
    Range_borders* range_borders_ip_protocol; //IP protocol (e.g. UDP, TCP, ICMP)

    //For reference: definition of flow in flow.h
    /*
     * uint32_t nw_src;            
     *  uint32_t nw_dst;            
     * uint16_t in_port;           
     * uint16_t dl_vlan;           
     * uint16_t dl_type;           
     * uint16_t tp_src;            
     * uint16_t tp_dst;            
     * uint8_t dl_src[6];          
     * uint8_t dl_dst[6];          
     * uint8_t dl_vlan_pcp;        
     * uint8_t nw_tos;
     * uint8_t nw_proto;
     */

};

static struct sw_flow* table_jit_lookup(struct sw_table* swt,
                                           const struct sw_flow_key* key)
{
    struct sw_table_jit *tj = (struct sw_table_jit *) swt;
    
    Bitvector* bv_array[12];
    bv_array[0] = lookup_dimension(range_borders_ip_src, key->flow.nw_src);
    bv_array[1] = lookup_dimension(range_borders_ip_dst, key->flow.nw_dst);
    bv_array[2] = lookup_dimension(range_borders_port_number, key->flow.in_port);
    bv_array[3] = lookup_dimension(range_borders_vlan_id, key->flow.dl_vlan);
    bv_array[4] = lookup_dimension(range_borders_eth_type, key->flow.dl_type);
    bv_array[5] = lookup_dimension(range_borders_transport_src, key->flow.tp_src);
    bv_array[6] = lookup_dimension(range_borders_transport_dst, key->flow.tp_dst);
    bv_array[7] = lookup_dimension(range_borders_eth_src, key->flow.dl_src);
    bv_array[8] = lookup_dimension(range_borders_eth_dst, key->flow.dl_dst);
    bv_array[9] = lookup_dimension(range_borders_vlan_prio, key->flow.dl_vlan_pcp);
    bv_array[10] = lookup_dimension(range_borders_ip_dcsp, key->flow.nw_tos);
    bv_array[11] = lookup_dimension(range_borders_ip_protocol, key->flow.nw_proto);
    
    uint64_t rule_index_result = bv_array[0]->bitvector[0];
    uint32_t bitvector_subindex = 0;
    //connect all Bitvectors with AND and get result
    for (uint32_t i = 0; i < (bv_array[0]->bitvector_length) / sizeof(uint64_t); ++i) {
        for (j = 1; j < 12; ++j) {
            rule_index_result &= bv_array[j]->bitvector[i];
        }
        if (rule_index_result != 0) {
            bitvector_subindex = i;
            break;
        }
        rule_index_result = bv_array[0]->bitvector[i];
    }
    
    //no match found -> return NULL flow
    if (rule_index_result == 0)
        return NULL;
    
    //find leftmost bit in rule_index_result
    uint32_t leftmostbit = find_leftmost_bit(rule_index_result);
    
    //ERROR! no match found
    if (leftmostbit == 255) {
        return NULL;
    }
    
    //re-calculate offset in original bitvector-array
    leftmostbit += bitvector_subindex * 64;
    
    return tj->flow_array[leftmostbit];
}

uint8_t find_leftmost_bit (uint64_t value) {
    for (uint32_t i = 0; i < sizeof(uint64_t); ++i) {
        if (value & 0x8000000000000000 == 0x8000000000000000) {
            return i;
        }
        value <<= 1;
    }
    
    return 255;
}

Bitvector* lookup_dimension(struct Range_borders* rb_dimension, uint64_t value)
{
    Bitvector* bv = bitvector_ctor();
    uint8_t result = rb_dimension->match_packet(rb_dimension, &bv, value);
    if (result == 0) {
        return bv;
    }
    return NULL;
}

static int table_jit_insert(struct sw_table *swt, struct sw_flow *flow)
{
    struct sw_table_jit *tj = (struct sw_table_jit *) swt;
    uint32_t rule_index = n_flows;
    
    //check if flow already exists, if yes overwrite
    for (uint32_t i = 0; i < tj->stats.n_flows; ++i)
    {
        sw_flow f = *(flow_array[i]);
        if (f == NULL)
            continue;
        if (f->priority == flow->priority
                && f->key.wildcards == flow->key.wildcards
                && flow_matches_2wild(&f->key, &flow->key)) {
            flow->serial = f->serial;
            list_replace(&flow->node, &f->node);
            list_replace(&flow->iter_node, &f->iter_node);
            flow_free(f);
            return 1;
        }
        
        //TODO: needed?
        if (f->priority < flow->priority) {
            rule_index = i;
            break;
        }
    }
    
    //create new flow    
    /* Make sure there's room in the table. */
    if (tj->n_flows >= tj->max_flows) {
        return 0;
    }

    //create free position in our sw_flow* array
    for (i = n_flows + 1; i > rule_index; --i)
    {
        tj->flow_array[i] = tj->flow_array[i - 1];
    }
    
    tj->n_flows++;
    tj->flow_array[rule_index] = flow;
    
    //short handle for flow struct in sw_flow
    struct flow network_flow = flow->key.flow;
    
    /* Get MAC addresses in one int */
    uint64_t mac_src = 0;
    uint64_t mac_dst = 0;
    for (uint32_t i = 0; i < 6; i++)
    {
        mac_src += (((uint64_t)(network_flow.dl_src[i])) << (i * 8));
        mac_dst += (((uint64_t)(network_flow.dl_dst[i])) << (i * 8));
    }
    
    /* Get IP address start and end points */
    uint64_t ip_src_start = network_flow.nw_src & flow->nw_src_mask;
    uint64_t ip_dst_start = network_flow.nw_dst & flow->nw_dst_mask;
    uint64_t ip_src_end = (network_flow.nw_src & flow->nw_src_mask) + (~flow->nw_src_mask);
    uint64_t ip_dst_end = (network_flow.nw_dst & flow->nw_dst_mask) + (~flow->nw_dst_mask);
    
    // Add data to range_border structs
    range_borders_ip_src->add_rule(range_borders_ip_src, ip_src_start, ip_src_end + 1, rule_index);
    range_borders_ip_dst->add_rule(range_borders_ip_dst, ip_dst_start, ip_dst_end + 1, rule_index);
    range_borders_port_number->add_rule(range_borders_port_number, network_flow.in_port, network_flow.in_port + 1, rule_index);
    range_borders_vlan_id->add_rule(range_borders_vlan_id, network_flow.dl_vlan, network_flow.dl_vlan + 1, rule_index);
    range_borders_eth_type->add_rule(range_borders_eth_type, network_flow.dl_type, network_flow.dl_type + 1, rule_index);
    range_borders_transport_src->add_rule(range_borders_transport_src, network_flow.tp_src, network_flow.tp_src + 1, rule_index);
    range_borders_transport_dst->add_rule(range_borders_transport_dst, network_flow.tp_dst, network_flow.tp_dst + 1, rule_index);
    range_borders_eth_src->add_rule(range_borders_eth_src, mac_src, mac_src + 1, rule_index);
    range_borders_eth_dst->add_rule(range_borders_eth_dst, mac_dst, mac_dst + 1, rule_index);
    range_borders_vlan_prio->add_rule(range_borders_vlan_prio, network_flow.dl_vlan_pcp, network_flow.dl_vlan_pcp + 1, rule_index);
    range_borders_ip_dcsp->add_rule(range_borders_ip_dcsp, network_flow.nw_tos, network_flow.nw_tos + 1, rule_index);
    range_borders_ip_protocol->add_rule(range_borders_ip_protocol, network_flow.nw_proto, network_flow.nw_proto + 1, rule_index);
    
    //SUCCESS
    return 1;
}

static int table_jit_modify(struct sw_table *swt,
                const struct sw_flow_key *key, uint16_t priority, int strict,
                const struct ofp_action_header *actions, size_t actions_len)
{
    struct sw_table_jit *tj = (struct sw_table_jit *) swt;
    struct sw_flow *flow;
    unsigned int count = 0;

    for (uint32_t i = 0; i < tj->n_flows; ++i)
    {
        if (flow_matches_desc(&flow->key, key, strict)
                && (!strict || (flow->priority == priority))) {
            flow_replace_acts(flow, actions, actions_len);
            count++;
        }
    }
    
    return count;
}

//TODO: Impl
static int table_jit_has_conflict(struct sw_table *swt,
                                     const struct sw_flow_key *key,
                                     uint16_t priority, int strict)
{
    struct sw_table_linear *tl = (struct sw_table_linear *) swt;
    struct sw_flow *flow;

    LIST_FOR_EACH (flow, struct sw_flow, node, &tl->flows) {
        if (flow_matches_2desc(&flow->key, key, strict)
                && (flow->priority == priority)) {
            return true;
        }
    }
    return false;
}

static int table_jit_delete(struct datapath *dp, struct sw_table *swt,
                               const struct sw_flow_key *key, 
                               uint16_t out_port, 
                               uint16_t priority, int strict)
{
    struct sw_table_jit *tj = (struct sw_table_jit *) swt;
    struct sw_flow *flow, *n;
    uint32_t count = 0;
    
    for (uint32_t i = 0; i < tj->n_flows; ++i) {
        if (flow_matches_desc(&flow->key, key, strict)
                && flow_has_out_port(flow, out_port)
                && (!strict || (flow->priority == priority))) {
            dp_send_flow_end(dp, flow, OFPRR_DELETE);
            
            // delete flow* from flow_array
            for (uint32_t j = i; j < tj->n_flows; ++j) {
                flow_array[j] = flow_array[j + 1];
            }
            
            do_delete(flow);
            
            range_borders_ip_src->delete_element(range_borders_ip_src, i);
            range_borders_ip_dst->delete_element(range_borders_ip_dst, i);
            range_borders_port_number->delete_element(range_borders_port_number, i);
            range_borders_vlan_id->delete_element(range_borders_vlan_id, i);
            range_borders_eth_type->delete_element(range_borders_eth_type, i);
            range_borders_transport_src->delete_element(range_borders_transport_src, i);
            range_borders_transport_dst->delete_element(range_borders_transport_dst, i);
            range_borders_eth_src->delete_element(range_borders_eth_src, i);
            range_borders_eth_dst->delete_element(range_borders_eth_dst, i);
            range_borders_vlan_prio->delete_element(range_borders_vlan_prio, i);
            range_borders_ip_dcsp->delete_element(range_borders_ip_dcsp, i);
            range_borders_ip_protocol->delete_element(range_borders_ip_protocol, i);
            
            count++;
        }
    }
    
    tl->n_flows -= count;
    return count;
}

static void
do_delete(struct sw_flow *flow) 
{
    list_remove(&flow->node);
    list_remove(&flow->iter_node);
    flow_free(flow);
}

//TODO: Impl
static void table_jit_timeout(struct sw_table *swt, struct list *deleted)
{
    struct sw_table_linear *tl = (struct sw_table_linear *) swt;
    struct sw_flow *flow, *n;

    LIST_FOR_EACH_SAFE (flow, n, struct sw_flow, node, &tl->flows) {
        if (flow_timeout(flow)) {
            list_remove(&flow->node);
            list_remove(&flow->iter_node);
            list_push_back(deleted, &flow->node);
            tl->n_flows--;
        }
    }
}

static void table_jit_destroy(struct sw_table *swt)
{
    range_borders_dtor(range_borders_ip_src);
    range_borders_dtor(range_borders_ip_dst);
    range_borders_dtor(range_borders_ip_number);
    range_borders_dtor(range_borders_eth_src);
    range_borders_dtor(range_borders_eth_dst);
    range_borders_dtor(range_borders_eth_type);
    range_borders_dtor(range_borders_port_number);
    range_borders_dtor(range_borders_transport_type);
    range_borders_dtor(range_borders_transport_src);
    range_borders_dtor(range_borders_transport_dst);
    
    free(swt);
}

static void table_jit_stats(struct sw_table *swt,
                               struct sw_table_stats *stats)
{
    struct sw_table_linear *tl = (struct sw_table_linear *) swt;
    stats->name = "jit-table";
    stats->wildcards = OFPFW_ALL;
    stats->n_flows   = tl->n_flows;
    stats->max_flows = tl->max_flows;
    stats->n_lookup  = swt->n_lookup;
    stats->n_matched = swt->n_matched;
}

struct sw_table *table_jit_create(unsigned int max_flows)
{
    struct sw_table_jit *tj;
    struct sw_table *swt;
    
    flow_array = calloc(max_flows, sizeof(sw_flow*));

    tj = calloc(1, sizeof *tj);
    if (tj == NULL)
        return NULL;

    swt = &tj->swt;
    swt->lookup = table_jit_lookup;
    swt->insert = table_jit_insert;
    swt->modify = table_jit_modify;
    swt->has_conflict = table_jit_has_conflict;
    swt->delete = table_jit_delete;
    swt->timeout = table_jit_timeout;
    swt->destroy = table_jit_destroy;
    swt->stats = table_jit_stats;

    tj->max_flows = max_flows;
    tj->n_flows = 0;
    
    range_borders_ip_src = range_borders_ctor();
    range_borders_ip_dst = range_borders_ctor();
    range_borders_port_number = range_borders_ctor();
    range_borders_vlan_id = range_borders_ctor();
    range_borders_eth_type = range_borders_ctor();
    range_borders_transport_src = range_borders_ctor();
    range_borders_transport_dst = range_borders_ctor();
    range_borders_eth_src = range_borders_ctor();
    range_borders_eth_dst = range_borders_ctor();
    range_borders_vlan_prio = range_borders_ctor();
    range_borders_ip_dcsp = range_borders_ctor();
    range_borders_ip_protocol = range_borders_ctor();

    return swt;
}
