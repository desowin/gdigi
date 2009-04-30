/*
 *  Copyright (c) 2009 Tomasz Moń <desowin@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; under version 3 of the License.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses>.
 */

#include <stdio.h>
#include <gtk/gtk.h>
#include <getopt.h>
#include <alsa/asoundlib.h>
#include <alloca.h>
#include "gdigi.h"
#include "gui.h"

static unsigned char device_id = 0x7F;
static unsigned char family_id = 0x7F;
static unsigned char product_id = 0x7F;

static snd_rawmidi_t *output = NULL;
static snd_rawmidi_t *input = NULL;
static char *device_port = "hw:1,0,0";

static GQueue *message_queue = NULL;
static GMutex *message_queue_mutex = NULL;

/**
 *  \param array data to calculate checksum
 *  \param length data length
 *
 *  Calculates message checksum.
 *
 *  \return calculated checksum.
 **/
static char calculate_checksum(gchar *array, gint length)
{
    int x;
    int checksum = 0;

    for (x = 0; x<length; x++) {
        checksum ^= array[x];
    }

    return checksum;
}

/**
 *  Opens MIDI device. This function modifies global input and output variables.
 *
 *  \return FALSE on success, TRUE on error.
 **/
gboolean open_device()
{
    int err;

    err = snd_rawmidi_open(&input, &output, device_port, SND_RAWMIDI_SYNC);
    if (err) {
        fprintf(stderr, "snd_rawmidi_open %s failed: %d\n", device_port, err);
        return TRUE;
    }

    err = snd_rawmidi_nonblock(output, 0);
    if (err) {
        fprintf(stderr, "snd_rawmidi_nonblock failed: %d\n", err);
        return TRUE;
    }

    snd_rawmidi_read(input, NULL, 0); /* trigger reading */

    return FALSE;
}

/**
 *  \param data data to be sent
 *  \param length data length
 *
 *  Sends data to device. This function uses global output variable.
 **/
void send_data(char *data, int length)
{
    snd_rawmidi_write(output, data, length);
}

/**
 *  \param data data to be packed
 *  \param len data length
 *
 *  Packs data using method used on all newer DigiTech products.
 *
 *  \return GString containing packed data
 **/
GString *pack_data(gchar *data, gint len)
{
    GString *packed;
    gint i;
    gint new_len;
    unsigned char status;
    gint offset;
    gint status_byte;

    new_len = len + (len/7);
    packed = g_string_sized_new(new_len);
    status = 0;
    offset = -1;
    status_byte = 0;

    for (i=0; i<len; i++) {
        if ((i % 7) == 0) {
            packed->str[status_byte] = status;
            status = 0;
            status_byte = packed->len;
            g_string_append_c(packed, '\0');
        }
        g_string_append_c(packed, (data[i] & 0x7F));
        status |= (data[i] & 0x80) >> ((i%7) + 1);
    }
    packed->str[status_byte] = status;

    return packed;
}

/**
 *  \param msg message to unpack
 *
 *  Unpacks message data. This function modifies given GString.
 **/
static void unpack_message(GString *msg)
{
    int offset;
    int x;
    int i;
    unsigned char status;
    unsigned char *str;
    gboolean stop = FALSE;

    g_return_if_fail(msg != NULL);
    g_return_if_fail(msg->len > 9);

    offset = 1;
    x = 0;
    i = 8;

    str = (unsigned char*)msg->str;
    do {
        offset += 8;
        status = str[offset-1];
        for (x=0; x<7; x++) {
            if (offset+x >= msg->len) {
                stop = TRUE;
                break;
            }
            if (str[offset+x] == 0xF7) {
                str[i] = 0xF7;
                stop = TRUE;
            }

            str[i] = (((status << (x+1)) & 0x80) | str[x+offset]);
            i++;
        }
    } while (!stop && (offset+x < msg->len));

    g_string_truncate(msg, i);
}

/**
 *  \param msg SysEx message
 *
 *  Checks message ID.
 *
 *  \return MessageID, or -1 on error.
 **/
static MessageID get_message_id(GString *msg)
{
    /** \todo check if msg is valid SysEx message */
    g_return_val_if_fail(msg != NULL, -1);
    g_return_val_if_fail(msg->str != NULL, -1);

    if (msg->len > 7) {
        return (unsigned char)msg->str[7];
    }
    return -1;
}

void push_message(GString *msg)
{
    if (((unsigned char)msg->str[0] == 0xF0) && ((unsigned char)msg->str[msg->len-1] == 0xF7))
        g_message("Pushing correct message!");
    else
        g_warning("Pushing incorrect message!");

    int x;
    for (x = 0; x<msg->len; x++)
        printf("%02x ", (unsigned char)msg->str[x]);
    printf("\n");

    if (get_message_id(msg) == ACK) {
        g_message("Received ACK");
        g_string_free(msg, TRUE);
        return;
    }

    if (get_message_id(msg) == NACK) {
        g_message("Received NACK");
        g_string_free(msg, TRUE);
        return;
    }

    g_mutex_lock(message_queue_mutex);
    g_queue_push_tail(message_queue, msg);
    g_mutex_unlock(message_queue_mutex);
}

gpointer read_data_thread(gboolean *stop)
{
    /* This is mostly taken straight from alsa-utils-1.0.19 amidi/amidi.c
       by Clemens Ladisch <clemens@ladisch.de> */
    int err;
    int npfds;
    struct pollfd *pfds;
    GString *string = NULL;

    npfds = snd_rawmidi_poll_descriptors_count(input);
    pfds = alloca(npfds * sizeof(struct pollfd));
    snd_rawmidi_poll_descriptors(input, pfds, npfds);

    do {
        unsigned char buf[256];
        int i, length;
        unsigned short revents;

        /* SysEx messages can't contain bytes with 8th bit set.
           memset our buffer to 0xFF, so if for some reason we'll get out of reply bounds, we'll catch it */
        memset(buf, sizeof(buf), 0xFF);

        err = poll(pfds, npfds, 200);
        if (err < 0 && errno == EINTR)
            break;
        if (err < 0) {
            g_error("poll failed: %s", strerror(errno));
            break;
        }
        if ((err = snd_rawmidi_poll_descriptors_revents(input, pfds, npfds, &revents)) < 0) {
            g_error("cannot get poll events: %s", snd_strerror(errno));
            break;
        }
        if (revents & (POLLERR | POLLHUP))
            break;
        if (!(revents & POLLIN))
            continue;

        err = snd_rawmidi_read(input, buf, sizeof(buf));
        if (err == -EAGAIN)
            continue;
        if (err < 0) {
            g_error("cannot read: %s", snd_strerror(err));
            break;
        }

        length = 0;
        for (i = 0; i < err; ++i)
            if (buf[i] != 0xFE) /* ignore active sensing */
                buf[length++] = buf[i];

        i = 0;

        while (i < length) {
            int pos;
            int bytes;

            pos = i;

            for (bytes = 0; (bytes<length-i) && (buf[i+bytes] != 0xF7); bytes++);

            if (buf[i+bytes] == 0xF7) bytes++;

            i += bytes;

            if (string == NULL)
                string = g_string_new_len((gchar*)&buf[pos], bytes);
            else
                g_string_append_len(string, (gchar*)&buf[pos], bytes);

            if ((unsigned char)string->str[string->len-1] == 0xF7) {
                /* push message on stack */
                push_message(string);
                string = NULL;
            }
        }
    } while (*stop == FALSE);

    if (string) {
        g_string_free(string, TRUE);
        string = NULL;
    }

    return NULL;
}

/**
 *  \param procedure procedure ID
 *  \param data unpacked message data
 *  \param len data length
 *
 *  Creates SysEx message then sends it. This function uses folowing global variables: device_id, family_id and product_id.
 **/
void send_message(gint procedure, gchar *data, gint len)
{
    GString *msg = g_string_new_len("\xF0"          /* SysEx status byte */
                                    "\x00\x00\x10", /* Manufacturer ID   */
                                    4);
    g_string_append_printf(msg,
                           "%c%c%c"   /* device, family, product ID */
                           "%c",      /* procedure */
                           device_id, family_id, product_id,
                           procedure);

    if (len > 0) {
        GString *tmp = pack_data(data, len);
        g_string_append_len(msg, tmp->str, tmp->len);
        g_string_free(tmp, TRUE);
    }

    g_string_append_printf(msg, "%c\xF7",
                           calculate_checksum(&msg->str[1], msg->len - 1));

    send_data(msg->str, msg->len);

    g_string_free(msg, TRUE);
}

/**
 *  \param id MessageID of requested message
 *
 *  Reads data from message queue until message with matching id is found.
 *
 *  \return GString containing unpacked message.
 **/
GString *get_message_by_id(MessageID id)
{
    GString *data = NULL;
    guint x, len;
    gboolean found = FALSE;

    do {
        g_mutex_lock(message_queue_mutex);

        len = g_queue_get_length(message_queue);
        for (x = 0; x<len; x++) {
            data = g_queue_peek_nth(message_queue, x);
            if (get_message_id(data) == id) {
                found = TRUE;
                g_queue_pop_nth(message_queue, x);
                break;
            }
        }
        g_mutex_unlock(message_queue_mutex);
    } while (found == FALSE);

    unpack_message(data);

    return data;
}

/**
 *  \param msg message to append value
 *  \param value value to append
 *
 *  Packs value using scheme used on all newer DigiTech products.
 **/
void append_value(GString *msg, guint value)
{
    /* check how many bytes long the value is */
    guint temp = value;
    gint n = 0;
    do {
        n++;
        temp = temp >> 8;
    } while (temp);

    if (n == 1) {
        if (value & 0x80)
            n = 2;
        else
            g_string_append_printf(msg, "%c", value);
    }

    if (n > 1) {
        gint x;
        g_string_append_printf(msg, "%c", (n | 0x80));
        for (x=0; x<n; x++) {
            g_string_append_printf(msg, "%c",
                                   ((value >> (8*(n-x-1))) & 0xFF));
        }
    }
}

/**
 *  \param id Parameter ID
 *  \param position Parameter position
 *  \param value Parameter value
 *
 *  Forms SysEx message to set parameter then sends it to device.
 **/
void set_option(guint id, guint position, guint value)
{
    GString *msg = g_string_sized_new(9);
    g_string_append_printf(msg, "%c%c%c",
                           ((id & 0xFF00) >> 8), (id & 0xFF),
                           position);
    append_value(msg, value);
    send_message(RECEIVE_PARAMETER_VALUE, msg->str, msg->len);
    g_string_free(msg, TRUE);
}

/**
 *  \param bank preset bank
 *  \param x preset index
 *
 *  Switches to selected preset.
 **/
void switch_preset(guint bank, guint x)
{
    GString *msg = g_string_sized_new(6);
    g_string_append_printf(msg, "%c%c%c%c%c%c",
                           bank, x,                /* source */
                           PRESETS_EDIT_BUFFER, 0, /* destination */
                           0,                      /* keep existing name */
                           1);                     /* load */
    send_message(MOVE_PRESET, msg->str, msg->len);
    g_string_free(msg, TRUE);
}

/**
 *  \param x preset index
 *  \param name preset name
 *
 *  Stores current edit buffer in user presets bank.
 **/
void store_preset_name(int x, const gchar *name)
{
    GString *msg = g_string_sized_new(6);
    g_string_append_printf(msg, "%c%c%c%c%s%c%c",
                           PRESETS_EDIT_BUFFER, 0, /* source */
                           PRESETS_USER, x,        /* destination */
                           name, 0,                /* name */
                           1);                     /* load */
    send_message(MOVE_PRESET, msg->str, msg->len);
    g_string_free(msg, TRUE);
}

/**
 *  \param x preset index
 *  \param name preset name
 *
 *  Sets preset name.
 **/
void set_preset_name(int x, gchar *name)
{
    GString *msg = g_string_sized_new(12);
    g_string_append_printf(msg, "%c%c%s%c",
                           PRESETS_USER,    /* preset bank */
                           x,               /* preset index */
                           name, 0);        /* name */
    send_message(RECEIVE_PRESET_NAME, msg->str, msg->len);
    g_string_free(msg, TRUE);
}

/**
 *  \param bank preset bank
 *
 *  Queries preset names.
 *
 *  \return GStrv which must be freed with g_strfreev, or NULL on error.
 **/
GStrv query_preset_names(gchar bank)
{
    GString *data = NULL;
    int x;                    /* used to iterate over whole reply */
    int n = 0;                /* current preset number */
    int n_total;              /* total number of presets */
    gchar **str_array = NULL;

    /* query user preset names */
    send_message(REQUEST_PRESET_NAMES, &bank, 1);

    /* read reply */
    data = get_message_by_id(RECEIVE_PRESET_NAMES);

    if (data != NULL) {
        if (data->len >= 10) {
            n_total = data->str[9];
            str_array = g_new(gchar*, n_total + 1);
            str_array[n_total] = NULL;
        }

        for (x=10; ((x<data->len) && (n<n_total)); x++) {
            if (data->str[x] == 0xF7) /* every message ends with 0xF7 */
                break;

            str_array[n] = g_strdup(&data->str[x]);
            x += strlen(str_array[n]);
            n++;
        }
        g_string_free(data, TRUE);
    }
    return str_array;
}

/**
 *  Queries current edit buffer.
 *
 *  \return GString containing RECEIVE_PRESET_PARAMETERS SysEx message.
 **/
GString *get_current_preset()
{
    GString *data = NULL;

    send_message(REQUEST_PRESET, "\x04\x00", 2);

    /* read reply */
    data = get_message_by_id(RECEIVE_PRESET_PARAMETERS);

    return data;
}

/**
 *  \param device_id Variable to hold device ID
 *  \param family_id Variable to hold family ID
 *  \param product_id Variable to hold product ID
 *
 *  Requests device information.
 *
 *  \return TRUE on success, FALSE on error.
 **/
static gboolean request_who_am_i(unsigned char *device_id, unsigned char *family_id,
                                 unsigned char *product_id)
{
    send_message(REQUEST_WHO_AM_I, NULL, 0);

    GString *data = get_message_by_id(RECEIVE_WHO_AM_I);
    if (data != NULL) {
        if (data->len == 14) {
            *device_id = data->str[8];
            *family_id = data->str[9];
            *product_id = data->str[10];
            g_string_free(data, TRUE);
            return TRUE;
        }
        g_string_free(data, TRUE);
    }
    return FALSE;
}

static void request_device_configuration()
{
    gint os_major, os_minor;
    gint cpu_major, cpu_minor;
    gint protocol_version;
    gint current_bank, current_preset;
    gint media_card;

    send_message(REQUEST_DEVICE_CONFIGURATION, NULL, 0);

    GString *data = get_message_by_id(RECEIVE_DEVICE_CONFIGURATION);

    if (data->len > 14) {
        os_major = data->str[8];
        os_minor = (((data->str[9] & 0xF0) >> 4) * 10) |
                   (data->str[9] & 0x0F);
        g_message("OS version: %d.%d", os_major, os_minor);

        cpu_major = data->str[10];
        cpu_minor = data->str[11];
        g_message("CPU version: %d.%d", cpu_major, cpu_minor);

        protocol_version = data->str[12];
        g_message("Protocol version: %d", protocol_version);

        current_bank = data->str[13];
        current_preset = data->str[14];
        g_message("Active bank: %d", current_bank);
        g_message("Active preset: %d", current_preset);

        if (os_major >= 1) {
            media_card = data->str[15];
            g_message("Media card present: %d", media_card);
        }
    }
}

#ifndef DOXYGEN_SHOULD_SKIP_THIS

static GOptionEntry options[] = {
    {"device", 'd', G_OPTION_FLAG_IN_MAIN, G_OPTION_ARG_STRING, &device_port, "MIDI device port to use", NULL},
    {NULL}
};

#endif /* DOXYGEN_SHOULD_SKIP_THIS */

static void message_queue_free_func(GString *msg, gpointer user_data)
{
    g_string_free(msg, TRUE);
}

int main(int argc, char *argv[]) {
    GError *error = NULL;
    GOptionContext *context;
    static gboolean stop_read_thread = FALSE;
    GThread *read_thread = NULL;

    g_thread_init(NULL);

    context = g_option_context_new(NULL);
    g_option_context_add_main_entries(context, options, NULL);
    g_option_context_add_group(context, gtk_get_option_group(TRUE));

    if (!g_option_context_parse(context, &argc, &argv, &error)) {
        g_message("option parsing failed: %s\n", error->message);
        g_error_free(error);
        g_option_context_free(context);
        exit(EXIT_FAILURE);
    }

    g_option_context_free(context);

    if (open_device() == TRUE) {
        show_error_message(NULL, "Failed to open MIDI device");
    } else {
        message_queue = g_queue_new();
        message_queue_mutex = g_mutex_new();
        read_thread = g_thread_create((GThreadFunc)read_data_thread,
                                      &stop_read_thread,
                                      TRUE, NULL);

        if (request_who_am_i(&device_id, &family_id, &product_id) == FALSE) {
            show_error_message(NULL, "No suitable reply from device");
        } else {
            Device *device = NULL;

            if (get_device_info(device_id, family_id, product_id, &device) == FALSE) {
                if (unsupported_device_dialog(&device) == FALSE) {
                    g_message("Shutting down");
                }
            }

            if (device != NULL) {
                gui_create(device);
                gtk_main();
                gui_free();
            }
        }
    }

    if (read_thread != NULL) {
        stop_read_thread = TRUE;
        g_thread_join(read_thread);
    }

    if (message_queue_mutex != NULL) {
        g_mutex_free(message_queue_mutex);
    }

    if (message_queue != NULL) {
        g_message("%d unread messages in queue",
                  g_queue_get_length(message_queue));
        g_queue_foreach(message_queue, (GFunc) message_queue_free_func, NULL);
        g_queue_free(message_queue);
    }

    if (output != NULL) {
        snd_rawmidi_drain(output);
        snd_rawmidi_close(output);
    }

    if (input != NULL) {
        snd_rawmidi_drain(input);
        snd_rawmidi_close(input);
    }

    return EXIT_SUCCESS;
}
