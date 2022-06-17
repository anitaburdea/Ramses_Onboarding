/*
 * dbus_client.h
 *
 *  Created on: Jun 14, 2022
 *      Author: anita
 */

#ifndef TRAINING_APPLICATION_INCLUDE_DBUS_CLIENT_H_
#define TRAINING_APPLICATION_INCLUDE_DBUS_CLIENT_H_

#include <stdint.h>

void DBUS_Client_SetTime(int hour, int minutes);

void DBUS_Client_SetAlarmTime(int alarmHour, int alarmMinutes);

void DBUS_Client_SetAlarmStatus(const char *alarmStatus);

void DBUS_Client_GetAlarmStatus(char *alarmStatus);

int DBUS_Client_Init(const char *dbusServerBusName, const char *dbusObjPath);

void DBUS_Client_SignalConnect(void);

void DBUS_Client_SetCBRegister(void (*alarmCallback)(void));

#endif /* TRAINING_APPLICATION_INCLUDE_DBUS_CLIENT_H_ */
