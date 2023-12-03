#ifndef WINGS_BASE_LISTS_H_
#define WINGS_BASE_LISTS_H_

#include "wings/base/macros.c"
#include "wings/base/allocators.c"

struct cstring_list_node
{
   char *cstring;
   struct cstring_list_node *next;
};

struct cstring_list
{
   struct cstring_list_node *head;
};

struct cstring_list_iterator
{
   struct cstring_list_node *next;
};

error
cstring_list__add(struct cstring_list *list,
                  char *cstring,
                  struct allocator *allocator);

void
cstring_list__remove(struct cstring_list *list);

struct cstring_list_iterator
cstring_list_iterator__new(struct cstring_list *list);

struct cstring_list_node *
cstring_list_iterator__next(struct cstring_list_iterator *iterator);

#endif
