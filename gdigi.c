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

/* level = 0 to 99 */
void set_wah_min(int level)
{
    static char set_min[] = {0x00, 0xF0, 0x00, 0x00, 0x10, 0x00, 0x5E, 0x02, 0x41, 0x00, 0x20, 0x03, 0x14, 0x00 /* level */, 0x00 /* checksum */, 0xF7};

    set_min[13] = level;
    set_min[14] = calculate_checksum(set_min, sizeof(set_min), 14) ^ 0x07;

    send_data(set_min, sizeof(set_min));
}

/* level = 0 to 99 */
void set_wah_max(int level)
{
    static char set_max[] = {0x00, 0xF0, 0x00, 0x00, 0x10, 0x00, 0x5E, 0x02, 0x41, 0x00, 0x20, 0x04, 0x14, 0x00 /* level */, 0x00 /* checksum */, 0xF7};

    set_max[13] = level;
    set_max[14] = calculate_checksum(set_max, sizeof(set_max), 14) ^ 0x07;

    send_data(set_max, sizeof(set_max));
}

/* level = 0 to 12 */
void set_wah_level(int level)
{
    static char set_level[] = {0x00, 0xF0, 0x00, 0x00, 0x10, 0x00, 0x5E, 0x02, 0x41, 0x20, 0x00, 0x05, 0x03, 0x00 /* level */, 0x00 /* checksum */, 0xF7};

    set_level[13] = level;
    set_level[14] = calculate_checksum(set_level, sizeof(set_level), 14) ^ 0x07;

    send_data(set_level, sizeof(set_level));
}

void set_wah_type(int type)
{
    static char set_type[] = {0x00, 0xF0, 0x00, 0x00, 0x10, 0x00, 0x5E, 0x02, 0x41, 0x2C, 0x00, 0x00, 0x03, 0x01, 0x00 /* type */, 0x00 /* confirm */, 0xF7};

    switch (type) {
      case WAH_TYPE_CRY: set_type[14] = 4; break;
      case WAH_TYPE_FULLRANGE: set_type[14] = 5; break;
      case WAH_TYPE_CLYDE: set_type[14] = 6; break;
      default: break;
    }

    set_type[15] = calculate_checksum(set_type, sizeof(set_type), 15) ^ 0x07;

    send_data(set_type, sizeof(set_type));
}

void set_wah_on_off(gboolean val)
{
    static char set_wah[] = {0x00, 0xF0, 0x00, 0x00, 0x10, 0x00, 0x5E, 0x02, 0x41, 0x20, 0x00, 0x01, 0x03, 0x00 /* on/off */, 0x00 /* checksum */, 0xF7};

    if (val == FALSE) { /* turn wah off */
        set_wah[13] = 0;
    } else { /* turn wah on */
        set_wah[13] = 1;
    }

    set_wah[14] = calculate_checksum(set_wah, sizeof(set_wah), 14) ^ 0x07;

    send_data(set_wah, sizeof(set_wah));
}

/* level = 0 to 99 */
void set_comp_sustain(int level)
{
    static char set_sust[] = {0x00, 0xF0, 0x00, 0x00, 0x10, 0x00, 0x5E, 0x02, 0x41, 0x20, 0x00, 0x50, 0x04, 0x00 /* value */, 0x00 /* checksum */, 0xF7};

    set_sust[13] = level;
    set_sust[14] = calculate_checksum(set_sust, sizeof(set_sust), 14) ^ 0x07;

    send_data(set_sust, sizeof(set_sust));
}

/* level = 0 to 99, available only in digi comp */
void set_comp_tone(int level)
{
    static char set_tone[] = {0x00, 0xF0, 0x00, 0x00, 0x10, 0x00, 0x5E, 0x02, 0x41, 0x20, 0x00, 0x51, 0x04, 0x00 /* value */, 0x00 /* checksum */, 0xF7};

    set_tone[13] = level;
    set_tone[14] = calculate_checksum(set_tone, sizeof(set_tone), 14) ^ 0x07;

    send_data(set_tone, sizeof(set_tone));
}

/* level = 0 to 99 */
void set_comp_attack(int level)
{
    static char set_attack[] = {0x00, 0xF0, 0x00, 0x00, 0x10, 0x00, 0x5E, 0x02, 0x41, 0x20, 0x00, 0x53, 0x04, 0x00 /* value */, 0x00 /* checksum */, 0xf7};

    set_attack[13] = level;
    set_attack[14] = calculate_checksum(set_attack, sizeof(set_attack), 14) ^ 0x07;

    send_data(set_attack, sizeof(set_attack));
}

/* level = 0 to 99 */
void set_comp_level(int level)
{
    static char set_level[] = {0x00, 0xF0, 0x00, 0x00, 0x10, 0x00, 0x5E, 0x02, 0x41, 0x20, 0x00, 0x52, 0x04, 0x00 /* value */, 0x00 /* checksum */, 0xF7};

    set_level[13] = level;
    set_level[14] = calculate_checksum(set_level, sizeof(set_level), 14) ^ 0x07;

    send_data(set_level, sizeof(set_level));
}

void set_comp_type(int type)
{
    static char set_type[] = {0x00, 0xF0, 0x00, 0x00, 0x10, 0x00, 0x5E, 0x02, 0x41, 0x2C, 0x00, 0x4F, 0x04, 0x01, 0x00 /* type */, 0x00 /* checksum */, 0xF7};

    switch (type) {
      case COMP_TYPE_DIGI: set_type[14] = 0x43; break;
      case COMP_TYPE_CS: set_type[14] = 0x44; break;
      default: break;
    }

    set_type[15] = calculate_checksum(set_type, sizeof(set_type), 15) ^ 0x07;

    send_data(set_type, sizeof(set_type));
}

void set_comp_on_off(gboolean val)
{
    static char set_comp[] = {0x00, 0xF0, 0x00, 0x00, 0x10, 0x00, 0x5E, 0x02, 0x41, 0x20, 0x00, 0x41, 0x04, 0x00 /* on/off */, 0x00 /* checksum */, 0xF7};

    if (val == FALSE) { /* turn comp off */
        set_comp[13] = 0;
    } else { /* turn comp on */
        set_comp[13] = 1;
    }

    set_comp[14] = calculate_checksum(set_comp, sizeof(set_comp), 14) ^ 0x07;

    send_data(set_comp, sizeof(set_comp));
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
    static char pickup[] = {0x00, 0xF0, 0x00, 0x00, 0x10, 0x00, 0x5E, 0x02, 0x41, 0x00, 0x00, 0x40, 0x02, 0x00 /* type1 */, 0x00 /* checksum */, 0xF7};

    switch (type) {
        case PICKUP_TYPE_HB_SC: pickup[13] = 0x42; break;
        case PICKUP_TYPE_SC_HB: pickup[13] = 0x41; break;
        default: break;
    }

    pickup[14] = calculate_checksum(pickup, sizeof(pickup), 14) ^ 0x07;

    send_data(pickup, sizeof(pickup));
}

void set_pickup_on_off(gboolean val)
{
    static char set_pickup[] = {0x00, 0xF0, 0x00, 0x00, 0x10, 0x00, 0x5E, 0x02, 0x41, 0x00, 0x00, 0x41, 0x02, 0x00 /* on/off */, 0x00 /* checksum */, 0xF7};

    if (val == FALSE) { /* turn pickup off */
        set_pickup[13] = 0;
    } else { /* turn pickup on */
        set_pickup[13] = 1;
    }

    set_pickup[14] = calculate_checksum(set_pickup, sizeof(set_pickup), 14) ^ 0x07;

    send_data(set_pickup, sizeof(set_pickup));
}

void set_dist_type(int type)
{
    static char set_dist[] = {0x00, 0xF0, 0x00, 0x00, 0x10, 0x00, 0x5E, 0x02, 0x41, 0x28, 0x09, 0x00, 0x06, 0x02, 0x05, 0x00 /* type1 */, 0x00 /* checksum */, 0xF7};

    switch (type) {
        case DIST_TYPE_SCREAMER: set_dist[15] = 0x00; break;
        case DIST_TYPE_808:      set_dist[15] = 0x0C; break;
        case DIST_TYPE_GUYOD:    set_dist[15] = 0x05; break;
        case DIST_TYPE_DOD250:   set_dist[15] = 0x03; break;
        case DIST_TYPE_RODENT:   set_dist[15] = 0x01; break;
        case DIST_TYPE_MX:       set_dist[15] = 0x0B; break;
        case DIST_TYPE_DS:       set_dist[15] = 0x02; break;
        case DIST_TYPE_GRUNGE:   set_dist[15] = 0x07; break;
        case DIST_TYPE_ZONE:     set_dist[15] = 0x09; break;
        case DIST_TYPE_DEATH:    set_dist[15] = 0x0E; break;
        case DIST_TYPE_GONK:     set_dist[15] = 0x0D; break;
        case DIST_TYPE_FUZZY:    set_dist[15] = 0x08; break;
        case DIST_TYPE_MP:       set_dist[15] = 0x04; break;
        default: break;
    }

    set_dist[16] = calculate_checksum(set_dist, sizeof(set_dist), 16) ^ 0x07;

    send_data(set_dist, sizeof(set_dist));
}

void set_dist_option(guint32 option, int value)
{
    static char set_option[] = {0x00, 0xF0, 0x00, 0x00, 0x10, 0x00, 0x5E, 0x02, 0x41, 0x20, 0x09, 0x00 /* option */, 0x06, 0x00 /* value */, 0x00 /* checksum */, 0xF7};

    set_option[11] = option;
    set_option[13] = value;
    set_option[14] = calculate_checksum(set_option, sizeof(set_option), 14) ^ 0x07;

    send_data(set_option, sizeof(set_option));
}

void set_dist_on_off(gboolean val)
{
    static char set_dist[] = {0x00, 0xF0, 0x00, 0x00, 0x10, 0x00, 0x5E, 0x02, 0x41, 0x20, 0x09, 0x01, 0x06, 0x00 /* on/off */, 0x00 /* checksum */, 0xF7};

    if (val == FALSE) { /* turn dist off */
        set_dist[13] = 0;
    } else { /* turn dist on */
        set_dist[13] = 1;
    }

    set_dist[14] = calculate_checksum(set_dist, sizeof(set_dist), 14) ^ 0x07;

    send_data(set_dist, sizeof(set_dist));
}

/* level = 0 to 99 */
void set_preset_level(int level)
{
    static char set_level[] = {0x00, 0xF0, 0x00, 0x00, 0x10, 0x00, 0x5E, 0x02, 0x41, 0x00, 0x0A, 0x42, 0x12, 0x00 /* value */, 0x00 /* checksum */, 0xF7};

    set_level[13] = level;
    set_level[14] = calculate_checksum(set_level, sizeof(set_level), 14) ^ 0x07;

    send_data(set_level, sizeof(set_level));
}

void set_eq_type(int type)
{
    static char set_eq[] = {0x00, 0xF0, 0x00, 0x00, 0x10, 0x00, 0x5E, 0x02, 0x41, 0x2A, 0x0C, 0x02, 0x18, 0x02, 0x05, 0x00 /* type1 */, 0x00 /* checksum */, 0xF7};

    switch (type) {
        case EQ_TYPE_BRIGHT:   set_eq[15] = 0x42; break;
        case EQ_TYPE_MIDBOOST: set_eq[15] = 0x40; break;
        case EQ_TYPE_SCOOP:    set_eq[15] = 0x41; break;
        case EQ_TYPE_WARM:     set_eq[15] = 0x43; break;
        default: break;
    }

    set_eq[16] = calculate_checksum(set_eq, sizeof(set_eq), 16) ^ 0x07;

    send_data(set_eq, sizeof(set_eq));
}

/* x = 0 to 99 */
void set_eq_gain(int x)
{
    static char set_gain[] = {0x00, 0xF0, 0x00, 0x00, 0x10, 0x00, 0x5E, 0x02, 0x41, 0x20, 0x09, 0x41, 0x08, 0x00 /* value */, 0x00 /* checksum */, 0xF7};

    set_gain[13] = x;
    set_gain[14] = calculate_checksum(set_gain, sizeof(set_gain), 14) ^ 0x07;

    send_data(set_gain, sizeof(set_gain));
}

/* x = 0 to 99 */
void set_eq_level(int x)
{
    static char set_level[] = {0x00, 0xF0, 0x00, 0x00, 0x10, 0x00, 0x5E, 0x02, 0x41, 0x20, 0x09, 0x42, 0x08, 0x00 /* value */, 0x00 /* checksum */, 0xF7};

    set_level[13] = x;
    set_level[14] = calculate_checksum(set_level, sizeof(set_level), 14) ^ 0x07;

    send_data(set_level, sizeof(set_level));
}

/* x = 0x00 (-12dB) to 0x18 (12dB) */
void set_eq_bass(int x)
{
    static char set_bass[] = {0x00, 0xF0, 0x00, 0x00, 0x10, 0x00, 0x5E, 0x02, 0x41, 0x20, 0x0C, 0x03, 0x18, 0x00 /* value */, 0x00 /* checksum */, 0xF7};

    set_bass[13] = x;
    set_bass[14] = calculate_checksum(set_bass, sizeof(set_bass), 14) ^ 0x07;

    send_data(set_bass, sizeof(set_bass));
}

/* x = 0x00 (-12dB) to 0x18 (12dB) */
void set_eq_mid(int x)
{
    static char set_mid[] = {0x00, 0xF0, 0x00, 0x00, 0x10, 0x00, 0x5E, 0x02, 0x41, 0x20, 0x0C, 0x04, 0x18, 0x00 /* value */, 0x00 /* checksum */, 0xF7};

    set_mid[13] = x;
    set_mid[14] = calculate_checksum(set_mid, sizeof(set_mid), 14) ^ 0x07;

    send_data(set_mid, sizeof(set_mid));
}

/*
   x = 0 to 4700 (which transforms into 300-5000 Hz)
   device itself will accept higher value, but X-Edit allows only 300-5000Hz
*/
void set_eq_mid_hz(int x)
{
    if (x <= 0x7F) {
        static char set_hz[] = {0x00, 0xF0, 0x00, 0x00, 0x10, 0x00, 0x5E, 0x02, 0x41, 0x20, 0x0C, 0x06, 0x18, 0x00 /* value */, 0x00 /* checksum */, 0xF7};

        set_hz[13] = x;
        set_hz[14] = calculate_checksum(set_hz, sizeof(set_hz), 14) ^ 0x07;

        send_data(set_hz, sizeof(set_hz));
    } else {
        x -= 0x80;
        if (x <= 0x7F) {
            static char set_hz[] = {0x00, 0xF0, 0x00, 0x00, 0x10, 0x00, 0x5E, 0x02, 0x41, 0x2C, 0x0C, 0x06, 0x18, 0x01, 0x00 /* value */, 0x00 /* checksum */, 0xF7};

            set_hz[14] = x;
            set_hz[15] = calculate_checksum(set_hz, sizeof(set_hz), 15) ^ 0x07;

            send_data(set_hz, sizeof(set_hz));
        } else {
            x -= 0x80;
            int a, b, c;
            a = 0x28;
            b = 0x01;
            c = 0;
            while (x > 0x7F) {
               c++;
               if ((c % 2) == 0) {
                   a ^= 0x02; b++;
               } else {
                   a ^= 0x02;
               }

               x -= 0x80;
            }

            static char set_hz[] = {0x00, 0xF0, 0x00, 0x00, 0x10, 0x00, 0x5E, 0x02, 0x41, 0x00 /* a */, 0x0C, 0x06, 0x18, 0x02, 0x00 /* b */, 0x00 /* value */, 0x00 /* checksum */, 0xF7};

            set_hz[9] = a;
            set_hz[14] = b;
            set_hz[15] = x;
            set_hz[16] = calculate_checksum(set_hz, sizeof(set_hz), 16) ^ 0x07;

            send_data(set_hz, sizeof(set_hz));
        }
    }
}

/* x = 0x00 (-12dB) to 0x18 (12dB) */
void set_eq_treble(int x)
{
    static char set_treble[] = {0x00, 0xF0, 0x00, 0x00, 0x10, 0x00, 0x5E, 0x02, 0x41, 0x20, 0x0C, 0x05, 0x18, 0x00 /* value */, 0x00 /* checksum */, 0xF7};

    set_treble[13] = x;
    set_treble[14] = calculate_checksum(set_treble, sizeof(set_treble), 14) ^ 0x07;

    send_data(set_treble, sizeof(set_treble));
}

/*
   x = 0 to 7500 (which transforms into 500-8000 Hz)
   device itself will accept higher value, but X-Edit allows only 500-8000Hz
*/
void set_eq_treb_hz(int x)
{
    if (x <= 0x7F) {
        static char set_hz[] = {0x00, 0xF0, 0x00, 0x00, 0x10, 0x00, 0x5E, 0x02, 0x41, 0x20, 0x0C, 0x0B, 0x18, 0x00 /* value */, 0x00 /* checksum */, 0xF7};

        set_hz[13] = x;
        set_hz[14] = calculate_checksum(set_hz, sizeof(set_hz), 14) ^ 0x07;

        send_data(set_hz, sizeof(set_hz));
    } else {
        x -= 0x80;
        if (x <= 0x7F) {
            static char set_hz[] = {0x00, 0xF0, 0x00, 0x00, 0x10, 0x00, 0x5E, 0x02, 0x41, 0x2C, 0x0C, 0x0B, 0x18, 0x01, 0x00 /* value */, 0x00 /* checksum */, 0xF7};

            set_hz[14] = x;
            set_hz[15] = calculate_checksum(set_hz, sizeof(set_hz), 15) ^ 0x07;

            send_data(set_hz, sizeof(set_hz));
        } else {
            x -= 0x80;
            int a, b, c;
            a = 0x28;
            b = 0x01;
            c = 0;
            while (x > 0x7F) {
               c++;
               if ((c % 2) == 0) {
                   a ^= 0x02; b++;
               } else {
                   a ^= 0x02;
               }

               x -= 0x80;
            }

            static char set_hz[] = {0x00, 0xF0, 0x00, 0x00, 0x10, 0x00, 0x5E, 0x02, 0x41, 0x00 /* a */, 0x0C, 0x0B, 0x18, 0x02, 0x00 /* b */, 0x00 /* value */, 0x00 /* checksum */, 0xF7};

            set_hz[9] = a;
            set_hz[14] = b;
            set_hz[15] = x;
            set_hz[16] = calculate_checksum(set_hz, sizeof(set_hz), 16) ^ 0x07;

            send_data(set_hz, sizeof(set_hz));
        }
    }
}

void set_eq_on_off(gboolean val)
{
    static char set_eq[] = {0x00, 0xF0, 0x00, 0x00, 0x10, 0x00, 0x5E, 0x02, 0x41, 0x20, 0x0C, 0x0C, 0x18, 0x00 /* on/off */, 0x00 /* checksum */, 0xF7};

    if (val == FALSE) { /* turn eq off */
        set_eq[13] = 0;
    } else { /* turn eq on */
        set_eq[13] = 1;
    }

    set_eq[14] = calculate_checksum(set_eq, sizeof(set_eq), 14) ^ 0x07;

    send_data(set_eq, sizeof(set_eq));
}

void set_noisegate_type(int type)
{
    static char set_type[] = {0x00, 0xF0, 0x00, 0x00, 0x10, 0x00, 0x5E, 0x02, 0x41, 0x28, 0x02, 0x40, 0x0C, 0x02, 0x03, 0x00 /* type1 */, 0x00 /* checksum */, 0xF7};

    switch (type) {
        case NOISEGATE_GATE: set_type[15] = 0; break;
        case NOISEGATE_SWELL: set_type[15] = 1; break;
        default: break;
    }

    set_type[16] = calculate_checksum(set_type, sizeof(set_type), 16) ^ 0x07;

    send_data(set_type, sizeof(set_type));
}

/* x = 0 to 99 */
void set_gate_option(guint32 option, int x)
{
    static char set_option[] = {0x00, 0xF0, 0x00, 0x00, 0x10, 0x00, 0x5E, 0x02, 0x41, 0x20, 0x02, 0x00 /* option */, 0x0C, 0x00 /* value */, 0x00 /* checksum */, 0xF7};

    set_option[11] = option;
    set_option[13] = x;
    set_option[14] = calculate_checksum(set_option, sizeof(set_option), 14) ^ 0x07;

    send_data(set_option, sizeof(set_option));
}

void set_noisegate_on_off(gboolean val)
{
    static char set_gate[] = {0x00, 0xF0, 0x00, 0x00, 0x10, 0x00, 0x5E, 0x02, 0x41, 0x20, 0x02, 0x41, 0x0C, 0x00 /* on/off */, 0x00 /* checksum */, 0xF7};

    if (val == FALSE) { /* turn noisegate off */
        set_gate[13] = 0;
    } else { /* turn noisegate on */
        set_gate[13] = 1;
    }

    set_gate[14] = calculate_checksum(set_gate, sizeof(set_gate), 14) ^ 0x07;

    send_data(set_gate, sizeof(set_gate));
}

void set_chorusfx_option(guint32 option, int x)
{
    static char set_option[] = {0xF0, 0x00, 0x00, 0x10, 0x00, 0x5E, 0x02, 0x41, 0x00 /* option1 */, 0x00 /* option2 */, 0x00 /* option3 */, 0x0E, 0x00 /* value */, 0x00 /* checksum */, 0xF7};

    set_option[8]  = ((option & 0xFF0000) >> 16);
    set_option[9]  = ((option & 0x00FF00) >> 8);
    set_option[10] = ((option & 0x0000FF));

    set_option[12] = x;
    set_option[13] = calculate_checksum(set_option, sizeof(set_option), 13) ^ 0x07;

    send_data(set_option, sizeof(set_option));
}

void set_chorusfx_type(int type)
{
    static char set_type[] = {0x00, 0xF0, 0x00, 0x00, 0x10, 0x00, 0x5E, 0x02, 0x41, 0x00 /* type */, 0x03, 0x00, 0x0E, 0x02, 0x00 /* type */, 0x00 /* type1 */, 0x00 /* checksum */, 0xF7};

    switch (type) {
        case CHORUS_TYPE_CE: set_type[9] = 0x08; set_type[15] = 0x7B; set_type[14] = 0x03; break;
        case CHORUS_TYPE_DUAL: set_type[9] = 0x08; set_type[15] = 0x79; set_type[14] = 0x03; break;
        case CHORUS_TYPE_MULTI: set_type[9] = 0x08; set_type[15] = 0x7A; set_type[14] = 0x03; break;
        case CHORUS_TYPE_FLANGER: set_type[9] = 0x08; set_type[15] = 0x7D; set_type[14] = 0x03; break;
        case CHORUS_TYPE_MXR_FLANGER: set_type[9] = 0x08; set_type[15] = 0x7F; set_type[14] = 0x03; break;
        case CHORUS_TYPE_PHASER: set_type[9] = 0x0A; set_type[15] = 0x01; set_type[14] = 0x03; break;
        case CHORUS_TYPE_VIBRATO: set_type[9] = 0x08; set_type[15] = 0x60; set_type[14] = 0x03; break;
        case CHORUS_TYPE_ROTARY: set_type[9] = 0x08; set_type[15] = 0x61; set_type[14] = 0x03; break;
        case CHORUS_TYPE_VIBROPAN: set_type[9] = 0x0A; set_type[15] = 0x0F; set_type[14] = 0x03; break;
        case CHORUS_TYPE_TREMOLO: set_type[9] = 0x08; set_type[15] = 0x5E; set_type[14] = 0x03; break;
        case CHORUS_TYPE_PANNER: set_type[9] = 0x08; set_type[15] = 0x5F; set_type[14] = 0x03; break;
        case CHORUS_TYPE_ENVELOPE: set_type[9] = 0x0A; set_type[15] = 0x0A; set_type[14] = 0x03; break;
        case CHORUS_TYPE_AUTOYA: set_type[9] = 0x0A; set_type[15] = 0x0B; set_type[14] = 0x03; break;
        case CHORUS_TYPE_YAYA: set_type[9] = 0x0A; set_type[15] = 0x0C; set_type[14] = 0x03; break;
        case CHORUS_TYPE_STEP_FILTER: set_type[9] = 0x0A; set_type[15] = 0x0D; set_type[14] = 0x03; break;
        case CHORUS_TYPE_WHAMMY: set_type[9] = 0x08; set_type[15] = 0x40; set_type[14] = 0x05; break;
        case CHORUS_TYPE_PITCH_SHIFT: set_type[9] = 0x08; set_type[15] = 0x43; set_type[14] = 0x05; break;
        case CHORUS_TYPE_DETUNE: set_type[9] = 0x08; set_type[15] = 0x42; set_type[14] = 0x05; break;
        case CHORUS_TYPE_IPS: set_type[9] = 0x08; set_type[15] = 0x41; set_type[14] = 0x05; break;
        default: break;
    }

    set_type[16] = calculate_checksum(set_type, sizeof(set_type), 16) ^ 0x07;

    send_data(set_type, sizeof(set_type));
}

void set_chorusfx_on_off(gboolean val)
{
    static char set_chorus[] = {0x00, 0xF0, 0x00, 0x00, 0x10, 0x00, 0x5E, 0x02, 0x41, 0x00, 0x03, 0x01, 0x0E, 0x00 /* on/off */, 0x00 /* checksum */, 0xF7};

    if (val == FALSE) { /* turn chorusfx off */
        set_chorus[13] = 0;
    } else { /* turn chorusfx on */
        set_chorus[13] = 1;
    }

    set_chorus[14] = calculate_checksum(set_chorus, sizeof(set_chorus), 14) ^ 0x07;

    send_data(set_chorus, sizeof(set_chorus));
}

/* x = 0 to 139 */
void set_delay_time(int x)
{
    if (x <= 0x7F) {
        static char set_time[] = {0x00, 0xF0, 0x00, 0x00, 0x10, 0x00, 0x5E, 0x02, 0x41, 0x00, 0x07, 0x60, 0x0F, 0x00 /* value */, 0x00 /* checksum */, 0xF7};

        set_time[13] = x;
        set_time[14] = calculate_checksum(set_time, sizeof(set_time), 14) ^ 0x07;

        send_data(set_time, sizeof(set_time));
    } else {
        static char set_time[] = {0x00, 0xF0, 0x00, 0x00, 0x10, 0x00, 0x5E, 0x02, 0x41, 0x0C, 0x07, 0x60, 0x0F, 0x01, 0x00 /* value */, 0x00 /* checksum */, 0xF7};

        set_time[14] = x - 0x80;
        set_time[15] = calculate_checksum(set_time, sizeof(set_time), 15) ^ 0x07;

        send_data(set_time, sizeof(set_time));
    }
}

void set_delay_type(int type)
{
    static char set_type[] = {0x00, 0xF0, 0x00, 0x00, 0x10, 0x00, 0x5E, 0x02, 0x41, 0x08, 0x07, 0x40, 0x0F, 0x02, 0x04, 0x00 /* type1 */, 0x00 /* checksum */, 0xF7};

    switch (type) {
        case DELAY_TYPE_ANALOG: set_type[15] = 0x16; break;
        case DELAY_TYPE_DIGITAL: set_type[15] = 0x15; break;
        case DELAY_TYPE_MODULATED: set_type[15] = 0x17; break;
        case DELAY_TYPE_PONG: set_type[15] = 0x18; break;
        case DELAY_TYPE_TAPE: set_type[15] = 0x19; break;
        default: break;
    }

    set_type[16] = calculate_checksum(set_type, sizeof(set_type), 16) ^ 0x07;

    send_data(set_type, sizeof(set_type));
}

void set_delay_option(guint32 option, int x)
{
    static char set_option[] = {0x00, 0xF0, 0x00, 0x00, 0x10, 0x00, 0x5E, 0x02, 0x41, 0x00, 0x07, 0x00 /* option */, 0x0F, 0x00 /* value */, 0x00 /* checksum */, 0xF7};

    set_option[11] = option;
    set_option[13] = x;
    set_option[14] = calculate_checksum(set_option, sizeof(set_option), 14) ^ 0x07;

    send_data(set_option, sizeof(set_option));
}

void set_delay_on_off(gboolean val)
{
    static char set_delay[] = {0x00, 0xF0, 0x00, 0x00, 0x10, 0x00, 0x5E, 0x02, 0x41, 0x00, 0x07, 0x41, 0x0F, 0x00 /* on/off */, 0x00 /* checksum */, 0xF7};

    if (val == FALSE) { /* turn delay off */
        set_delay[13] = 0;
    } else { /* turn delay on */
        set_delay[13] = 1;
    }

    set_delay[14] = calculate_checksum(set_delay, sizeof(set_delay), 14) ^ 0x07;

    send_data(set_delay, sizeof(set_delay));
}

/* x = 0 to 15 (predelay), otherwise 0 to 99 */
void set_reverb_option(guint32 option, int x)
{
    static char set_option[] = {0x00, 0xF0, 0x00, 0x00, 0x10, 0x00, 0x5E, 0x02, 0x41, 0x20, 0x07, 0x00 /* option */, 0x10, 0x00 /* value */, 0x00 /* checksum */, 0xF7};

    set_option[11] = option;
    set_option[13] = x;
    set_option[14] = calculate_checksum(set_option, sizeof(set_option), 14) ^ 0x07;

    send_data(set_option, sizeof(set_option));
}

void set_reverb_type(int type)
{
    static char set_type[] = {0x00, 0xF0, 0x00, 0x00, 0x10, 0x00, 0x5E, 0x02, 0x41, 0x28, 0x07, 0x00, 0x10, 0x02, 0x04, 0x00 /* type1 */, 0x00 /* checksum */, 0xF7};

    switch (type) {
        case REVERB_TYPE_TWIN: set_type[15] = 0x7A; break;
        case REVERB_TYPE_LEX_AMBIENCE: set_type[15] = 0x7E; break;
        case REVERB_TYPE_LEX_STUDIO: set_type[15] = 0x7D; break;
        case REVERB_TYPE_LEX_ROOM: set_type[15] = 0x7C; break;
        case REVERB_TYPE_LEX_HALL: set_type[15] = 0x7B; break;
        case REVERB_TYPE_EMT240_PLATE: set_type[15] = 0x7F; break;
        default: break;
    }

    set_type[16] = calculate_checksum(set_type, sizeof(set_type), 16) ^ 0x07;

    send_data(set_type, sizeof(set_type));
}

void set_reverb_on_off(gboolean val)
{
    static char set_reverb[] = {0x00, 0xF0, 0x00, 0x00, 0x10, 0x00, 0x5E, 0x02, 0x41, 0x20, 0x07, 0x01, 0x10, 0x00 /* on/off */, 0x00 /* checksum */, 0xF7};

    if (val == FALSE) { /* turn reverb off */
        set_reverb[13] = 0;
    } else { /* turn reverb on */
        set_reverb[13] = 1;
    }

    set_reverb[14] = calculate_checksum(set_reverb, sizeof(set_reverb), 14) ^ 0x07;

    send_data(set_reverb, sizeof(set_reverb));
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
