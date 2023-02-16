


struct node {
    void *data;
    struct node next;
}

struct list {
    struct node *start;
    size_t size;
}

typedef struct list list_t;
typedef struct node node_t;

node_t* create_node(void*data)
{
    node_t* node = (node_t*) malloc(sizeof(node_t));
    
    // Data should be free'd by the caller
    node->data = data;

    node->next = NULL;

    return node;
}

list_t* create()
{
    list_t *list = (list_t*) malloc(sizeof(list_t));

    list->start = NULL;
    list->size  = 0;

    return list;
}


void insert_back(list_t*list, void * data)
{
    node_t* node = create_node(data);

    list_t* ptr;

    // If Start is NULL its the first item in the list
    if ( list->start == NULL )
    {
        list->start = node;
    }
    else 
    {
        // Itterate through the list till we get the end
        ptr = list->start;
        while ( ptr->next != NULL )
        {
            ptr = ptr->next;
        }
        // Set the last next slot to the new node
        ptr->next = node;
    }
}

void insert_front(list_t* list, void * data)
{
    node_t* node = create_node(data);

    list_t* ptr;

}
