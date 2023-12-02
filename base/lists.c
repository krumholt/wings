#ifndef WINGS_BASE_LISTS_C_
#define WINGS_BASE_LISTS_C_

#include "wings/base/types.h"
#include "wings/base/allocators.c"
#include "wings/base/lists.h"

error
cstring_list__add(struct cstring_list *list,
                  char *cstring,
                  struct allocator *allocator)
{
   if (list->head == 0)
   {
      error error = allocate_struct(&list->head, allocator, struct cstring_list_node);
      IF_ERROR_RETURN(error);
   }
   else
   {
      struct cstring_list_node *node = list->head;
      error error = allocate_struct(&list->head, allocator, struct cstring_list_node);
      IF_ERROR_RETURN(error);
      list->head->next = node;
   }
   list->head->cstring = cstring;

   return (ec__no_error);
}

void
cstring_list__remove(struct cstring_list *list)
{
   if (list->head)
   {
      list->head = list->head->next;
   }
}

void
cstring_list__iterator(struct cstring_list *list)
{
   if (list->head)
   {
      list->head = list->head->next;
   }
}

struct cstring_list_node *
cstring_list_iterator__next(struct cstring_list_iterator *iterator)
{
   struct cstring_list_node *current = iterator->next;
   if (iterator->next)
   {
      iterator->next = iterator->next->next;
   }
   return (current);
}

#endif

