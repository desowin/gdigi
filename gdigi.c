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

#define TIMEOUT 100

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
    int i, x;
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
    static char set_min[] = {0x04, 0xF0, 0x00, 0x00, 0x04, 0x10, 0x00, 0x5E, 0x04, 0x02, 0x41, 0x00, 0x04, 0x20, 0x03, 0x14, 0x07, 0x00 /* level */, 0x00 /* confirm value? */, 0xF7};
    int val;

    switch((level & 0xf0) >> 4) {
      case 0: val = 0x30; break;
      case 1: val = 0x20; break;
      case 2: val = 0x10; break;
      case 3: val = 0x00; break;
      case 4: val = 0x70; break;
      case 5: val = 0x60; break;
      case 6: val = 0x50; break;
      default: break;
    }
    switch ((level & 0x0f)) {
      case 0: val += 0xA; break;
      case 1: val += 0xB; break;
      case 2: val += 8; break;
      case 3: val += 9; break;
      case 4: val += 0xE; break;
      case 5: val += 0xF; break;
      case 6: val += 0xC; break;
      case 7: val += 0xD; break;
      case 8: val += 2; break;
      case 9: val += 3; break;
      case 0xA: val += 0; break;
      case 0xB: val += 1; break;
      case 0xC: val += 6; break;
      case 0xD: val += 7; break;
      case 0xE: val += 4; break;
      case 0xF: val += 5; break;
      default: break;
    }
    set_min[17] = level;
    set_min[18] = val;
    int i;
    i = usb_bulk_write(handle, 4, set_min, sizeof(set_min), TIMEOUT);
    printf("wrote: %d\n", i);
}

/* level = 0 to 99 */
void set_wah_max(struct usb_dev_handle *handle, int level)
{
    static char set_max[] = {0x04, 0xF0, 0x00, 0x00, 0x04, 0x10, 0x00, 0x5E, 0x04, 0x02, 0x41, 0x00, 0x04, 0x20, 0x04, 0x14, 0x07, 0x00 /* level */, 0x00 /* confirm value? */, 0xF7};
    int val;

    switch((level & 0xf0) >> 4) {
      case 0: val = 0x30; break;
      case 1: val = 0x20; break;
      case 2: val = 0x10; break;
      case 3: val = 0x00; break;
      case 4: val = 0x70; break;
      case 5: val = 0x60; break;
      case 6: val = 0x50; break;
      default: break;
    }
    switch ((level & 0x0f)) {
      case 0: val += 0xD; break;
      case 1: val += 0xC; break;
      case 2: val += 0xF; break;
      case 3: val += 0xE; break;
      case 4: val += 9; break;
      case 5: val += 8; break;
      case 6: val += 0xB; break;
      case 7: val += 0xA; break;
      case 8: val += 5; break;
      case 9: val += 4; break;
      case 0xA: val += 7; break;
      case 0xB: val += 6; break;
      case 0xC: val += 1; break;
      case 0xD: val += 0; break;
      case 0xE: val += 3; break;
      case 0xF: val += 2; break;
      default: break;
    }
    set_max[17] = level;
    set_max[18] = val;
    int i;
    i = usb_bulk_write(handle, 4, set_max, sizeof(set_max), TIMEOUT);
    printf("wrote: %d\n", i);
}

/* level = 0 to 12 */
void set_wah_level(struct usb_dev_handle *handle, int level)
{
    static char set_level[] = {0x04, 0xF0, 0x00, 0x00, 0x04, 0x10, 0x00, 0x5E, 0x04, 0x02, 0x41, 0x20, 0x04, 0x00, 0x05, 0x03, 0x07, 0x00 /* level */, 0x00 /* confirm value? */, 0xF7};
    int val;

    switch (level) {
      case 0: val = 0x2B; break;
      case 1: val = 0x2A; break;
      case 2: val = 0x29; break;
      case 3: val = 0x28; break;
      case 4: val = 0x2F; break;
      case 5: val = 0x2E; break;
      case 6: val = 0x2D; break;
      case 7: val = 0x2C; break;
      case 8: val = 0x23; break;
      case 9: val = 0x22; break;
      case 0xA: val = 0x21; break;
      case 0xB: val = 0x20; break;
      case 0xC: val = 0x27; break;
      default: break;
    }
    set_level[17] = level;
    set_level[18] = val;
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
    static char set_wah[] = {0x04, 0xF0, 0x00, 0x00, 0x04, 0x10, 0x00, 0x5E, 0x04, 0x02, 0x41, 0x20, 0x04, 0x00, 0x01, 0x03, 0x07, 0x00 /* on/off */, 0x00 /* confirm */, 0xF7};

    if (val == FALSE) { /* turn wah off */
        set_wah[17] = 0;
        set_wah[18] = 0x2F;
    } else { /* turn wah on */
        set_wah[17] = 1;
        set_wah[18] = 0x2E;
    }

    int i;
    i = usb_bulk_write(handle, 4, set_wah, sizeof(set_wah), TIMEOUT);
    printf("wrote: %d\n", i);
}

/* level = 0 to 99 */
void set_comp_sustain(struct usb_dev_handle *handle, int level)
{
    static char set_sust[] = {0x04, 0xF0, 0x00, 0x00, 0x04, 0x10, 0x00, 0x5E, 0x04, 0x02, 0x41, 0x20, 0x04, 0x00, 0x50, 0x04, 0x07, 0x00 /* value */, 0x00 /* confirm */, 0xf7};
    int val;

    val = (7 - ((level & 0xf0) >> 4)) << 4;
    switch (level & 0xf) {
      case 0: val += 9; break;
      case 1: val += 8; break;
      case 2: val += 0xB; break;
      case 3: val += 0xA; break;
      case 4: val += 0xD; break;
      case 5: val += 0xC; break;
      case 6: val += 0xF; break;
      case 7: val += 0xE; break;
      case 8: val += 1; break;
      case 9: val += 0; break;
      case 0xA: val += 3; break;
      case 0xB: val += 2; break;
      case 0xC: val += 5; break;
      case 0xD: val += 4; break;
      case 0xE: val += 7; break;
      case 0xF: val += 6; break;
      default: break;
    }

    set_sust[17] = level;
    set_sust[18] = val;

    int i;
    i = usb_bulk_write(handle, 4, set_sust, sizeof(set_sust), TIMEOUT);
    printf("wrote: %d\n", i);
}

/* level = 0 to 99, available only in digi comp */
void set_comp_tone(struct usb_dev_handle *handle, int level)
{
    static char set_tone[] = {0x04, 0xF0, 0x00, 0x00, 0x04, 0x10, 0x00, 0x5E, 0x04, 0x02, 0x41, 0x20, 0x04, 0x00, 0x51, 0x04, 0x07, 0x00 /* value */, 0x00 /* confirm */, 0xf7};
    int val;

    val = (7 - ((level & 0xf0) >> 4)) << 4;
    if ((level & 0xf) >= 8) val += ((level & 0xf) - 8);
    else val += (8 + (level & 0xf));

    set_tone[17] = level;
    set_tone[18] = val;

    int i;
    i = usb_bulk_write(handle, 4, set_tone, sizeof(set_tone), TIMEOUT);
    printf("wrote: %d\n", i);
}

/* level = 0 to 99 */
void set_comp_attack(struct usb_dev_handle *handle, int level)
{
    static char set_attack[] = {0x04, 0xF0, 0x00, 0x00, 0x04, 0x10, 0x00, 0x5E, 0x04, 0x02, 0x41, 0x20, 0x04, 0x00, 0x53, 0x04, 0x07, 0x00 /* value */, 0x00 /* confirm */, 0xf7};
    int val;

    val = (7 - ((level & 0xf0) >> 4)) << 4;
    switch (level & 0xf) {
      case 0: val += 0xA; break;
      case 1: val += 0xB; break;
      case 2: val += 8; break;
      case 3: val += 9; break;
      case 4: val += 0xE; break;
      case 5: val += 0xF; break;
      case 6: val += 0xC; break;
      case 7: val += 0xD; break;
      case 8: val += 2; break;
      case 9: val += 3; break;
      case 0xA: val += 0; break;
      case 0xB: val += 1; break;
      case 0xC: val += 6; break;
      case 0xD: val += 7; break;
      case 0xE: val += 4; break;
      case 0xF: val += 5; break;
      default: break;
    }
    set_attack[17] = level;
    set_attack[18] = val;

    int i;
    i = usb_bulk_write(handle, 4, set_attack, sizeof(set_attack), TIMEOUT);
    printf("wrote: %d\n", i);
}

/* level = 0 to 99 */
void set_comp_level(struct usb_dev_handle *handle, int level)
{
    static char set_level[] = {0x04, 0xF0, 0x00, 0x00, 0x04, 0x10, 0x00, 0x5E, 0x04, 0x02, 0x41, 0x20, 0x04, 0x00, 0x52, 0x04, 0x07, 0x00 /* value */, 0x00 /* confirm */, 0xf7};
    int val;

    val = (7 - ((level & 0xf0) >> 4)) << 4;
    switch (level & 0xf) {
      case 0: val += 0xB; break;
      case 1: val += 0xA; break;
      case 2: val += 9; break;
      case 3: val += 8; break;
      case 4: val += 0xF; break;
      case 5: val += 0xE; break;
      case 6: val += 0xD; break;
      case 7: val += 0xC; break;
      case 8: val += 3; break;
      case 9: val += 2; break;
      case 0xA: val += 1; break;
      case 0xB: val += 0; break;
      case 0xC: val += 7; break;
      case 0xD: val += 6; break;
      case 0xE: val += 5; break;
      case 0xF: val += 4; break;
      default: break;
    }
    set_level[17] = level;
    set_level[18] = val;

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
    static char set_type[] = {0x04, 0xF0, 0x00, 0x00, 0x04, 0x10, 0x00, 0x5E, 0x04, 0x02, 0x41, 0x2C, 0x04, 0x00, 0x4F, 0x04, 0x04, 0x01, 0x00 /* type */, 0x00 /* confirm */, 0x05, 0xF7, 0x00, 0x00};

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
    static char set_comp[] = {0x04, 0xF0, 0x00, 0x00, 0x04, 0x10, 0x00, 0x5E, 0x04, 0x02, 0x41, 0x20, 0x04, 0x00, 0x41, 0x04, 0x07, 0x00 /* on/off */, 0x00 /* confirm */, 0xF7};

    if (val == FALSE) { /* turn comp off */
        set_comp[17] = 0;
        set_comp[18] = 0x68;
    } else { /* turn comp on */
        set_comp[17] = 1;
        set_comp[18] = 0x69;
    }

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
    static char set_pickup[] = {0x04, 0xF0, 0x00, 0x00, 0x04, 0x10, 0x00, 0x5E, 0x04, 0x02, 0x41, 0x00, 0x04, 0x00, 0x41, 0x02, 0x07, 0x00 /* on/off */, 0x00 /* confirm */, 0xF7};

    if (val == FALSE) { /* turn pickup off */
        set_pickup[17] = 0;
        set_pickup[18] = 0x4E;
    } else { /* turn pickup on */
        set_pickup[17] = 1;
        set_pickup[18] = 0x4F;
    }

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

void set_screamer_drive(struct usb_dev_handle* handle, int x)
{
    static char set_drive[] = {0x04, 0xF0, 0x00, 0x00, 0x04, 0x10, 0x00, 0x5E, 0x04, 0x02, 0x41, 0x20, 0x04, 0x09, 0x02, 0x06, 0x07, 0x00 /* value */, 0x00 /* confirm */, 0xF7};

    int val;
    switch ((x & 0xf0) >> 4) {
        case 0: val = 0x20; break;
        case 1: val = 0x30; break;
        case 2: val = 0x00; break;
        case 3: val = 0x10; break;
        case 4: val = 0x60; break;
        case 5: val = 0x70; break;
        case 6: val = 0x40; break;
        default: break;
    }
    val += (x & 0xf);

    set_drive[17] = x;
    set_drive[18] = val;

    int i;
    i = usb_bulk_write(handle, 4, set_drive, sizeof(set_drive), TIMEOUT);
    printf("wrote: %d\n", i);
}

void set_screamer_tone(struct usb_dev_handle* handle, int x)
{
    static char set_tone[] = {0x04, 0xF0, 0x00, 0x00, 0x04, 0x10, 0x00, 0x5E, 0x04, 0x02, 0x41, 0x20, 0x04, 0x09, 0x03, 0x06, 0x07, 0x00 /* value */, 0x00 /* confirm */, 0xF7};

    int val;
    switch ((x & 0xf0) >> 4) {
        case 0: val = 0x20; break;
        case 1: val = 0x30; break;
        case 2: val = 0x00; break;
        case 3: val = 0x10; break;
        case 4: val = 0x60; break;
        case 5: val = 0x70; break;
        case 6: val = 0x40; break;
        default: break;
    }
    val += (x & 0xf);
    if (((x & 0xf) % 2) == 0) {
        val += 1;
    } else {
        val -= 1;
    }

    set_tone[17] = x;
    set_tone[18] = val;

    int i;
    i = usb_bulk_write(handle, 4, set_tone, sizeof(set_tone), TIMEOUT);
    printf("wrote: %d\n", i);
}

void set_screamer_level(struct usb_dev_handle* handle, int x)
{
    static char set_level[] = {0x04, 0xF0, 0x00, 0x00, 0x04, 0x10, 0x00, 0x5E, 0x04, 0x02, 0x41, 0x20, 0x04, 0x09, 0x04, 0x06, 0x07, 0x00 /* value */, 0x00 /* confirm */, 0xF7};

    int val;
    switch ((x & 0xf0) >> 4) {
        case 0: val = 0x20; break;
        case 1: val = 0x30; break;
        case 2: val = 0x00; break;
        case 3: val = 0x10; break;
        case 4: val = 0x60; break;
        case 5: val = 0x70; break;
        case 6: val = 0x40; break;
        default: break;
    }
    switch (x & 0xf) {
        case 0: val += 0x6; break;
        case 1: val += 0x7; break;
        case 2: val += 0x4; break;
        case 3: val += 0x5; break;
        case 4: val += 0x2; break;
        case 5: val += 0x3; break;
        case 6: val += 0x0; break;
        case 7: val += 0x1; break;
        case 8: val += 0xE; break;
        case 9: val += 0xF; break;
        case 0xA: val += 0xC; break;
        case 0xB: val += 0xD; break;
        case 0xC: val += 0xA; break;
        case 0xD: val += 0xB; break;
        case 0xE: val += 0x8; break;
        case 0xF: val += 0x9; break;
        default: break;
    }

    set_level[17] = x;
    set_level[18] = val;

    int i;
    i = usb_bulk_write(handle, 4, set_level, sizeof(set_level), TIMEOUT);
    printf("wrote: %d\n", i);
}

void set_808_overdrive(struct usb_dev_handle *handle, int x)
{
    static char set_overdrive[] = {0x04, 0xF0, 0x00, 0x00, 0x04, 0x10, 0x00, 0x5E, 0x04, 0x02, 0x41, 0x20, 0x04, 0x09, 0x29, 0x06, 0x07, 0x00 /* value */, 0x00 /* confirm */, 0xF7};

    int val;
    val = (x & 0xf0);
    switch (x & 0xf) {
        case 0: val += 0xB; break;
        case 1: val += 0xA; break;
        case 2: val += 0x9; break;
        case 3: val += 0x8; break;
        case 4: val += 0xF; break;
        case 5: val += 0xE; break;
        case 6: val += 0xD; break;
        case 7: val += 0xC; break;
        case 8: val += 0x3; break;
        case 9: val += 0x2; break;
        case 0xA: val += 0x1; break;
        case 0xB: val += 0x0; break;
        case 0xC: val += 0x7; break;
        case 0xD: val += 0x6; break;
        case 0xE: val += 0x5; break;
        case 0xF: val += 0x4; break;
        default: break;
    }

    set_overdrive[17] = x;
    set_overdrive[18] = val;

    int i;
    i = usb_bulk_write(handle, 4, set_overdrive, sizeof(set_overdrive), TIMEOUT);
    printf("wrote: %d\n", i);
}

void set_808_tone(struct usb_dev_handle *handle, int x)
{
    static char set_tone[] = {0x04, 0xF0, 0x00, 0x00, 0x04, 0x10, 0x00, 0x5E, 0x04, 0x02, 0x41, 0x20, 0x04, 0x09, 0x2A, 0x06, 0x07, 0x00 /* value */, 0x00 /* confirm */, 0xF7};

    int val;
    val = (x & 0xf0);
    val += (((x & 0xf) + 8) & 0xf);

    set_tone[17] = x;
    set_tone[18] = val;

    int i;
    i = usb_bulk_write(handle, 4, set_tone, sizeof(set_tone), TIMEOUT);
    printf("wrote: %d\n", i);
}

void set_808_level(struct usb_dev_handle *handle, int x)
{
    static char set_level[] = {0x04, 0xF0, 0x00, 0x00, 0x04, 0x10, 0x00, 0x5E, 0x04, 0x02, 0x41, 0x20, 0x04, 0x09, 0x2B, 0x06, 0x07, 0x00 /* value */, 0x00 /* confirm */, 0xF7};

    int val;
    val = (x & 0xf0);
    val += (((x & 0xf) + 8) & 0xf);
    if (((x & 0xf) % 2) == 0) {
        val += 1;
    } else {
        val -= 1;
    }

    set_level[17] = x;
    set_level[18] = val;

    int i;
    i = usb_bulk_write(handle, 4, set_level, sizeof(set_level), TIMEOUT);
    printf("wrote: %d\n", i);
}

void set_guyod_drive(struct usb_dev_handle *handle, int x)
{
    static char set_drive[] = {0x04, 0xF0, 0x00, 0x00, 0x04, 0x10, 0x00, 0x5E, 0x04, 0x02, 0x41, 0x20, 0x04, 0x09, 0x10, 0x06, 0x07, 0x00 /* value */, 0x00 /* confirm */, 0xF7};

    int val;
    switch ((x & 0xf0) >> 4) {
        case 0: val = 0x30; break;
        case 1: val = 0x20; break;
        case 2: val = 0x10; break;
        case 3: val = 0x00; break;
        case 4: val = 0x70; break;
        case 5: val = 0x60; break;
        case 6: val = 0x50; break;
        default: break;
    }
    switch (x & 0xf) {
        case 0: val += 2; break;
        case 1: val += 3; break;
        case 2: val += 0; break;
        case 3: val += 1; break;
        case 4: val += 6; break;
        case 5: val += 7; break;
        case 6: val += 4; break;
        case 7: val += 5; break;
        case 8: val += 0xA; break;
        case 9: val += 0xB; break;
        case 0xA: val += 8; break;
        case 0xB: val += 9; break;
        case 0xC: val += 0xE; break;
        case 0xD: val += 0xF; break;
        case 0xE: val += 0xC; break;
        case 0xF: val += 0xD; break;
        default: break;
    }

    set_drive[17] = x;
    set_drive[18] = val;

    int i;
    i = usb_bulk_write(handle, 4, set_drive, sizeof(set_drive), TIMEOUT);
    printf("wrote: %d\n", i);
}

void set_guyod_level(struct usb_dev_handle *handle, int x)
{
    static char set_level[] = {0x04, 0xF0, 0x00, 0x00, 0x04, 0x10, 0x00, 0x5E, 0x04, 0x02, 0x41, 0x20, 0x04, 0x09, 0x11, 0x06, 0x07, 0x00 /* value */, 0x00 /* confirm */, 0xF7};

    int val;
    switch ((x & 0xf0) >> 4) {
        case 0: val = 0x30; break;
        case 1: val = 0x20; break;
        case 2: val = 0x10; break;
        case 3: val = 0x00; break;
        case 4: val = 0x70; break;
        case 5: val = 0x60; break;
        case 6: val = 0x50; break;
        default: break;
    }
    switch (x & 0xf) {
        case 0: val += 3; break;
        case 1: val += 2; break;
        case 2: val += 1; break;
        case 3: val += 0; break;
        case 4: val += 7; break;
        case 5: val += 6; break;
        case 6: val += 5; break;
        case 7: val += 4; break;
        case 8: val += 0xB; break;
        case 9: val += 0xA; break;
        case 0xA: val += 9; break;
        case 0xB: val += 8; break;
        case 0xC: val += 0xF; break;
        case 0xD: val += 0xE; break;
        case 0xE: val += 0xD; break;
        case 0xF: val += 0xC; break;
        default: break;
    }

    set_level[17] = x;
    set_level[18] = val;

    int i;
    i = usb_bulk_write(handle, 4, set_level, sizeof(set_level), TIMEOUT);
    printf("wrote: %d\n", i);
}

void set_dod250_gain(struct usb_dev_handle *handle, int x)
{
    static char set_gain[] = {0x04, 0xF0, 0x00, 0x00, 0x04, 0x10, 0x00, 0x5E, 0x04, 0x02, 0x41, 0x20, 0x04, 0x09, 0x0B, 0x06, 0x07, 0x00 /* value */, 0x00 /* confirm */, 0xF7};

    int val;
    switch ((x & 0xf0) >> 4) {
        case 0: val = 0x20; break;
        case 1: val = 0x30; break;
        case 2: val = 0x00; break;
        case 3: val = 0x10; break;
        case 4: val = 0x60; break;
        case 5: val = 0x70; break;
        case 6: val = 0x40; break;
        default: break;
    }
    switch (x & 0xf) {
        case 0: val += 9; break;
        case 1: val += 8; break;
        case 2: val += 0xB; break;
        case 3: val += 0xA; break;
        case 4: val += 0xD; break;
        case 5: val += 0xC; break;
        case 6: val += 0xF; break;
        case 7: val += 0xE; break;
        case 8: val += 1; break;
        case 9: val += 0; break;
        case 0xA: val += 3; break;
        case 0xB: val += 2; break;
        case 0xC: val += 5; break;
        case 0xD: val += 4; break;
        case 0xE: val += 7; break;
        case 0xF: val += 6; break;
        default: break;
    }

    set_gain[17] = x;
    set_gain[18] = val;

    int i;
    i = usb_bulk_write(handle, 4, set_gain, sizeof(set_gain), TIMEOUT);
    printf("wrote: %d\n", i);
}

void set_dod250_level(struct usb_dev_handle *handle, int x)
{
    static char set_level[] = {0x04, 0xF0, 0x00, 0x00, 0x04, 0x10, 0x00, 0x5E, 0x04, 0x02, 0x41, 0x20, 0x04, 0x09, 0x0C, 0x06, 0x07, 0x00 /* value */, 0x00 /* confirm */, 0xF7};

    int val;
    switch ((x & 0xf0) >> 4) {
        case 0: val = 0x20; break;
        case 1: val = 0x30; break;
        case 2: val = 0x00; break;
        case 3: val = 0x10; break;
        case 4: val = 0x60; break;
        case 5: val = 0x70; break;
        case 6: val = 0x40; break;
        default: break;
    }
    val += (0xF - (x & 0xf));
    if (((x & 0xf) % 2) == 0) {
        val -= 1;
    } else {
        val += 1;
    }

    set_level[17] = x;
    set_level[18] = val;

    int i;
    i = usb_bulk_write(handle, 4, set_level, sizeof(set_level), TIMEOUT);
    printf("wrote: %d\n", i);
}

void set_rodent_dist(struct usb_dev_handle *handle, int x)
{
    static char set_dist[] = {0x04, 0xF0, 0x00, 0x00, 0x04, 0x10, 0x00, 0x5E, 0x04, 0x02, 0x41, 0x20, 0x04, 0x09, 0x05, 0x06, 0x07, 0x00 /* value */, 0x00 /* confirm */, 0xF7};

    int val;
    switch ((x & 0xf0) >> 4) {
        case 0: val = 0x20; break;
        case 1: val = 0x30; break;
        case 2: val = 0x00; break;
        case 3: val = 0x10; break;
        case 4: val = 0x60; break;
        case 5: val = 0x70; break;
        case 6: val = 0x40; break;
        default: break;
    }
    if ((x & 0xf) < 8) {
        val += (7 - (x & 0xf));
    } else {
        val += (0xF + (8 - (x & 0xf)));
    }

    set_dist[17] = x;
    set_dist[18] = val;

    int i;
    i = usb_bulk_write(handle, 4, set_dist, sizeof(set_dist), TIMEOUT);
    printf("wrote: %d\n", i);
}

void set_rodent_filter(struct usb_dev_handle *handle, int x)
{
    static char set_filter[] = {0x04, 0xF0, 0x00, 0x00, 0x04, 0x10, 0x00, 0x5E, 0x04, 0x02, 0x41, 0x20, 0x04, 0x09, 0x06, 0x06, 0x07, 0x00 /* value */, 0x00 /* confirm */, 0xF7};

    int val;
    switch ((x & 0xf0) >> 4) {
        case 0: val = 0x20; break;
        case 1: val = 0x30; break;
        case 2: val = 0x00; break;
        case 3: val = 0x10; break;
        case 4: val = 0x60; break;
        case 5: val = 0x70; break;
        case 6: val = 0x40; break;
        default: break;
    }
    switch (x & 0xf) {
        case 0: val += 4; break;
        case 1: val += 5; break;
        case 2: val += 6; break;
        case 3: val += 7; break;
        case 4: val += 0; break;
        case 5: val += 1; break;
        case 6: val += 2; break;
        case 7: val += 3; break;
        case 8: val += 0xC; break;
        case 9: val += 0xD; break;
        case 0xA: val += 0xE; break;
        case 0xB: val += 0xF; break;
        case 0xC: val += 8; break;
        case 0xD: val += 9; break;
        case 0xE: val += 0xA; break;
        case 0xF: val += 0xB; break;
        default: break;
    }


    set_filter[17] = x;
    set_filter[18] = val;

    int i;
    i = usb_bulk_write(handle, 4, set_filter, sizeof(set_filter), TIMEOUT);
    printf("wrote: %d\n", i);
}

void set_rodent_level(struct usb_dev_handle *handle, int x)
{
    static char set_level[] = {0x04, 0xF0, 0x00, 0x00, 0x04, 0x10, 0x00, 0x5E, 0x04, 0x02, 0x41, 0x20, 0x04, 0x09, 0x07, 0x06, 0x07, 0x00 /* value */, 0x00 /* confirm */, 0xF7};

    int val;
    switch ((x & 0xf0) >> 4) {
        case 0: val = 0x20; break;
        case 1: val = 0x30; break;
        case 2: val = 0x00; break;
        case 3: val = 0x10; break;
        case 4: val = 0x60; break;
        case 5: val = 0x70; break;
        case 6: val = 0x40; break;
        default: break;
    }
    switch (x & 0xf) {
        case 0: val += 5; break;
        case 1: val += 4; break;
        case 2: val += 7; break;
        case 3: val += 6; break;
        case 4: val += 1; break;
        case 5: val += 0; break;
        case 6: val += 3; break;
        case 7: val += 2; break;
        case 8: val += 0xD; break;
        case 9: val += 0xC; break;
        case 0xA: val += 0xF; break;
        case 0xB: val += 0xE; break;
        case 0xC: val += 9; break;
        case 0xD: val += 8; break;
        case 0xE: val += 0xB; break;
        case 0xF: val += 0xA; break;
        default: break;
    }


    set_level[17] = x;
    set_level[18] = val;

    int i;
    i = usb_bulk_write(handle, 4, set_level, sizeof(set_level), TIMEOUT);
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

                /*
                set_wah_type(handle, WAH_TYPE_CRY);
                sleep(1);
                set_wah_type(handle, WAH_TYPE_FULLRANGE);
                sleep(1);
                set_wah_type(handle, WAH_TYPE_CLYDE);
                sleep(1);
                set_comp_type(handle, COMP_TYPE_DIGI);
                sleep(1);
                set_comp_type(handle, COMP_TYPE_CS);
                */

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

