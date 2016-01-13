#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <massert.h>
#include <limits.h>
#include <string.h>

#include <igraph.h>
#include <igraph_interface.h>
#include <igraph_attributes.h>

#include "iclust.h"
#include "logger.h"

iclust_collection_element * iclust_collection_element_new(const char * name, unsigned long cluster) {
	iclust_collection_element * element = malloc(sizeof(*element));
	massert((element !=NULL), "Can not initialize memory for collection element");

	element->name = strdup(name);
	element->cluster = cluster;
	return element;
}

int iclust_collection_element_compare(const void * a, const void * b) {
	iclust_collection_element* clister1 = *((iclust_collection_element**) a);
	iclust_collection_element* clister2 = *((iclust_collection_element**) b);

	massert(clister1 !=NULL, "Object1 to compare can not be NULL");
	massert(clister2 !=NULL, "Object2 to compare can not be NULL");

	return (clister1->cluster < clister2->cluster) ? -1 : 1;
}

void iclust_collection_element_destroy(iclust_collection_element * element) {
	free((char *) element->name);
	free(element);
}

iclust_collection * iclust_collection_new() {
	iclust_collection * collection = malloc(sizeof(*collection));
	massert((collection !=NULL), "Can not initialize memory for collection element");

	collection->length = 0;
	collection->collection = NULL;

	return collection;
}

void iclust_collection_append(iclust_collection * collection, iclust_collection_element * element) {
	unsigned long size_new = collection->length + 1;

	massert((collection !=NULL), "Collection object can not be empty");
	collection->collection = realloc(collection->collection, (sizeof((*collection->collection)) * size_new));
	massert((collection->collection !=NULL), "Collection pointer can not be NULL");

	collection->collection[collection->length] = element;
	collection->length = size_new;
}

void iclust_collection_print(iclust_collection * collection, void (*printer)(iclust_collection_element * element)) {
	massert((collection !=NULL), "Collection object can not be empty");
	for (unsigned long i = 0; i < collection->length; i++) {
		printer(collection->collection[i]);
	}
}

void iclust_collection_sort(iclust_collection * collection) {
	massert((collection !=NULL), "Collection object can not be empty");
	qsort(collection->collection, collection->length, sizeof(*collection->collection),
			iclust_collection_element_compare);
}

void iclust_collection_destroy(iclust_collection * collection) {
	massert((collection !=NULL), "Collection object can not be empty");
	for (unsigned long i = 0; i < collection->length; i++) {
		iclust_collection_element * element = collection->collection[i];
		iclust_collection_element_destroy(element);
	}
	free(collection->collection);
	free(collection);
}

void iclust_collection_fill_leading_eigenvector(iclust_collection * collection, igraph_t *graph, unsigned int clbase,
		unsigned int delimiter) {

	int response;
	igraph_es_t esequences;
	igraph_vector_t weights;
	igraph_vector_t membership;
	igraph_arpack_options_t options;
	time_t time_start, time_end;

	massert((graph !=NULL), "Graph object can not be empty");
	massert((collection !=NULL), "Collection object can not be empty");

	response = igraph_vector_init(&weights, 0);
	massert((response == IGRAPH_SUCCESS), "Can not initialize vector");

	response = igraph_vector_init(&membership, 0);
	massert((response == IGRAPH_SUCCESS), "Can not initialize vector");

	igraph_arpack_options_init(&options);
	(&options)->mxiter = INT_MAX;

	response = igraph_es_all(&esequences, IGRAPH_EDGEORDER_ID);
	massert((response == IGRAPH_SUCCESS), "Ca not fill edge sequence vector");

	response = igraph_cattribute_EANV(graph, "weight", esequences, &weights);
	massert((response == IGRAPH_SUCCESS), "Can not finish a leading_eigenvector clustering");
	igraph_es_destroy(&esequences);

	unsigned int steps = igraph_vcount(graph) / delimiter;
	if (steps <= 0) {
		steps = igraph_vcount(graph);
	}

	logger_info("Found:\t subgraph V:%u, E:%u, steps: %u", igraph_vcount(graph), igraph_ecount(graph), steps);

	time(&time_start);
	response = igraph_community_leading_eigenvector( //
			/* The undirected input graph. */
			graph,
			/* The weights of the edges, or a
			 * null pointer for unweighted graphs. */
			&weights,
			/* The result of the algorithm, a matrix containing the information about the splits performed. The matrix is built in the opposite
			 * way however, it is like the result of an agglomerative algorithm. If at the end of the algorithm (after steps steps was done)
			 * there are “p” communities, then these are numbered from zero to “p-1”. The first line of the matrix contains the first “merge”
			 * (which is in reality the last split) of two communities into community “p”, the merge in the second line forms community “p+1”,
			 * etc. The matrix should be initialized before calling and will be resized as needed. This argument is ignored of it is NULL. */
			NULL,
			/*The membership of the vertices after all the splits were performed will be stored here. The vector must be initialized before calling
			 * and will be resized as needed. This argument is ignored if it is NULL. This argument can also be used to supply a starting configuration
			 * for the community finding, in the format of a membership vector. In this case the start argument must be set to 1. */
			&membership,
			/* The maximum number of steps to perform. It might happen that some component (or the whole network) has no underlying
			 * community structure and no further steps can be done. If you want as many steps as possible then supply the number
			 * of vertices in the network here. */
			steps,
			/* The options for ARPACK. n is always
			 * overwritten. ncv is set to at least 4. */
			&options,
			/* If not a null pointer, then it must be a pointer to a real number
			 * and the modularity score of the final division is stored here. */
			NULL,
			/* Boolean, whether to use the community structure
			 * given in the membership argument as a starting point.  */
			0,
			/* Pointer to an initialized vector or a null pointer. If not a null pointer, then the eigenvalues calculated along
			 * the community structure detection are stored here. The non-positive eigenvalues, that do not result a split,
			 * are stored as well. */
			NULL,
			/* If not a null pointer, then the eigenvectors that are calculated in each step of the algorithm, are stored here,
			 * in a pointer vector. Each eigenvector is stored in an igraph_vector_t object. The user is responsible of
			 * deallocating the memory that belongs to the individual vectors, by calling first igraph_vector_destroy(),
			 * and then free() on them. */
			NULL,
			/* Pointer to an initialized vector or a null pointer. If not a null
			 * pointer, then a trace of the algorithm is stored here, encoded numerically. */
			NULL,
			/* A null pointer or a function of type igraph_community_leading_eigenvector_callback_t.
			 * If given, this callback function is called after each eigenvector/eigenvalue calculation.
			 * If the callback returns a non-zero value, then the community finding algorithm stops.
			 * See the arguments passed to the callback at the documentation of igraph_community_leading_eigenvector_callback_t. */
			NULL,
			/* Extra argument to pass to the callback function. */
			NULL);
	time(&time_end);

	logger_info("Time:\t communities %f", difftime(time_end, time_start));

	/* printf("Time:\t igraph_community_leading_eigenvector in %f sec\n", difftime(time_end, time_start));*/

	massert((response == IGRAPH_SUCCESS), "Can not finish a leading_eigenvector clustering");
	for (unsigned long i = 0; i < igraph_vector_size(&membership); i++) {
		unsigned long member = VECTOR(membership)[i];
		const char* name = igraph_cattribute_VAS(graph, "name", i);
		massert((strlen(name) > 0), "Vertex name can not be empty");

		iclust_collection_element * element = iclust_collection_element_new(name, (clbase + member));
		massert((element != NULL), "Vertex name can not be empty");

		iclust_collection_append(collection, element);
	}

	igraph_vector_destroy(&membership);
	igraph_vector_destroy(&weights);
}
