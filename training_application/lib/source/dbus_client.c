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

void DBUS_Client_SetTime(int hour, int minutes)
{
    GError *err = NULL;
    printf("Method set time\n");

    training_application__call_set_time_sync(proxy, hour, minutes, NULL, &err);
}

void DBUS_Client_SetAlarmTime(int alarmHour, int alarmMinutes)
{
    GError *err = NULL;
    printf("Method set alarm time\n");

    training_application__call_set_alarm_time_sync(proxy, alarmHour, alarmMinutes, NULL, &err);
}

void DBUS_Client_SetAlarmStatus(const char *alarmStatus)
{
    GError *err = NULL;
    printf("Method set alarm status\n");

    training_application__call_set_alarm_status_sync(proxy, alarmStatus, NULL, &err);
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

