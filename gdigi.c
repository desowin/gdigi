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

#include <usb.h>
#include <stdio.h>
#include <glib.h>
#include <gtk/gtk.h>

#define TIMEOUT 1000

char buf[255];

struct usb_device *dev;
struct usb_dev_handle *handle;

static char magic[] = {0x04, 0xF0, 0x00, 0x00, 0x04, 0x10, 0x7F, 0x7F, 0x04, 0x7F, 0x01, 0x00, 0x04, 0x00, 0x00, 0x00, 0x06, 0x6E, 0xF7, 0x00}; /* causes COMAND to be displayed on device? */
static char presets_user[] =   {0x04, 0xF0, 0x00, 0x00, 0x04, 0x10, 0x00, 0x5E, 0x04, 0x02, 0x21, 0x00, 0x07, 0x01, 0x6C, 0xF7}; /* this command seems to query user presets? */
static char presets_system[] = {0x04, 0xF0, 0x00, 0x00, 0x04, 0x10, 0x00, 0x5E, 0x04, 0x02, 0x21, 0x00, 0x07, 0x00, 0x6D, 0xF7}; /* this command seems to query system presets? */

static char magic3[] = {0x04, 0xF0, 0x00, 0x00, 0x04, 0x10, 0x00, 0x5E, 0x04, 0x02, 0x2A, 0x00, 0x04, 0x04, 0x00, 0x62, 0x05, 0xF7, 0x00, 0x00}; /* seems to query active preset name */
static char magic4[] = {0x04, 0xF0, 0x00, 0x00, 0x04, 0x10, 0x00, 0x5E, 0x04, 0x02, 0x2B, 0x00, 0x04, 0x00, 0x01, 0x77, 0x05, 0xF7, 0x00, 0x00};
static char magic5[] = {0x04, 0xF0, 0x00, 0x00, 0x04, 0x10, 0x00, 0x5E, 0x04, 0x02, 0x3A, 0x00, 0x04, 0x00, 0x01, 0x77, 0x05, 0xF7, 0x00, 0x00};


/* xedit calls those after calling hbsc pickup change */
static char magic12[] = {0x04, 0xF0, 0x00, 0x00, 0x04, 0x10, 0x00, 0x5E, 0x04, 0x02, 0x3A, 0x00, 0x04, 0x00, 0x01, 0x77, 0x05, 0xF7, 0x00, 0x00};
static char magic13[] = {0x07, 0x41, 0x73, 0xF7, 0x04, 0x10, 0x00, 0x5E, 0x04, 0x02, 0x7E, 0x00, 0x04, 0x00, 0x01, 0x77, 0x05, 0xF7, 0x00, 0x00};

/* xedit calls those after calling schb pickup change */
static char magic22[] = {0x04, 0xF0, 0x00, 0x00, 0x04, 0x10, 0x00, 0x5E, 0x04, 0x02, 0x41, 0x00, 0x04, 0x00, 0x01, 0x77, 0x05, 0xF7, 0x00, 0x00};
static char magic23[] = {0x04, 0xF0, 0x00, 0x00, 0x04, 0x10, 0x00, 0x5E, 0x04, 0x02, 0x3A, 0x00, 0x04, 0x00, 0x01, 0x77, 0x05, 0xF7, 0x00, 0x00};
static char magic24[] = {0x04, 0xF0, 0x00, 0x00, 0x04, 0x10, 0x00, 0x5E, 0x04, 0x02, 0x3B, 0x00, 0x04, 0x00, 0x00, 0x00, 0x06, 0x6E, 0xF7, 0x00};

/* seems to be called after changing dist type */
static char magic8[] = {0x04, 0xF0, 0x00, 0x00, 0x04, 0x10, 0x00, 0x5E, 0x04, 0x02, 0x3A, 0x00, 0x04, 0x00, 0x01, 0x77, 0x05, 0xF7, 0x00, 0x00};
/* X-edit also calls 2 different commands, but those seems to vary each time */


int read_device(struct usb_dev_handle *handle, int bytes)
{
    int i, x;
    i = usb_bulk_read(handle, 132, buf, bytes, TIMEOUT);
    //printf("Called read %d. Device returned %d bytes\n", bytes, i);
    for (x=0; x<i; x++) {
       //printf("0x%02x(%c) ", buf[x], buf[x] ? buf[x] : 32);
    }

    //printf("\n");
    if (bytes > 0) return i;
    else return 0;
}

int check_preset(struct usb_dev_handle *handle)
{
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

enum {
  WAH_TYPE_CRY = 0,
  WAH_TYPE_FULLRANGE,
  WAH_TYPE_CLYDE
};

void set_wah_type(struct usb_dev_handle *handle, int type)
{
    static char set_type[] = {0x04, 0xF0, 0x00, 0x00, 0x04, 0x10, 0x00, 0x5E, 0x04, 0x02, 0x41, 0x2C, 0x04, 0x00, 0x00, 0x03, 0x04, 0x01, 0x00 /* type */, 0x00 /* confirm */, 0x05, 0xF7, 0x00, 0x00};

    switch (type) {
      case WAH_TYPE_CRY: set_type[18] = 4; set_type[19] = 0x27; break;
      case WAH_TYPE_FULLRANGE: set_type[18] = 5; set_type[19] = 0x26; break;
      case WAH_TYPE_CLYDE: set_type[18] = 6; set_type[19] = 0x25; break;
      default: break;
    }

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
    static char set_sust[] = {0x04, 0xF0, 0x00, 0x00, 0x04, 0x10, 0x00, 0x5E, 0x04, 0x02, 0x41, 0x20, 0x04, 0x00, 0x50, 0x04, 0x07, 0x00 /* value */, 0x00 /* checksum */, 0xf7};

    set_sust[17] = level;
    set_sust[18] = calculate_checksum(set_sust, sizeof(set_sust), 18);

    int i;
    i = usb_bulk_write(handle, 4, set_sust, sizeof(set_sust), TIMEOUT);
    printf("wrote: %d\n", i);
}

/* level = 0 to 99, available only in digi comp */
void set_comp_tone(struct usb_dev_handle *handle, int level)
{
    static char set_tone[] = {0x04, 0xF0, 0x00, 0x00, 0x04, 0x10, 0x00, 0x5E, 0x04, 0x02, 0x41, 0x20, 0x04, 0x00, 0x51, 0x04, 0x07, 0x00 /* value */, 0x00 /* checksum */, 0xf7};

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
    static char set_level[] = {0x04, 0xF0, 0x00, 0x00, 0x04, 0x10, 0x00, 0x5E, 0x04, 0x02, 0x41, 0x20, 0x04, 0x00, 0x52, 0x04, 0x07, 0x00 /* value */, 0x00 /* checksum */, 0xf7};

    set_level[17] = level;
    set_level[18] = calculate_checksum(set_level, sizeof(set_level), 18);

    int i;
    i = usb_bulk_write(handle, 4, set_level, sizeof(set_level), TIMEOUT);
    printf("wrote: %d\n", i);
}

enum {
  COMP_TYPE_DIGI = 0,
  COMP_TYPE_CS
};

void set_comp_type(struct usb_dev_handle *handle, int type)
{
    static char set_type[] = {0x04, 0xF0, 0x00, 0x00, 0x04, 0x10, 0x00, 0x5E, 0x04, 0x02, 0x41, 0x2C, 0x04, 0x00, 0x4F, 0x04, 0x04, 0x01, 0x00 /* type */, 0x00 /* checksum */, 0x05, 0xF7, 0x00, 0x00};

    switch (type) {
      case COMP_TYPE_DIGI: set_type[18] = 0x43; set_type[19] = 0x28; break;
      case COMP_TYPE_CS: set_type[18] = 0x44; set_type[19] = 0x2F; break;
      default: break;
    }

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
    int val;

    switch_preset[14] = x;
    val = (7 - ((x & 0xf0) >> 4)) << 4;
    val += (x & 0x0f);
    if (((x & 0x0f) % 2) == 1) {
        val -= 1;
    } else {
        val += 1;
    }
    switch_preset[21] = val;

    int i;
    i = usb_bulk_write(handle, 4, switch_preset, sizeof(switch_preset), TIMEOUT);
    printf("wrote: %d\n", i);
}

/* x = 0 to 60 */
void switch_system_preset(struct usb_dev_handle *handle, int x)
{
    static char switch_preset[] = {0x04, 0xF0, 0x00, 0x00, 0x04, 0x10, 0x00, 0x5E, 0x04, 0x02, 0x39, 0x00, 0x04, 0x00 /* bank = system */, 0x00 /* no */, 0x04, 0x04, 0x00, 0x00, 0x01, 0x06, 0x00 /* confirm */, 0xF7, 0x00, 0x00};
    int val;

    switch_preset[14] = x;
    val = (7 - ((x & 0xf0) >> 4)) << 4;
    val += (x & 0x0f);
    switch_preset[21] = val;

    int i;
    i = usb_bulk_write(handle, 4, switch_preset, sizeof(switch_preset), TIMEOUT);
    printf("wrote: %d\n", i);
}

enum {
  PICKUP_TYPE_HB_SC = 0,
  PICKUP_TYPE_SC_HB
};

void set_pickup_type(struct usb_dev_handle *handle, int type)
{
    static char pickup[] = {0x04, 0xF0, 0x00, 0x00, 0x04, 0x10, 0x00, 0x5E, 0x04, 0x02, 0x41, 0x00, 0x04, 0x00, 0x40, 0x02, 0x07, 0x00 /* type1 */, 0x00 /* type2 */, 0xF7};

    switch (type) {
        case PICKUP_TYPE_HB_SC: pickup[17] = 0x42; pickup[18] = 0x0D; break;
        case PICKUP_TYPE_SC_HB: pickup[17] = 0x41; pickup[18] = 0x0E; break;
        default: break;
    }

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

enum {
  DIST_TYPE_SCREAMER = 0,
  DIST_TYPE_808,
  DIST_TYPE_GUYOD,
  DIST_TYPE_DOD250,
  DIST_TYPE_RODENT,
  DIST_TYPE_MX,
  DIST_TYPE_DS,
  DIST_TYPE_GRUNGE,
  DIST_TYPE_ZONE,
  DIST_TYPE_DEATH,
  DIST_TYPE_GONK,
  DIST_TYPE_FUZZY,
  DIST_TYPE_MP
};

void set_dist_type(struct usb_dev_handle *handle, int type)
{
    static char set_dist[] = {0x04, 0xF0, 0x00, 0x00, 0x04, 0x10, 0x00, 0x5E, 0x04, 0x02, 0x41, 0x28, 0x04, 0x09, 0x00, 0x06, 0x04, 0x02, 0x05, 0x00 /* type1 */, 0x06, 0x00 /* type2 */, 0xF7, 0x00};

    switch (type) {
        case DIST_TYPE_SCREAMER: set_dist[19] = 0x00; set_dist[21] = 0x2D; break;
        case DIST_TYPE_808:      set_dist[19] = 0x0C; set_dist[21] = 0x21; break;
        case DIST_TYPE_GUYOD:    set_dist[19] = 0x05; set_dist[21] = 0x28; break;
        case DIST_TYPE_DOD250:   set_dist[19] = 0x03; set_dist[21] = 0x2E; break;
        case DIST_TYPE_RODENT:   set_dist[19] = 0x01; set_dist[21] = 0x2C; break;
        case DIST_TYPE_MX:       set_dist[19] = 0x0B; set_dist[21] = 0x26; break;
        case DIST_TYPE_DS:       set_dist[19] = 0x02; set_dist[21] = 0x2F; break;
        case DIST_TYPE_GRUNGE:   set_dist[19] = 0x07; set_dist[21] = 0x2A; break;
        case DIST_TYPE_ZONE:     set_dist[19] = 0x09; set_dist[21] = 0x24; break;
        case DIST_TYPE_DEATH:    set_dist[19] = 0x0E; set_dist[21] = 0x23; break;
        case DIST_TYPE_GONK:     set_dist[19] = 0x0D; set_dist[21] = 0x20; break;
        case DIST_TYPE_FUZZY:    set_dist[19] = 0x08; set_dist[21] = 0x25; break;
        case DIST_TYPE_MP:       set_dist[19] = 0x04; set_dist[21] = 0x29; break;
        default: break;
    }

    int i;
    i = usb_bulk_write(handle, 4, set_dist, sizeof(set_dist), TIMEOUT);
    printf("wrote: %d\n", i);
}

#define DIST_SCREAMER_DRIVE 0x02
#define DIST_SCREAMER_TONE  0x03
#define DIST_SCREAMER_LVL   0x04
#define DIST_808_OVERDRIVE  0x29
#define DIST_808_TONE       0x2A
#define DIST_808_LVL        0x2B
#define DIST_GUYOD_DRIVE    0x10
#define DIST_GUYOD_LVL      0x11
#define DIST_DOD250_GAIN    0x0B
#define DIST_DOD250_LVL     0x0C
#define DIST_RODENT_DIST    0x05
#define DIST_RODENT_FILTER  0x06
#define DIST_RODENT_LVL     0x07
#define DIST_MX_DIST        0x24
#define DIST_MX_OUTPUT      0x25
#define DIST_DS_GAIN        0x08
#define DIST_DS_TONE        0x09
#define DIST_DS_LVL         0x0A
#define DIST_GRUNGE_GRUNGE  0x16
#define DIST_GRUNGE_FACE    0x18
#define DIST_GRUNGE_LOUD    0x19
#define DIST_GRUNGE_BUTT    0x17
#define DIST_ZONE_GAIN      0x1C
#define DIST_ZONE_LOW       0x1F
#define DIST_ZONE_MID_LVL   0x1E
#define DIST_ZONE_MID_FREQ  0x1D
#define DIST_ZONE_HIGH      0x20
#define DIST_ZONE_LEVEL     0x21
#define DIST_DEATH_LOW      0x2D
#define DIST_DEATH_MID      0x2C
#define DIST_DEATH_HIGH     0x2F
#define DIST_DEATH_LVL      0x2E
#define DIST_GONK_GONK      0x30
#define DIST_GONK_SMEAR     0x27
#define DIST_GONK_SUCK      0x26
#define DIST_GONK_HEAVE     0x28
#define DIST_FUZZY_FUZZ     0x1A
#define DIST_FUZZY_VOLUME   0x1B
#define DIST_MP_SUSTAIN     0x0D
#define DIST_MP_TONE        0x0E
#define DIST_MP_VOLUME      0x0F

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

enum {
  NOISEGATE_GATE = 0,
  NOISEGATE_SWELL
};

void set_noisegate_type(struct usb_dev_handle *handle, int type)
{
    static char set_type[] = {0x04, 0xF0, 0x00, 0x00, 0x04, 0x10, 0x00, 0x5E, 0x04, 0x02, 0x41, 0x28, 0x04, 0x02, 0x40, 0x0C, 0x04, 0x02, 0x03, 0x00 /* type1 */, 0x06, 0x00 /* type2 */, 0xF7, 0x00};

    switch (type) {
        case NOISEGATE_GATE: set_type[19] = 0; set_type[21] = 0x6A; break;
        case NOISEGATE_SWELL: set_type[19] = 1; set_type[21] = 0x6B; break;
        default: break;
    }

    int i;
    i = usb_bulk_write(handle, 4, set_type, sizeof(set_type), TIMEOUT);
    printf("wrote: %d\n", i);
}

/* available only in Gate mode */
#define NOISEGATE_GATE_TRESHOLD 0x46
/* available only in Swell mode */
#define NOISEGATE_SWELL_SENS    0x47
/* available in both Gate and Swell modes */
#define NOISEGATE_ATTACK        0x48
#define NOISEGATE_RELEASE       0x49
#define NOISEGATE_ATTN          0x4A

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

#define CE_CHORUS_SPEED 0x45
#define CE_CHORUS_DEPTH 0x46
#define DUAL_CHORUS_SPEED 0x45
#define DUAL_CHORUS_DEPTH 0x46
#define DUAL_CHORUS_LEVEL 0x44
#define DUAL_CHORUS_WAVE 0x48
/* DUAL_CHORUS_WAVE, MULTI_CHORUS_WAVE, FLANGER_WAVE, PHASER_WAVE and VIBROPAN_WAVE valid values */
#define WAVE_TRI    0x00
#define WAVE_SINE   0x01
#define WAVE_SQUARE 0x02

#define MULTI_CHORUS_SPEED 0x45
#define MULTI_CHORUS_DEPTH 0x46
#define MULTI_CHORUS_WAVE  0x48
#define MULTI_CHORUS_LEVEL 0x44

#define FLANGER_SPEED 0x06
#define FLANGER_DEPTH 0x07
#define FLANGER_REGEN 0x08
#define FLANGER_LEVEL 0x05
#define FLANGER_WAVE 0x09

#define MXR_FLANGER_SPEED 0x06
#define MXR_FLANGER_WIDTH 0x12
#define MXR_FLANGER_REGEN 0x08
#define MXR_FLANGER_MANUAL 0x15

#define PHASER_SPEED 0x42
#define PHASER_DEPTH 0x43
#define PHASER_REGEN 0x46
#define PHASER_LEVEL 0x45
#define PHASER_WAVE 0x47

#define VIBRATO_SPEED 0x04
#define VIBRATO_DEPTH 0x05

#define ROTARY_SPEED 0x42
#define ROTARY_INTENSITY 0x44
#define ROTARY_DOPPLER 0x46
#define ROTARY_CROSSOVER 0x47

#define VIBROPAN_SPEED 0x22
#define VIBROPAN_DEPTH 0x23
#define VIBROPAN_VIBRA 0x24
#define VIBROPAN_WAVE 0x25

#define TREMOLO_SPEED 0x04
#define TREMOLO_DEPTH 0x03
#define TREMOLO_WAVE 0x05

#define PANNER_SPEED 0x44
#define PANNER_DEPTH 0x43
#define PANNER_WAVE 0x45

#define ENVELOPE_SENSITIVITY 0x46
#define ENVELOPE_RANGE 0x45

#define AUTOYA_SPEED 0x46
#define AUTOYA_INTENSITY 0x4A
#define AUTOYA_RANGE 0x4B

#define YAYA_PEDAL 0x02
#define YAYA_INTENSITY 0x09
#define YAYA_RANGE 0x0A

#define STEP_FILTER_SPEED 0x42
#define STEP_FILTER_INTENSITY 0x43

#define WHAMMY_AMOUNT 0x05
#define WHAMMY_PEDAL 0x03
#define WHAMMY_MIX 0x04
/* WHAMMY_AMOUNT valid values */
#define WHAMMY_OCT_UP 0x00
#define WHAMMY_2OCT_UP 0x01
#define WHAMMY_2ND_DN 0x02
#define WHAMMY_RV_2ND 0x03
#define WHAMMY_4TH_DN 0x04
#define WHAMMY_OCT_DN 0x05
#define WHAMMY_2OCT_DN 0x06
#define WHAMMY_DIV_BMB 0x07
#define WHAMMY_M3_MA 0x08
#define WHAMMY_2ND_MA3 0x09
#define WHAMMY_3RD_4TH 0x0A
#define WHAMMY_4TH_5TH 0x0B
#define WHAMMY_5TH_OCT 0x0C
#define WHAMMY_HOCT_UP 0x0D
#define WHAMMY_HOCT_DN 0x0E
#define WHAMMY_OCT_UD 0x0F

#define PITCH_AMOUNT 0x42
#define PITCH_MIX 0x51

#define DETUNE_AMOUNT 0x04
#define DETUNE_LEVEL 0x03

#define IPS_SHIFT_AMOUNT 0x42
#define IPS_KEY 0x44
#define IPS_SCALE 0x43
#define IPS_LEVEL 0x45

/* IPS_SHIFT_AMOUNT valid values */
#define IPS_OCT_D 0x00
#define IPS_7TH_DN 0x01
#define IPS_6TH_DN 0x02
#define IPS_5TH_DN 0x03
#define IPS_4TH_DN 0x04
#define IPS_3RD_DN 0x05
#define IPS_2ND_DN 0x06
#define IPS_2ND_UP 0x07
#define IPS_3RD_UP 0x08
#define IPS_4TH_UP 0x09
#define IPS_5TH_UP 0x0A
#define IPS_6TH_UP 0x0B
#define IPS_7TH_UP 0x0C
#define IPS_OCT_U 0x0D

/* IPS_KEY valid values */
#define IPS_E 0x00
#define IPS_F 0x01
#define IPS_GB 0x02
#define IPS_G 0x03
#define IPS_AB 0x04
#define IPS_A 0x05
#define IPS_BB 0x06
#define IPS_B 0x07
#define IPS_C 0x08
#define IPS_DD 0x09
#define IPS_D 0x0A
#define IPS_EB 0x0B

/* IPS_SCALE valid values */
#define IPS_MAJOR 0x00
#define IPS_MINOR 0x01
#define IPS_DORIA 0x02
#define IPS_MIXLYD 0x03
#define IPS_LYDIAN 0x04
#define IPS_HMINO 0x05

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

enum {
  CHORUS_TYPE_CE = 0,
  CHORUS_TYPE_DUAL,
  CHORUS_TYPE_MULTI,
  CHORUS_TYPE_FLANGER,
  CHORUS_TYPE_MXR_FLANGER,
  CHORUS_TYPE_PHASER,
  CHORUS_TYPE_VIBRATO,
  CHORUS_TYPE_ROTARY,
  CHORUS_TYPE_VIBROPAN,
  CHORUS_TYPE_TREMOLO,
  CHORUS_TYPE_PANNER,
  CHORUS_TYPE_ENVELOPE,
  CHORUS_TYPE_AUTOYA,
  CHORUS_TYPE_YAYA,
  CHORUS_TYPE_STEP_FILTER,
  CHORUS_TYPE_WHAMMY,
  CHORUS_TYPE_PITCH_SHIFT,
  CHORUS_TYPE_DETUNE,
  CHORUS_TYPE_IPS
};

void set_chorusfx_type(struct usb_dev_handle *handle, int type)
{
    static char set_type[] = {0x04, 0xF0, 0x00, 0x00, 0x04, 0x10, 0x00, 0x5E, 0x04, 0x02, 0x41, 0x00 /* type */, 0x04, 0x03, 0x00, 0x0E, 0x04, 0x02, 0x00 /* type */, 0x00 /* type1 */, 0x06, 0x00 /* type2 */, 0xF7, 0x00};

    switch (type) {
        case CHORUS_TYPE_CE: set_type[11] = 0x08; set_type[19] = 0x7B; set_type[18] = 0x03; set_type[21] = 0x72; break;
        case CHORUS_TYPE_DUAL: set_type[11] = 0x08; set_type[19] = 0x79; set_type[18] = 0x03; set_type[21] = 0x70; break;
        case CHORUS_TYPE_MULTI: set_type[11] = 0x08; set_type[19] = 0x7A; set_type[18] = 0x03; set_type[21] = 0x73; break;
        case CHORUS_TYPE_FLANGER: set_type[11] = 0x08; set_type[19] = 0x7D; set_type[18] = 0x03; set_type[21] = 0x74; break;
        case CHORUS_TYPE_MXR_FLANGER: set_type[11] = 0x08; set_type[19] = 0x7F; set_type[18] = 0x03; set_type[21] = 0x76; break;
        case CHORUS_TYPE_PHASER: set_type[11] = 0x0A; set_type[19] = 0x01; set_type[18] = 0x03; set_type[21] = 0x0A; break;
        case CHORUS_TYPE_VIBRATO: set_type[11] = 0x08; set_type[19] = 0x60; set_type[18] = 0x03; set_type[21] = 0x69; break;
        case CHORUS_TYPE_ROTARY: set_type[11] = 0x08; set_type[19] = 0x61; set_type[18] = 0x03; set_type[21] = 0x68; break;
        case CHORUS_TYPE_VIBROPAN: set_type[11] = 0x0A; set_type[19] = 0x0F; set_type[18] = 0x03; set_type[21] = 0x04; break;
        case CHORUS_TYPE_TREMOLO: set_type[11] = 0x08; set_type[19] = 0x5E; set_type[18] = 0x03; set_type[21] = 0x57; break;
        case CHORUS_TYPE_PANNER: set_type[11] = 0x08; set_type[19] = 0x5F; set_type[18] = 0x03; set_type[21] = 0x56; break;
        case CHORUS_TYPE_ENVELOPE: set_type[11] = 0x0A; set_type[19] = 0x0A; set_type[18] = 0x03; set_type[21] = 0x01; break;
        case CHORUS_TYPE_AUTOYA: set_type[11] = 0x0A; set_type[19] = 0x0B; set_type[18] = 0x03; set_type[21] = 0x00; break;
        case CHORUS_TYPE_YAYA: set_type[11] = 0x0A; set_type[19] = 0x0C; set_type[18] = 0x03; set_type[21] = 0x07; break;
        case CHORUS_TYPE_STEP_FILTER: set_type[11] = 0x0A; set_type[19] = 0x0D; set_type[18] = 0x03; set_type[21] = 0x06; break;
        case CHORUS_TYPE_WHAMMY: set_type[11] = 0x08; set_type[19] = 0x40; set_type[18] = 0x05; set_type[21] = 0x4F; break;
        case CHORUS_TYPE_PITCH_SHIFT: set_type[11] = 0x08; set_type[19] = 0x43; set_type[18] = 0x05; set_type[21] = 0x4C; break;
        case CHORUS_TYPE_DETUNE: set_type[11] = 0x08; set_type[19] = 0x42; set_type[18] = 0x05; set_type[21] = 0x4D; break;
        case CHORUS_TYPE_IPS: set_type[11] = 0x08; set_type[19] = 0x41; set_type[18] = 0x05; set_type[21] = 0x4E; break;
        default: break;
    }

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
        set_time[19] = set_time[18] ^ 0x68;

        i = usb_bulk_write(handle, 4, set_time, sizeof(set_time), TIMEOUT);
    }
    printf("wrote: %d\n", i);
}

enum {
  DELAY_TYPE_ANALOG = 0,
  DELAY_TYPE_DIGITAL,
  DELAY_TYPE_MODULATED,
  DELAY_TYPE_PONG,
  DELAY_TYPE_TAPE
};

void set_delay_type(struct usb_dev_handle *handle, int type)
{
    static char set_type[] = {0x04, 0xF0, 0x00, 0x00, 0x04, 0x10, 0x00, 0x5E, 0x04, 0x02, 0x41, 0x08, 0x04, 0x07, 0x40, 0x0F, 0x04, 0x02, 0x04, 0x00 /* type1 */, 0x06, 0x00 /* type2 */, 0xF7, 0x00};

    switch (type) {
        case DELAY_TYPE_ANALOG: set_type[19] = 0x16; set_type[21] = 0x5D; break;
        case DELAY_TYPE_DIGITAL: set_type[19] = 0x15; set_type[21] = 0x5E; break;
        case DELAY_TYPE_MODULATED: set_type[19] = 0x17; set_type[21] = 0x5C; break;
        case DELAY_TYPE_PONG: set_type[19] = 0x18; set_type[21] = 0x53; break;
        case DELAY_TYPE_TAPE: set_type[19] = 0x19; set_type[21] = 0x52; break;
        default: break;
    }

    int i;
    i = usb_bulk_write(handle, 4, set_type, sizeof(set_type), TIMEOUT);
    printf("wrote: %d\n", i);
}

#define ANALOG_LEVEL 0x44
#define ANALOG_REPEATS 0x47
#define DIGITAL_LEVEL 0x44
#define DIGITAL_REPEATS 0x47
#define DIGITAL_DUCKER_THRESH 0x61
#define DIGITAL_DUCKER_LEVEL 0x62
#define MODULATED_LEVEL 0x44
#define MODULATED_REPEATS 0x47
#define MODULATED_DEPTH 0x51
#define PONG_LEVEL 0x44
#define PONG_REPEATS 0x47
#define PONG_DUCKER_THRESH 0x61
#define PONG_DUCKER_LEVEL 0x62
#define TAPE_LEVEL 0x44
#define TAPE_REPEATS 0x47
#define TAPE_WOW 0x63
#define TAPE_FLUTTER 0x64

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

#define TWIN_REVERB 0x05
#define LEX_AMBIENCE_PREDELAY 0x02
#define LEX_AMBIENCE_DECAY 0x07
#define LEX_AMBIENCE_LIVELINESS 0x0D
#define LEX_AMBIENCE_LEVEL 0x05
#define LEX_STUDIO_PREDELAY 0x02
#define LEX_STUDIO_DECAY 0x07
#define LEX_STUDIO_LIVELINESS 0x0D
#define LEX_STUDIO_LEVEL 0x05
#define LEX_ROOM_PREDELAY 0x02
#define LEX_ROOM_DECAY 0x07
#define LEX_ROOM_LIVELINESS 0x0D
#define LEX_ROOM_LEVEL 0x05
#define LEX_HALL_PREDELAY 0x02
#define LEX_HALL_DECAY 0x07
#define LEX_HALL_LIVELINESS 0x0D
#define LEX_HALL_LEVEL 0x05
#define EMT240_PLATE_PREDELAY 0x02
#define EMT240_PLATE_DECAY 0x07
#define EMT240_PLATE_LIVELINESS 0x0D
#define EMT240_PLATE_LEVEL 0x05

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

enum {
  REVERB_TYPE_TWIN = 0,
  REVERB_TYPE_LEX_AMBIENCE,
  REVERB_TYPE_LEX_STUDIO,
  REVERB_TYPE_LEX_ROOM,
  REVERB_TYPE_LEX_HALL,
  REVERB_TYPE_EMT240_PLATE
};

void set_reverb_type(struct usb_dev_handle *handle, int type)
{
    static char set_type[] = {0x04, 0xF0, 0x00, 0x00, 0x04, 0x10, 0x00, 0x5E, 0x04, 0x02, 0x41, 0x28, 0x04, 0x07, 0x00, 0x10, 0x04, 0x02, 0x04, 0x00 /* type1 */, 0x06, 0x00 /* type2 */, 0xF7, 0x00};

    switch (type) {
        case REVERB_TYPE_TWIN: set_type[19] = 0x7A; set_type[21] = 0x4E; break;
        case REVERB_TYPE_LEX_AMBIENCE: set_type[19] = 0x7E; set_type[21] = 0x4A; break;
        case REVERB_TYPE_LEX_STUDIO: set_type[19] = 0x7D; set_type[21] = 0x49; break;
        case REVERB_TYPE_LEX_ROOM: set_type[19] = 0x7C; set_type[21] = 0x48; break;
        case REVERB_TYPE_LEX_HALL: set_type[19] = 0x7B; set_type[21] = 0x4F; break;
        case REVERB_TYPE_EMT240_PLATE: set_type[19] = 0x7F; set_type[21] = 0x4B; break;
        default: break;
    }

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

void value_changed_cb(GtkSpinButton *spinbutton, void (*callback)(struct usb_dev_handle*, int))
{
    int val = gtk_spin_button_get_value_as_int(spinbutton);
    callback(handle, val);
}

void toggled_cb(GtkToggleButton *button, void (*callback)(struct usb_dev_handle*, gboolean))
{
    gboolean val = gtk_toggle_button_get_active(button);
    callback(handle, val);
}

typedef struct {
    char *label;
    void (*callback)(struct usb_dev_handle*, int);
    gdouble min;
    gdouble max;
} SettingsWidget;

static SettingsWidget wah_widgets[] = {
    {"Wah min", set_wah_min, 0.0, 99.0},
    {"Wah max", set_wah_max, 0.0, 99.0},
    {"Wah level", set_wah_level, 0.0, 12.0},
};

static SettingsWidget comp_widgets[] = {
    {"Compressor sustain", set_comp_sustain, 0.0, 99.0},
    {"Compressor tone (digi only!)", set_comp_tone, 0.0, 99.0},
    {"Compressor attack", set_comp_attack, 0.0, 99.0},
    {"Compressor level", set_comp_level, 0.0, 99.0},
};

GtkWidget *create_table(SettingsWidget *widgets, gint amt)
{
    GtkWidget *table, *label, *widget;
    GtkObject *adj;
    int x;

    table = gtk_table_new(2, amt, FALSE);

    for (x = 0; x<amt; x++) {
        label = gtk_label_new(widgets[x].label);
        adj = gtk_adjustment_new(0.0, widgets[x].min, widgets[x].max, 1.0, 1.0, 1.0);
        widget = gtk_spin_button_new(GTK_ADJUSTMENT(adj), 1.0, 0);
        g_signal_connect(G_OBJECT(widget), "value-changed", G_CALLBACK(value_changed_cb), widgets[x].callback);

        gtk_table_attach(GTK_TABLE(table), label, 0, 1, x, x+1, GTK_SHRINK, GTK_SHRINK, 2, 2);
        gtk_table_attach(GTK_TABLE(table), widget, 1, 2, x, x+1, GTK_SHRINK, GTK_SHRINK, 2, 2);
    }

    return table;
}

GtkWidget *create_on_off_button(const gchar *label, gboolean state, void (*callback)(struct usb_dev_handle*, int))
{
    GtkWidget *button = gtk_toggle_button_new_with_label(label);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button), state);
    g_signal_connect(G_OBJECT(button), "toggled", G_CALLBACK(toggled_cb), callback);
    return button;
}

void create_window()
{
    GtkWidget *window;
    GtkWidget *vbox;
    GtkWidget *wah_vbox;
    GtkWidget *comp_vbox;
    GtkWidget *table;
    GtkWidget *widget;

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

    vbox = gtk_vbox_new(FALSE, 0);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    wah_vbox = gtk_vbox_new(FALSE, 0);
    gtk_container_add(GTK_CONTAINER(vbox), wah_vbox);

    widget = create_on_off_button("Wah", FALSE, set_wah_on_off);
    gtk_container_add(GTK_CONTAINER(wah_vbox), widget);

    table = create_table(wah_widgets, G_N_ELEMENTS(wah_widgets));
    gtk_container_add(GTK_CONTAINER(wah_vbox), table);

    comp_vbox = gtk_vbox_new(FALSE, 0);
    gtk_container_add(GTK_CONTAINER(vbox), comp_vbox);

    widget = create_on_off_button("Compressor", FALSE, set_comp_on_off);
    gtk_container_add(GTK_CONTAINER(comp_vbox), widget);

    table = create_table(comp_widgets, G_N_ELEMENTS(comp_widgets));
    gtk_container_add(GTK_CONTAINER(comp_vbox), table);

    gtk_widget_show_all(window);

    g_signal_connect(G_OBJECT(window), "delete_event", G_CALLBACK(gtk_main_quit), NULL);
}

void test_all(struct usb_dev_handle *handle)
{
    int x;
    set_wah_type(handle, WAH_TYPE_CRY);
    for (x=0; x<=99; x++)
        set_wah_min(handle, x);

    for (x=0; x<=99; x++)
        set_wah_max(handle, x);

    for (x=0; x<=12; x++)
        set_wah_level(handle, x);

    set_wah_type(handle, WAH_TYPE_FULLRANGE);
    for (x=0; x<=99; x++)
        set_wah_min(handle, x);

    for (x=0; x<=99; x++)
        set_wah_max(handle, x);

    for (x=0; x<=12; x++)
        set_wah_level(handle, x);

    set_wah_type(handle, WAH_TYPE_CLYDE);
    for (x=0; x<=99; x++)
        set_wah_min(handle, x);

    for (x=0; x<=99; x++)
        set_wah_max(handle, x);

    for (x=0; x<=12; x++)
        set_wah_level(handle, x);

    set_comp_type(handle, COMP_TYPE_DIGI);
    for (x=0; x<=99; x++)
        set_comp_sustain(handle, x);

    for (x=0; x<=99; x++)
        set_comp_tone(handle, x);

    for (x=0; x<=99; x++)
        set_comp_attack(handle, x);

    for (x=0; x<=99; x++)
        set_comp_level(handle, x);

    set_comp_type(handle, COMP_TYPE_CS);
    for (x=0; x<=99; x++)
        set_comp_sustain(handle, x);

    for (x=0; x<=99; x++)
        set_comp_attack(handle, x);

    for (x=0; x<=99; x++)
        set_comp_level(handle, x);

    set_dist_on_off(handle, TRUE);
    set_dist_on_off(handle, FALSE);

    set_dist_type(handle, DIST_TYPE_SCREAMER);
    for (x=0; x<=99; x++)
        set_dist_option(handle, DIST_SCREAMER_DRIVE, x);
    for (x=0; x<=99; x++)
        set_dist_option(handle, DIST_SCREAMER_TONE, x);
    for (x=0; x<=99; x++)
        set_dist_option(handle, DIST_SCREAMER_LVL, x);

    set_dist_type(handle, DIST_TYPE_808);
    for (x=0; x<=99; x++)
        set_dist_option(handle, DIST_808_OVERDRIVE, x);
    for (x=0; x<=99; x++)
        set_dist_option(handle, DIST_808_TONE, x);
    for (x=0; x<=99; x++)
        set_dist_option(handle, DIST_808_LVL, x);

    set_dist_type(handle, DIST_TYPE_GUYOD);
    for (x=0; x<=99; x++)
        set_dist_option(handle, DIST_GUYOD_DRIVE, x);
    for (x=0; x<=99; x++)
        set_dist_option(handle, DIST_GUYOD_LVL, x);

    set_dist_type(handle, DIST_TYPE_DOD250);
    for (x=0; x<=99; x++)
        set_dist_option(handle, DIST_DOD250_GAIN, x);
    for (x=0; x<=99; x++)
        set_dist_option(handle, DIST_DOD250_LVL, x);

    set_dist_type(handle, DIST_TYPE_RODENT);
    for (x=0; x<=99; x++)
        set_dist_option(handle, DIST_RODENT_DIST, x);
    for (x=0; x<=99; x++)
        set_dist_option(handle, DIST_RODENT_FILTER, x);
    for (x=0; x<=99; x++)
        set_dist_option(handle, DIST_RODENT_LVL, x);

    set_dist_type(handle, DIST_TYPE_MX);
    for (x=0; x<=99; x++)
        set_dist_option(handle, DIST_MX_DIST, x);
    for (x=0; x<=99; x++)
        set_dist_option(handle, DIST_MX_OUTPUT, x);

    set_dist_type(handle, DIST_TYPE_DS);
    for (x=0; x<=99; x++)
        set_dist_option(handle, DIST_DS_GAIN, x);
    for (x=0; x<=99; x++)
        set_dist_option(handle, DIST_DS_TONE, x);
    for (x=0; x<=99; x++)
        set_dist_option(handle, DIST_DS_LVL, x);

    set_dist_type(handle, DIST_TYPE_GRUNGE);
    for (x=0; x<=99; x++)
        set_dist_option(handle, DIST_GRUNGE_GRUNGE, x);
    for (x=0; x<=99; x++)
        set_dist_option(handle, DIST_GRUNGE_FACE, x);
    for (x=0; x<=99; x++)
        set_dist_option(handle, DIST_GRUNGE_LOUD, x);
    for (x=0; x<=99; x++)
        set_dist_option(handle, DIST_GRUNGE_BUTT, x);

    set_dist_type(handle, DIST_TYPE_ZONE);
    for (x=0; x<=99; x++)
        set_dist_option(handle, DIST_ZONE_GAIN, x);
    for (x=0; x<=99; x++)
        set_dist_option(handle, DIST_ZONE_LOW, x);
    for (x=0; x<=99; x++)
        set_dist_option(handle, DIST_ZONE_MID_LVL, x);
    for (x=0; x<=99; x++)
        set_dist_option(handle, DIST_ZONE_MID_FREQ, x);
    for (x=0; x<=99; x++)
        set_dist_option(handle, DIST_ZONE_HIGH, x);
    for (x=0; x<=99; x++)
        set_dist_option(handle, DIST_ZONE_LEVEL, x);

    set_dist_type(handle, DIST_TYPE_DEATH);
    for (x=0; x<=99; x++)
        set_dist_option(handle, DIST_DEATH_LOW, x);
    for (x=0; x<=99; x++)
        set_dist_option(handle, DIST_DEATH_MID, x);
    for (x=0; x<=99; x++)
        set_dist_option(handle, DIST_DEATH_HIGH, x);
    for (x=0; x<=99; x++)
        set_dist_option(handle, DIST_DEATH_LVL, x);

    set_dist_type(handle, DIST_TYPE_GONK);
    for (x=0; x<=99; x++)
        set_dist_option(handle, DIST_GONK_GONK, x);
    for (x=0; x<=99; x++)
        set_dist_option(handle, DIST_GONK_SMEAR, x);
    for (x=0; x<=99; x++)
        set_dist_option(handle, DIST_GONK_SUCK, x);
    for (x=0; x<=99; x++)
        set_dist_option(handle, DIST_GONK_HEAVE, x);

    set_dist_type(handle, DIST_TYPE_FUZZY);
    for (x=0; x<=99; x++)
        set_dist_option(handle, DIST_FUZZY_FUZZ, x);
    for (x=0; x<=99; x++)
        set_dist_option(handle, DIST_FUZZY_VOLUME, x);

    set_dist_type(handle, DIST_TYPE_MP);
    for (x=0; x<=99; x++)
        set_dist_option(handle, DIST_MP_SUSTAIN, x);
    for (x=0; x<=99; x++)
        set_dist_option(handle, DIST_MP_TONE, x);
    for (x=0; x<=99; x++)
        set_dist_option(handle, DIST_MP_VOLUME, x);

    for (x=0; x<=60; x++)
        switch_user_preset(handle, x);

    for (x=0; x<=60; x++)
        switch_system_preset(handle, x);

    set_pickup_on_off(handle, TRUE);
    set_pickup_on_off(handle, FALSE);

    for (x=0; x<=99; x++)
        set_preset_level(handle, x);

    for (x=0; x<=99; x++)
        set_eq_gain(handle, x);
    for (x=0; x<=99; x++)
        set_eq_level(handle, x);

    for (x=0; x<=0x18; x++)
        set_eq_bass(handle, x);
    for (x=0; x<=0x18; x++)
        set_eq_mid(handle, x);
    for (x=0; x<=0x18; x++)
        set_eq_treble(handle, x);

    set_eq_on_off(handle, TRUE);
    set_eq_on_off(handle, FALSE);

    set_noisegate_type(handle, NOISEGATE_GATE);
    for (x=0; x<=99; x++)
        set_gate_option(handle, NOISEGATE_GATE_TRESHOLD, x);
    for (x=0; x<=99; x++)
        set_gate_option(handle, NOISEGATE_ATTACK, x);
    for (x=0; x<=99; x++)
        set_gate_option(handle, NOISEGATE_RELEASE, x);
    for (x=0; x<=99; x++)
        set_gate_option(handle, NOISEGATE_ATTN, x);

    set_noisegate_type(handle, NOISEGATE_SWELL);
    for (x=0; x<=99; x++)
        set_gate_option(handle, NOISEGATE_SWELL_SENS, x);
    for (x=0; x<=99; x++)
        set_gate_option(handle, NOISEGATE_ATTACK, x);
    for (x=0; x<=99; x++)
        set_gate_option(handle, NOISEGATE_RELEASE, x);
    for (x=0; x<=99; x++)
        set_gate_option(handle, NOISEGATE_ATTN, x);

    set_noisegate_on_off(handle, TRUE);
    set_noisegate_on_off(handle, FALSE);

    set_chorusfx_type(handle, CHORUS_TYPE_CE);
    for (x=0; x<=99; x++)
        set_chorusfx_option(handle, CE_CHORUS_SPEED, x);
    for (x=0; x<=99; x++)
        set_chorusfx_option(handle, CE_CHORUS_DEPTH, x);

    set_chorusfx_type(handle, CHORUS_TYPE_DUAL);
    for (x=0; x<=99; x++)
        set_chorusfx_option(handle, DUAL_CHORUS_SPEED, x);
    for (x=0; x<=99; x++)
        set_chorusfx_option(handle, DUAL_CHORUS_DEPTH, x);
    for (x=0; x<=99; x++)
        set_chorusfx_option(handle, DUAL_CHORUS_LEVEL, x);
    set_chorusfx_option(handle, DUAL_CHORUS_WAVE, WAVE_TRI);
    set_chorusfx_option(handle, DUAL_CHORUS_WAVE, WAVE_SINE);
    set_chorusfx_option(handle, DUAL_CHORUS_WAVE, WAVE_SQUARE);

    set_chorusfx_type(handle, CHORUS_TYPE_MULTI);
    for (x=0; x<=99; x++)
        set_chorusfx_option(handle, MULTI_CHORUS_SPEED, x);
    for (x=0; x<=99; x++)
        set_chorusfx_option(handle, MULTI_CHORUS_DEPTH, x);
    for (x=0; x<=99; x++)
        set_chorusfx_option(handle, MULTI_CHORUS_LEVEL, x);
    set_chorusfx_option(handle, MULTI_CHORUS_WAVE, WAVE_TRI);
    set_chorusfx_option(handle, MULTI_CHORUS_WAVE, WAVE_SINE);
    set_chorusfx_option(handle, MULTI_CHORUS_WAVE, WAVE_SQUARE);

    set_chorusfx_type(handle, CHORUS_TYPE_FLANGER);
    for (x=0; x<=99; x++)
        set_flanger_option(handle, FLANGER_SPEED, x);
    for (x=0; x<=99; x++)
        set_flanger_option(handle, FLANGER_DEPTH, x);
    for (x=0; x<=99; x++)
        set_flanger_option(handle, FLANGER_REGEN, x);
    for (x=0; x<=99; x++)
        set_flanger_option(handle, FLANGER_LEVEL, x);
    set_flanger_option(handle, FLANGER_WAVE, WAVE_TRI);
    set_flanger_option(handle, FLANGER_WAVE, WAVE_SINE);
    set_flanger_option(handle, FLANGER_WAVE, WAVE_SQUARE);

    set_chorusfx_type(handle, CHORUS_TYPE_MXR_FLANGER);
    for (x=0; x<=99; x++)
        set_flanger_option(handle, MXR_FLANGER_SPEED, x);
    for (x=0; x<=99; x++)
        set_flanger_option(handle, MXR_FLANGER_WIDTH, x);
    for (x=0; x<=99; x++)
        set_flanger_option(handle, MXR_FLANGER_REGEN, x);
    for (x=0; x<=99; x++)
        set_flanger_option(handle, MXR_FLANGER_MANUAL, x);

    set_chorusfx_type(handle, CHORUS_TYPE_PHASER);
    for (x=0; x<=99; x++)
        set_flanger_option(handle, PHASER_SPEED, x);
    for (x=0; x<=99; x++)
        set_flanger_option(handle, PHASER_DEPTH, x);
    for (x=0; x<=99; x++)
        set_flanger_option(handle, PHASER_REGEN, x);
    for (x=0; x<=99; x++)
        set_flanger_option(handle, PHASER_LEVEL, x);
    set_flanger_option(handle, PHASER_WAVE, WAVE_TRI);
    set_flanger_option(handle, PHASER_WAVE, WAVE_SINE);
    set_flanger_option(handle, PHASER_WAVE, WAVE_SQUARE);

    set_chorusfx_type(handle, CHORUS_TYPE_VIBRATO);
    for (x=0; x<=99; x++)
        set_vibrato_option(handle, VIBRATO_SPEED, x);
    for (x=0; x<=99; x++)
        set_vibrato_option(handle, VIBRATO_DEPTH, x);

    set_chorusfx_type(handle, CHORUS_TYPE_ROTARY);
    for (x=0; x<=99; x++)
        set_vibrato_option(handle, ROTARY_SPEED, x);
    for (x=0; x<=99; x++)
        set_vibrato_option(handle, ROTARY_INTENSITY, x);
    for (x=0; x<=99; x++)
        set_vibrato_option(handle, ROTARY_DOPPLER, x);
    for (x=0; x<=99; x++)
        set_vibrato_option(handle, ROTARY_CROSSOVER, x);

    set_chorusfx_type(handle, CHORUS_TYPE_VIBROPAN);
    for (x=0; x<=99; x++)
        set_vibrato_option(handle, VIBROPAN_SPEED, x);
    for (x=0; x<=99; x++)
        set_vibrato_option(handle, VIBROPAN_DEPTH, x);
    for (x=0; x<=99; x++)
        set_vibrato_option(handle, VIBROPAN_VIBRA, x);
    set_vibrato_option(handle, VIBROPAN_WAVE, WAVE_TRI);
    set_vibrato_option(handle, VIBROPAN_WAVE, WAVE_SINE);
    set_vibrato_option(handle, VIBROPAN_WAVE, WAVE_SQUARE);

    set_chorusfx_type(handle, CHORUS_TYPE_TREMOLO);
    for (x=0; x<=99; x++)
        set_tremolo_option(handle, TREMOLO_SPEED, x);
    for (x=0; x<=99; x++)
        set_tremolo_option(handle, TREMOLO_DEPTH, x);
    set_tremolo_option(handle, TREMOLO_WAVE, WAVE_TRI);
    set_tremolo_option(handle, TREMOLO_WAVE, WAVE_SINE);
    set_tremolo_option(handle, TREMOLO_WAVE, WAVE_SQUARE);

    set_chorusfx_type(handle, CHORUS_TYPE_PANNER);
    for (x=0; x<=99; x++)
        set_tremolo_option(handle, PANNER_SPEED, x);
    for (x=0; x<=99; x++)
        set_tremolo_option(handle, PANNER_DEPTH, x);
    set_tremolo_option(handle, PANNER_WAVE, WAVE_TRI);
    set_tremolo_option(handle, PANNER_WAVE, WAVE_SINE);
    set_tremolo_option(handle, PANNER_WAVE, WAVE_SQUARE);

    set_chorusfx_type(handle, CHORUS_TYPE_ENVELOPE);
    for (x=0; x<=99; x++)
        set_envelope_option(handle, ENVELOPE_SENSITIVITY, x);
    for (x=0; x<=99; x++)
        set_envelope_option(handle, ENVELOPE_RANGE, x);

    set_chorusfx_type(handle, CHORUS_TYPE_AUTOYA);
    for (x=0; x<=99; x++)
        set_ya_option(handle, AUTOYA_SPEED, x);
    for (x=0; x<=99; x++)
        set_ya_option(handle, AUTOYA_INTENSITY, x);
    for (x=0; x<=0x31; x++)
        set_ya_option(handle, AUTOYA_RANGE, x);

    set_chorusfx_type(handle, CHORUS_TYPE_YAYA);
    for (x=0; x<=99; x++)
        set_ya_option(handle, YAYA_PEDAL, x);
    for (x=0; x<=99; x++)
        set_ya_option(handle, YAYA_INTENSITY, x);
    for (x=0; x<=0x31; x++)
        set_ya_option(handle, YAYA_RANGE, x);

    set_chorusfx_type(handle, CHORUS_TYPE_STEP_FILTER);
    for (x=0; x<=99; x++)
        set_filter_option(handle, STEP_FILTER_SPEED, x);
    for (x=0; x<=99; x++)
        set_filter_option(handle, STEP_FILTER_INTENSITY, x);

    set_chorusfx_type(handle, CHORUS_TYPE_WHAMMY);
    set_whammy_option(handle, WHAMMY_AMOUNT, WHAMMY_OCT_UP);
    set_whammy_option(handle, WHAMMY_AMOUNT, WHAMMY_2OCT_UP);
    set_whammy_option(handle, WHAMMY_AMOUNT, WHAMMY_2ND_DN);
    set_whammy_option(handle, WHAMMY_AMOUNT, WHAMMY_RV_2ND);
    set_whammy_option(handle, WHAMMY_AMOUNT, WHAMMY_4TH_DN);
    set_whammy_option(handle, WHAMMY_AMOUNT, WHAMMY_OCT_DN);
    set_whammy_option(handle, WHAMMY_AMOUNT, WHAMMY_2OCT_DN);
    set_whammy_option(handle, WHAMMY_AMOUNT, WHAMMY_DIV_BMB);
    set_whammy_option(handle, WHAMMY_AMOUNT, WHAMMY_M3_MA);
    set_whammy_option(handle, WHAMMY_AMOUNT, WHAMMY_2ND_MA3);
    set_whammy_option(handle, WHAMMY_AMOUNT, WHAMMY_3RD_4TH);
    set_whammy_option(handle, WHAMMY_AMOUNT, WHAMMY_4TH_5TH);
    set_whammy_option(handle, WHAMMY_AMOUNT, WHAMMY_5TH_OCT);
    set_whammy_option(handle, WHAMMY_AMOUNT, WHAMMY_HOCT_UP);
    set_whammy_option(handle, WHAMMY_AMOUNT, WHAMMY_HOCT_DN);
    set_whammy_option(handle, WHAMMY_AMOUNT, WHAMMY_OCT_UD);
    for (x=0; x<=99; x++)
        set_whammy_option(handle, WHAMMY_PEDAL, x);
    for (x=0; x<=99; x++)
        set_whammy_option(handle, WHAMMY_MIX, x);

    set_chorusfx_type(handle, CHORUS_TYPE_PITCH_SHIFT);
    for (x=0; x<=0x30; x++)
        set_pitch_option(handle, PITCH_AMOUNT, x);
    for (x=0; x<=99; x++)
        set_pitch_option(handle, PITCH_MIX, x);

    set_chorusfx_type(handle, CHORUS_TYPE_DETUNE);
    for (x=0; x<=0x30; x++)
        set_pitch_option(handle, DETUNE_AMOUNT, x);
    for (x=0; x<=99; x++)
        set_pitch_option(handle, DETUNE_LEVEL, x);

    set_chorusfx_type(handle, CHORUS_TYPE_IPS);
    set_ips_option(handle, IPS_SHIFT_AMOUNT, IPS_OCT_D);
    set_ips_option(handle, IPS_SHIFT_AMOUNT, IPS_7TH_DN);
    set_ips_option(handle, IPS_SHIFT_AMOUNT, IPS_6TH_DN);
    set_ips_option(handle, IPS_SHIFT_AMOUNT, IPS_5TH_DN);
    set_ips_option(handle, IPS_SHIFT_AMOUNT, IPS_4TH_DN);
    set_ips_option(handle, IPS_SHIFT_AMOUNT, IPS_3RD_DN);
    set_ips_option(handle, IPS_SHIFT_AMOUNT, IPS_2ND_DN);
    set_ips_option(handle, IPS_SHIFT_AMOUNT, IPS_2ND_UP);
    set_ips_option(handle, IPS_SHIFT_AMOUNT, IPS_3RD_UP);
    set_ips_option(handle, IPS_SHIFT_AMOUNT, IPS_4TH_UP);
    set_ips_option(handle, IPS_SHIFT_AMOUNT, IPS_5TH_UP);
    set_ips_option(handle, IPS_SHIFT_AMOUNT, IPS_6TH_UP);
    set_ips_option(handle, IPS_SHIFT_AMOUNT, IPS_7TH_UP);
    set_ips_option(handle, IPS_SHIFT_AMOUNT, IPS_OCT_U);
    set_ips_option(handle, IPS_KEY, IPS_E);
    set_ips_option(handle, IPS_KEY, IPS_F);
    set_ips_option(handle, IPS_KEY, IPS_GB);
    set_ips_option(handle, IPS_KEY, IPS_G);
    set_ips_option(handle, IPS_KEY, IPS_AB);
    set_ips_option(handle, IPS_KEY, IPS_A);
    set_ips_option(handle, IPS_KEY, IPS_BB);
    set_ips_option(handle, IPS_KEY, IPS_B);
    set_ips_option(handle, IPS_KEY, IPS_C);
    set_ips_option(handle, IPS_KEY, IPS_DD);
    set_ips_option(handle, IPS_KEY, IPS_D);
    set_ips_option(handle, IPS_KEY, IPS_EB);
    set_ips_option(handle, IPS_SCALE, IPS_MAJOR);
    set_ips_option(handle, IPS_SCALE, IPS_MINOR);
    set_ips_option(handle, IPS_SCALE, IPS_DORIA);
    set_ips_option(handle, IPS_SCALE, IPS_MIXLYD);
    set_ips_option(handle, IPS_SCALE, IPS_LYDIAN);
    set_ips_option(handle, IPS_SCALE, IPS_HMINO);
    for (x=0; x<=99; x++)
        set_ips_option(handle, IPS_LEVEL, x);

    set_chorusfx_on_off(handle, TRUE);
    set_chorusfx_on_off(handle, FALSE);

    set_delay_type(handle, DELAY_TYPE_ANALOG);
    for (x=0; x<=139; x++)
        set_delay_time(handle, x);
    for (x=0; x<=99; x++)
        set_delay_option(handle, ANALOG_LEVEL, x);
    for (x=0; x<=100; x++)
        set_delay_option(handle, ANALOG_REPEATS, x);

    set_delay_type(handle, DELAY_TYPE_DIGITAL);
    for (x=0; x<=139; x++)
        set_delay_time(handle, x);
    for (x=0; x<=99; x++)
        set_delay_option(handle, DIGITAL_LEVEL, x);
    for (x=0; x<=100; x++)
        set_delay_option(handle, DIGITAL_REPEATS, x);
    for (x=0; x<=99; x++)
        set_delay_option(handle, DIGITAL_DUCKER_THRESH, x);
    for (x=0; x<=99; x++)
        set_delay_option(handle, DIGITAL_DUCKER_LEVEL, x);

    set_delay_type(handle, DELAY_TYPE_MODULATED);
    for (x=0; x<=139; x++)
        set_delay_time(handle, x);
    for (x=0; x<=99; x++)
        set_delay_option(handle, MODULATED_LEVEL, x);
    for (x=0; x<=100; x++)
        set_delay_option(handle, MODULATED_REPEATS, x);
    for (x=0; x<=99; x++)
        set_delay_option(handle, MODULATED_DEPTH, x);

    set_delay_type(handle, DELAY_TYPE_PONG);
    for (x=0; x<=139; x++)
        set_delay_time(handle, x);
    for (x=0; x<=99; x++)
        set_delay_option(handle, PONG_LEVEL, x);
    for (x=0; x<=100; x++)
        set_delay_option(handle, PONG_REPEATS, x);
    for (x=0; x<=99; x++)
        set_delay_option(handle, PONG_DUCKER_THRESH, x);
    for (x=0; x<=99; x++)
        set_delay_option(handle, PONG_DUCKER_LEVEL, x);

    set_delay_type(handle, DELAY_TYPE_TAPE);
    for (x=0; x<=139; x++)
        set_delay_time(handle, x);
    for (x=0; x<=99; x++)
        set_delay_option(handle, TAPE_LEVEL, x);
    for (x=0; x<=100; x++)
        set_delay_option(handle, TAPE_REPEATS, x);
    for (x=0; x<=99; x++)
        set_delay_option(handle, TAPE_WOW, x);
    for (x=0; x<=99; x++)
        set_delay_option(handle, TAPE_FLUTTER, x);

    set_delay_on_off(handle, TRUE);
    set_delay_on_off(handle, FALSE);


    set_reverb_type(handle, REVERB_TYPE_TWIN);
    for (x=0; x<=99; x++)
        set_reverb_option(handle, TWIN_REVERB, x);

    set_reverb_type(handle, REVERB_TYPE_LEX_AMBIENCE);
    for (x=0; x<=15; x++)
        set_reverb_option(handle, LEX_AMBIENCE_PREDELAY, x);
    for (x=0; x<=99; x++)
        set_reverb_option(handle, LEX_AMBIENCE_DECAY, x);
    for (x=0; x<=99; x++)
        set_reverb_option(handle, LEX_AMBIENCE_LIVELINESS, x);
    for (x=0; x<=99; x++)
        set_reverb_option(handle, LEX_AMBIENCE_LEVEL, x);

    set_reverb_type(handle, REVERB_TYPE_LEX_STUDIO);
    for (x=0; x<=15; x++)
        set_reverb_option(handle, LEX_STUDIO_PREDELAY, x);
    for (x=0; x<=99; x++)
        set_reverb_option(handle, LEX_STUDIO_DECAY, x);
    for (x=0; x<=99; x++)
        set_reverb_option(handle, LEX_STUDIO_LIVELINESS, x);
    for (x=0; x<=99; x++)
        set_reverb_option(handle, LEX_STUDIO_LEVEL, x);

    set_reverb_type(handle, REVERB_TYPE_LEX_ROOM);
    for (x=0; x<=15; x++)
        set_reverb_option(handle, LEX_ROOM_PREDELAY, x);
    for (x=0; x<=99; x++)
        set_reverb_option(handle, LEX_ROOM_DECAY, x);
    for (x=0; x<=99; x++)
        set_reverb_option(handle, LEX_ROOM_LIVELINESS, x);
    for (x=0; x<=99; x++)
        set_reverb_option(handle, LEX_ROOM_LEVEL, x);

    set_reverb_type(handle, REVERB_TYPE_LEX_HALL);
    for (x=0; x<=15; x++)
        set_reverb_option(handle, LEX_HALL_PREDELAY, x);
    for (x=0; x<=99; x++)
        set_reverb_option(handle, LEX_HALL_DECAY, x);
    for (x=0; x<=99; x++)
        set_reverb_option(handle, LEX_HALL_LIVELINESS, x);
    for (x=0; x<=99; x++)
        set_reverb_option(handle, LEX_HALL_LEVEL, x);

    set_reverb_type(handle, REVERB_TYPE_EMT240_PLATE);
    for (x=0; x<=15; x++)
        set_reverb_option(handle, EMT240_PLATE_PREDELAY, x);
    for (x=0; x<=99; x++)
        set_reverb_option(handle, EMT240_PLATE_DECAY, x);
    for (x=0; x<=99; x++)
        set_reverb_option(handle, EMT240_PLATE_LIVELINESS, x);
    for (x=0; x<=99; x++)
        set_reverb_option(handle, EMT240_PLATE_LEVEL, x);

    set_reverb_on_off(handle, TRUE);
    set_reverb_on_off(handle, FALSE);
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

