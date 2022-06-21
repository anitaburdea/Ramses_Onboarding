/*
 * dbus_server.c
 *
 *  Created on: Jun 8, 2022
 *      Author: Anita Burdea
 */
#include "dbus_server.h"
#include <stdio.h>
#include <time.h>
#include <string.h>

#include "../include/app-generated.h"
#include "g_async_queue.h"

#define ALARM_STATUS_MAX_LEN   50

typedef enum
{
    DBUS_METHOD_SET_TIME = 0,
    DBUS_METHOD_SET_ALARM_TIME,
    DBUS_METHOD_SET_ALARM_STATUS,
    DBUS_METHOD_GET_ALARM_STATUS

}eMethodType;

typedef struct
{
    char alarmStatus[ALARM_STATUS_MAX_LEN];
    struct tm alarmTime;
    struct tm time;
} sAlarmClock;

typedef struct
{
    sAlarmClock alarmClock;
    eMethodType methodType;
    int messageLength;
    GDBusMethodInvocation *invocation;
}sElement;

static sAlarmClock alarmClock;
static trainingapplication *_interface;
static GQueue *queue;
static GMutex *_mutex;
static GMainLoop *loop;
const char *const trainingApp = "org.gtk.GDBus.TestInterface";
const char *const SERVER_BUS_NAME = "org.gtk.GDBus";
const char *const DBUS_PATH_OBJ = "/org/gtk/GDBus/TestInterface";

static gboolean _on_handle_get_alarm_status(trainingapplication *interface, GDBusMethodInvocation *invocation,
        const char *alarmStatus, gpointer user_data)
{
    gchar *response = g_strdup_printf("Alarm status is: %s.", alarmClock.alarmStatus);

    sElement *msg = g_new(sElement, 1);

    // Copy and set the alarm status message
    strcpy(msg->alarmClock.alarmStatus, response);
    msg->methodType = DBUS_METHOD_GET_ALARM_STATUS;
    msg->messageLength = strlen(response);
    msg->invocation = invocation;

    G_ASYNC_QUEUE_PUSH(queue, msg);

    return TRUE;
}

static int _on_handle_ring_alarm(void *arg)
{
    printf("Entered in on handle ring alarm\n");

    if ((alarmClock.time.tm_hour == alarmClock.alarmTime.tm_hour)
            && (alarmClock.time.tm_min == alarmClock.alarmTime.tm_min)
            && (strcmp(alarmClock.alarmStatus, "active") == 0))
    {
        printf("Called emit ring alarm\n");

        training_application__emit_ring_alarm(_interface, "Ring-ring!!");

        G_ASYNC_QUEUE_Destroy(queue);

        printf("Async queue is destroyed\n");

        printf("Quit the Server main loop\n");
        g_main_loop_quit(loop);

        return FALSE;
    }

    return TRUE;
}

static gboolean _on_handle_set_alarm_status(trainingapplication *interface, GDBusMethodInvocation *invocation,
        const char *alarmStatus, gpointer user_data)
{
    printf("Set alarm status method: %s\n", alarmStatus);

    sElement *msg = g_new(sElement, 1);

    // Copy the alarm status in static variable
    strcpy(alarmClock.alarmStatus, alarmStatus);

    // Copy and set the alarm status message
    strcpy(msg->alarmClock.alarmStatus, alarmStatus);
    msg->methodType = DBUS_METHOD_SET_ALARM_STATUS;
    msg->messageLength = strlen(alarmStatus);
    msg->invocation = invocation;

    g_timeout_add(30000, &_on_handle_ring_alarm, NULL);

    G_ASYNC_QUEUE_PUSH(queue, msg);

    return TRUE;
}

static gboolean _on_handle_set_alarm_time(trainingapplication *interface, GDBusMethodInvocation *invocation,
        int hourAlarmTime, int minAlarmTime, gpointer user_data)
{
    printf("Set alarm time hour and minutes: %d:%d\n", hourAlarmTime, minAlarmTime);

    sElement *msg = g_new(sElement, 1);

    // Set the static structure
    alarmClock.alarmTime.tm_hour = hourAlarmTime;
    alarmClock.alarmTime.tm_min = minAlarmTime;

    // Set the alarm time hour and minutes
    msg->alarmClock.alarmTime.tm_hour = hourAlarmTime;
    msg->alarmClock.alarmTime.tm_min = minAlarmTime;
    msg->methodType = DBUS_METHOD_SET_ALARM_TIME;
    msg->messageLength = 2;
    msg->invocation = invocation;

    G_ASYNC_QUEUE_PUSH(queue, msg);

    return TRUE;
}

static gboolean _on_handle_set_time(trainingapplication *interface, GDBusMethodInvocation *invocation, int hourTime,
        int minTime, gpointer user_data)
{
    sElement *msg = g_new(sElement, 1);

    printf("Set time hour and minutes: %d:%d\n", hourTime, minTime);

    // Set the static structure
    alarmClock.time.tm_hour = hourTime;
    alarmClock.time.tm_min = minTime;

    // Set the time hour and minutes
    msg->alarmClock.time.tm_hour = hourTime;
    msg->alarmClock.time.tm_min = minTime;
    msg->methodType = DBUS_METHOD_SET_TIME;
    msg->messageLength = 2;
    msg->invocation = invocation;

    G_ASYNC_QUEUE_PUSH(queue, msg);

    return TRUE;
}

static void _on_bus_aquired(GDBusConnection *connection, const char *name, gpointer user_data)
{
    GError *error = NULL;

    _interface = training_application__skeleton_new();

    g_signal_connect(_interface, "handle-get-alarm-status", G_CALLBACK(_on_handle_get_alarm_status), NULL);

    g_signal_connect(_interface, "handle-set-alarm-status", G_CALLBACK(_on_handle_set_alarm_status), NULL);

    g_signal_connect(_interface, "handle-set-alarm-time", G_CALLBACK(_on_handle_set_alarm_time), NULL);

    g_signal_connect(_interface, "handle-set-time", G_CALLBACK(_on_handle_set_time), NULL);

    if (!g_dbus_interface_skeleton_export(G_DBUS_INTERFACE_SKELETON(_interface), connection, DBUS_PATH_OBJ, &error))
    {
        printf("Error while exporting the object\n");
    }

    printf("Bus acquired %s\n", name);
}

static void _on_name_acquired(GDBusConnection *connection, const char *name, gpointer user_data)
{
    printf("Acquired the name %s on the system bus\n", name);
}

static void _on_name_lost(GDBusConnection *connection, const char *name, gpointer user_data)
{
    printf("Lost the name %s on the system bus\n", name);
}

int DBUS_Server(void)
{
    guint owner_id;

    // Initialize the used queue
    queue = G_ASYNC_QUEUE_Init();

    owner_id = g_bus_own_name(G_BUS_TYPE_SYSTEM, SERVER_BUS_NAME, G_BUS_NAME_OWNER_FLAGS_NONE, _on_bus_aquired,
            _on_name_acquired, _on_name_lost, NULL, NULL);

    loop = g_main_loop_new(NULL, FALSE);
    g_main_loop_run(loop);

    g_bus_unown_name(owner_id);

    return 0;
}

void* DBUS_Server_MethodsProc(gpointer data)
{
    sElement *msg;
    int i = 0;

    printf("Thread created for methods processing\n");

    // Initialize the used queue
    queue = G_ASYNC_QUEUE_Init();

    while (i < 3)
    {
        g_usleep(3000000);

        msg = G_ASYNC_QUEUE_POP(queue);

        if (msg != NULL)
        {
            // This function will be called in the thread
            switch (msg->methodType)
            {
            case DBUS_METHOD_SET_TIME:
                printf("Complete set time method\n");
                training_application__complete_set_time(_interface, msg->invocation);
                break;
            case DBUS_METHOD_SET_ALARM_TIME:
                printf("Complete set alarm time\n");
                training_application__complete_set_alarm_time(_interface, msg->invocation);
                break;
            case DBUS_METHOD_SET_ALARM_STATUS:
                printf("Complete set alarm status method\n");
                training_application__complete_set_alarm_status(_interface, msg->invocation);
                break;
            case DBUS_METHOD_GET_ALARM_STATUS:
                training_application__complete_get_alarm_status(_interface, msg->invocation, msg->alarmClock.alarmStatus);
                break;
            default:
                printf("Invalid method type\n");
                break;
            }

            g_free(msg);
        }
        i++;
    }
}
