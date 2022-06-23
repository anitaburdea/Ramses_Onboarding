/*
 * g_async_queue.h
 *
 *  Created on: Jun 17, 2022
 *      Author: anita
 */

#ifndef TRAINING_APPLICATION_INCLUDE_G_ASYNC_QUEUE_H_
#define TRAINING_APPLICATION_INCLUDE_G_ASYNC_QUEUE_H_

#include <stdint.h>

#include <glib.h>

typedef struct
{
    GMutex mutex;
    GQueue queue;
    GCond cond;
}sGAsyncQueue;

sGAsyncQueue* G_ASYNC_QUEUE_Init(void);

void G_ASYNC_QUEUE_PUSH(sGAsyncQueue *queue, gpointer data);

gpointer G_ASYNC_QUEUE_POP(sGAsyncQueue *queue);

void G_ASYNC_QUEUE_Destroy(sGAsyncQueue *queue);


#endif /* TRAINING_APPLICATION_INCLUDE_G_ASYNC_QUEUE_H_ */
