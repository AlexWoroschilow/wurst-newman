#ifndef WURST_IGRAPH
#define WURST_IGRAPH

void igraph_edges_remove_by(igraph_t *graph, const char * name, double value,
		int (*compare)(void * value1, void * value2));

#endif
