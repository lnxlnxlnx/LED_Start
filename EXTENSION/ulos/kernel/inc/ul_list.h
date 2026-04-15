/*
 * Copyright (c) 2025 ulOS Community
 *
 * SPDX-License-Identifier: GPL-2.0-or-late
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-12-17     zhuqinsheng  first version
 */
#ifndef _UL_LIST_H
#define _UL_LIST_H

#include "ul_config.h"

#define POISON_POINTER_DELTA 0

#define LIST_POISON1  ((void *) 0 + POISON_POINTER_DELTA)
#define LIST_POISON2  ((void *) 0 + POISON_POINTER_DELTA)

typedef struct ul_list_node
{
    struct ul_list_node *next;
    struct ul_list_node *prev;
}ul_list_t;
   
#define UL_LIST_HEAD_INIT(name) \
    { &(name), &(name) }  

#define UL_LIST_HEAD(name) \
    struct ul_list_node name = UL_LIST_HEAD_INIT(name)
        
/**
 * @brief tests whether @list is the first entry in list @head
 *
 * @param list the entry to test
 * @param head the head of the list
 * @return 1 if first, 0 if not
 */
ul_inline int ul_list_is_first(const ul_list_t *list, const ul_list_t *head)
{
    return list->prev == head;
}

/**
 * @brief tests whether @list is the last entry in list @head
 *
 * @param list the entry to test
 * @param head the head of the list
 * @return 1 if last, 0 if not
 */
ul_inline int ul_list_is_last(const ul_list_t *list, const ul_list_t *head)
{
    return list->next == head;
}

/**
 * @brief tests whether a list is empty
 *
 * @param l the list to test
 * @return 1 if empty, 0 if not
 */
ul_inline int ul_list_is_empty(const ul_list_t *l)
{
    return l->next == l;
}

/**
 * @brief tests whether a list has only one entry
 *
 * @param head the head of the list
 * @return 1 if singular, 0 if not
 */
ul_inline int ul_list_is_singular(const ul_list_t *head)
{
    return !ul_list_is_empty(head) && (head->next == head->prev);
}

/**
 * @brief initialize a list
 *
 * @param list list to be initialized
 */
ul_inline void ul_list_init(ul_list_t *list)
{
    list->next = list;
    list->prev = list;
}

/**
 * @brief add a new node between prev and next
 *
 * @param new new node to be added
 * @param prev previous node
 * @param next next node
 */
ul_inline void __ul_list_add(ul_list_t *new,
                ul_list_t *prev,
                ul_list_t *next)
{
    next->prev = new;
    new->next = next;
    new->prev = prev;
    prev->next = new;
}

/**
 * @brief add a new node after head
 *
 * @param new new node to be added
 * @param head the head of the list
 */
ul_inline void ul_list_add_head(ul_list_t *new, ul_list_t *head)
{
    __ul_list_add(new, head, head->next);
}

/**
 * @brief add a new node before head (at tail)
 *
 * @param new new node to be added
 * @param head the head of the list
 */
ul_inline void ul_list_add_tail(ul_list_t *new, ul_list_t *head)
{
    __ul_list_add(new, head->prev, head);
}

/**
 * @brief delete nodes between prev and next
 *
 * @param prev previous node
 * @param next next node
 */
ul_inline void __ul_list_del(ul_list_t *prev, ul_list_t *next)
{
    next->prev = prev;
    prev->next = next;
}

/**
 * @brief delete an entry from list
 *
 * @param entry the entry to delete
 */
ul_inline void __ul_list_del_entry(ul_list_t *entry)
{
    __ul_list_del(entry->prev, entry->next);
}

/**
 * @brief delete an entry from list and poison it
 *
 * @param entry the entry to delete
 */
ul_inline void ul_list_del(ul_list_t *entry)
{
    __ul_list_del_entry(entry);
    entry->next = UL_NULL;
    entry->prev = UL_NULL;
}

/**
 * @brief delete an entry from list and reinitialize it
 *
 * @param entry the entry to delete
 */
ul_inline void ul_list_del_init(ul_list_t *entry)
{
    __ul_list_del_entry(entry);
    ul_list_init(entry);
}

/**
 * @brief delete from one list and add as another's head
 *
 * @param list the entry to move
 * @param head the head that will precede our entry
 */
ul_inline void ul_list_move(ul_list_t *list, ul_list_t *head)
{
    __ul_list_del_entry(list);
    ul_list_add_head(list, head);
}

/**
 * @brief delete from one list and add as another's tail
 *
 * @param list the entry to move
 * @param head the head that will follow our entry
 */
ul_inline void ul_list_move_tail(ul_list_t *list,
                    ul_list_t *head)
{
    __ul_list_del_entry(list);
    ul_list_add_tail(list, head);
}

/**
 * @brief move a range of nodes to the tail of another list
 *
 * @param head the head of the list to move to
 * @param first first node in range
 * @param last last node in range
 */
ul_inline void ul_list_bulk_move_tail(ul_list_t *head,
                        ul_list_t *first,
                        ul_list_t *last)
{
    first->prev->next = last->next;
    last->next->prev = first->prev;

    head->prev->next = first;
    first->prev = head->prev;

    last->next = head;
    head->prev = last;
}

/**
 * @brief replace old node with new node
 *
 * @param old the node to be replaced
 * @param new the new node
 */
ul_inline void ul_list_replace(ul_list_t *old,
                ul_list_t *new)
{
    new->next = old->next;
    new->next->prev = new;
    new->prev = old->prev;
    new->prev->next = new;
}

/**
 * @brief replace old node with new node and initialize old node
 *
 * @param old the node to be replaced
 * @param new the new node
 */
ul_inline void ul_list_replace_init(ul_list_t *old,
                    ul_list_t *new)
{
    ul_list_replace(old, new);
    ul_list_init(old);
}

/**
 * @brief swap two list entries
 *
 * @param entry1 first entry
 * @param entry2 second entry
 */
ul_inline void ul_list_swap(ul_list_t *entry1,
                ul_list_t *entry2)
{
    ul_list_t *pos = entry2->prev;

    ul_list_del(entry2);
    ul_list_replace(entry1, entry2);
    if (pos == entry1)
        pos = entry2;
    ul_list_add_head(entry1, pos);
}

/**
 * @brief rotate the list to the left
 *
 * @param head the head of the list
 */
ul_inline void ul_list_rotate_left(ul_list_t *head)
{
    ul_list_t *first;

    if (!ul_list_is_empty(head)) {
        first = head->next;
        ul_list_move_tail(first, head);
    }
}

/**
 * @brief rotate the list to bring a specific entry to front
 *
 * @param list the entry to bring to front
 * @param head the head of the list
 */
ul_inline void ul_list_rotate_to_front(ul_list_t *list,
                    ul_list_t *head)
{
    ul_list_move_tail(head, list);
}

/**
 * @brief insert a node after a list
 *
 * @param list list to insert it
 * @param n new node to be inserted
 */
ul_inline void ul_list_insert_after(ul_list_t *list, ul_list_t *new)
{
    list->next->prev = new;
    new->next = list->next;

    list->next = new;
    new->prev = list;
}

/**
 * @brief insert a node before a list
 *
 * @param list list to insert it
 * @param new new node to be inserted
 */
ul_inline void ul_list_insert_before(ul_list_t *list, ul_list_t *new)
{
    list->prev->next = new;
    new->prev = list->prev;

    list->prev = new;
    new->next = list;
}

/**
 * @brief iterate over a list
 *
 * @param pos the ul_list_t * to use as a loop cursor
 * @param head the head for your list
 */
#define ul_list_for_each(pos, head) \
    for (pos = (head)->next; pos != (head); pos = pos->next)

/**
 * @brief iterate over a list safe against removal of list entry
 *
 * @param pos the ul_list_t * to use as a loop cursor
 * @param n another ul_list_t * to use as temporary storage
 * @param head the head for your list
 */
#define ul_list_for_each_safe(pos, n, head) \
    for (pos = (head)->next, n = pos->next; pos != (head); \
         pos = n, n = pos->next)

/**
 * @brief iterate over a list backwards
 *
 * @param pos the ul_list_t * to use as a loop cursor
 * @param head the head for your list
 */
#define ul_list_for_each_prev(pos, head) \
    for (pos = (head)->prev; pos != (head); pos = pos->prev)

/**
 * @brief iterate over a list backwards safe against removal of list entry
 *
 * @param pos the ul_list_t * to use as a loop cursor
 * @param n another ul_list_t * to use as temporary storage
 * @param head the head for your list
 */
#define ul_list_for_each_prev_safe(pos, n, head) \
    for (pos = (head)->prev, n = pos->prev; pos != (head); \
         pos = n, n = pos->prev)

/**
 * @brief get the struct containing this member
 *
 * @param node the pointer to the member
 * @param type the type of the container struct
 * @param member the name of the member within the struct
 */
#define container_of(node, type, member) \
    ((type *)((char *)(node) - (unsigned long)(&((type *)0)->member)))
        
/**
 * @brief get the struct for this entry
 *
 * @param node the entry point
 * @param type the type of structure
 * @param member the name of list in structure
 */
#define ul_list_entry(node, type, member) \
    container_of(node, type, member)

/**
 * @brief iterate over list of given type
 *
 * @param pos the type * to use as a loop cursor
 * @param head the head for your list
 * @param member the name of the list_head within the struct
 */
#define ul_list_for_each_entry(pos, head, member)			\
	for (pos = list_first_entry(head, typeof(*pos), member);\
	     !ul_list_entry_is_head(pos, head, member);			\
	     pos = list_next_entry(pos, member))
    
/**
 * @brief iterate over list of given type safe against removal of list entry
 *
 * @param pos the type * to use as a loop cursor
 * @param n another type * to use as temporary storage
 * @param head the head for your list
 * @param member the name of the list_head within the struct
 */
#define ul_list_for_each_entry_safe(pos, n, head, member) \
    for (pos = ul_list_entry((head)->next, typeof(*pos), member), \
         n = ul_list_entry(pos->member.next, typeof(*pos), member); \
         !ul_list_entry_is_head(pos, head, member); \
         pos = n, n = ul_list_entry(n->member.next, typeof(*n), member))

#endif
