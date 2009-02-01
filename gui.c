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

#include <gtk/gtk.h>
#include "gdigi.h"

void value_changed_cb(GtkSpinButton *spinbutton, void (*callback)(struct usb_dev_handle*, int))
{
    int val = gtk_spin_button_get_value_as_int(spinbutton);
    callback(handle, val);
}

void value_changed_option_cb(GtkSpinButton *spinbutton, void (*callback)(struct usb_dev_handle*, char, int))
{
    int val = gtk_spin_button_get_value_as_int(spinbutton);
    gint option = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(spinbutton), "option_id"));
    callback(handle, (char)option, val);
}

void toggled_cb(GtkToggleButton *button, void (*callback)(struct usb_dev_handle*, gboolean))
{
    gboolean val = gtk_toggle_button_get_active(button);
    callback(handle, val);
}

typedef struct {
    char *label;
    void (*callback)(struct usb_dev_handle*, int);
    void (*callback_with_option)(struct usb_dev_handle*, char, int);
    gdouble min;
    gdouble max;
    gint option;
} SettingsWidget;

static SettingsWidget wah_widgets[] = {
    {"Wah min", set_wah_min, NULL, 0.0, 99.0},
    {"Wah max", set_wah_max, NULL, 0.0, 99.0},
    {"Wah level", set_wah_level, NULL, 0.0, 12.0},
};

static SettingsWidget comp_widgets[] = {
    {"Compressor sustain", set_comp_sustain, NULL, 0.0, 99.0},
    {"Compressor tone (digi only!)", set_comp_tone, NULL, 0.0, 99.0},
    {"Compressor attack", set_comp_attack, NULL, 0.0, 99.0},
    {"Compressor level", set_comp_level, NULL, 0.0, 99.0},
};

static SettingsWidget dist_screamer_widgets[] = {
    {"Screamer drive", NULL, set_dist_option, 0.0, 99.0, DIST_SCREAMER_DRIVE},
    {"Screamer tone", NULL, set_dist_option, 0.0, 99.0, DIST_SCREAMER_TONE},
    {"Screamer level", NULL, set_dist_option, 0.0, 99.0, DIST_SCREAMER_LVL},
};

static SettingsWidget dist_808_widgets[] = {
    {"808 overdrive", NULL, set_dist_option, 0.0, 99.0, DIST_808_OVERDRIVE},
    {"808 tone", NULL, set_dist_option, 0.0, 99.0, DIST_808_TONE},
    {"808 level", NULL, set_dist_option, 0.0, 99.0, DIST_808_LVL},
};

static SettingsWidget dist_guyod_widgets[] = {
    {"GuyOD drive", NULL, set_dist_option, 0.0, 99.0, DIST_GUYOD_DRIVE},
    {"GuyOD level", NULL, set_dist_option, 0.0, 99.0, DIST_GUYOD_LVL},
};

static SettingsWidget dist_dod250_widgets[] = {
    {"DOD250 gain", NULL, set_dist_option, 0.0, 99.0, DIST_DOD250_GAIN},
    {"DOD250 level", NULL, set_dist_option, 0.0, 99.0, DIST_DOD250_LVL},
};

static SettingsWidget dist_rodent_widgets[] = {
    {"Rodent dist", NULL, set_dist_option, 0.0, 99.0, DIST_RODENT_DIST},
    {"Rodent filter", NULL, set_dist_option, 0.0, 99.0, DIST_RODENT_FILTER},
    {"Rodent level", NULL, set_dist_option, 0.0, 99.0, DIST_RODENT_LVL},
};

static SettingsWidget dist_mx_widgets[] = {
    {"MX dist", NULL, set_dist_option, 0.0, 99.0, DIST_MX_DIST},
    {"MX output", NULL, set_dist_option, 0.0, 99.0, DIST_MX_OUTPUT},
};

static SettingsWidget dist_ds_widgets[] = {
    {"DS gain", NULL, set_dist_option, 0.0, 99.0, DIST_DS_GAIN},
    {"DS tone", NULL, set_dist_option, 0.0, 99.0, DIST_DS_TONE},
    {"DS level", NULL, set_dist_option, 0.0, 99.0, DIST_DS_LVL},
};

static SettingsWidget dist_grunge_widgets[] = {
    {"Grunge", NULL, set_dist_option, 0.0, 99.0, DIST_GRUNGE_GRUNGE},
    {"Grunge face", NULL, set_dist_option, 0.0, 99.0, DIST_GRUNGE_FACE},
    {"Grunge loud", NULL, set_dist_option, 0.0, 99.0, DIST_GRUNGE_LOUD},
    {"Grunge butt", NULL, set_dist_option, 0.0, 99.0, DIST_GRUNGE_BUTT},
};

static SettingsWidget dist_zone_widgets[] = {
    {"Zone gain", NULL, set_dist_option, 0.0, 99.0, DIST_ZONE_GAIN},
    {"Zone low", NULL, set_dist_option, 0.0, 99.0, DIST_ZONE_LOW},
    {"Zone mid level", NULL, set_dist_option, 0.0, 99.0, DIST_ZONE_MID_LVL},
    {"Zone mid freq", NULL, set_dist_option, 0.0, 99.0, DIST_ZONE_MID_FREQ},
    {"Zone high", NULL, set_dist_option, 0.0, 99.0, DIST_ZONE_HIGH},
    {"Zone level", NULL, set_dist_option, 0.0, 99.0, DIST_ZONE_LEVEL},
};

static SettingsWidget dist_death_widgets[] = {
    {"Death low", NULL, set_dist_option, 0.0, 99.0, DIST_DEATH_LOW},
    {"Death mid", NULL, set_dist_option, 0.0, 99.0, DIST_DEATH_MID},
    {"Death high", NULL, set_dist_option, 0.0, 99.0, DIST_DEATH_HIGH},
    {"Death level", NULL, set_dist_option, 0.0, 99.0, DIST_DEATH_LVL},
};

static SettingsWidget dist_gonk_widgets[] = {
    {"Gonk gonk", NULL, set_dist_option, 0.0, 99.0, DIST_GONK_GONK},
    {"Gonk smear", NULL, set_dist_option, 0.0, 99.0, DIST_GONK_SMEAR},
    {"Gonk suck", NULL, set_dist_option, 0.0, 99.0, DIST_GONK_SUCK},
    {"Gonk heave", NULL, set_dist_option, 0.0, 99.0, DIST_GONK_HEAVE},
};

static SettingsWidget dist_fuzzy_widgets[] = {
    {"Fuzzy fuzz", NULL, set_dist_option, 0.0, 99.0, DIST_FUZZY_FUZZ},
    {"Fuzzy volume", NULL, set_dist_option, 0.0, 99.0, DIST_FUZZY_VOLUME},
};

static SettingsWidget dist_mp_widgets[] = {
    {"MP sustain", NULL, set_dist_option, 0.0, 99.0, DIST_MP_SUSTAIN},
    {"MP tone", NULL, set_dist_option, 0.0, 99.0, DIST_MP_TONE},
    {"MP volume", NULL, set_dist_option, 0.0, 99.0, DIST_MP_VOLUME},
};

static SettingsWidget eq_widgets[] = {
    {"EQ gain", set_eq_gain, NULL, 0.0, 99.0},
    {"EQ level", set_eq_level, NULL, 0.0, 99.0},
    // TODO: make those display propertly (display range -12 to 12)
    {"EQ bass", set_eq_bass, NULL, 0.0, 24.0},
    {"EQ mid", set_eq_mid, NULL, 0.0, 24.0},
    {"EQ treble", set_eq_treble, NULL, 0.0, 24.0},
};

static SettingsWidget noisegate_gate_widgets[] = {
    {"Gate treshold", NULL, set_gate_option, 0.0, 99.0, NOISEGATE_GATE_TRESHOLD},
    {"Gate attack", NULL, set_gate_option, 0.0, 99.0, NOISEGATE_ATTACK},
    {"Gate release", NULL, set_gate_option, 0.0, 99.0, NOISEGATE_RELEASE},
    {"Gate attn", NULL, set_gate_option, 0.0, 99.0, NOISEGATE_ATTN},
};

static SettingsWidget noisegate_swell_widgets[] = {
    {"Swell sens", NULL, set_gate_option, 0.0, 99.0, NOISEGATE_SWELL_SENS},
    {"Swell attack", NULL, set_gate_option, 0.0, 99.0, NOISEGATE_ATTACK},
    {"Swell release", NULL, set_gate_option, 0.0, 99.0, NOISEGATE_RELEASE},
    {"Swell attn", NULL, set_gate_option, 0.0, 99.0, NOISEGATE_ATTN},
};

static SettingsWidget chorusfx_ce_widgets[] = {
    {"CE chorus speed", NULL, set_chorusfx_option, 0.0, 99.0, CE_CHORUS_SPEED},
    {"CE chorus depth", NULL, set_chorusfx_option, 0.0, 99.0, CE_CHORUS_DEPTH},
};

static SettingsWidget chorusfx_dual_widgets[] = {
    {"Dual chorus speed", NULL, set_chorusfx_option, 0.0, 99.0, DUAL_CHORUS_SPEED},
    {"Dual chorus depth", NULL, set_chorusfx_option, 0.0, 99.0, DUAL_CHORUS_DEPTH},
    {"Dual chorus level", NULL, set_chorusfx_option, 0.0, 99.0, DUAL_CHORUS_LEVEL},
    // TODO: DUAL_CHORUS_WAVE with valid options WAVE_TRI, WAVE_SINE, WAVE_SQUARE
};

static SettingsWidget chorusfx_multi_widgets[] = {
    {"Multi chorus speed", NULL, set_chorusfx_option, 0.0, 99.0, MULTI_CHORUS_SPEED},
    {"Multi chorus depth", NULL, set_chorusfx_option, 0.0, 99.0, MULTI_CHORUS_DEPTH},
    {"Multi chorus level", NULL, set_chorusfx_option, 0.0, 99.0, MULTI_CHORUS_LEVEL},
    // TODO: MULTI_CHORUS_WAVE with valid options WAVE_TRI, WAVE_SINE, WAVE_SQUARE
};

static SettingsWidget chorusfx_flanger_widgets[] = {
    {"Flanger speed", NULL, set_flanger_option, 0.0, 99.0, FLANGER_SPEED},
    {"Flanger depth", NULL, set_flanger_option, 0.0, 99.0, FLANGER_DEPTH},
    {"Flanger regen", NULL, set_flanger_option, 0.0, 99.0, FLANGER_REGEN},
    {"Flanger level", NULL, set_flanger_option, 0.0, 99.0, FLANGER_LEVEL},
    // TODO: FLANGER_WAVE with valid options WAVE_TRI, WAVE_SINE, WAVE_SQUARE
};

static SettingsWidget chorusfx_mxr_flanger_widgets[] = {
    {"MXR flanger speed", NULL, set_flanger_option, 0.0, 99.0, MXR_FLANGER_SPEED},
    {"MXR flanger width", NULL, set_flanger_option, 0.0, 99.0, MXR_FLANGER_WIDTH},
    {"MXR flanger regen", NULL, set_flanger_option, 0.0, 99.0, MXR_FLANGER_REGEN},
    {"MXR flanger manual", NULL, set_flanger_option, 0.0, 99.0, MXR_FLANGER_MANUAL},
};

static SettingsWidget chorusfx_phaser_widgets[] = {
    {"Phaser speed", NULL, set_flanger_option, 0.0, 99.0, PHASER_SPEED},
    {"Phaser depth", NULL, set_flanger_option, 0.0, 99.0, PHASER_DEPTH},
    {"Phaser regen", NULL, set_flanger_option, 0.0, 99.0, PHASER_REGEN},
    {"Phaser level", NULL, set_flanger_option, 0.0, 99.0, PHASER_LEVEL},
    // TODO: PHASER_WAVE with valid options WAVE_TRI, WAVE_SINE, WAVE_SQUARE
};

static SettingsWidget chorusfx_vibrato_widgets[] = {
    {"Vibrato speed", NULL, set_vibrato_option, 0.0, 99.0, VIBRATO_SPEED},
    {"Vibrato depth", NULL, set_vibrato_option, 0.0, 99.0, VIBRATO_DEPTH},
};

static SettingsWidget chorusfx_rotary_widgets[] = {
    {"Rotary speed", NULL, set_vibrato_option, 0.0, 99.0, ROTARY_SPEED},
    {"Rotary intensity", NULL, set_vibrato_option, 0.0, 99.0, ROTARY_INTENSITY},
    {"Rotary doppler", NULL, set_vibrato_option, 0.0, 99.0, ROTARY_DOPPLER},
    {"Rotary crossover", NULL, set_vibrato_option, 0.0, 99.0, ROTARY_CROSSOVER},
};

static SettingsWidget chorusfx_vibropan_widgets[] = {
    {"Vibropan speed", NULL, set_vibrato_option, 0.0, 99.0, VIBROPAN_SPEED},
    {"Vibropan depth", NULL, set_vibrato_option, 0.0, 99.0, VIBROPAN_DEPTH},
    {"Vibropan vibra", NULL, set_vibrato_option, 0.0, 99.0, VIBROPAN_VIBRA},
    // TODO: VIBROPAN_WAVE with valid options WAVE_TRI, WAVE_SINE, WAVE_SQUARE
};

static SettingsWidget chorusfx_tremolo_widgets[] = {
    {"Tremolo speed", NULL, set_tremolo_option, 0.0, 99.0, TREMOLO_SPEED},
    {"Tremolo depth", NULL, set_tremolo_option, 0.0, 99.0, TREMOLO_DEPTH},
    // TODO: TREMOLO_WAVE with valid options WAVE_TRI, WAVE_SINE, WAVE_SQUARE
};

static SettingsWidget chorusfx_panner_widgets[] = {
    {"Panner speed", NULL, set_tremolo_option, 0.0, 99.0, PANNER_SPEED},
    {"Panner depth", NULL, set_tremolo_option, 0.0, 99.0, PANNER_DEPTH},
    // TODO: PANNER_WAVE with valid options WAVE_TRI, WAVE_SINE, WAVE_SQUARE
};

static SettingsWidget chorusfx_envelope_widgets[] = {
    {"Envelope sensitivity", NULL, set_envelope_option, 0.0, 99.0, ENVELOPE_SENSITIVITY},
    {"Envelope range", NULL, set_envelope_option, 0.0, 99.0, ENVELOPE_RANGE},
};

static SettingsWidget chorusfx_autoya_widgets[] = {
    {"AutoYa speed", NULL, set_ya_option, 0.0, 99.0, AUTOYA_SPEED},
    {"AutoYa intensity", NULL, set_ya_option, 0.0, 99.0, AUTOYA_INTENSITY},
    {"AutoYa range", NULL, set_ya_option, 0.0, 49.0, AUTOYA_RANGE},
};

static SettingsWidget chorusfx_yaya_widgets[] = {
    {"YaYa pedal", NULL, set_ya_option, 0.0, 99.0, YAYA_PEDAL},
    {"YaYa intensity", NULL, set_ya_option, 0.0, 99.0, YAYA_INTENSITY},
    {"YaYa range", NULL, set_ya_option, 0.0, 49.0, YAYA_RANGE},
};

static SettingsWidget chorusfx_step_filter_widgets[] = {
    {"Step filter speed", NULL, set_filter_option, 0.0, 99.0, STEP_FILTER_SPEED},
    {"Step filter intensity", NULL, set_filter_option, 0.0, 99.0, STEP_FILTER_INTENSITY},
};

static SettingsWidget chorusfx_whammy_widgets[] = {
    {"Whammy pedal", NULL, set_whammy_option, 0.0, 99.0, WHAMMY_PEDAL},
    {"Whammy mix", NULL, set_whammy_option, 0.0, 99.0, WHAMMY_MIX},
    //TODO: WHAMMY_AMOUNT with valid options:
    //      WHAMMY_OCT_UP, WHAMMY_2OCT_UP, WHAMMY_2ND_DN, WHAMMY_RV_2ND,
    //      WHAMMY_4TH_DN, WHAMMY_OCT_DN, WHAMMY_2OCT_DN, WHAMMY_DIV_BMB,
    //      WHAMMY_M3_MA, WHAMMY_2ND_MA3, WHAMMY_3RD_4TH, WHAMMY_4TH_5TH,
    //      WHAMMY_5TH_OCT, WHAMMY_HOCT_UP, WHAMMY_HOCT_DN, WHAMMY_OCT_UD
};

static SettingsWidget chorusfx_pitch_shift_widgets[] = {
    // TODO: make this display propertly (display range -24 to 24)
    {"Pitch amount", NULL, set_pitch_option, 0.0, 48.0, PITCH_AMOUNT},
    {"Pitch mix", NULL, set_pitch_option, 0.0, 99.0, PITCH_MIX},
};

static SettingsWidget chorusfx_detune_widgets[] = {
    {"Detune amount", NULL, set_pitch_option, 0.0, 48.0, DETUNE_AMOUNT},
    {"Detune level", NULL, set_pitch_option, 0.0, 99.0, DETUNE_LEVEL},
};

static SettingsWidget chorusfx_ips_widgets[] = {
    // TODO: IPS_SHIFT_AMOUNT with valid options:
    //       IPS_7TH_DN, IPS_6TH_DN, IPS_5TH_DN, IPS_4TH_DN, IPS_3RD_DN,
    //       IPS_2ND_DN, IPS_2ND_UP, IPS_3RD_UP, IPS_4TH_UP, IPS_5TH_UP,
    //       IPS_6TH_UP, IPS_7TH_UP, IPS_OCT_U

    // TODO: IPS_KEY with valid options:
    //       IPS_E, IPS_F, IPS_GB, IPS_G, IPS_AB, IPS_A, IPS_BB, IPS_B,
    //       IPS_C, IPS_DD, IPS_D, IPS_EB

    // TODO: IPS_SCALE with valid options:
    //       IPS_MAJOR, IPS_MINOR, IPS_DORIA, IPS_MIXLYD, IPS_LYDIAN, IPS_HMINO

    {"IPS level", NULL, set_ips_option, 0.0, 99.0, IPS_LEVEL},
};

static SettingsWidget delay_analog_widgets[] = {
    // TODO: make this display propertly (10 msec to 5 sec)
    {"Delay time", set_delay_time, NULL, 0.0, 139.0},
    {"Delay analog level", NULL, set_delay_option, 0.0, 99.0, ANALOG_LEVEL},
    // TODO: make last value display propertly
    {"Delay analog repeats", NULL, set_delay_option, 0.0, 100.0, ANALOG_REPEATS},
};

static SettingsWidget delay_digital_widgets[] = {
    // TODO: make this display propertly (10 msec to 5 sec)
    {"Delay time", set_delay_time, NULL, 0.0, 139.0},
    {"Delay digital level", NULL, set_delay_option, 0.0, 99.0, DIGITAL_LEVEL},
    // TODO: make last value display propertly
    {"Delay digital repeats", NULL, set_delay_option, 0.0, 100.0, DIGITAL_REPEATS},
    {"Delay digital ducker thresh", NULL, set_delay_option, 0.0, 99.0, DIGITAL_DUCKER_THRESH},
    {"Delay digital ducker level", NULL, set_delay_option, 0.0, 99.0, DIGITAL_DUCKER_LEVEL},
};

static SettingsWidget delay_modulated_widgets[] = {
    // TODO: make this display propertly (10 msec to 5 sec)
    {"Delay time", set_delay_time, NULL, 0.0, 139.0},
    {"Delay modulated level", NULL, set_delay_option, 0.0, 99.0, MODULATED_LEVEL},
    // TODO: make last value display propertly
    {"Delay modulated repeats", NULL, set_delay_option, 0.0, 100.0, MODULATED_REPEATS},
    {"Delay modulated depth", NULL, set_delay_option, 0.0, 99.0, MODULATED_DEPTH},
};

static SettingsWidget delay_pong_widgets[] = {
    // TODO: make this display propertly (10 msec to 5 sec)
    {"Delay time", set_delay_time, NULL, 0.0, 139.0},
    {"Delay pong level", NULL, set_delay_option, 0.0, 99.0, PONG_LEVEL},
    // TODO: make last value display propertly
    {"Delay pong repeats", NULL, set_delay_option, 0.0, 100.0, PONG_REPEATS},
    {"Delay pong ducker thresh", NULL, set_delay_option, 0.0, 99.0, PONG_DUCKER_THRESH},
    {"Delay pong ducker level", NULL, set_delay_option, 0.0, 99.0, PONG_DUCKER_LEVEL},
};

static SettingsWidget delay_tape_widgets[] = {
    // TODO: make this display propertly (10 msec to 5 sec)
    {"Delay time", set_delay_time, NULL, 0.0, 139.0},
    {"Delay tape level", NULL, set_delay_option, 0.0, 99.0, TAPE_LEVEL},
    // TODO: make last value display propertly
    {"Delay tape repeats", NULL, set_delay_option, 0.0, 100.0, TAPE_REPEATS},
    {"Delay tape wow", NULL, set_delay_option, 0.0, 99.0, TAPE_WOW},
    {"Delay tape flutter", NULL, set_delay_option, 0.0, 99.0, TAPE_FLUTTER},
};

static SettingsWidget reverb_twin_widgets[] = {
    {"Twin reverb", NULL, set_reverb_option, 0.0, 99.0, TWIN_REVERB},
};

static SettingsWidget reverb_lex_ambience_widgets[] = {
    {"Predelay", NULL, set_reverb_option, 0.0, 15.0, LEX_AMBIENCE_PREDELAY},
    {"Decay", NULL, set_reverb_option, 0.0, 99.0, LEX_AMBIENCE_DECAY},
    {"Liveliness", NULL, set_reverb_option, 0.0, 99.0, LEX_AMBIENCE_LIVELINESS},
    {"Level", NULL, set_reverb_option, 0.0, 99.0, LEX_AMBIENCE_LEVEL},
};

static SettingsWidget reverb_lex_studio_widgets[] = {
    {"Predelay", NULL, set_reverb_option, 0.0, 15.0, LEX_STUDIO_PREDELAY},
    {"Decay", NULL, set_reverb_option, 0.0, 99.0, LEX_STUDIO_DECAY},
    {"Liveliness", NULL, set_reverb_option, 0.0, 99.0, LEX_STUDIO_LIVELINESS},
    {"Level", NULL, set_reverb_option, 0.0, 99.0, LEX_STUDIO_LEVEL},
};

static SettingsWidget reverb_lex_room_widgets[] = {
    {"Predelay", NULL, set_reverb_option, 0.0, 15.0, LEX_ROOM_PREDELAY},
    {"Decay", NULL, set_reverb_option, 0.0, 99.0, LEX_ROOM_DECAY},
    {"Liveliness", NULL, set_reverb_option, 0.0, 99.0, LEX_ROOM_LIVELINESS},
    {"Level", NULL, set_reverb_option, 0.0, 99.0, LEX_ROOM_LEVEL},
};

static SettingsWidget reverb_lex_hall_widgets[] = {
    {"Predelay", NULL, set_reverb_option, 0.0, 15.0, LEX_HALL_PREDELAY},
    {"Decay", NULL, set_reverb_option, 0.0, 99.0, LEX_HALL_DECAY},
    {"Liveliness", NULL, set_reverb_option, 0.0, 99.0, LEX_HALL_LIVELINESS},
    {"Level", NULL, set_reverb_option, 0.0, 99.0, LEX_HALL_LEVEL},
};

static SettingsWidget reverb_emt240_plate_widgets[] = {
    {"Predelay", NULL, set_reverb_option, 0.0, 15.0, EMT240_PLATE_PREDELAY},
    {"Decay", NULL, set_reverb_option, 0.0, 99.0, EMT240_PLATE_DECAY},
    {"Liveliness", NULL, set_reverb_option, 0.0, 99.0, EMT240_PLATE_LIVELINESS},
    {"Level", NULL, set_reverb_option, 0.0, 99.0, EMT240_PLATE_LEVEL},
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
        if (widgets[x].callback)
            g_signal_connect(G_OBJECT(widget), "value-changed", G_CALLBACK(value_changed_cb), widgets[x].callback);

        if (widgets[x].callback_with_option) {
            g_object_set_data(G_OBJECT(widget), "option_id", GINT_TO_POINTER(widgets[x].option));
            g_signal_connect(G_OBJECT(widget), "value-changed", G_CALLBACK(value_changed_option_cb), widgets[x].callback_with_option);
        }

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
