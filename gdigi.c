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

#define TIMEOUT 1000

char buf[255];

static snd_rawmidi_t *output;
static char *device = "hw:1,0,0";

/*
static char magic[] = {0x04, 0xF0, 0x00, 0x00, 0x04, 0x10, 0x7F, 0x7F, 0x04, 0x7F, 0x01, 0x00, 0x04, 0x00, 0x00, 0x00, 0x06, 0x6E, 0xF7, 0x00}; // causes COMAND to be displayed on device?
static char presets_user[] =   {0x04, 0xF0, 0x00, 0x00, 0x04, 0x10, 0x00, 0x5E, 0x04, 0x02, 0x21, 0x00, 0x07, 0x01, 0x6C, 0xF7}; // this command seems to query user presets?
static char presets_system[] = {0x04, 0xF0, 0x00, 0x00, 0x04, 0x10, 0x00, 0x5E, 0x04, 0x02, 0x21, 0x00, 0x07, 0x00, 0x6D, 0xF7}; // this command seems to query system presets?

static char magic4[] = {0x04, 0xF0, 0x00, 0x00, 0x04, 0x10, 0x00, 0x5E, 0x04, 0x02, 0x2B, 0x00, 0x04, 0x00, 0x01, 0x77, 0x05, 0xF7, 0x00, 0x00};
static char magic5[] = {0x04, 0xF0, 0x00, 0x00, 0x04, 0x10, 0x00, 0x5E, 0x04, 0x02, 0x3A, 0x00, 0x04, 0x00, 0x01, 0x77, 0x05, 0xF7, 0x00, 0x00};


// xedit calls those after calling hbsc pickup change
static char magic12[] = {0x04, 0xF0, 0x00, 0x00, 0x04, 0x10, 0x00, 0x5E, 0x04, 0x02, 0x3A, 0x00, 0x04, 0x00, 0x01, 0x77, 0x05, 0xF7, 0x00, 0x00};
static char magic13[] = {0x07, 0x41, 0x73, 0xF7, 0x04, 0x10, 0x00, 0x5E, 0x04, 0x02, 0x7E, 0x00, 0x04, 0x00, 0x01, 0x77, 0x05, 0xF7, 0x00, 0x00};

// xedit calls those after calling schb pickup change
static char magic22[] = {0x04, 0xF0, 0x00, 0x00, 0x04, 0x10, 0x00, 0x5E, 0x04, 0x02, 0x41, 0x00, 0x04, 0x00, 0x01, 0x77, 0x05, 0xF7, 0x00, 0x00};
static char magic23[] = {0x04, 0xF0, 0x00, 0x00, 0x04, 0x10, 0x00, 0x5E, 0x04, 0x02, 0x3A, 0x00, 0x04, 0x00, 0x01, 0x77, 0x05, 0xF7, 0x00, 0x00};
static char magic24[] = {0x04, 0xF0, 0x00, 0x00, 0x04, 0x10, 0x00, 0x5E, 0x04, 0x02, 0x3B, 0x00, 0x04, 0x00, 0x00, 0x00, 0x06, 0x6E, 0xF7, 0x00};

// seems to be called after changing dist type
static char magic8[] = {0x04, 0xF0, 0x00, 0x00, 0x04, 0x10, 0x00, 0x5E, 0x04, 0x02, 0x3A, 0x00, 0x04, 0x00, 0x01, 0x77, 0x05, 0xF7, 0x00, 0x00};
// X-edit also calls 2 different commands, but those seems to vary each time
*/

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
    checksum = 0;
    for (x = 0; x<length; x++) {
        if (x == check) continue;
        checksum ^= array[x];
    }
    return checksum;
}

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

void send_data(char *data, int lenght)
{
    if (output == NULL)
        open_device();

    snd_rawmidi_write(output, data, lenght);
}

void check_preset(struct usb_dev_handle *handle)
{
    /*static char magic3[] = {0x04, 0xF0, 0x00, 0x00, 0x04, 0x10, 0x00, 0x5E, 0x04, 0x02, 0x2A, 0x00, 0x04, 0x04, 0x00, 0x62, 0x05, 0xF7, 0x00, 0x00}; // seems to query active preset

    int i;
    i = usb_bulk_write(handle, 4, magic3, sizeof(magic3), TIMEOUT);

    // UGLY - have to figure out reply formatting
    do {
        i = read_device(handle, 12);
        if (i==12) {
            if (buf[0]==0x04 && buf[1]==0x20 && buf[2]==0x03 && buf[3]==0x14 && buf[4]==0x04 &&
                buf[6]==0x02 && buf[7]==0x20 && buf[8]==0x04 && buf[9]==0x04 && buf[10]==0x14)
                printf("Wah min: %d\nWah max: %d\n", buf[5], buf[11]);
            if (buf[0]==0x04 && buf[1]==0x00 && buf[2]==0x05 && buf[3]==0x03 && buf[4]==0x04 &&
                buf[5]==0x11 && buf[7]==0x00 && buf[8]==0x04 && buf[9]==0x41 && buf[10]==0x04)
                printf("Wah level: %d\nCompressor status (0-off, 1-on): %d\n", buf[6], buf[11]);
            if (buf[0]==0x04 && buf[1]==0x04 && buf[2]==0x11 && buf[4]==0x04 && buf[5]==0x00 &&
                buf[6]==0x53 && // we want to display this only for CS compressor
                //buf[6]==0x53 (for CS comp) buf[6]==0x51 (for digi comp)
                buf[7]==0x04 && buf[8]==0x04 && buf[10]==0x00 && buf[11]==0x52)
                printf("Compressor sustain: %d\nCompressor attack: %d\n", buf[3], buf[9]);
            if (buf[0]==0x04 && buf[1]==0x08 && buf[2]==0x04 && buf[4]==0x04 && buf[5]==0x09 && buf[6]==0x01 &&
                buf[7]==0x06 && buf[8]==0x04 && buf[9]==0x01 && buf[10]==0x09 && buf[11]==0x51) // CS comp
                printf("Compressor level: %d\n", buf[3]);
            if (buf[0]==0x04 && buf[1]==0x08 && buf[2]==0x04 && buf[4]==0x04 && buf[5]==0x00 && buf[6]==0x53 &&
                buf[7]==0x04 && buf[8]==0x04 && buf[9]==0x00 && buf[10]==0x09 && buf[11]==0x45) // digi comp
                printf("Compressor level: %d\n", buf[3]);
            if (buf[0]==0x04 && buf[1]==0x04 && buf[2]==0x11 && buf[4]==0x04 && buf[5]==0x00 &&
                buf[6]==0x51 && buf[7]==0x04 && buf[8]==0x04 && buf[10]==0x00 && buf[11]==0x52)
                printf("Compressor sustain: %d\nCompressor tone: %d\n", buf[3], buf[9]);
            if (buf[0]==0x04 && buf[1]==0x00 && buf[2]==0x4f && buf[3]==0x2A && buf[4]==0x04 && buf[5]==0x04 &&
                buf[6]==0x02 && buf[7]==0x00 && buf[8]==0x04 && buf[10]==0x00 && buf[11]==0x50)
                printf("Compressor (43-DigiComp 44-CS): %02x\n", buf[9]);
            if (buf[0]==0x04 && buf[1]==0x03 && buf[3]==0x00 && buf[4]==0x04 && buf[5]==0x00 && buf[6]==0x03 &&
                buf[7]==0x50 && buf[8]==0x04 && buf[9]==0x02 && buf[10]==0x00)
                printf("Wah status (0-off, 1-on): %d\nWah type (4-crywah, 5-fulrng, 6-clyde): %d\n", buf[2], buf[11]);
            if (buf[0]==0x04 && buf[1]==0x08 && buf[2]==0x04 && buf[4]==0x04 && buf[5]==0x00 &&
                buf[6]==0x53 && buf[7]==0x04 && buf[8]==0x04 && buf[10]==0x09 && buf[11]==0x45)
                printf("Compressor level: %d\nCompressor attack (X-Edit only for DigiComp): %d\n", buf[3], buf[9]);
        }
    } while (i > 0);*/
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
                            0x00, /* value */
                            0x00, /* checksum */
                            0xF7};

    if (((id & 0x80) >> 7) == 1) /* 8th bit equals 1 */
        option[8] = 0x20;
    else                         /* otherwise */
        option[8] = 0x00;

    option[9]  = ((id & 0xFF00) >> 8);
    option[10] = ((id & 0x007F));      /* 8th bit has to be zero */

    option[11] = position;
    option[12] = value;
    option[13] = calculate_checksum(option, sizeof(option), 13) ^ 0x07;

    send_data(option, sizeof(option));
}

/* level = 0 to 99 */
void set_wah_min(int level)
{
    set_option(WAH_MIN, WAH_POSITION_MIN_MAX, level);
}

/* level = 0 to 99 */
void set_wah_max(int level)
{
    set_option(WAH_MAX, WAH_POSITION_MIN_MAX, level);
}

/* level = 0 to 12 */
void set_wah_level(int level)
{
    set_option(WAH_LEVEL, WAH_POSITION, level);
}

void set_higher_option(gint id, gint position, int type)
{
    static char set_type[] = {0x00, 0xF0, 0x00, 0x00, 0x10, 0x00, 0x5E, 0x02, 0x41,
                              0x00, 0x00, 0x00, /* ID */
                              0x00, /* position */
                              0x00, /* value length */
                              0x00, 0x00, 0x00, /* type */
                              0x00, /* checksum */ 0xF7};

    set_type[9] = ((id & 0x80) >> 2);

    set_type[10] = ((id & 0xFF00) >> 8);
    set_type[11] = ((id & 0x007F));

    set_type[12] = position;

    if (type < 0x80) {
        // [13] = type, [14] - checksum, [15] = 0xF7
    } else if (type <= 0xFF) {
        set_type[9] |= 0x08; // there'll be length before value
        if (((type & 0x80) >> 7) == 1)
            set_type[9] |= 0x04;

        set_type[13] = 0x01;
        // [14] = type, [15] - checksum, [16] = 0xF7
        set_type[14] = (type & 0x007F);

        set_type[16] = 0xF7;
        set_type[15] = calculate_checksum(set_type, 17, 15) ^ 0x07;

        send_data(set_type, 17);
    } else if (type < 0xFFFF) {
        set_type[9] |= 0x08; // there'll be length before value
        if (((type & 0x80) >> 7) == 1)
            set_type[9] |= 0x02;

        set_type[13] = 0x02;

        set_type[14] = ((type & 0xFF00) >> 8);
        set_type[15] = ((type & 0x007F));

        set_type[17] = 0xF7;
        set_type[16] = calculate_checksum(set_type, 18, 16) ^ 0x07;

        send_data(set_type, 18);
    } else if (type < 0xFFFFFF) {
        set_type[9] |= 0x08; // there'll be length before value
        if (((type & 0x80) >> 7) == 1)
            set_type[9] |= 0x01;

        set_type[13] = 0x03;

        set_type[14] = ((type & 0xFF0000) >> 16);
        set_type[15] = ((type & 0x00FF00) >> 8);
        set_type[16] = ((type & 0x00007F));

        set_type[18] = 0xF7;
        set_type[17] = calculate_checksum(set_type, 19, 17) ^ 0x07;

        send_data(set_type, 19);
    }
}

void set_wah_type(int type)
{
    set_higher_option(WAH_TYPE, WAH_POSITION, type);
}

void set_wah_on_off(gboolean val)
{
    set_option(WAH_ON_OFF, WAH_POSITION, (val == TRUE) ? 1 : 0);
}

/* level = 0 to 99 */
void set_comp_sustain(int level)
{
    set_option(COMP_SUSTAIN, COMP_POSITION, level);
}

/* level = 0 to 99, available only in digi comp */
void set_comp_tone(int level)
{
    set_option(COMP_TONE, COMP_POSITION, level);
}

/* level = 0 to 99 */
void set_comp_attack(int level)
{
    set_option(COMP_ATTACK, COMP_POSITION, level);
}

/* level = 0 to 99 */
void set_comp_level(int level)
{
    set_option(COMP_LEVEL, COMP_POSITION, level);
}

void set_comp_type(int type)
{
    set_higher_option(COMP_TYPE, COMP_POSITION, type);
}

void set_comp_on_off(gboolean val)
{
    set_option(COMP_ON_OFF, COMP_POSITION, (val == TRUE) ? 1 : 0);
}

/* x = 0 to 60 */
void switch_user_preset(int x)
{
    static char switch_preset[] = {0x00, 0xF0, 0x00, 0x00, 0x10, 0x00, 0x5E, 0x02, 0x39, 0x00, 0x01 /* bank = user */, 0x00 /* no */, 0x04, 0x00, 0x00, 0x01, 0x00 /* confirm */, 0xF7};

    switch_preset[11] = x;
    switch_preset[16] = calculate_checksum(switch_preset, sizeof(switch_preset), 16) ^ 0x07;

    send_data(switch_preset, sizeof(switch_preset));
}

/* x = 0 to 60 */
void switch_system_preset(int x)
{
    static char switch_preset[] = {0x00, 0xF0, 0x00, 0x00, 0x10, 0x00, 0x5E, 0x02, 0x39, 0x00, 0x00 /* bank = system */, 0x00 /* no */, 0x04, 0x00, 0x00, 0x01, 0x00 /* confirm */, 0xF7};

    switch_preset[11] = x;
    switch_preset[16] = calculate_checksum(switch_preset, sizeof(switch_preset), 16) ^ 0x07;

    send_data(switch_preset, sizeof(switch_preset));
}

void set_pickup_type(int type)
{
    set_option(PICKUP_TYPE, PICKUP_POSITION, type);
}

void set_pickup_on_off(gboolean val)
{
    set_option(PICKUP_ON_OFF, PICKUP_POSITION, (val == TRUE) ? 1 : 0);
}

void set_dist_type(int type)
{
    set_higher_option(DIST_TYPE, DIST_POSITION, type);
}

void set_dist_option(guint32 option, int value)
{
    set_option(option, DIST_POSITION, value);
}

void set_dist_on_off(gboolean val)
{
    set_option(DIST_ON_OFF, DIST_POSITION, (val == TRUE) ? 1 : 0);
}

/* level = 0 to 99 */
void set_preset_level(int level)
{
    set_option(PRESET_LEVEL, PRESET_POSITION, level);
}

void set_eq_type(int type)
{
    set_higher_option(EQ_TYPE, EQ_POSITION, type);
}

/* x = 0 to 99 */
void set_eq_gain(int x)
{
    set_option(AMP_GAIN, AMP_POSITION, x);
}

/* x = 0 to 99 */
void set_eq_level(int x)
{
    set_option(AMP_LEVEL, AMP_POSITION, x);
}

/* x = 0x00 (-12dB) to 0x18 (12dB) */
void set_eq_bass(int x)
{
    set_option(EQ_BASS, EQ_POSITION, x);
}

/* x = 0x00 (-12dB) to 0x18 (12dB) */
void set_eq_mid(int x)
{
    set_option(EQ_MID, EQ_POSITION, x);
}

/*
   x = 0 to 4700 (which transforms into 300-5000 Hz)
   device itself will accept higher value, but X-Edit allows only 300-5000Hz
*/
void set_eq_mid_hz(int x)
{
    if (x <= 0x7F) {
        set_option(EQ_MID_HZ, EQ_POSITION, x);
    } else {
        set_higher_option(EQ_MID_HZ, EQ_POSITION, x);
    }
}

/* x = 0x00 (-12dB) to 0x18 (12dB) */
void set_eq_treble(int x)
{
    set_option(EQ_TREBLE, EQ_POSITION, x);
}

/*
   x = 0 to 7500 (which transforms into 500-8000 Hz)
   device itself will accept higher value, but X-Edit allows only 500-8000Hz
*/
void set_eq_treb_hz(int x)
{
    if (x <= 0x7F) {
        set_option(EQ_TREBLE_HZ, EQ_POSITION, x);
    } else {
        set_higher_option(EQ_TREBLE_HZ, EQ_POSITION, x);
    }
}

void set_eq_on_off(gboolean val)
{
    set_option(EQ_ON_OFF, EQ_POSITION, (val == TRUE) ? 1 : 0);
}

void set_noisegate_type(int type)
{
    set_higher_option(NOISEGATE_TYPE, NOISEGATE_POSITION, type);
}

/* x = 0 to 99 */
void set_gate_option(guint32 option, int x)
{
    set_option(option, NOISEGATE_POSITION, x);
}

void set_noisegate_on_off(gboolean val)
{
    set_option(NOISEGATE_ON_OFF, NOISEGATE_POSITION, (val == TRUE) ? 1 : 0);
}

void set_chorusfx_option(guint32 option, int x)
{
    set_option(option, CHORUSFX_POSITION, x);
}

void set_chorusfx_type(int type)
{
    set_higher_option(CHORUSFX_TYPE, CHORUSFX_POSITION, type);
}

void set_chorusfx_on_off(gboolean val)
{
    set_option(CHORUSFX_ON_OFF, CHORUSFX_POSITION, (val == TRUE) ? 1 : 0);
}

/* x = 0 to 139 */
void set_delay_time(int x)
{
    if (x <= 0x7F) {
        set_option(DELAY_TIME, DELAY_POSITION, x);
    } else {
        set_higher_option(DELAY_TIME, DELAY_POSITION, x);
    }
}

void set_delay_type(int type)
{
    set_higher_option(DELAY_TYPE, DELAY_POSITION, type);
}

void set_delay_option(guint32 option, int x)
{
    set_option(option, DELAY_POSITION, x);
}

void set_delay_on_off(gboolean val)
{
    set_option(DELAY_ON_OFF, DELAY_POSITION, (val == TRUE) ? 1 : 0);
}

/* x = 0 to 15 (predelay), otherwise 0 to 99 */
void set_reverb_option(guint32 option, int x)
{
    set_option(option, REVERB_POSITION, x);
}

void set_reverb_type(int type)
{
    set_higher_option(REVERB_TYPE, REVERB_POSITION, type);
}

void set_reverb_on_off(gboolean val)
{
    set_option(REVERB_ON_OFF, REVERB_POSITION, (val == TRUE) ? 1 : 0);
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
    set_name[12+a+1+b] = calculate_checksum(set_name, 12+a+3+b, 12+a+1+b) ^ 0x07;

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
