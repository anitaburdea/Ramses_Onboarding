/*
 * dbus_client.c
 *
 *  Created on: Jun 14, 2022
 *      Author: anita
 */
#include "dbus_client.h"
#include <stdio.h>
#include <gio/gio.h>

#include "app-generated.h"

static trainingapplication *proxy;
static GMainLoop *loop;

static void (*alarmCallback_g)(void);

void DBUS_Client_SetCBRegister(void (*alarmCallback)(void))
{
    alarmCallback_g = alarmCallback;
}

static void _on_handle_ring_alarm(void)
{
    if(alarmCallback_g)
    {
        alarmCallback_g();
    }

    printf("Quit the main loop\n");
    g_main_loop_quit(loop);
}

static void _clientSetTimeCB(GObject *source_object, GAsyncResult *res, gpointer user_data)
{
    (void) *source_object;

    guint status;
    GError *err = NULL;

    printf("Client set time callback\n");

    // Get response
    if (training_application__call_set_time_finish(proxy, res, &err))
    {
        printf("Set time finished successfully!\n");
    }

}

static void _clientSetAlarmTimeCB(GObject *source_object, GAsyncResult *res, gpointer user_data)
{
    (void) *source_object;

    guint status;
    GError *err = NULL;

    printf("Client set alarm time callback\n");

    // Get response
    if (training_application__call_set_alarm_time_finish(proxy, res, &err))
    {
        printf("Set alarm time finished successfully!\n");
    }

}

static void _clientSetAlarmStatusCB(GObject *source_object, GAsyncResult *res, gpointer user_data)
{
    (void) *source_object;

    guint status;
    GError *err = NULL;

    printf("Client set alarm status callback\n");

    // Get response
    if (training_application__call_set_alarm_status_finish(proxy, res, &err))
    {
        printf("Set alarm status finished successfully!\n");
    }
}

void DBUS_Client_SetTime(int hour, int minutes)
{
    printf("Method set time\n");

    training_application__call_set_time(proxy, hour, minutes, NULL, &_clientSetTimeCB, NULL);
}

void DBUS_Client_SetAlarmTime(int alarmHour, int alarmMinutes)
{
    printf("Method set alarm time\n");

    training_application__call_set_alarm_time(proxy, alarmHour, alarmMinutes, NULL, &_clientSetAlarmTimeCB, NULL);
}

void DBUS_Client_SetAlarmStatus(const char *alarmStatus)
{
    printf("Method set alarm status\n");

    training_application__call_set_alarm_status(proxy, alarmStatus, NULL, &_clientSetAlarmStatusCB, NULL);
}

void DBUS_Client_GetAlarmStatus(char *alarmStatus)
{
    GError *err = NULL;

    printf("Method get alarm status\n");

    training_application__call_get_alarm_status_finish(proxy, &alarmStatus, NULL, &err);
}

int DBUS_Client_Init(const char *dbusServerBusName, const char *dbusObjPath)
{
    GError *err = NULL;

    proxy = training_application__proxy_new_for_bus_sync(G_BUS_TYPE_SYSTEM, G_DBUS_PROXY_FLAGS_NONE, dbusServerBusName,
            dbusObjPath, NULL, &err);

    return 0;
}

void DBUS_Client_SignalConnect(void)
{
    g_signal_connect(proxy, "ring-alarm", G_CALLBACK(_on_handle_ring_alarm), NULL);

    loop = g_main_loop_new(NULL, FALSE);
    g_main_loop_run(loop);

}

