#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <massert.h>

#include <igraph.h>
#include <igraph_interface.h>
#include <igraph_attributes.h>

#include "logger.h"

void igraph_edges_remove_by(igraph_t *graph, const char * name, double value, int (*compare)(void * value1, void * value2)) {

	igraph_integer_t i = 0;
	igraph_integer_t j = 0;
	igraph_integer_t t = igraph_ecount(graph);

	igraph_vector_t vector;
	igraph_vector_init(&vector, 0);

	for (i = 0; i < t; i++) {
		const double edge_attribute_value = igraph_cattribute_EAN(graph, name, i);
		if (compare((void *) &edge_attribute_value, (void *) &value)) {
			igraph_vector_resize(&vector, j + 1);
			VECTOR(vector)[j++] = i;
			continue;
		}
	}

	igraph_es_t edges = igraph_ess_vector((const igraph_vector_t *) &vector);

	igraph_delete_edges(graph, edges);
	igraph_vector_destroy(&vector);
	igraph_es_destroy(&edges);
}
