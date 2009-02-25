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
#include <string.h>
#include "gdigi.h"
#include "gui.h"

static snd_rawmidi_t *output;
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

    err = snd_rawmidi_open(NULL, &output, device, SND_RAWMIDI_NONBLOCK);
    if (err) {
        fprintf(stderr, "snd_rawmidi_open %s failed: %d\n", device, err);
        return TRUE;
    }

    err = snd_rawmidi_nonblock(output, 0);
    if (err) {
        fprintf(stderr, "snd_rawmidi_nonblock failed: %d\n", err);
        return TRUE;
    }
    return FALSE;
}

void send_data(char *data, int length)
{
    if (output == NULL)
        open_device();

    snd_rawmidi_write(output, data, length);
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

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    int c;
    while (1) {
        static struct option long_options[] = {
            {"device", required_argument, 0, 'd'},
            {0, 0, 0, 0}
        };
        int option_index = 0;
        c = getopt_long(argc, argv, "d:", long_options, &option_index);
        if (c == -1)
            break;

        switch(c) {
            case 'd':
                device = optarg;
                break;
            default:
                abort();
        }
    }

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

    return 0;
}
