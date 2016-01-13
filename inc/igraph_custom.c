#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <massert.h>

#include <igraph.h>
#include <igraph_interface.h>
#include <igraph_attributes.h>

void igraph_edges_remove_by(igraph_t *graph, const char * name, double value,
		int (*compare)(void * value1, void * value2)) {

	for (unsigned long i = 0; i < igraph_ecount(graph); i++) {

		const double edge_attribute_value = igraph_cattribute_EAN(graph, name, i);
		if (!compare((void *) &edge_attribute_value, (void *) &value)) {
			continue;
		}

		igraph_es_t edges = igraph_ess_1(i);
		igraph_delete_edges(graph, edges);
		igraph_es_destroy(&edges);

		// todo: it works only recursive, why?
		igraph_edges_remove_by(graph, name, value, compare);
		break;
	}
}
