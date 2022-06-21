/*
 * dbus_server.h
 *
 *  Created on: Jun 8, 2022
 *      Author: anita
 */

#ifndef TRAINING_APPLICATION_INCLUDE_DBUS_SERVER_H_
#define TRAINING_APPLICATION_INCLUDE_DBUS_SERVER_H_

#include <stdint.h>
#include <gio/gio.h>

int DBUS_Server(void);

void* DBUS_Server_MethodsProc(gpointer data);

#endif /* TRAINING_APPLICATION_INCLUDE_DBUS_SERVER_H_ */
