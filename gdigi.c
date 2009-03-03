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

static u_char device_id = 0x7F;
static u_char family_id = 0x7F;
static u_char product_id = 0x7F;

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
            if (buf[i] != 0xFE) // ignore active sensing
                buf[length++] = buf[i];

        if ((u_char)buf[length-1] == 0xF7)
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

static void send_message(gint procedure, gchar *data, gint len)
{
    GString *msg = g_string_new_len("\xF0"          /* SysEx status byte */
                                    "\x00\x00\x10", /* Manufacturer ID   */
                                    4);
    g_string_append_printf(msg,
                           "%c%c%c"   /* device, family, product ID */
                           "%c",      /* procedure */
                           device_id, family_id, product_id,
                           procedure);

    if (len > 0)
        g_string_append_len(msg, data, len);

    g_string_append_printf(msg, "%c\xF7",
                           calc_checksum(&msg->str[1], msg->len - 1));

    send_data(msg->str, msg->len);

    g_string_free(msg, TRUE);
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
void switch_preset(guint bank, guint x)
{
    static char switch_preset[] = {0x00, 0xF0, 0x00, 0x00, 0x10, 0x00, 0x5E, 0x02, 0x39, 0x00, 0x00 /* bank */, 0x00 /* no */, 0x04, 0x00, 0x00, 0x01, 0x00 /* confirm */, 0xF7};

    switch_preset[10] = bank;
    switch_preset[11] = x;
    switch_preset[16] = calculate_checksum(switch_preset, sizeof(switch_preset), 16);

    send_data(switch_preset, sizeof(switch_preset));
}

/* level = 0 to 99 */
void set_preset_level(int level)
{
    set_option(PRESET_LEVEL, PRESET_POSITION, level);
}

void store_preset_name(int x, const gchar *name)
{
    static char set_name[] = {0xF0, 0x00, 0x00, 0x10, 0x00, 0x5e, 0x02, 0x39, 0x00, 0x04, 0x00, 0x01, 0x00 /* preset number */, 0x00 /* name starts here */, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    set_name[12] = x;

    int a;
    int b;
    b = 0;
    for (a=0; (name != NULL && a<strlen(name)) && a<10 ; a++) {
        if (a == 3) {
            set_name[13+a] = 0x00;
            b++;
        }
        set_name[13+a+b] = name[a];
    }

    if (a == 3) {
        set_name[13+a+1+b] = 0x00;
        a++;
    } else
        set_name[13+a+b] = 0x00;

    set_name[13+a+1+b] = 0x00;
    set_name[13+a+3+b] = 0xF7;
    set_name[13+a+2+b] = calculate_checksum(set_name, 13+a+4+b, 13+a+2+b);

    send_data(set_name, 14+a+3+b);

    switch_preset(PRESETS_USER, x);
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
GStrv query_preset_names(guint bank)
{
    GString *data = NULL;
    int x;                    /* used to iterate over whole reply */
    int n = 0;                /* current preset number */
    int n_total;              /* total number of presets */
    gchar **str_array = NULL;

    /* clear MIDI IN buffer */
    clear_midi_in_buffer();

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

            for (x=11; ((x<data->len) && (n<n_total)); x++) {
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

static void unpack_message(GString *msg)
{
    int offset;
    int x;
    int i;
    u_char status;
    gboolean finish = FALSE;

    offset = 9;
    x = 0;
    i = 8;

    do {
        printf("status byte [%d] 0x%02x\n", offset-1, msg->str[offset-1]);
        status = (u_char)msg->str[offset-1];
        for (x=0; x<7; x++) {
            if ((u_char)msg->str[offset+x] == 0xF7) {
                msg->str[i] = 0xF7;
                finish = TRUE;
            }

            msg->str[i] = (((status << (x+1)) & 0x80) | (u_char)msg->str[x+offset]);
            printf("merging byte [%d] with byte [%d] MSB is %d\n",
                   i, x+offset, (((status << (x+1)) & 0x80) >> 7));
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

    send_message(REQUEST_PRESET, "\x00\x04\x00", 3);

    /* read reply */
    data = read_data();
    g_string_free(data, TRUE);

    data = read_data();

    unpack_message(data);

    return data;
}

static gboolean request_who_am_i(u_char *device_id, u_char *family_id,
                                 u_char *product_id)
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
