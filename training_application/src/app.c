#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <ctype.h>

#include <glib.h>
#include <gio/gio.h>
#include <sys/wait.h>

#include "dbus_server.h"

int cnt = 0;

GMutex lock;
GMainLoop *loop;

/**
 * @brief Function used for threads, that increments the global variable cnt
 */
static void* _funcIncrement(gpointer data)
{
    g_mutex_lock(&lock);

    cnt++;
    printf("Counter %d\n", cnt);

    g_mutex_unlock(&lock);

    return NULL;
}

/**
 * @brief Removes the digits for the input string
 */
static char* _removeDigits(char *input)
{
    char *dest = input;
    char *src = input;

    while (*src)
    {
        if (isdigit(*src))
        {
            src++;
            continue;
        }
        *dest++ = *src++;
    }
    *dest = '\0';

    return input;
}

/**
 * @brief Counts the appearance of the input sub string in the input string
 */
static int _countSubString(const char *input, const char *subString)
{
    int cnt = 0;
    const char *tmp = input;

    while ((tmp = strstr(tmp, subString)) != NULL)
    {
        cnt++;
        tmp++;
    }

    return cnt;
}

/**
 * @brief Function that handles the Client Part
 */
static int _funcClient(void)
{
    GError *err = NULL;
    GSocketConnection *connection = NULL;

    // Create TCP/IP socket
    GSocketClient *client = g_socket_client_new();

    // Connect to the host
    connection = g_socket_client_connect_to_host(client, (char*) "localhost", 80, NULL, &err);

    if (err != NULL)
    {
        printf("Failed to connect to the host\n");
    }

    GInputStream *istream = g_io_stream_get_input_stream(G_IO_STREAM(connection));
    GOutputStream *ostream = g_io_stream_get_output_stream(G_IO_STREAM(connection));

    const char str[] = "Hello1234 upd, 10upd!\0";

    g_output_stream_write(ostream, str, strlen(str) + 1, NULL, &err);

    if (err != NULL)
    {
        printf("Error received: %s\n", err->message);
    }

    char message[128];
    g_input_stream_read(istream, message, 128, NULL, NULL);

    printf("Client received message: \"%s\"\n", message);

    // Wait to be sure the parent(this process) doesn’t end before the child process
    wait(NULL);

    return 0;
}

/**
 * @brief Callback for the Server
 */
bool incoming_callback(GSocketService *service, GSocketConnection *connection, GObject *source_object,
        gpointer user_data)
{
    printf("Received Connection from client!\n");

    GInputStream *istream = g_io_stream_get_input_stream(G_IO_STREAM(connection));
    GOutputStream *ostream = g_io_stream_get_output_stream(G_IO_STREAM(connection));

    char message[128];
    g_input_stream_read(istream, message, 80, NULL, NULL);
    printf("Server received message: \"%s\"\n", message);

    // Parse the string to eliminate the digits
    char *parsedStr = _removeDigits(message);

    // Count the number of word 'upd' in the string and concatenate this string to the previous one
    snprintf(parsedStr + strlen(parsedStr), 50, "Number of word 'upd' appearance in the string: %d",
            _countSubString(message, "upd"));

    GError *err = NULL;
    g_output_stream_write(ostream, parsedStr, strlen(parsedStr), NULL, &err);

    if (err != NULL)
    {
        printf("Error received: %s\n", err->message);
    }

    printf("Quit main loop\n");
    g_main_loop_quit(loop);

    return false;
}

/**
 * @brief Function that handles the Server Part
 */
static int _funcServer(void)
{
    GError *err = NULL;

    /* Create new socket service */
    GSocketService *service = g_socket_service_new();

    /* Connect to the port */
    g_socket_listener_add_inet_port((GSocketListener*) service, 80,
    NULL, &err);

    if (err != NULL)
    {
        printf("Error received: %s\n", err->message);
    }

    /* Listen to the 'incoming' signal */
    g_signal_connect(service, "incoming", G_CALLBACK (incoming_callback), NULL);

    /* Start the socket service */
    g_socket_service_start(service);

    /* Enter main loop */
    printf("Waiting for client!\n");
    loop = g_main_loop_new(NULL, FALSE);
    g_main_loop_run(loop);

    return 0;
}

int main(void)
{
    DBUS_Server();

    return 0;
}

