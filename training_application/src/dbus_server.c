/*
 * dbus_server.c
 *
 *  Created on: Jun 8, 2022
 *      Author: Anita Burdea
 */
#include <stdio.h>
#include <gio/gio.h>
#include <time.h>
#include <string.h>

#include "app-generated.h"

#define ALARM_STATUS_MAX_LEN   50

typedef struct
{
    char alarmStatus[ALARM_STATUS_MAX_LEN];
    struct tm alarmTime;
    struct tm time;
}sAlarmClock;

static sAlarmClock alarmClock;
trainingapplication * interface;
const char *const trainingApp = "org.gtk.GDBus.TestInterface";
const char *const SERVER_BUS_NAME = "org.gtk.GDBus";
const char *const DBUS_PATH_OBJ = "/org/gtk/GDBus/TestInterface";

static gboolean _on_handle_get_alarm_status(trainingapplication *interface, GDBusMethodInvocation *invocation,
        const char *alarmStatus, gpointer user_data)
{
    gchar *response = g_strdup_printf("Alarm status is: %s.", alarmClock.alarmStatus);

    training_application__complete_get_alarm_status(interface, invocation, response);

    return TRUE;
}

static gboolean _on_handle_set_alarm_status(trainingapplication *interface, GDBusMethodInvocation *invocation,
        const char *alarmStatus, gpointer user_data)
{
    printf("Set alarm status method: %s\n", alarmStatus);

    // Copy the alarm status in static variable
    strcpy(alarmClock.alarmStatus, alarmStatus);

    training_application__complete_set_alarm_status(interface, invocation);

    return TRUE;
}

static gboolean _on_handle_set_alarm_time(trainingapplication *interface, GDBusMethodInvocation *invocation,
        int hourAlarmTime, int minAlarmTime, gpointer user_data)
{
    printf("Set alarm time hour and minutes: %d:%d\n", hourAlarmTime, minAlarmTime);

    // Set the alarm time hour and minutes
    alarmClock.alarmTime.tm_hour = hourAlarmTime;
    alarmClock.alarmTime.tm_min = minAlarmTime;

    training_application__complete_set_alarm_time(interface, invocation);

    return TRUE;
}

static gboolean _on_handle_set_time(trainingapplication *interface, GDBusMethodInvocation *invocation, int hourTime,
        int minTime, gpointer user_data)
{
    printf("Set time hour and minutes: %d:%d\n", hourTime, minTime);

    // Set the time hour and minutes
    alarmClock.time.tm_hour = hourTime;
    alarmClock.time.tm_min = minTime;

    training_application__complete_set_time(interface, invocation);

    return TRUE;
}

static int _on_handle_ring_alarm (void *arg)
{
    printf("Ring alarm handler\n");

    if ((alarmClock.time.tm_hour == alarmClock.alarmTime.tm_hour)
            && (alarmClock.time.tm_min == alarmClock.alarmTime.tm_min)
            && (strcmp(alarmClock.alarmStatus, "active") == 0))
    {
        printf("Called emit ring alarm\n");

        training_application__emit_ring_alarm(interface, "Ring-ring!!");

        return FALSE;
    }

    return TRUE;
}

static void _on_bus_aquired(GDBusConnection *connection, const char *name, gpointer user_data)
{
    GError *error = NULL;

    interface = training_application__skeleton_new();

    g_signal_connect(interface, "handle-get-alarm-status", G_CALLBACK(_on_handle_get_alarm_status), NULL);

    g_signal_connect(interface, "handle-set-alarm-status", G_CALLBACK(_on_handle_set_alarm_status), NULL);

    g_signal_connect(interface, "handle-set-alarm-time", G_CALLBACK(_on_handle_set_alarm_time), NULL);

    g_signal_connect(interface, "handle-set-time", G_CALLBACK(_on_handle_set_time), NULL);

    g_timeout_add(10000, &_on_handle_ring_alarm, NULL);

    if(!g_dbus_interface_skeleton_export(G_DBUS_INTERFACE_SKELETON(interface), connection, DBUS_PATH_OBJ, &error))
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
    GMainLoop *loop;
    guint owner_id;

    owner_id = g_bus_own_name(G_BUS_TYPE_SYSTEM, SERVER_BUS_NAME, G_BUS_NAME_OWNER_FLAGS_NONE, _on_bus_aquired,
            _on_name_acquired, _on_name_lost, NULL, NULL);

    loop = g_main_loop_new(NULL, FALSE);
    g_main_loop_run(loop);

    g_bus_unown_name(owner_id);

    return 0;
}
