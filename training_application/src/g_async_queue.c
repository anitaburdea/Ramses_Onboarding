/*
 * g_async_queue.c
 *
 *  Created on: Jun 17, 2022
 *      Author: anita
 */
#include "g_async_queue.h"

#include <stdio.h>

GQueue* G_ASYNC_QUEUE_Init(void)
{
   GQueue *queue = g_queue_new();

   g_queue_init(queue);

   printf("Initialize queue\n");

   return queue;
}

void G_ASYNC_QUEUE_PUSH(GQueue *queue, gpointer data)
{
    if (queue == NULL || data == NULL)
    {
        printf("Null queue or data\n");

        return;
    }

    g_queue_push_head(queue, data);
}

gpointer G_ASYNC_QUEUE_POP(GQueue *queue)
{
    if (g_queue_is_empty(queue))
    {
        return NULL;
    }

    return g_queue_pop_tail(queue);
}

void G_ASYNC_QUEUE_Destroy(GQueue *queue)
{
    if (queue == NULL)
    {
        return;
    }

    g_queue_clear(queue);

    g_queue_free(queue);
}
