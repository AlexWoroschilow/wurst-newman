#ifndef WURST_ICLUST
#define WURST_ICLUST

typedef struct iclust_collection_element {
	const char * name;
	unsigned long cluster;
} iclust_collection_element;

typedef struct iclust_collection {
	iclust_collection_element ** collection;
	unsigned long length;
} iclust_collection;

iclust_collection_element * iclust_collection_element_new(const char * name, unsigned long cluster);
int iclust_collection_element_compare(const void * a, const void * b);

iclust_collection * iclust_collection_new();
void iclust_collection_print(iclust_collection * collection, void (*printer)(iclust_collection_element * element));
void iclust_collection_sort(iclust_collection * collection);
void iclust_collection_destroy(iclust_collection * collection);

void iclust_collection_fill_leading_eigenvector(iclust_collection * collection, igraph_t *graph, unsigned int clbase,
		unsigned int delimiter);

#endif
