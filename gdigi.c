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
#include <string.h>
#include "gdigi.h"
#include "gui.h"

#define TIMEOUT 1000

char buf[255];

struct usb_device *dev;
struct usb_dev_handle *handle;

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

int read_device(struct usb_dev_handle *handle, int bytes)
{
    int i, x;
    i = usb_bulk_read(handle, 132, buf, bytes, TIMEOUT);
    //printf("Called read %d. Device returned %d bytes\n", bytes, i);
    for (x=0; x<i; x++) {
       printf("0x%02x(%c) ", (u_char) buf[x], buf[x] ? (u_char) buf[x] : 32);
    }

    printf("\n");
    if (bytes > 0) return i;
    else return 0;
}

void check_preset(struct usb_dev_handle *handle)
{
    static char magic3[] = {0x04, 0xF0, 0x00, 0x00, 0x04, 0x10, 0x00, 0x5E, 0x04, 0x02, 0x2A, 0x00, 0x04, 0x04, 0x00, 0x62, 0x05, 0xF7, 0x00, 0x00}; // seems to query active preset

    int i;
    i = usb_bulk_write(handle, 4, magic3, sizeof(magic3), TIMEOUT);

    /* UGLY - have to figure out reply formatting */
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
    } while (i > 0);
}

/* level = 0 to 99 */
void set_wah_min(struct usb_dev_handle *handle, int level)
{
    static char set_min[] = {0x04, 0xF0, 0x00, 0x00, 0x04, 0x10, 0x00, 0x5E, 0x04, 0x02, 0x41, 0x00, 0x04, 0x20, 0x03, 0x14, 0x07, 0x00 /* level */, 0x00 /* checksum */, 0xF7};

    set_min[17] = level;
    set_min[18] = calculate_checksum(set_min, sizeof(set_min), 18);

    int i;
    i = usb_bulk_write(handle, 4, set_min, sizeof(set_min), TIMEOUT);
    printf("wrote: %d\n", i);
}

/* level = 0 to 99 */
void set_wah_max(struct usb_dev_handle *handle, int level)
{
    static char set_max[] = {0x04, 0xF0, 0x00, 0x00, 0x04, 0x10, 0x00, 0x5E, 0x04, 0x02, 0x41, 0x00, 0x04, 0x20, 0x04, 0x14, 0x07, 0x00 /* level */, 0x00 /* checksum */, 0xF7};

    set_max[17] = level;
    set_max[18] = calculate_checksum(set_max, sizeof(set_max), 18);

    int i;
    i = usb_bulk_write(handle, 4, set_max, sizeof(set_max), TIMEOUT);
    printf("wrote: %d\n", i);
}

/* level = 0 to 12 */
void set_wah_level(struct usb_dev_handle *handle, int level)
{
    static char set_level[] = {0x04, 0xF0, 0x00, 0x00, 0x04, 0x10, 0x00, 0x5E, 0x04, 0x02, 0x41, 0x20, 0x04, 0x00, 0x05, 0x03, 0x07, 0x00 /* level */, 0x00 /* checksum */, 0xF7};

    set_level[17] = level;
    set_level[18] = calculate_checksum(set_level, sizeof(set_level), 18);

    int i;
    i = usb_bulk_write(handle, 4, set_level, sizeof(set_level), TIMEOUT);
    printf("wrote: %d\n", i);
}

void set_wah_type(struct usb_dev_handle *handle, int type)
{
    static char set_type[] = {0x04, 0xF0, 0x00, 0x00, 0x04, 0x10, 0x00, 0x5E, 0x04, 0x02, 0x41, 0x2C, 0x04, 0x00, 0x00, 0x03, 0x04, 0x01, 0x00 /* type */, 0x00 /* confirm */, 0x05, 0xF7, 0x00, 0x00};

    switch (type) {
      case WAH_TYPE_CRY: set_type[18] = 4; break;
      case WAH_TYPE_FULLRANGE: set_type[18] = 5; break;
      case WAH_TYPE_CLYDE: set_type[18] = 6; break;
      default: break;
    }

    set_type[19] = calculate_checksum(set_type, sizeof(set_type), 19) ^ 0x06;

    int i;
    i = usb_bulk_write(handle, 4, set_type, sizeof(set_type), TIMEOUT);
    printf("wrote: %d\n", i);
}

void set_wah_on_off(struct usb_dev_handle *handle, gboolean val)
{
    static char set_wah[] = {0x04, 0xF0, 0x00, 0x00, 0x04, 0x10, 0x00, 0x5E, 0x04, 0x02, 0x41, 0x20, 0x04, 0x00, 0x01, 0x03, 0x07, 0x00 /* on/off */, 0x00 /* checksum */, 0xF7};

    if (val == FALSE) { /* turn wah off */
        set_wah[17] = 0;
    } else { /* turn wah on */
        set_wah[17] = 1;
    }

    set_wah[18] = calculate_checksum(set_wah, sizeof(set_wah), 18);

    int i;
    i = usb_bulk_write(handle, 4, set_wah, sizeof(set_wah), TIMEOUT);
    printf("wrote: %d\n", i);
}

/* level = 0 to 99 */
void set_comp_sustain(struct usb_dev_handle *handle, int level)
{
    static char set_sust[] = {0x04, 0xF0, 0x00, 0x00, 0x04, 0x10, 0x00, 0x5E, 0x04, 0x02, 0x41, 0x20, 0x04, 0x00, 0x50, 0x04, 0x07, 0x00 /* value */, 0x00 /* checksum */, 0xF7};

    set_sust[17] = level;
    set_sust[18] = calculate_checksum(set_sust, sizeof(set_sust), 18);

    int i;
    i = usb_bulk_write(handle, 4, set_sust, sizeof(set_sust), TIMEOUT);
    printf("wrote: %d\n", i);
}

/* level = 0 to 99, available only in digi comp */
void set_comp_tone(struct usb_dev_handle *handle, int level)
{
    static char set_tone[] = {0x04, 0xF0, 0x00, 0x00, 0x04, 0x10, 0x00, 0x5E, 0x04, 0x02, 0x41, 0x20, 0x04, 0x00, 0x51, 0x04, 0x07, 0x00 /* value */, 0x00 /* checksum */, 0xF7};

    set_tone[17] = level;
    set_tone[18] = calculate_checksum(set_tone, sizeof(set_tone), 18);

    int i;
    i = usb_bulk_write(handle, 4, set_tone, sizeof(set_tone), TIMEOUT);
    printf("wrote: %d\n", i);
}

/* level = 0 to 99 */
void set_comp_attack(struct usb_dev_handle *handle, int level)
{
    static char set_attack[] = {0x04, 0xF0, 0x00, 0x00, 0x04, 0x10, 0x00, 0x5E, 0x04, 0x02, 0x41, 0x20, 0x04, 0x00, 0x53, 0x04, 0x07, 0x00 /* value */, 0x00 /* checksum */, 0xf7};

    set_attack[17] = level;
    set_attack[18] = calculate_checksum(set_attack, sizeof(set_attack), 18);

    int i;
    i = usb_bulk_write(handle, 4, set_attack, sizeof(set_attack), TIMEOUT);
    printf("wrote: %d\n", i);
}

/* level = 0 to 99 */
void set_comp_level(struct usb_dev_handle *handle, int level)
{
    static char set_level[] = {0x04, 0xF0, 0x00, 0x00, 0x04, 0x10, 0x00, 0x5E, 0x04, 0x02, 0x41, 0x20, 0x04, 0x00, 0x52, 0x04, 0x07, 0x00 /* value */, 0x00 /* checksum */, 0xF7};

    set_level[17] = level;
    set_level[18] = calculate_checksum(set_level, sizeof(set_level), 18);

    int i;
    i = usb_bulk_write(handle, 4, set_level, sizeof(set_level), TIMEOUT);
    printf("wrote: %d\n", i);
}

void set_comp_type(struct usb_dev_handle *handle, int type)
{
    static char set_type[] = {0x04, 0xF0, 0x00, 0x00, 0x04, 0x10, 0x00, 0x5E, 0x04, 0x02, 0x41, 0x2C, 0x04, 0x00, 0x4F, 0x04, 0x04, 0x01, 0x00 /* type */, 0x00 /* checksum */, 0x05, 0xF7, 0x00, 0x00};

    switch (type) {
      case COMP_TYPE_DIGI: set_type[18] = 0x43; break;
      case COMP_TYPE_CS: set_type[18] = 0x44; break;
      default: break;
    }

    set_type[19] = calculate_checksum(set_type, sizeof(set_type), 19) ^ 0x06;

    int i;
    i = usb_bulk_write(handle, 4, set_type, sizeof(set_type), TIMEOUT);
    printf("wrote: %d\n", i);
}

void set_comp_on_off(struct usb_dev_handle *handle, gboolean val)
{
    static char set_comp[] = {0x04, 0xF0, 0x00, 0x00, 0x04, 0x10, 0x00, 0x5E, 0x04, 0x02, 0x41, 0x20, 0x04, 0x00, 0x41, 0x04, 0x07, 0x00 /* on/off */, 0x00 /* checksum */, 0xF7};

    if (val == FALSE) { /* turn comp off */
        set_comp[17] = 0;
    } else { /* turn comp on */
        set_comp[17] = 1;
    }

    set_comp[18] = calculate_checksum(set_comp, sizeof(set_comp), 18);

    int i;
    i = usb_bulk_write(handle, 4, set_comp, sizeof(set_comp), TIMEOUT);
    printf("wrote: %d\n", i);
}

/* x = 0 to 60 */
void switch_user_preset(struct usb_dev_handle *handle, int x)
{
    static char switch_preset[] = {0x04, 0xF0, 0x00, 0x00, 0x04, 0x10, 0x00, 0x5E, 0x04, 0x02, 0x39, 0x00, 0x04, 0x01 /* bank = user */, 0x00 /* no */, 0x04, 0x04, 0x00, 0x00, 0x01, 0x06, 0x00 /* confirm */, 0xF7, 0x00, 0x00};

    switch_preset[14] = x;
    switch_preset[21] = calculate_checksum(switch_preset, sizeof(switch_preset), 21) ^ 0x05;

    int i;
    i = usb_bulk_write(handle, 4, switch_preset, sizeof(switch_preset), TIMEOUT);
    printf("wrote: %d\n", i);
}

/* x = 0 to 60 */
void switch_system_preset(struct usb_dev_handle *handle, int x)
{
    static char switch_preset[] = {0x04, 0xF0, 0x00, 0x00, 0x04, 0x10, 0x00, 0x5E, 0x04, 0x02, 0x39, 0x00, 0x04, 0x00 /* bank = system */, 0x00 /* no */, 0x04, 0x04, 0x00, 0x00, 0x01, 0x06, 0x00 /* confirm */, 0xF7, 0x00, 0x00};

    switch_preset[14] = x;
    switch_preset[21] = calculate_checksum(switch_preset, sizeof(switch_preset), 21) ^ 0x05;

    int i;
    i = usb_bulk_write(handle, 4, switch_preset, sizeof(switch_preset), TIMEOUT);
    printf("wrote: %d\n", i);
}

void set_pickup_type(struct usb_dev_handle *handle, int type)
{
    static char pickup[] = {0x04, 0xF0, 0x00, 0x00, 0x04, 0x10, 0x00, 0x5E, 0x04, 0x02, 0x41, 0x00, 0x04, 0x00, 0x40, 0x02, 0x07, 0x00 /* type1 */, 0x00 /* checksum */, 0xF7};

    switch (type) {
        case PICKUP_TYPE_HB_SC: pickup[17] = 0x42; break;
        case PICKUP_TYPE_SC_HB: pickup[17] = 0x41; break;
        default: break;
    }

    pickup[18] = calculate_checksum(pickup, sizeof(pickup), 18);

    int i;
    i = usb_bulk_write(handle, 4, pickup, sizeof(pickup), TIMEOUT);
    printf("wrote: %d\n", i);
}

void set_pickup_on_off(struct usb_dev_handle *handle, gboolean val)
{
    static char set_pickup[] = {0x04, 0xF0, 0x00, 0x00, 0x04, 0x10, 0x00, 0x5E, 0x04, 0x02, 0x41, 0x00, 0x04, 0x00, 0x41, 0x02, 0x07, 0x00 /* on/off */, 0x00 /* checksum */, 0xF7};

    if (val == FALSE) { /* turn pickup off */
        set_pickup[17] = 0;
    } else { /* turn pickup on */
        set_pickup[17] = 1;
    }

    set_pickup[18] = calculate_checksum(set_pickup, sizeof(set_pickup), 18);

    int i;
    i = usb_bulk_write(handle, 4, set_pickup, sizeof(set_pickup), TIMEOUT);
    printf("wrote: %d\n", i);
}

void set_dist_type(struct usb_dev_handle *handle, int type)
{
    static char set_dist[] = {0x04, 0xF0, 0x00, 0x00, 0x04, 0x10, 0x00, 0x5E, 0x04, 0x02, 0x41, 0x28, 0x04, 0x09, 0x00, 0x06, 0x04, 0x02, 0x05, 0x00 /* type1 */, 0x06, 0x00 /* checksum */, 0xF7, 0x00};

    switch (type) {
        case DIST_TYPE_SCREAMER: set_dist[19] = 0x00; break;
        case DIST_TYPE_808:      set_dist[19] = 0x0C; break;
        case DIST_TYPE_GUYOD:    set_dist[19] = 0x05; break;
        case DIST_TYPE_DOD250:   set_dist[19] = 0x03; break;
        case DIST_TYPE_RODENT:   set_dist[19] = 0x01; break;
        case DIST_TYPE_MX:       set_dist[19] = 0x0B; break;
        case DIST_TYPE_DS:       set_dist[19] = 0x02; break;
        case DIST_TYPE_GRUNGE:   set_dist[19] = 0x07; break;
        case DIST_TYPE_ZONE:     set_dist[19] = 0x09; break;
        case DIST_TYPE_DEATH:    set_dist[19] = 0x0E; break;
        case DIST_TYPE_GONK:     set_dist[19] = 0x0D; break;
        case DIST_TYPE_FUZZY:    set_dist[19] = 0x08; break;
        case DIST_TYPE_MP:       set_dist[19] = 0x04; break;
        default: break;
    }

    set_dist[21] = calculate_checksum(set_dist, sizeof(set_dist), 21) ^ 0x05;

    int i;
    i = usb_bulk_write(handle, 4, set_dist, sizeof(set_dist), TIMEOUT);
    printf("wrote: %d\n", i);
}

void set_dist_option(struct usb_dev_handle *handle, char option, int value)
{
    static char set_option[] = {0x04, 0xF0, 0x00, 0x00, 0x04, 0x10, 0x00, 0x5E, 0x04, 0x02, 0x41, 0x20, 0x04, 0x09, 0x00 /* option */, 0x06, 0x07, 0x00 /* value */, 0x00 /* checksum */, 0xF7};

    set_option[14] = option;
    set_option[17] = value;
    set_option[18] = calculate_checksum(set_option, sizeof(set_option), 18);

    int i;
    i = usb_bulk_write(handle, 4, set_option, sizeof(set_option), TIMEOUT);
    printf("wrote: %d\n", i);
}

void set_dist_on_off(struct usb_dev_handle *handle, gboolean val)
{
    static char set_dist[] = {0x04, 0xF0, 0x00, 0x00, 0x04, 0x10, 0x00, 0x5E, 0x04, 0x02, 0x41, 0x20, 0x04, 0x09, 0x01, 0x06, 0x07, 0x00 /* on/off */, 0x00 /* checksum */, 0xF7};

    if (val == FALSE) { /* turn dist off */
        set_dist[17] = 0;
    } else { /* turn dist on */
        set_dist[17] = 1;
    }

    set_dist[18] = calculate_checksum(set_dist, sizeof(set_dist), 18);

    int i;
    i = usb_bulk_write(handle, 4, set_dist, sizeof(set_dist), TIMEOUT);
    printf("wrote: %d\n", i);
}

/* level = 0 to 99 */
void set_preset_level(struct usb_dev_handle *handle, int level)
{
    static char set_level[] = {0x04, 0xF0, 0x00, 0x00, 0x04, 0x10, 0x00, 0x5E, 0x04, 0x02, 0x41, 0x00, 0x04, 0x0A, 0x42, 0x12, 0x07, 0x00 /* value */, 0x00 /* checksum */, 0xF7};

    set_level[17] = level;
    set_level[18] = calculate_checksum(set_level, sizeof(set_level), 18);

    int i;
    i = usb_bulk_write(handle, 4, set_level, sizeof(set_level), TIMEOUT);
    printf("wrote: %d\n", i);
}

void set_eq_type(struct usb_dev_handle *handle, int type)
{
    static char set_eq[] = {0x04, 0xF0, 0x00, 0x00, 0x04, 0x10, 0x00, 0x5E, 0x04, 0x02, 0x41, 0x2A, 0x04, 0x0C, 0x02, 0x18, 0x04, 0x02, 0x05, 0x00 /* type1 */, 0x06, 0x00 /* checksum */, 0xF7, 0x00};

    switch (type) {
        case EQ_TYPE_BRIGHT:   set_eq[19] = 0x42; break;
        case EQ_TYPE_MIDBOOST: set_eq[19] = 0x40; break;
        case EQ_TYPE_SCOOP:    set_eq[19] = 0x41; break;
        case EQ_TYPE_WARM:     set_eq[19] = 0x43; break;
        default: break;
    }

    set_eq[21] = calculate_checksum(set_eq, sizeof(set_eq), 21) ^ 0x05;

    int i;
    i = usb_bulk_write(handle, 4, set_eq, sizeof(set_eq), TIMEOUT);
    printf("wrote: %d\n", i);
}

/* x = 0 to 99 */
void set_eq_gain(struct usb_dev_handle *handle, int x)
{
    static char set_gain[] = {0x04, 0xF0, 0x00, 0x00, 0x04, 0x10, 0x00, 0x5E, 0x04, 0x02, 0x41, 0x20, 0x04, 0x09, 0x41, 0x08, 0x07, 0x00 /* value */, 0x00 /* checksum */, 0xF7};

    set_gain[17] = x;
    set_gain[18] = calculate_checksum(set_gain, sizeof(set_gain), 18);

    int i;
    i = usb_bulk_write(handle, 4, set_gain, sizeof(set_gain), TIMEOUT);
    printf("wrote: %d\n", i);
}

/* x = 0 to 99 */
void set_eq_level(struct usb_dev_handle *handle, int x)
{
    static char set_level[] = {0x04, 0xF0, 0x00, 0x00, 0x04, 0x10, 0x00, 0x5E, 0x04, 0x02, 0x41, 0x20, 0x04, 0x09, 0x42, 0x08, 0x07, 0x00 /* value */, 0x00 /* checksum */, 0xF7};

    set_level[17] = x;
    set_level[18] = calculate_checksum(set_level, sizeof(set_level), 18);

    int i;
    i = usb_bulk_write(handle, 4, set_level, sizeof(set_level), TIMEOUT);
    printf("wrote: %d\n", i);
}

/* x = 0x00 (-12dB) to 0x18 (12dB) */
void set_eq_bass(struct usb_dev_handle *handle, int x)
{
    static char set_bass[] = {0x04, 0xF0, 0x00, 0x00, 0x04, 0x10, 0x00, 0x5E, 0x04, 0x02, 0x41, 0x20, 0x04, 0x0C, 0x03, 0x18, 0x07, 0x00 /* value */, 0x00 /* checksum */, 0xF7};

    set_bass[17] = x;
    set_bass[18] = calculate_checksum(set_bass, sizeof(set_bass), 18);

    int i;
    i = usb_bulk_write(handle, 4, set_bass, sizeof(set_bass), TIMEOUT);
    printf("wrote: %d\n", i);
}

/* x = 0x00 (-12dB) to 0x18 (12dB) */
void set_eq_mid(struct usb_dev_handle *handle, int x)
{
    static char set_mid[] = {0x04, 0xF0, 0x00, 0x00, 0x04, 0x10, 0x00, 0x5E, 0x04, 0x02, 0x41, 0x20, 0x04, 0x0C, 0x04, 0x18, 0x07, 0x00 /* value */, 0x00 /* checksum */, 0xF7};

    set_mid[17] = x;
    set_mid[18] = calculate_checksum(set_mid, sizeof(set_mid), 18);

    int i;
    i = usb_bulk_write(handle, 4, set_mid, sizeof(set_mid), TIMEOUT);
    printf("wrote: %d\n", i);
}

/*
   x = 0 to 4700 (which transforms into 300-5000 Hz)
   device itself will accept higher value, but X-Edit allows only 300-5000Hz
*/
void set_eq_mid_hz(struct usb_dev_handle *handle, int x)
{
    int i;

    if (x <= 0x7F) {
        static char set_hz[] = {0x04, 0xF0, 0x00, 0x00, 0x04, 0x10, 0x00, 0x5E, 0x04, 0x02, 0x41, 0x20, 0x04, 0x0C, 0x06, 0x18, 0x07, 0x00 /* value */, 0x00 /* checksum */, 0xF7};

        set_hz[17] = x;
        set_hz[18] = calculate_checksum(set_hz, sizeof(set_hz), 18);

        i = usb_bulk_write(handle, 4, set_hz, sizeof(set_hz), TIMEOUT);
    } else {
        x -= 0x80;
        if (x <= 0x7F) {
            static char set_hz[] = {0x04, 0xF0, 0x00, 0x00, 0x04, 0x10, 0x00, 0x5E, 0x04, 0x02, 0x41, 0x2C, 0x04, 0x0C, 0x06, 0x18, 0x04, 0x01, 0x00 /* value */, 0x00 /* checksum */, 0x05, 0xF7, 0x00, 0x00};

            set_hz[18] = x;
            set_hz[19] = x^0x32;

            i = usb_bulk_write(handle, 4, set_hz, sizeof(set_hz), TIMEOUT);
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

            static char set_hz[] = {0x04, 0xF0, 0x00, 0x00, 0x04, 0x10, 0x00, 0x5E, 0x04, 0x02, 0x41, 0x00 /* a */, 0x04, 0x0C, 0x06, 0x18, 0x04, 0x02, 0x00 /* b */, 0x00 /* value */, 0x06, 0x00 /* checksum */, 0xF7, 0x00};

            set_hz[11] = a;
            set_hz[18] = b;
            set_hz[19] = x;
            set_hz[21] = a^b^x^0x1B^set_hz[14];

            i = usb_bulk_write(handle, 4, set_hz, sizeof(set_hz), TIMEOUT);
        }
    }
    printf("wrote: %d\n", i);
}

/* x = 0x00 (-12dB) to 0x18 (12dB) */
void set_eq_treble(struct usb_dev_handle *handle, int x)
{
    static char set_treble[] = {0x04, 0xF0, 0x00, 0x00, 0x04, 0x10, 0x00, 0x5E, 0x04, 0x02, 0x41, 0x20, 0x04, 0x0C, 0x05, 0x18, 0x07, 0x00 /* value */, 0x00 /* checksum */, 0xF7};

    set_treble[17] = x;
    set_treble[18] = calculate_checksum(set_treble, sizeof(set_treble), 18);

    int i;
    i = usb_bulk_write(handle, 4, set_treble, sizeof(set_treble), TIMEOUT);
    printf("wrote: %d\n", i);
}

/*
   x = 0 to 7500 (which transforms into 500-8000 Hz)
   device itself will accept higher value, but X-Edit allows only 500-8000Hz
*/
void set_eq_treb_hz(struct usb_dev_handle *handle, int x)
{
    int i;

    if (x <= 0x7F) {
        static char set_hz[] = {0x04, 0xF0, 0x00, 0x00, 0x04, 0x10, 0x00, 0x5E, 0x04, 0x02, 0x41, 0x20, 0x04, 0x0C, 0x0B, 0x18, 0x07, 0x00 /* value */, 0x00 /* checksum */, 0xF7};

        set_hz[17] = x;
        set_hz[18] = calculate_checksum(set_hz, sizeof(set_hz), 18);

        i = usb_bulk_write(handle, 4, set_hz, sizeof(set_hz), TIMEOUT);
    } else {
        x -= 0x80;
        if (x <= 0x7F) {
            static char set_hz[] = {0x04, 0xF0, 0x00, 0x00, 0x04, 0x10, 0x00, 0x5E, 0x04, 0x02, 0x41, 0x2C, 0x04, 0x0C, 0x0B, 0x18, 0x04, 0x01, 0x00 /* value */, 0x00 /* checksum */, 0x05, 0xF7, 0x00, 0x00};

            set_hz[18] = x;
            set_hz[19] = x^0x3F;

            i = usb_bulk_write(handle, 4, set_hz, sizeof(set_hz), TIMEOUT);
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

            static char set_hz[] = {0x04, 0xF0, 0x00, 0x00, 0x04, 0x10, 0x00, 0x5E, 0x04, 0x02, 0x41, 0x00 /* a */, 0x04, 0x0C, 0x0B, 0x18, 0x04, 0x02, 0x00 /* b */, 0x00 /* value */, 0x06, 0x00 /* checksum */, 0xF7, 0x00};

            set_hz[11] = a;
            set_hz[18] = b;
            set_hz[19] = x;
            set_hz[21] = a^b^x^0x1B^set_hz[14];

            i = usb_bulk_write(handle, 4, set_hz, sizeof(set_hz), TIMEOUT);
        }
    }
    printf("wrote: %d\n", i);
}

void set_eq_on_off(struct usb_dev_handle *handle, gboolean val)
{
    static char set_eq[] = {0x04, 0xF0, 0x00, 0x00, 0x04, 0x10, 0x00, 0x5E, 0x04, 0x02, 0x41, 0x20, 0x04, 0x0C, 0x0C, 0x18, 0x07, 0x00 /* on/off */, 0x00 /* checksum */, 0xF7};

    if (val == FALSE) { /* turn eq off */
        set_eq[17] = 0;
    } else { /* turn eq on */
        set_eq[17] = 1;
    }

    set_eq[18] = calculate_checksum(set_eq, sizeof(set_eq), 18);

    int i;
    i = usb_bulk_write(handle, 4, set_eq, sizeof(set_eq), TIMEOUT);
    printf("wrote: %d\n", i);
}

void set_noisegate_type(struct usb_dev_handle *handle, int type)
{
    static char set_type[] = {0x04, 0xF0, 0x00, 0x00, 0x04, 0x10, 0x00, 0x5E, 0x04, 0x02, 0x41, 0x28, 0x04, 0x02, 0x40, 0x0C, 0x04, 0x02, 0x03, 0x00 /* type1 */, 0x06, 0x00 /* checksum */, 0xF7, 0x00};

    switch (type) {
        case NOISEGATE_GATE: set_type[19] = 0; break;
        case NOISEGATE_SWELL: set_type[19] = 1; break;
        default: break;
    }

    set_type[21] = calculate_checksum(set_type, sizeof(set_type), 21) ^ 0x05;

    int i;
    i = usb_bulk_write(handle, 4, set_type, sizeof(set_type), TIMEOUT);
    printf("wrote: %d\n", i);
}

/* x = 0 to 99 */
void set_gate_option(struct usb_dev_handle *handle, char option, int x)
{
    static char set_option[] = {0x04, 0xF0, 0x00, 0x00, 0x04, 0x10, 0x00, 0x5E, 0x04, 0x02, 0x41, 0x20, 0x04, 0x02, 0x00 /* option */, 0x0C, 0x07, 0x00 /* value */, 0x00 /* checksum */, 0xF7};

    set_option[14] = option;
    set_option[17] = x;
    set_option[18] = calculate_checksum(set_option, sizeof(set_option), 18);

    int i;
    i = usb_bulk_write(handle, 4, set_option, sizeof(set_option), TIMEOUT);
    printf("wrote: %d\n", i);
}

void set_noisegate_on_off(struct usb_dev_handle *handle, gboolean val)
{
    static char set_gate[] = {0x04, 0xF0, 0x00, 0x00, 0x04, 0x10, 0x00, 0x5E, 0x04, 0x02, 0x41, 0x20, 0x04, 0x02, 0x41, 0x0C, 0x07, 0x00 /* on/off */, 0x00 /* checksum */, 0xF7};

    if (val == FALSE) { /* turn noisegate off */
        set_gate[17] = 0;
    } else { /* turn noisegate on */
        set_gate[17] = 1;
    }

    set_gate[18] = calculate_checksum(set_gate, sizeof(set_gate), 18);

    int i;
    i = usb_bulk_write(handle, 4, set_gate, sizeof(set_gate), TIMEOUT);
    printf("wrote: %d\n", i);
}

void set_chorusfx_option(struct usb_dev_handle *handle, char option, int x)
{
    static char set_option[] = {0x04, 0xF0, 0x00, 0x00, 0x04, 0x10, 0x00, 0x5E, 0x04, 0x02, 0x41, 0x00, 0x04, 0x03, 0x00 /* option */, 0x0E, 0x07, 0x00 /* value */, 0x00 /* checksum */, 0xF7};

    set_option[14] = option;
    set_option[17] = x;
    set_option[18] = calculate_checksum(set_option, sizeof(set_option), 18);

    int i;
    i = usb_bulk_write(handle, 4, set_option, sizeof(set_option), TIMEOUT);
    printf("wrote: %d\n", i);
}

void set_flanger_option(struct usb_dev_handle *handle, char option, int x)
{
    static char set_option[] = {0x04, 0xF0, 0x00, 0x00, 0x04, 0x10, 0x00, 0x5E, 0x04, 0x02, 0x41, 0x20, 0x04, 0x03, 0x00 /* option */, 0x0E, 0x07, 0x00 /* value */, 0x00 /* checksum */, 0xF7};

    set_option[14] = option;
    set_option[17] = x;
    set_option[18] = calculate_checksum(set_option, sizeof(set_option), 18);

    int i;
    i = usb_bulk_write(handle, 4, set_option, sizeof(set_option), TIMEOUT);
    printf("wrote: %d\n", i);
}

void set_vibrato_option(struct usb_dev_handle *handle, char option, int x)
{
    static char set_option[] = {0x04, 0xF0, 0x00, 0x00, 0x04, 0x10, 0x00, 0x5E, 0x04, 0x02, 0x41, 0x00, 0x04, 0x05, 0x00 /* option */, 0x0E, 0x07, 0x00 /* value */, 0x00 /* checksum */, 0xF7};

    set_option[14] = option;
    set_option[17] = x;
    set_option[18] = calculate_checksum(set_option, sizeof(set_option), 18);

    int i;
    i = usb_bulk_write(handle, 4, set_option, sizeof(set_option), TIMEOUT);
    printf("wrote: %d\n", i);
}

void set_tremolo_option(struct usb_dev_handle *handle, char option, int x)
{
    static char set_option[] = {0x04, 0xF0, 0x00, 0x00, 0x04, 0x10, 0x00, 0x5E, 0x04, 0x02, 0x41, 0x20, 0x04, 0x04, 0x00 /* option */, 0x0E, 0x07, 0x00 /* value */, 0x00 /* checksum */, 0xF7};

    set_option[14] = option;
    set_option[17] = x;
    set_option[18] = calculate_checksum(set_option, sizeof(set_option), 18);

    int i;
    i = usb_bulk_write(handle, 4, set_option, sizeof(set_option), TIMEOUT);
    printf("wrote: %d\n", i);
}

void set_envelope_option(struct usb_dev_handle *handle, char option, int x)
{
    static char set_option[] = {0x04, 0xF0, 0x00, 0x00, 0x04, 0x10, 0x00, 0x5E, 0x04, 0x02, 0x41, 0x00, 0x04, 0x06, 0x00 /* option */, 0x0E, 0x07, 0x00 /* value */, 0x00 /* checksum */, 0xF7};

    set_option[14] = option;
    set_option[17] = x;
    set_option[18] = calculate_checksum(set_option, sizeof(set_option), 18);

    int i;
    i = usb_bulk_write(handle, 4, set_option, sizeof(set_option), TIMEOUT);
    printf("wrote: %d\n", i);
}

void set_ya_option(struct usb_dev_handle *handle, char option, int x)
{
    static char set_option[] = {0x04, 0xF0, 0x00, 0x00, 0x04, 0x10, 0x00, 0x5E, 0x04, 0x02, 0x41, 0x20, 0x04, 0x05, 0x00 /* option */, 0x0E, 0x07, 0x00 /* value */, 0x00 /* checksum */, 0xF7};

    set_option[14] = option;
    set_option[17] = x;
    set_option[18] = calculate_checksum(set_option, sizeof(set_option), 18);

    int i;
    i = usb_bulk_write(handle, 4, set_option, sizeof(set_option), TIMEOUT);
    printf("wrote: %d\n", i);
}

void set_filter_option(struct usb_dev_handle *handle, char option, int x)
{
    static char set_option[] = {0x04, 0xF0, 0x00, 0x00, 0x04, 0x10, 0x00, 0x5E, 0x04, 0x02, 0x41, 0x20, 0x04, 0x0B, 0x00 /* option */, 0x0E, 0x07, 0x00 /* value */, 0x00 /* checksum */, 0xF7};

    set_option[14] = option;
    set_option[17] = x;
    set_option[18] = calculate_checksum(set_option, sizeof(set_option), 18);

    int i;
    i = usb_bulk_write(handle, 4, set_option, sizeof(set_option), TIMEOUT);
    printf("wrote: %d\n", i);
}

void set_whammy_option(struct usb_dev_handle *handle, char option, int x)
{
    static char set_option[] = {0x04, 0xF0, 0x00, 0x00, 0x04, 0x10, 0x00, 0x5E, 0x04, 0x02, 0x41, 0x00, 0x04, 0x07, 0x00 /* option */, 0x0E, 0x07, 0x00 /* value */, 0x00 /* checksum */, 0xF7};

    set_option[14] = option;
    set_option[17] = x;
    set_option[18] = calculate_checksum(set_option, sizeof(set_option), 18);

    int i;
    i = usb_bulk_write(handle, 4, set_option, sizeof(set_option), TIMEOUT);
    printf("wrote: %d\n", i);
}

void set_pitch_option(struct usb_dev_handle *handle, char option, int x)
{
    static char set_option[] = {0x04, 0xF0, 0x00, 0x00, 0x04, 0x10, 0x00, 0x5E, 0x04, 0x02, 0x41, 0x20, 0x04, 0x06, 0x00 /* option */, 0x0E, 0x07, 0x00 /* value */, 0x00 /* checksum */, 0xF7};

    set_option[14] = option;
    set_option[17] = x;
    set_option[18] = calculate_checksum(set_option, sizeof(set_option), 18);

    int i;
    i = usb_bulk_write(handle, 4, set_option, sizeof(set_option), TIMEOUT);
    printf("wrote: %d\n", i);
}

void set_ips_option(struct usb_dev_handle *handle, char option, int x)
{
    static char set_option[] = {0x04, 0xF0, 0x00, 0x00, 0x04, 0x10, 0x00, 0x5E, 0x04, 0x02, 0x41, 0x20, 0x04, 0x0A, 0x00 /* option */, 0x0E, 0x07, 0x00 /* value */, 0x00 /* checksum */, 0xF7};

    set_option[14] = option;
    set_option[17] = x;
    set_option[18] = calculate_checksum(set_option, sizeof(set_option), 18);

    int i;
    i = usb_bulk_write(handle, 4, set_option, sizeof(set_option), TIMEOUT);
    printf("wrote: %d\n", i);
}

void set_chorusfx_type(struct usb_dev_handle *handle, int type)
{
    static char set_type[] = {0x04, 0xF0, 0x00, 0x00, 0x04, 0x10, 0x00, 0x5E, 0x04, 0x02, 0x41, 0x00 /* type */, 0x04, 0x03, 0x00, 0x0E, 0x04, 0x02, 0x00 /* type */, 0x00 /* type1 */, 0x06, 0x00 /* checksum */, 0xF7, 0x00};

    switch (type) {
        case CHORUS_TYPE_CE: set_type[11] = 0x08; set_type[19] = 0x7B; set_type[18] = 0x03; break;
        case CHORUS_TYPE_DUAL: set_type[11] = 0x08; set_type[19] = 0x79; set_type[18] = 0x03; break;
        case CHORUS_TYPE_MULTI: set_type[11] = 0x08; set_type[19] = 0x7A; set_type[18] = 0x03; break;
        case CHORUS_TYPE_FLANGER: set_type[11] = 0x08; set_type[19] = 0x7D; set_type[18] = 0x03; break;
        case CHORUS_TYPE_MXR_FLANGER: set_type[11] = 0x08; set_type[19] = 0x7F; set_type[18] = 0x03; break;
        case CHORUS_TYPE_PHASER: set_type[11] = 0x0A; set_type[19] = 0x01; set_type[18] = 0x03; break;
        case CHORUS_TYPE_VIBRATO: set_type[11] = 0x08; set_type[19] = 0x60; set_type[18] = 0x03; break;
        case CHORUS_TYPE_ROTARY: set_type[11] = 0x08; set_type[19] = 0x61; set_type[18] = 0x03; break;
        case CHORUS_TYPE_VIBROPAN: set_type[11] = 0x0A; set_type[19] = 0x0F; set_type[18] = 0x03; break;
        case CHORUS_TYPE_TREMOLO: set_type[11] = 0x08; set_type[19] = 0x5E; set_type[18] = 0x03; break;
        case CHORUS_TYPE_PANNER: set_type[11] = 0x08; set_type[19] = 0x5F; set_type[18] = 0x03; break;
        case CHORUS_TYPE_ENVELOPE: set_type[11] = 0x0A; set_type[19] = 0x0A; set_type[18] = 0x03; break;
        case CHORUS_TYPE_AUTOYA: set_type[11] = 0x0A; set_type[19] = 0x0B; set_type[18] = 0x03; break;
        case CHORUS_TYPE_YAYA: set_type[11] = 0x0A; set_type[19] = 0x0C; set_type[18] = 0x03; break;
        case CHORUS_TYPE_STEP_FILTER: set_type[11] = 0x0A; set_type[19] = 0x0D; set_type[18] = 0x03; break;
        case CHORUS_TYPE_WHAMMY: set_type[11] = 0x08; set_type[19] = 0x40; set_type[18] = 0x05; break;
        case CHORUS_TYPE_PITCH_SHIFT: set_type[11] = 0x08; set_type[19] = 0x43; set_type[18] = 0x05; break;
        case CHORUS_TYPE_DETUNE: set_type[11] = 0x08; set_type[19] = 0x42; set_type[18] = 0x05; break;
        case CHORUS_TYPE_IPS: set_type[11] = 0x08; set_type[19] = 0x41; set_type[18] = 0x05; break;
        default: break;
    }

    set_type[21] = calculate_checksum(set_type, sizeof(set_type), 21) ^ 0x05;

    int i;
    i = usb_bulk_write(handle, 4, set_type, sizeof(set_type), TIMEOUT);
    printf("wrote: %d\n", i);
}

void set_chorusfx_on_off(struct usb_dev_handle *handle, gboolean val)
{
    static char set_chorus[] = {0x04, 0xF0, 0x00, 0x00, 0x04, 0x10, 0x00, 0x5E, 0x04, 0x02, 0x41, 0x00, 0x04, 0x03, 0x01, 0x0E, 0x07, 0x00 /* on/off */, 0x00 /* checksum */, 0xF7};

    if (val == FALSE) { /* turn chorusfx off */
        set_chorus[17] = 0;
    } else { /* turn chorusfx on */
        set_chorus[17] = 1;
    }

    set_chorus[18] = calculate_checksum(set_chorus, sizeof(set_chorus), 18);

    int i;
    i = usb_bulk_write(handle, 4, set_chorus, sizeof(set_chorus), TIMEOUT);
    printf("wrote: %d\n", i);
}

/* x = 0 to 139 */
void set_delay_time(struct usb_dev_handle *handle, int x)
{
    int i;

    if (x <= 0x7F) { /* "short" message format */
        static char set_time[] = {0x04, 0xF0, 0x00, 0x00, 0x04, 0x10, 0x00, 0x5E, 0x04, 0x02, 0x41, 0x00, 0x04, 0x07, 0x60, 0x0F, 0x07, 0x00 /* value */, 0x00 /* checksum */, 0xF7};

        set_time[17] = x;
        set_time[18] = calculate_checksum(set_time, sizeof(set_time), 18);

        i = usb_bulk_write(handle, 4, set_time, sizeof(set_time), TIMEOUT);
    } else { /* "long" message format */
        static char set_time[] = {0x04, 0xF0, 0x00, 0x00, 0x04, 0x10, 0x00, 0x5E, 0x04, 0x02, 0x41, 0x0C, 0x04, 0x07, 0x60, 0x0F, 0x04, 0x01, 0x00 /* value */, 0x00 /* checksum */, 0x05, 0xF7, 0x00, 0x00};

        set_time[18] = x - 0x80;
        set_time[19] = calculate_checksum(set_time, sizeof(set_time), 19) ^ 0x06;

        i = usb_bulk_write(handle, 4, set_time, sizeof(set_time), TIMEOUT);
    }
    printf("wrote: %d\n", i);
}

void set_delay_type(struct usb_dev_handle *handle, int type)
{
    static char set_type[] = {0x04, 0xF0, 0x00, 0x00, 0x04, 0x10, 0x00, 0x5E, 0x04, 0x02, 0x41, 0x08, 0x04, 0x07, 0x40, 0x0F, 0x04, 0x02, 0x04, 0x00 /* type1 */, 0x06, 0x00 /* checksum */, 0xF7, 0x00};

    switch (type) {
        case DELAY_TYPE_ANALOG: set_type[19] = 0x16; break;
        case DELAY_TYPE_DIGITAL: set_type[19] = 0x15; break;
        case DELAY_TYPE_MODULATED: set_type[19] = 0x17; break;
        case DELAY_TYPE_PONG: set_type[19] = 0x18; break;
        case DELAY_TYPE_TAPE: set_type[19] = 0x19; break;
        default: break;
    }

    set_type[21] = calculate_checksum(set_type, sizeof(set_type), 21) ^ 0x05;

    int i;
    i = usb_bulk_write(handle, 4, set_type, sizeof(set_type), TIMEOUT);
    printf("wrote: %d\n", i);
}

void set_delay_option(struct usb_dev_handle *handle, char option, int x)
{
    static char set_option[] = {0x04, 0xF0, 0x00, 0x00, 0x04, 0x10, 0x00, 0x5E, 0x04, 0x02, 0x41, 0x00, 0x04, 0x07, 0x00 /* option */, 0x0F, 0x07, 0x00 /* value */, 0x00 /* checksum */, 0xF7};

    set_option[14] = option;
    set_option[17] = x;
    set_option[18] = calculate_checksum(set_option, sizeof(set_option), 18);

    int i;
    i = usb_bulk_write(handle, 4, set_option, sizeof(set_option), TIMEOUT);
    printf("wrote: %d\n", i);
}

void set_delay_on_off(struct usb_dev_handle *handle, gboolean val)
{
    static char set_delay[] = {0x04, 0xF0, 0x00, 0x00, 0x04, 0x10, 0x00, 0x5E, 0x04, 0x02, 0x41, 0x00, 0x04, 0x07, 0x41, 0x0F, 0x07, 0x00 /* on/off */, 0x00 /* checksum */, 0xF7};

    if (val == FALSE) { /* turn delay off */
        set_delay[17] = 0;
    } else { /* turn delay on */
        set_delay[17] = 1;
    }

    set_delay[18] = calculate_checksum(set_delay, sizeof(set_delay), 18);

    int i;
    i = usb_bulk_write(handle, 4, set_delay, sizeof(set_delay), TIMEOUT);
    printf("wrote: %d\n", i);
}

/* x = 0 to 15 (predelay), otherwise 0 to 99 */
void set_reverb_option(struct usb_dev_handle *handle, char option, int x)
{
    static char set_option[] = {0x04, 0xF0, 0x00, 0x00, 0x04, 0x10, 0x00, 0x5E, 0x04, 0x02, 0x41, 0x20, 0x04, 0x07, 0x00 /* option */, 0x10, 0x07, 0x00 /* value */, 0x00 /* checksum */, 0xF7};

    set_option[14] = option;
    set_option[17] = x;
    set_option[18] = calculate_checksum(set_option, sizeof(set_option), 18);

    int i;
    i = usb_bulk_write(handle, 4, set_option, sizeof(set_option), TIMEOUT);
    printf("wrote: %d\n", i);
}

void set_reverb_type(struct usb_dev_handle *handle, int type)
{
    static char set_type[] = {0x04, 0xF0, 0x00, 0x00, 0x04, 0x10, 0x00, 0x5E, 0x04, 0x02, 0x41, 0x28, 0x04, 0x07, 0x00, 0x10, 0x04, 0x02, 0x04, 0x00 /* type1 */, 0x06, 0x00 /* checksum */, 0xF7, 0x00};

    switch (type) {
        case REVERB_TYPE_TWIN: set_type[19] = 0x7A; break;
        case REVERB_TYPE_LEX_AMBIENCE: set_type[19] = 0x7E; break;
        case REVERB_TYPE_LEX_STUDIO: set_type[19] = 0x7D; break;
        case REVERB_TYPE_LEX_ROOM: set_type[19] = 0x7C; break;
        case REVERB_TYPE_LEX_HALL: set_type[19] = 0x7B; break;
        case REVERB_TYPE_EMT240_PLATE: set_type[19] = 0x7F; break;
        default: break;
    }

    set_type[21] = calculate_checksum(set_type, sizeof(set_type), 21) ^ 0x05;

    int i;
    i = usb_bulk_write(handle, 4, set_type, sizeof(set_type), TIMEOUT);
    printf("wrote: %d\n", i);
}

void set_reverb_on_off(struct usb_dev_handle *handle, gboolean val)
{
    static char set_reverb[] = {0x04, 0xF0, 0x00, 0x00, 0x04, 0x10, 0x00, 0x5E, 0x04, 0x02, 0x41, 0x20, 0x04, 0x07, 0x01, 0x10, 0x07, 0x00 /* on/off */, 0x00 /* checksum */, 0xF7};

    if (val == FALSE) { /* turn reverb off */
        set_reverb[17] = 0;
    } else { /* turn reverb on */
        set_reverb[17] = 1;
    }

    set_reverb[18] = calculate_checksum(set_reverb, sizeof(set_reverb), 18);

    int i;
    i = usb_bulk_write(handle, 4, set_reverb, sizeof(set_reverb), TIMEOUT);
    printf("wrote: %d\n", i);
}

/* x = 0 to 59 (preset number) */
void set_preset_name(struct usb_dev_handle *handle, int x, gchar *name)
{
    static char set_name[] = {0x04, 0xF0, 0x00, 0x00, 0x04, 0x10, 0x00, 0x5E, 0x04, 0x02, 0x29, 0x00, 0x04, 0x01, 0x00 /* preset no */, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    int write; /* number of bytes to write */

    set_name[14] = x;

    if (name == NULL || strlen(name) == 0) {
        set_name[15] = 0x00;
        set_name[16] = 0x06;
        set_name[18] = 0xF7;
        set_name[19] = 0x00;

        set_name[17] = calculate_checksum(set_name, 20, 17) ^ 0x01;

        write = 20;
    } else {
        static int v[] = {15, 17, 18, 19, 21, 23, 25, 26, 27, 29};

        int a;
        for (a=0; a<strlen(name) && a<10; a++) {
            set_name[v[a]] = name[a];
        }

        switch (strlen(name)) {
            case 1: set_name[16] = 0x07;
                    set_name[17] = 0x00;
                    set_name[19] = 0xF7;
                    set_name[18] = calculate_checksum(set_name, 20, 18);
                    write = 20;
                    break;
            case 2: set_name[16] = 0x04;
                    set_name[18] = 0x00;
                    set_name[20] = 0x05;
                    set_name[21] = 0xF7;
                    set_name[22] = 0x00;
                    set_name[23] = 0x00;
                    set_name[19] = calculate_checksum(set_name, 24, 19) ^ 0x06;
                    write = 24;
                    break;
            case 3: set_name[16] = 0x04;
                    set_name[19] = 0x00;
                    set_name[20] = 0x06;
                    set_name[22] = 0xF7;
                    set_name[23] = 0x00;
                    set_name[21] = calculate_checksum(set_name, 24, 21) ^ 0x05;
                    write = 24;
                    break;
            case 4: set_name[16] = 0x04;
                    set_name[20] = 0x07;
                    set_name[21] = 0x00;
                    set_name[23] = 0xF7;
                    set_name[22] = calculate_checksum(set_name, 24, 22) ^ 0x04;
                    write = 24;
                    break;
            case 5: set_name[16] = 0x04;
                    set_name[20] = 0x04;
                    set_name[22] = 0x00;
                    set_name[23] = 0x00;
                    set_name[24] = 0x06;
                    set_name[26] = 0xF7;
                    set_name[27] = 0x00;
                    set_name[25] = calculate_checksum(set_name, 28, 25) ^ 0x01;
                    write = 28;
                    break;
            case 6: set_name[16] = 0x04;
                    set_name[20] = 0x04;
                    set_name[22] = 0x00;
                    set_name[24] = 0x07;
                    set_name[25] = 0x00;
                    set_name[27] = 0xF7;
                    set_name[26] = calculate_checksum(set_name, 28, 26);
                    write = 28;
                    break;
            case 7: set_name[16] = 0x04;
                    set_name[20] = 0x04;
                    set_name[22] = 0x00;
                    set_name[24] = 0x04;
                    set_name[26] = 0x00;
                    set_name[28] = 0x05;
                    set_name[29] = 0xF7;
                    set_name[30] = 0x00;
                    set_name[31] = 0x00;
                    set_name[27] = calculate_checksum(set_name, 32, 27) ^ 0x06;
                    write = 32;
                    break;
            case 8: set_name[16] = 0x04;
                    set_name[20] = 0x04;
                    set_name[22] = 0x00;
                    set_name[24] = 0x04;
                    set_name[27] = 0x00;
                    set_name[28] = 0x06;
                    set_name[30] = 0xF7;
                    set_name[31] = 0x00;
                    set_name[29] = calculate_checksum(set_name, 32, 29) ^ 0x05;
                    write = 32;
                    break;
            case 9: set_name[16] = 0x04;
                    set_name[20] = 0x04;
                    set_name[22] = 0x00;
                    set_name[24] = 0x04;
                    set_name[28] = 0x07;
                    set_name[29] = 0x00;
                    set_name[31] = 0xF7;
                    set_name[30] = calculate_checksum(set_name, 32, 30) ^ 0x04;
                    write = 32;
                    break;
            case 10:set_name[16] = 0x04;
                    set_name[20] = 0x04;
                    set_name[22] = 0x00;
                    set_name[24] = 0x04;
                    set_name[28] = 0x04;
                    set_name[30] = 0x00;
                    set_name[32] = 0x05;
                    set_name[33] = 0xF7;
                    set_name[34] = 0x00;
                    set_name[35] = 0x00;
                    set_name[31] = calculate_checksum(set_name, 36, 31) ^ 0x02;
                    write = 36;
                    break;
        }
    }

    int i;
    i = usb_bulk_write(handle, 4, set_name, write, TIMEOUT);
    printf("wrote: %d\n", i);
}

int main(int argc, char **argv) {
    gtk_init(&argc, &argv);

    struct usb_bus *busses;

    usb_init();
    usb_find_busses();
    usb_find_devices();

    busses = usb_get_busses();

    struct usb_bus *bus;

    for (bus = busses; bus; bus = bus->next) {
        int result;
        gboolean found = FALSE;

        for (dev = bus->devices; dev && !found; dev = dev->next) {
            if (dev->descriptor.idVendor == 0x1210) {
                found = TRUE;
                handle = usb_open(dev);

                result = usb_get_string_simple(handle, dev->descriptor.iProduct, buf, sizeof(buf));
                printf("%d %s\n", result, buf);

                result = usb_claim_interface(handle, 0);
                if (result == 0) printf ("claim resulted with %d\n", result);

                /*int i = usb_bulk_write(handle, 4, presets_system, sizeof(presets_system), TIMEOUT);
                printf("wrote: %d\n", i);*/
                /* now have to read from device,
                   first packet appears to be smaller,
                   then are 12 bytes long packets,
                   last one is smaller */
            }
        }
    }

    if (handle != NULL) {
        create_window();
        gtk_main();

        if (usb_close(handle))
            printf("Error closing usb handle!");
    }

    return 0;
}
