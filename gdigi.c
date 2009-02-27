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
    snd_rawmidi_drain(output);
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
            if (buf[i] != 0xFE) // ignore active sensing
                buf[length++] = buf[i];

        if (length != 0) {
            if (string == NULL) {
                string = g_string_new_len(buf, length);
            } else {
                string = g_string_append_len(string, buf, length);
            }
        }
     } while (err != 0);

     return string;
}

/*
   id - ID as found in preset file
   position - Position as found in preset file
   value - Value as found in preset file
*/
void set_option(guint id, guint position, guint value)
{
    static char option[] = {0xF0, 0x00, 0x00, 0x10, 0x00, 0x5E, 0x02, 0x41,
                            0x00, 0x00, 0x00, /* ID */
                            0x00, /* position */
                            0x00, /* value length (not necesarry) */
                            0x00, 0x00, 0x00, /* value (can be either 1, 2 or 3 bytes) */
                            0x00, /* checksum */ 0xF7};

    option[8] = ((id & 0x80) >> 2);

    option[9] = ((id & 0xFF00) >> 8);
    option[10] = ((id & 0x007F));

    option[11] = position;

    if (value < 0x80) {
        // [12] = value, [13] - checksum, [14] = 0xF7
        option[12] = value;

        option[14] = 0xF7;
        option[13] = calculate_checksum(option, 15, 13);

        send_data(option, 15);
    } else if (value <= 0xFF) {
        option[8] |= 0x08; // there'll be length before value
        if (((value & 0x80) >> 7) == 1)
            option[8] |= 0x04;

        option[12] = 0x01;
        // [13] = value, [14] - checksum, [15] = 0xF7
        option[13] = (value & 0x007F);

        option[15] = 0xF7;
        option[14] = calculate_checksum(option, 16, 14);

        send_data(option, 16);
    } else if (value <= 0xFFFF) {
        option[8] |= 0x08; // there'll be length before value
        if (((value & 0x80) >> 7) == 1)
            option[8] |= 0x02;

        option[12] = 0x02;

        option[13] = ((value & 0xFF00) >> 8);
        option[14] = ((value & 0x007F));

        option[16] = 0xF7;
        option[15] = calculate_checksum(option, 17, 15);

        send_data(option, 17);
    } else if (value <= 0xFFFFFF) {
        option[8] |= 0x08; // there'll be length before value
        if (((value & 0x80) >> 7) == 1)
            option[8] |= 0x01;

        option[12] = 0x03;

        option[13] = ((value & 0xFF0000) >> 16);
        option[14] = ((value & 0x00FF00) >> 8);
        option[15] = ((value & 0x00007F));

        option[17] = 0xF7;
        option[16] = calculate_checksum(option, 18, 16);

        send_data(option, 18);
    }
}

/* x = 0 to 60 */
void switch_user_preset(int x)
{
    static char switch_preset[] = {0x00, 0xF0, 0x00, 0x00, 0x10, 0x00, 0x5E, 0x02, 0x39, 0x00, 0x01 /* bank = user */, 0x00 /* no */, 0x04, 0x00, 0x00, 0x01, 0x00 /* confirm */, 0xF7};

    switch_preset[11] = x;
    switch_preset[16] = calculate_checksum(switch_preset, sizeof(switch_preset), 16);

    send_data(switch_preset, sizeof(switch_preset));
}

/* x = 0 to 60 */
void switch_system_preset(int x)
{
    static char switch_preset[] = {0x00, 0xF0, 0x00, 0x00, 0x10, 0x00, 0x5E, 0x02, 0x39, 0x00, 0x00 /* bank = system */, 0x00 /* no */, 0x04, 0x00, 0x00, 0x01, 0x00 /* confirm */, 0xF7};

    switch_preset[11] = x;
    switch_preset[16] = calculate_checksum(switch_preset, sizeof(switch_preset), 16);

    send_data(switch_preset, sizeof(switch_preset));
}

/* level = 0 to 99 */
void set_preset_level(int level)
{
    set_option(PRESET_LEVEL, PRESET_POSITION, level);
}

/* x = 0 to 59 (preset number) */
void set_preset_name(int x, gchar *name)
{
    static char set_name[] = {0x00, 0xF0, 0x00, 0x00, 0x10, 0x00, 0x5E, 0x02, 0x29, 0x00, 0x01, 0x00 /* preset no */, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    set_name[11] = x;

    int a;
    int b;
    b = 0;
    for (a=0; (name != NULL && a<strlen(name)) && a<10 ; a++) {
        if (a == 5) {
            set_name[12+a] = 0x00;
            b++;
        }
        set_name[12+a+b] = name[a];
    }

    if (a == 5)
        a = 4;
    else
        set_name[12+a+b] = 0x00;

    set_name[12+a+2+b] = 0xF7;
    set_name[12+a+1+b] = calculate_checksum(set_name, 12+a+3+b, 12+a+1+b);

    send_data(set_name, 13+a+3+b);
}

/*
    Queries user preset names
    Valid bank values are PRESETS_SYSTEM and PRESETS_USER
    Returns GStrv which must be freed with g_strfreev
    Returns NULL on error
*/
GStrv query_preset_names(PresetBank bank)
{
    GString *data = NULL;
    int x;                    /* used to iterate over whole reply */
    int n = 0;                /* current preset number */
    int n_total;              /* total number of presets */
    gchar **str_array = NULL;

    /* clear MIDI IN buffer */
    data = read_data();
    if (data != NULL)
        g_string_free(data, TRUE);

    /* query user preset names */
    char command[] = {0xF0, 0x00, 0x00, 0x10, 0x00, 0x5E, 0x02, 0x21, 0x00, 0x00 /* bank */, 0x00 /* checksum */, 0xF7};
    command[9] = bank;
    command[10] = calculate_checksum(command, sizeof(command), 10);
    send_data(command, sizeof(command));

    /* read reply */
    data = read_data();

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

            for (x=11; x<data->len; x++) {
                if ((x % 8) == 0) // every 8th byte is 0x00
                    continue;

                if (data->str[x] == 0xF7) // every message ends with 0xF7
                    break;

                if (data->str[x] == 0) { // presets are splitted with 0x00
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
        GtkWidget *msg = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL,
                                                GTK_MESSAGE_ERROR,
                                                GTK_BUTTONS_OK,
                                                "Failed to open MIDI device");

        gtk_dialog_run(GTK_DIALOG(msg));
        gtk_widget_destroy(msg);
    } else {
        create_window();
        gtk_main();
    }

    if (output != NULL)
        snd_rawmidi_close(output);
    if (input != NULL)
        snd_rawmidi_close(input);

    return EXIT_SUCCESS;
}
