#include <stdio.h>
#include <stdbool.h>

#include <glib.h>

int cnt = 0;
GMutex lock;

void* funcIncrement(gpointer data)
{
	g_mutex_lock(&lock);

	cnt++;
	printf("Counter %d\n", cnt);

	g_mutex_unlock(&lock);

	return NULL;
}

int main(void)
{
 	g_mutex_init(&lock);

	GThread *firstThread = g_thread_new("First thread", &funcIncrement, NULL);
	GThread *secThread = g_thread_new("Second thread", &funcIncrement, NULL);

	g_thread_join(firstThread);
	g_thread_join(secThread);

	g_mutex_clear(&lock);

    return 0;
}

