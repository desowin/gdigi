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

/**
 *  calculate_checksum:
 *  @array: data to calculate checksum
 *  @length: data length
 *
 *  Calculates message checksum
 *
 *  Return value: calculated checksum
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
                           calculate_checksum(&msg->str[1], msg->len - 1));

    send_data(msg->str, msg->len);

    g_string_free(msg, TRUE);
}

static gint get_message_id(GString *msg)
{
    g_return_val_if_fail(msg != NULL, -1);

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

    unpack_message(data);

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

static void request_device_configuration()
{
    gint os_major, os_minor;
    gint cpu_major, cpu_minor;
    gint protocol_version;
    gint current_bank, current_preset;
    gint media_card;

    send_message(REQUEST_DEVICE_CONFIGURATION, NULL, 0);

    GString *data = NULL;

    do {
        if (data)
            g_string_free(data, TRUE);
        data = read_data();
    } while (get_message_id(data) != RECEIVE_DEVICE_CONFIGURATION);

    unpack_message(data);

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

typedef struct {
    const gchar *label;
    guint id;
    guint position;
} Modifiers;

static Modifiers modifiers[] = {
    {"None", 0, 0},
    {"Pickup Enable", PICKUP_ON_OFF, PICKUP_POSITION},
    {"Pickup Type", PICKUP_TYPE, PICKUP_POSITION},
    {"Compressor Enable", COMP_ON_OFF, COMP_POSITION},
    {"Compressor Sustain", COMP_SUSTAIN, COMP_POSITION},
    {"Compressor Tone", COMP_TONE, COMP_POSITION},
    {"Compressor Level", COMP_LEVEL, COMP_POSITION},
    {"Compressor Attack", COMP_ATTACK, COMP_POSITION},
    {"Dist Enable", DIST_ON_OFF, DIST_POSITION},
    {"Dist Drive", DIST_SCREAMER_DRIVE, DIST_POSITION},
    {"Dist Tone", DIST_SCREAMER_TONE, DIST_POSITION},
    {"Dist Level", DIST_SCREAMER_LVL, DIST_POSITION},
    {"Dist Overdrive", DIST_808_OVERDRIVE, DIST_POSITION},
    {"Dist Tone", DIST_808_TONE, DIST_POSITION},
    {"Dist Level", DIST_808_LVL, DIST_POSITION},
    {"Dist Drive", DIST_GUYOD_DRIVE, DIST_POSITION},
    {"Dist Level", DIST_GUYOD_LVL, DIST_POSITION},
    {"Dist Gain", DIST_DOD250_GAIN, DIST_POSITION},
    {"Dist Level", DIST_DOD250_LVL, DIST_POSITION},
    {"Dist Distortion", DIST_RODENT_DIST, DIST_POSITION},
    {"Dist Filter", DIST_RODENT_FILTER, DIST_POSITION},
    {"Dist Volume", DIST_RODENT_LVL, DIST_POSITION},
    {"Dist Distortion", DIST_MX_DIST, DIST_POSITION},
    {"Dist Output", DIST_MX_OUTPUT, DIST_POSITION},
    {"Dist Distortion", DIST_DS_GAIN, DIST_POSITION},
    {"Dist Tone", DIST_DS_TONE, DIST_POSITION},
    {"Dist Level", DIST_DS_LVL, DIST_POSITION},
    {"Dist Gain", DIST_GRUNGE_GRUNGE, DIST_POSITION},
    {"Dist Face", DIST_GRUNGE_FACE, DIST_POSITION},
    {"Dist Loud", DIST_GRUNGE_LOUD, DIST_POSITION},
    {"Dist Butt", DIST_GRUNGE_BUTT, DIST_POSITION},
    {"Dist Gain", DIST_ZONE_GAIN, DIST_POSITION},
    {"Dist Mid Level", DIST_ZONE_MID_LVL, DIST_POSITION},
    {"Dist Level", DIST_ZONE_LEVEL, DIST_POSITION},
    {"Dist Low", DIST_ZONE_LOW, DIST_POSITION},
    {"Dist Mid Freq", DIST_ZONE_MID_FREQ, DIST_POSITION},
    {"Dist High", DIST_ZONE_HIGH, DIST_POSITION},
    {"Dist Low", DIST_DEATH_LOW, DIST_POSITION},
    {"Dist Mid", DIST_DEATH_MID, DIST_POSITION},
    {"Dist Level", DIST_DEATH_LVL, DIST_POSITION},
    {"Dist High", DIST_DEATH_HIGH, DIST_POSITION},
    {"Dist Gunk", DIST_GONK_GONK, DIST_POSITION},
    {"Dist Smear", DIST_GONK_SMEAR, DIST_POSITION},
    {"Dist Suck", DIST_GONK_SUCK, DIST_POSITION},
    {"Dist Heave", DIST_GONK_HEAVE, DIST_POSITION},
    {"Dist Fuzz", DIST_FUZZY_FUZZ, DIST_POSITION},
    {"Dist Volume", DIST_FUZZY_VOLUME, DIST_POSITION},
    {"Dist Sustain", DIST_MP_SUSTAIN, DIST_POSITION},
    {"Dist Tone", DIST_MP_TONE, DIST_POSITION},
    {"Dist Volume", DIST_MP_VOLUME, DIST_POSITION},
    {"Amp Channel Enable", AMP_ON_OFF, AMP_POSITION},
    {"Amp Gain", AMP_GAIN, AMP_POSITION},
    {"Amp Level", AMP_LEVEL, AMP_POSITION},
    {"EQ Enable", EQ_ON_OFF, EQ_POSITION},
    {"EQ Bass", EQ_BASS, EQ_POSITION},
    {"EQ Mid", EQ_MID, EQ_POSITION},
    {"EQ Treb", EQ_TREBLE, EQ_POSITION},
    {"Gate Enable", NOISEGATE_ON_OFF, NOISEGATE_POSITION},
    {"Gate Pluck Sens", NOISEGATE_SWELL_SENS, NOISEGATE_POSITION},
    {"Gate Threshold", NOISEGATE_GATE_TRESHOLD, NOISEGATE_POSITION},
    {"Gate Attack", NOISEGATE_ATTACK, NOISEGATE_POSITION},
    {"Gate Release", NOISEGATE_RELEASE, NOISEGATE_POSITION},
    {"Gate Attenuation", NOISEGATE_ATTN, NOISEGATE_POSITION},
    {"Chorus/FX Enable", CHORUSFX_ON_OFF, CHORUSFX_POSITION},
    {"Phaser Speed", PHASER_SPEED, CHORUSFX_POSITION},
    {"Phaser Depth", PHASER_DEPTH, CHORUSFX_POSITION},
    {"Phaser Regen", PHASER_REGEN, CHORUSFX_POSITION},
    {"Phaser Waveform", PHASER_WAVE, CHORUSFX_POSITION},
    {"Phaser Level", PHASER_LEVEL, CHORUSFX_POSITION},
    {"Chorus Speed", CE_CHORUS_SPEED, CHORUSFX_POSITION},
    {"Chorus Depth", CE_CHORUS_DEPTH, CHORUSFX_POSITION},
    {"Chorus Level", DUAL_CHORUS_LEVEL, CHORUSFX_POSITION},
    {"Chorus Waveform", DUAL_CHORUS_WAVE, CHORUSFX_POSITION},
    {"Flanger Speed", FLANGER_SPEED, CHORUSFX_POSITION},
    {"Flanger Depth", FLANGER_DEPTH, CHORUSFX_POSITION},
    {"Flanger Regen", FLANGER_REGEN, CHORUSFX_POSITION},
    {"Flanger Waveform", FLANGER_WAVE, CHORUSFX_POSITION},
    {"Flanger Level", FLANGER_LEVEL, CHORUSFX_POSITION},
    {"Flanger Width", MXR_FLANGER_WIDTH, CHORUSFX_POSITION},
    {"Flanger Manual", MXR_FLANGER_MANUAL, CHORUSFX_POSITION},
    {"Vibrato Speed", VIBRATO_SPEED, CHORUSFX_POSITION},
    {"Vibrato Depth", VIBRATO_DEPTH, CHORUSFX_POSITION},
    {"Rotary Speed", ROTARY_SPEED, CHORUSFX_POSITION},
    {"Rotary Intensity", ROTARY_INTENSITY, CHORUSFX_POSITION},
    {"Rotary Doppler", ROTARY_DOPPLER, CHORUSFX_POSITION},
    {"Rotary Crossover", ROTARY_CROSSOVER, CHORUSFX_POSITION},
    {"VibroPan Speed", VIBROPAN_SPEED, CHORUSFX_POSITION},
    {"VibroPan Depth", VIBROPAN_DEPTH, CHORUSFX_POSITION},
    {"VibroPan VibratoPan", VIBROPAN_VIBRA, CHORUSFX_POSITION},
    {"VibroPan Waveform", VIBROPAN_WAVE, CHORUSFX_POSITION},
    {"Tremolo Speed", TREMOLO_SPEED, CHORUSFX_POSITION},
    {"Tremolo Depth", TREMOLO_DEPTH, CHORUSFX_POSITION},
    {"Tremolo Waveform", TREMOLO_WAVE, CHORUSFX_POSITION},
    {"Panner Speed", PANNER_SPEED, CHORUSFX_POSITION},
    {"Panner Depth", PANNER_DEPTH, CHORUSFX_POSITION},
    {"Panner Waveform", PANNER_WAVE, CHORUSFX_POSITION},
    {"Envelope Sens.", ENVELOPE_SENSITIVITY, CHORUSFX_POSITION},
    {"Envelope Range", ENVELOPE_RANGE, CHORUSFX_POSITION},
    {"AutoYa Speed", AUTOYA_SPEED, CHORUSFX_POSITION},
    {"AutoYa Intensity", AUTOYA_INTENSITY, CHORUSFX_POSITION},
    {"AutoYa Range", AUTOYA_RANGE, CHORUSFX_POSITION},
    {"YaYa Pedal", YAYA_PEDAL, CHORUSFX_POSITION},
    {"YaYa Intensity", YAYA_INTENSITY, CHORUSFX_POSITION},
    {"YaYa Range", YAYA_RANGE, CHORUSFX_POSITION},
    {"Step Filter Speed", STEP_FILTER_SPEED, CHORUSFX_POSITION},
    {"Step Filter Intensity", STEP_FILTER_INTENSITY, CHORUSFX_POSITION},
    {"Whammy Amount", WHAMMY_AMOUNT, CHORUSFX_POSITION},
    {"Whammy Pedal", WHAMMY_PEDAL, CHORUSFX_POSITION},
    {"Whammy Mix", WHAMMY_MIX, CHORUSFX_POSITION},
    {"Pitch Shift Amount", PITCH_AMOUNT, CHORUSFX_POSITION},
    {"Pitch Shift Mix", PITCH_MIX, CHORUSFX_POSITION},
    {"Detune Amount", DETUNE_AMOUNT, CHORUSFX_POSITION},
    {"Detune Level", DETUNE_LEVEL, CHORUSFX_POSITION},
    {"IPS Amount", IPS_SHIFT_AMOUNT, CHORUSFX_POSITION},
    {"IPS Key", IPS_KEY, CHORUSFX_POSITION},
    {"IPS Scale", IPS_SCALE, CHORUSFX_POSITION},
    {"IPS Level", IPS_LEVEL, CHORUSFX_POSITION},
    {"Delay Enable", DELAY_ON_OFF, DELAY_POSITION},
    {"Delay Time", DELAY_TIME, DELAY_POSITION},
    {"Delay Repeats", ANALOG_REPEATS, DELAY_POSITION},
    {"Delay Level", ANALOG_LEVEL, DELAY_POSITION},
    {"Delay Duck Thresh", DIGITAL_DUCKER_THRESH, DELAY_POSITION},
    {"Delay Duck Level", DIGITAL_DUCKER_LEVEL, DELAY_POSITION},
    {"Delay Mod Depth", MODULATED_DEPTH, DELAY_POSITION},
    {"Delay Tape Wow", TAPE_WOW, DELAY_POSITION},
    {"Delay Tape Flut", TAPE_FLUTTER, DELAY_POSITION},
    {"Reverb Enable", REVERB_ON_OFF, REVERB_POSITION},
    {"Reverb Decay", LEX_AMBIENCE_DECAY, REVERB_POSITION},
    {"Reverb Liveliness", LEX_STUDIO_LIVELINESS, REVERB_POSITION},
    {"Reverb Level", LEX_STUDIO_LEVEL, REVERB_POSITION},
    {"Reverb Predelay", LEX_STUDIO_PREDELAY, REVERB_POSITION},
    {"Volume Pre FX", 2626, 13},
    {"Volume Post FX", 2626, 17},
};

int n_modifiers = G_N_ELEMENTS(modifiers);

/*
    returned value must not be freed
*/
static const gchar *get_modifier_label(guint id, guint position)
{
    gint x;

    for (x=0; x<n_modifiers; x++)
        if ((modifiers[x].id == id) && (modifiers[x].position == position))
            return modifiers[x].label;

    return NULL;
}

static void modifier_linkable_list()
{
    guint group_id;
    guint count;
    guint i;

    send_message(REQUEST_MODIFIER_LINKABLE_LIST, "\x00\x01", 2);

    GString *data = NULL;

    do {
        if (data)
            g_string_free(data, TRUE);
        data = read_data();
    } while (get_message_id(data) != RECEIVE_MOFIFIER_LINKABLE_LIST);

    unpack_message(data);

    unsigned char *str = (unsigned char*)data->str;

    group_id = (str[8] << 8) | str[9];
    count = (str[10] << 8) | str[11];

    g_message("Group %d count %d", group_id, count);
    for (i=0; i<count; i++) {
        guint id = (str[12 + (i*3)] << 8) | str[13 + (i*3)];
        guint position = str[14 + (i*3)];

        g_message("ID: %d Position: %d Label: %s", id, position, get_modifier_label(id, position));
    }
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
            gui_create();
            gtk_main();
            gui_free();
        }
    }

    if (output != NULL)
        snd_rawmidi_close(output);
    if (input != NULL)
        snd_rawmidi_close(input);

    return EXIT_SUCCESS;
}
