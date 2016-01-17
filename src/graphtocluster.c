/*
 ============================================================================
 Name        : WurstNewman.c
 Author      : Alex Woroschilow
 Version     :
 Copyright   : GPL V3
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <limits.h>
#include <getopt.h>
#include <configini.h>

#include <igraph.h>
#include <igraph_error.h>
#include <igraph_foreign.h>
#include <igraph_interface.h>
#include <igraph_attributes.h>
#include <igraph_strvector.h>
#include <igraph_statusbar.h>

#include "massert.h"
#include "igraph_custom.h"
#include "iclust.h"
#include "logger.h"

int double_lt(void * value1, void * value2) {
	return *((double*) value1) <= *((double*) value2);
}

/* Try to find config from command
 * line parameters or use default */
const char * getopt_configfile(int argc, char** argv, const char * defaultfile) {
	int c;
	while ((c = getopt(argc, argv, "c:")) != -1) {
		switch (c) {
		case 'c':
			return (const char *) optarg;
		}
	}
	return defaultfile;
}

int main(int argc, char** argv) {

	int response;
	igraph_t graph;
	igraph_vector_ptr_t complist;
	iclust_collection * collection = NULL;
	time_t time_start, time_end;

	/* turn on attribute handling */
	igraph_i_set_attribute_table(&igraph_cattribute_table);

	double minimal_weight;
	unsigned int maximal_steps_delimieter;
	char graphncol[1024], logconfig[1024];

	Config *cfg = ConfigNew();
	const char * configuration = getopt_configfile(argc, argv, "./graphtocluster.conf");
	massert((ConfigReadFile(configuration, &cfg) == CONFIG_OK), "Configuration file is not readable");
	ConfigReadString(cfg, "sources", "graphncol", graphncol, sizeof(graphncol), 0);
	ConfigReadString(cfg, "sources", "logconfig", logconfig, sizeof(logconfig), 0);
	ConfigReadDouble(cfg, "limits", "minimal_weight", &minimal_weight, 0);
	ConfigReadUnsignedInt(cfg, "limits", "maximal_steps_delimieter", &maximal_steps_delimieter, 1);
	massert((maximal_steps_delimieter > 0), "Delimiter can not be equal to zero");
	ConfigFree(cfg);

	logger_init(logconfig, "graphtocluster");
	logger_info("File:\t configuration %s", configuration);
	logger_info("File:\t configuration logger %s", logconfig);
	logger_info("File:\t ncol graph source %s", graphncol);
	logger_info("Min:\t edge weight %f", minimal_weight);
	logger_info("Max:\t step delimeter %u", maximal_steps_delimieter);

	FILE *graph_source = fopen(graphncol, "r");
	response = igraph_read_graph_ncol(&graph, graph_source, NULL, true, IGRAPH_ADD_WEIGHTS_YES, 0);
	massert((response == IGRAPH_SUCCESS), "Can not read a graph");
	logger_info("Count:\t edges at start: %d", igraph_ecount(&graph));
	fclose(graph_source);

	time(&time_start);
	igraph_edges_remove_by(&graph, "weight", minimal_weight, double_lt);
	time(&time_end);
	logger_info("Time:\t remove edges: %f", difftime(time_end, time_start));
	logger_info("Count:\t edges after remove: %d", igraph_ecount(&graph));

	response = igraph_vector_ptr_init(&complist, 0);
	massert((response == IGRAPH_SUCCESS), "Can not initialize vector pointer");

	response = igraph_decompose(&graph, &complist, IGRAPH_WEAK, -1, 0);
	massert((response == IGRAPH_SUCCESS), "Can not decompose graph");

	unsigned int n = igraph_vector_ptr_size(&complist);

	collection = iclust_collection_new();
	massert((collection != NULL), "Cluster collection object can not be empty");

	time(&time_start);
	for (unsigned int i = 0; i < n; i++) {

		igraph_t *subgraph = VECTOR(complist)[i];
		massert((subgraph != NULL), "Subgraph object can not be empty");

		iclust_collection_fill_leading_eigenvector(collection, subgraph, (i + 1), maximal_steps_delimieter);
		igraph_destroy(subgraph);
	}
	time(&time_end);

	logger_info("Time:\t cluster: %f", difftime(time_end, time_start));

	/* Sort collection by cluster id to be
	 * able to build a second column correct*/
	time(&time_start);
	iclust_collection_sort(collection);
	time(&time_end);

	logger_info("Time:\t cluster sorting: %f", difftime(time_end, time_start));

	unsigned long cluster_index = 1, cluster = 0;
	for (unsigned long i = 0; i < collection->length; i++) {

		iclust_collection_element * element = collection->collection[i];
		massert((element != NULL), "Cluster collection element object can not be empty");

		if (element->cluster != cluster) {
			cluster = element->cluster;
			cluster_index = 0;
		}

		printf("%lu\t%lu\t%s\n", cluster, ++cluster_index, element->name);
	}

	iclust_collection_destroy(collection);
	igraph_vector_ptr_destroy(&complist);
	igraph_destroy(&graph);
	logger_destroy();

	return EXIT_SUCCESS;
}
