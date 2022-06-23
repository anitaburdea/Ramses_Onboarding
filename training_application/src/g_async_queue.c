/*
 * g_async_queue.c
 *
 *  Created on: Jun 17, 2022
 *      Author: anita
 */
#include "g_async_queue.h"

#include <stdio.h>

sGAsyncQueue* G_ASYNC_QUEUE_Init(void)
{
  // sGAsyncQueue *queue = g_queue_new();
    sGAsyncQueue *queue = g_new(sGAsyncQueue, 1);

   g_queue_init(&queue->queue);

   g_mutex_init (&queue->mutex);
   g_cond_init (&queue->cond);

   printf("Initialize queue\n");

   return queue;
}

void G_ASYNC_QUEUE_PUSH(sGAsyncQueue *queue, gpointer data)
{
    if (queue == NULL || data == NULL)
    {
        printf("Null queue or data\n");

        return;
    }

    g_mutex_lock(&queue->mutex);
    g_queue_push_head(&queue->queue, data);
    g_mutex_unlock(&queue->mutex);
}

gpointer G_ASYNC_QUEUE_POP(sGAsyncQueue *queue)
{
    if (g_queue_is_empty(&queue->queue))
    {
        return NULL;
    }
    gpointer retQueue;

    g_mutex_lock(&queue->mutex);
    retQueue =  g_queue_pop_tail(&queue->queue);
    g_mutex_unlock(&queue->mutex);

    return retQueue;
}

void G_ASYNC_QUEUE_Destroy(sGAsyncQueue *queue)
{
    if (queue == NULL)
    {
        return;
    }

    g_queue_clear(&queue->queue);
    g_mutex_clear(&queue->mutex);
    g_cond_clear(&queue->cond);

    g_queue_free(&queue->queue);
}
