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
static char *device = "hw:1,0,0";

/*
   calculate checksum
   array - the command to set over usb
   length - length of array
   check - position of checksum byte in array
*/
char calculate_checksum(gchar *array, int length, int check)
{
    int x;
    char checksum;

    checksum = 0x07;

    for (x = 0; x<length; x++) {
        if (x == check) continue;
        checksum ^= array[x];
    }

    return checksum;
}

static char calc_checksum(gchar *array, gint length)
{
    int x;
    int checksum = 0;

    for (x = 0; x<length; x++) {
        checksum ^= array[x];
    }

    return checksum;
}

/*
   opens MIDI device
   Returns TRUE on error
*/
gboolean open_device()
{
    int err;

    err = snd_rawmidi_open(&input, &output, device, SND_RAWMIDI_NONBLOCK);
    if (err) {
        fprintf(stderr, "snd_rawmidi_open %s failed: %d\n", device, err);
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

void send_data(char *data, int length)
{
    if (output == NULL)
        open_device();

    snd_rawmidi_write(output, data, length);
}

/*
   reads data from MIDI IN
   returns GString containing data
   if no data was read it returns NULL
*/
GString* read_data()
{
    /* This is mostly taken straight from alsa-utils-1.0.19 amidi/amidi.c
       by Clemens Ladisch <clemens@ladisch.de> */
    int err;
    int npfds;
    gboolean stop = FALSE;
    struct pollfd *pfds;
    GString *string = NULL;

    npfds = snd_rawmidi_poll_descriptors_count(input);
    pfds = alloca(npfds * sizeof(struct pollfd));
    snd_rawmidi_poll_descriptors(input, pfds, npfds);

    do {
        char buf[20];
        int i, length;
        unsigned short revents;

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

        if ((unsigned char)buf[length-1] == 0xF7)
            stop = TRUE;

        if (length != 0) {
            if (string == NULL) {
                string = g_string_new_len(buf, length);
            } else {
                string = g_string_append_len(string, buf, length);
            }
        }
    } while ((err != 0) && (stop == FALSE));

    return string;
}

static void clear_midi_in_buffer()
{
    GString *str;

    do {
        str = read_data();
    } while (str != NULL);
}

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

/*
   data - unpacked data to send
   len  - data length
*/
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
                           calc_checksum(&msg->str[1], msg->len - 1));

    send_data(msg->str, msg->len);

    g_string_free(msg, TRUE);
}

static gint get_message_id(GString *msg)
{
    if (msg->len > 7) {
        return (unsigned char)msg->str[7];
    }
    return -1;
}

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

/*
   id - ID as found in preset file
   position - Position as found in preset file
   value - Value as found in preset file
*/
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

/* x = 0 to 60 */
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

/* level = 0 to 99 */
void set_preset_level(int level)
{
    set_option(PRESET_LEVEL, PRESET_POSITION, level);
}

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

/* x = 0 to 59 (preset number) */
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

/*
    Queries user preset names
    Valid bank values are PRESETS_SYSTEM and PRESETS_USER
    Returns GStrv which must be freed with g_strfreev
    Returns NULL on error
*/
GStrv query_preset_names(gchar bank)
{
    GString *data = NULL;
    int x;                    /* used to iterate over whole reply */
    int n = 0;                /* current preset number */
    int n_total;              /* total number of presets */
    gchar **str_array = NULL;

    /* clear MIDI IN buffer */
    clear_midi_in_buffer();

    /* query user preset names */
    send_message(REQUEST_PRESET_NAMES, &bank, 1);

    /* read reply */
    do {
        if (data)
            g_string_free(data, TRUE);
        data = read_data();
    } while (get_message_id(data) != RECEIVE_PRESET_NAMES);

    if (data != NULL) {
        char preset_reply_magic[] = {0xF0, 0x00, 0x00, 0x10, 0x00, 0x5E, 0x02, 0x22, 0x00, 0x01};
        if (strncmp(data->str, preset_reply_magic, sizeof(preset_reply_magic)) == 0) {
            char buf[10]; /* temporary, used to read single preset name */
            int b = 0;    /* used to iterate over buf */

            if (data->len >= 10) {
                n_total = data->str[10];
                str_array = g_new(gchar*, n_total + 1);
                str_array[n_total] = NULL;
            }

            for (x=11; ((x<data->len) && (n<n_total)); x++) {
                if ((x % 8) == 0) /* every 8th byte is 0x00 */
                    continue;

                if (data->str[x] == 0xF7) /* every message ends with 0xF7 */
                    break;

                if (data->str[x] == 0) { /* presets are splitted with 0x00 */
                    gchar *name;

                    name = g_new(gchar, b+1);
                    strncpy(name, buf, b);
                    name[b] = 0;
                    if (n < n_total)
                        str_array[n] = name;

                    b = 0;
                    n++;
                } else {
                    if (b < 10) {
                        buf[b] = data->str[x];
                        b++;
                    } else {
                        g_message("Preset name seems to be longer than 10 chars!");
                    }
                }
            }
        }
        g_string_free(data, TRUE);
    }
    return str_array;
}

static void unpack_message(GString *msg)
{
    int offset;
    int x;
    int i;
    unsigned char status;
    gboolean finish = FALSE;

    offset = 9;
    x = 0;
    i = 8;

    do {
        status = (unsigned char)msg->str[offset-1];
        for (x=0; x<7; x++) {
            if ((unsigned char)msg->str[offset+x] == 0xF7) {
                msg->str[i] = 0xF7;
                finish = TRUE;
            }

            msg->str[i] = (((status << (x+1)) & 0x80) | (unsigned char)msg->str[x+offset]);
            i++;
        }
        offset += 8;
    } while (finish == FALSE && offset < msg->len);
}

GString *get_current_preset()
{
    GString *data = NULL;

    /* clear MIDI IN buffer */
    clear_midi_in_buffer();

    send_message(REQUEST_PRESET, "\x04\x00", 3);

    /* read reply */
    data = read_data();
    g_string_free(data, TRUE);

    data = read_data();

    unpack_message(data);

    return data;
}

static gboolean request_who_am_i(unsigned char *device_id, unsigned char *family_id,
                                 unsigned char *product_id)
{
    send_message(REQUEST_WHO_AM_I, NULL, 0);

    GString *data = read_data();
    if (data != NULL) {
        if ((data->len == 15) && (data->str[7] == RECEIVE_WHO_AM_I)) {
            *device_id = data->str[9];
            *family_id = data->str[10];
            *product_id = data->str[11];
            g_string_free(data, TRUE);
            return TRUE;
        }
        g_string_free(data, TRUE);
    }
    return FALSE;
}

static GOptionEntry options[] = {
    {"device", 'd', G_OPTION_FLAG_IN_MAIN, G_OPTION_ARG_STRING, &device, "MIDI device port to use", NULL},
    {NULL}
};

int main(int argc, char *argv[]) {
    GError *error = NULL;
    GOptionContext *context;
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
        if (request_who_am_i(&device_id, &family_id, &product_id) == FALSE) {
            show_error_message(NULL, "No suitable reply from device - is it connected?");
        } else {
            create_window();
            gtk_main();
        }
    }

    if (output != NULL)
        snd_rawmidi_close(output);
    if (input != NULL)
        snd_rawmidi_close(input);

    return EXIT_SUCCESS;
}
