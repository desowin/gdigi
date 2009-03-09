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

#include "gdigi.h"
#include "effects.h"

static gchar *wave_labels[] = {
    "Tri",
    "Sine",
    "Square",
    NULL,
};

static gchar *whammy_labels[] = {
    "OctUp",
    "2OctUp",
    "2ndDn",
    "Rv2nd",
    "4thDn",
    "OctDn",
    "2OctDn",
    "DivBmb",
    "M3>Ma3",
    "2ndMa3",
    "3rd4th",
    "4th5th",
    "5thOct",
    "HOctUp",
    "HOctDn",
    "OctU>D",
    NULL,
};

static gchar *ips_shift_labels[] = {
    "Oct Dn",
    "7th Dn",
    "6th Dn",
    "5th Dn",
    "4th Dn",
    "3rd Dn",
    "2nd Dn",
    "2nd Up",
    "3rd Up",
    "4th Up",
    "5th Up",
    "6th Up",
    "7th Up",
    "Oct Up",
    NULL,
};

static gchar *ips_key_labels[] = {
    "E",
    "F",
    "Gb",
    "G",
    "Ab",
    "A",
    "Bb",
    "B",
    "C",
    "Db",
    "D",
    "Eb",
    NULL,
};

static gchar *ips_scale_labels[] = {
    "Major",
    "Minor",
    "Dorian",
    "Mixolydian",
    "Lydian",
    "HMinor",
    NULL,
};

static EffectSettings wah_settings[] = {
    {"Wah min", 0.0, 99.0, WAH_MIN, WAH_POSITION_MIN_MAX},
    {"Wah max", 0.0, 99.0, WAH_MAX, WAH_POSITION_MIN_MAX},
    {"Wah level", 0.0, 12.0, WAH_LEVEL, WAH_POSITION},
};

static EffectSettings comp_digi_settings[] = {
    {"Compressor sustain", 0.0, 99.0, COMP_SUSTAIN, COMP_POSITION},
    {"Compressor tone", 0.0, 99.0, COMP_TONE, COMP_POSITION},
    {"Compressor attack", 0.0, 99.0, COMP_ATTACK, COMP_POSITION},
    {"Compressor level", 0.0, 99.0, COMP_LEVEL, COMP_POSITION},
};

static EffectSettings comp_cs_settings[] = {
    {"Compressor sustain", 0.0, 99.0, COMP_SUSTAIN, COMP_POSITION},
    {"Compressor attack", 0.0, 99.0, COMP_ATTACK, COMP_POSITION},
    {"Compressor level", 0.0, 99.0, COMP_LEVEL, COMP_POSITION},
};

static EffectSettings dist_screamer_settings[] = {
    {"Screamer drive", 0.0, 99.0, DIST_SCREAMER_DRIVE, DIST_POSITION},
    {"Screamer tone", 0.0, 99.0, DIST_SCREAMER_TONE, DIST_POSITION},
    {"Screamer level", 0.0, 99.0, DIST_SCREAMER_LVL, DIST_POSITION},
};

static EffectSettings dist_808_settings[] = {
    {"808 overdrive", 0.0, 99.0, DIST_808_OVERDRIVE, DIST_POSITION},
    {"808 tone", 0.0, 99.0, DIST_808_TONE, DIST_POSITION},
    {"808 level", 0.0, 99.0, DIST_808_LVL, DIST_POSITION},
};

static EffectSettings dist_guyod_settings[] = {
    {"GuyOD drive", 0.0, 99.0, DIST_GUYOD_DRIVE, DIST_POSITION},
    {"GuyOD level", 0.0, 99.0, DIST_GUYOD_LVL, DIST_POSITION},
};

static EffectSettings dist_dod250_settings[] = {
    {"DOD250 gain", 0.0, 99.0, DIST_DOD250_GAIN, DIST_POSITION},
    {"DOD250 level", 0.0, 99.0, DIST_DOD250_LVL, DIST_POSITION},
};

static EffectSettings dist_rodent_settings[] = {
    {"Rodent dist", 0.0, 99.0, DIST_RODENT_DIST, DIST_POSITION},
    {"Rodent filter", 0.0, 99.0, DIST_RODENT_FILTER, DIST_POSITION},
    {"Rodent level", 0.0, 99.0, DIST_RODENT_LVL, DIST_POSITION},
};

static EffectSettings dist_mx_settings[] = {
    {"MX dist", 0.0, 99.0, DIST_MX_DIST, DIST_POSITION},
    {"MX output", 0.0, 99.0, DIST_MX_OUTPUT, DIST_POSITION},
};

static EffectSettings dist_ds_settings[] = {
    {"DS gain", 0.0, 99.0, DIST_DS_GAIN, DIST_POSITION},
    {"DS tone", 0.0, 99.0, DIST_DS_TONE, DIST_POSITION},
    {"DS level", 0.0, 99.0, DIST_DS_LVL, DIST_POSITION},
};

static EffectSettings dist_grunge_settings[] = {
    {"Grunge", 0.0, 99.0, DIST_GRUNGE_GRUNGE, DIST_POSITION},
    {"Grunge face", 0.0, 99.0, DIST_GRUNGE_FACE, DIST_POSITION},
    {"Grunge loud", 0.0, 99.0, DIST_GRUNGE_LOUD, DIST_POSITION},
    {"Grunge butt", 0.0, 99.0, DIST_GRUNGE_BUTT, DIST_POSITION},
};

static EffectSettings dist_zone_settings[] = {
    {"Zone gain", 0.0, 99.0, DIST_ZONE_GAIN, DIST_POSITION},
    {"Zone low", 0.0, 99.0, DIST_ZONE_LOW, DIST_POSITION},
    {"Zone mid level", 0.0, 99.0, DIST_ZONE_MID_LVL, DIST_POSITION},
    {"Zone mid freq", 0.0, 99.0, DIST_ZONE_MID_FREQ, DIST_POSITION},
    {"Zone high", 0.0, 99.0, DIST_ZONE_HIGH, DIST_POSITION},
    {"Zone level", 0.0, 99.0, DIST_ZONE_LEVEL, DIST_POSITION},
};

static EffectSettings dist_death_settings[] = {
    {"Death low", 0.0, 99.0, DIST_DEATH_LOW, DIST_POSITION},
    {"Death mid", 0.0, 99.0, DIST_DEATH_MID, DIST_POSITION},
    {"Death high", 0.0, 99.0, DIST_DEATH_HIGH, DIST_POSITION},
    {"Death level", 0.0, 99.0, DIST_DEATH_LVL, DIST_POSITION},
};

static EffectSettings dist_gonk_settings[] = {
    {"Gonk gonk", 0.0, 99.0, DIST_GONK_GONK, DIST_POSITION},
    {"Gonk smear", 0.0, 99.0, DIST_GONK_SMEAR, DIST_POSITION},
    {"Gonk suck", 0.0, 99.0, DIST_GONK_SUCK, DIST_POSITION},
    {"Gonk heave", 0.0, 99.0, DIST_GONK_HEAVE, DIST_POSITION},
};

static EffectSettings dist_fuzzy_settings[] = {
    {"Fuzzy fuzz", 0.0, 99.0, DIST_FUZZY_FUZZ, DIST_POSITION},
    {"Fuzzy volume", 0.0, 99.0, DIST_FUZZY_VOLUME, DIST_POSITION},
};

static EffectSettings dist_mp_settings[] = {
    {"MP sustain", 0.0, 99.0, DIST_MP_SUSTAIN, DIST_POSITION},
    {"MP tone", 0.0, 99.0, DIST_MP_TONE, DIST_POSITION},
    {"MP volume", 0.0, 99.0, DIST_MP_VOLUME, DIST_POSITION},
};

static EffectSettings amp_settings[] = {
    {"AMP gain", 0.0, 99.0, AMP_GAIN, AMP_POSITION},
    {"AMP level", 0.0, 99.0, AMP_LEVEL, AMP_POSITION},
};

static EffectSettings amp_settings2[] = {
    {"AMP level", 0.0, 99.0, AMP_LEVEL, AMP_POSITION},
};

static EffectSettings eq_settings[] = {
    /* TODO: make those display propertly (display range -12 to 12) */
    {"EQ bass", 0.0, 24.0, EQ_BASS, EQ_POSITION},
    {"EQ mid", 0.0, 24.0, EQ_MID, EQ_POSITION},
    {"EQ treble", 0.0, 24.0, EQ_TREBLE, EQ_POSITION},
    /* TODO: make this display propertly (display range 300 to 5000) */
    {"EQ mid Hz", 0.0, 4700.0, EQ_MID_HZ, EQ_POSITION},
    /* TODO: make this display propertly (display range 500 to 8000) */
    {"EQ treb Hz", 0.0, 7500.0, EQ_TREBLE_HZ, EQ_POSITION},
};

static EffectSettings noisegate_gate_settings[] = {
    {"Gate treshold", 0.0, 99.0, NOISEGATE_GATE_TRESHOLD, NOISEGATE_POSITION},
    {"Gate attack", 0.0, 99.0, NOISEGATE_ATTACK, NOISEGATE_POSITION},
    {"Gate release", 0.0, 99.0, NOISEGATE_RELEASE, NOISEGATE_POSITION},
    {"Gate attn", 0.0, 99.0, NOISEGATE_ATTN, NOISEGATE_POSITION},
};

static EffectSettings noisegate_swell_settings[] = {
    {"Swell sens", 0.0, 99.0, NOISEGATE_SWELL_SENS, NOISEGATE_POSITION},
    {"Swell attack", 0.0, 99.0, NOISEGATE_ATTACK, NOISEGATE_POSITION},
    {"Swell release", 0.0, 99.0, NOISEGATE_RELEASE, NOISEGATE_POSITION},
    {"Swell attn", 0.0, 99.0, NOISEGATE_ATTN, NOISEGATE_POSITION},
};

static EffectSettings chorusfx_ce_settings[] = {
    {"CE chorus speed", 0.0, 99.0, CE_CHORUS_SPEED, CHORUSFX_POSITION},
    {"CE chorus depth", 0.0, 99.0, CE_CHORUS_DEPTH, CHORUSFX_POSITION},
};

static EffectSettings chorusfx_dual_settings[] = {
    {"Dual chorus speed", 0.0, 99.0, DUAL_CHORUS_SPEED, CHORUSFX_POSITION},
    {"Dual chorus depth", 0.0, 99.0, DUAL_CHORUS_DEPTH, CHORUSFX_POSITION},
    {"Dual chorus level", 0.0, 99.0, DUAL_CHORUS_LEVEL, CHORUSFX_POSITION},
    {"Dual chorus wave", 0.0, 2.0, DUAL_CHORUS_WAVE, CHORUSFX_POSITION, wave_labels},
};

static EffectSettings chorusfx_multi_settings[] = {
    {"Multi chorus speed", 0.0, 99.0, MULTI_CHORUS_SPEED, CHORUSFX_POSITION},
    {"Multi chorus depth", 0.0, 99.0, MULTI_CHORUS_DEPTH, CHORUSFX_POSITION},
    {"Multi chorus level", 0.0, 99.0, MULTI_CHORUS_LEVEL, CHORUSFX_POSITION},
    {"Multi chorus wave", 0.0, 2.0, MULTI_CHORUS_WAVE, CHORUSFX_POSITION, wave_labels},
};

static EffectSettings chorusfx_flanger_settings[] = {
    {"Flanger speed", 0.0, 99.0, FLANGER_SPEED, CHORUSFX_POSITION},
    {"Flanger depth", 0.0, 99.0, FLANGER_DEPTH, CHORUSFX_POSITION},
    {"Flanger regen", 0.0, 99.0, FLANGER_REGEN, CHORUSFX_POSITION},
    {"Flanger level", 0.0, 99.0, FLANGER_LEVEL, CHORUSFX_POSITION},
    {"Flanger wave", 0.0, 2.0, FLANGER_WAVE, CHORUSFX_POSITION, wave_labels},
};

static EffectSettings chorusfx_mxr_flanger_settings[] = {
    {"MXR flanger speed", 0.0, 99.0, MXR_FLANGER_SPEED, CHORUSFX_POSITION},
    {"MXR flanger width", 0.0, 99.0, MXR_FLANGER_WIDTH, CHORUSFX_POSITION},
    {"MXR flanger regen", 0.0, 99.0, MXR_FLANGER_REGEN, CHORUSFX_POSITION},
    {"MXR flanger manual", 0.0, 99.0, MXR_FLANGER_MANUAL, CHORUSFX_POSITION},
};

static EffectSettings chorusfx_phaser_settings[] = {
    {"Phaser speed", 0.0, 99.0, PHASER_SPEED, CHORUSFX_POSITION},
    {"Phaser depth", 0.0, 99.0, PHASER_DEPTH, CHORUSFX_POSITION},
    {"Phaser regen", 0.0, 99.0, PHASER_REGEN, CHORUSFX_POSITION},
    {"Phaser level", 0.0, 99.0, PHASER_LEVEL, CHORUSFX_POSITION},
    {"Phaser wave", 0.0, 2.0, PHASER_WAVE, CHORUSFX_POSITION, wave_labels},
};

static EffectSettings chorusfx_vibrato_settings[] = {
    {"Vibrato speed", 0.0, 99.0, VIBRATO_SPEED, CHORUSFX_POSITION},
    {"Vibrato depth", 0.0, 99.0, VIBRATO_DEPTH, CHORUSFX_POSITION},
};

static EffectSettings chorusfx_rotary_settings[] = {
    {"Rotary speed", 0.0, 99.0, ROTARY_SPEED, CHORUSFX_POSITION},
    {"Rotary intensity", 0.0, 99.0, ROTARY_INTENSITY, CHORUSFX_POSITION},
    {"Rotary doppler", 0.0, 99.0, ROTARY_DOPPLER, CHORUSFX_POSITION},
    {"Rotary crossover", 0.0, 99.0, ROTARY_CROSSOVER, CHORUSFX_POSITION},
};

static EffectSettings chorusfx_vibropan_settings[] = {
    {"Vibropan speed", 0.0, 99.0, VIBROPAN_SPEED, CHORUSFX_POSITION},
    {"Vibropan depth", 0.0, 99.0, VIBROPAN_DEPTH, CHORUSFX_POSITION},
    {"Vibropan vibra", 0.0, 99.0, VIBROPAN_VIBRA, CHORUSFX_POSITION},
    {"Vibropan wave", 0.0, 2.0, VIBROPAN_WAVE, CHORUSFX_POSITION, wave_labels},
};

static EffectSettings chorusfx_tremolo_settings[] = {
    {"Tremolo speed", 0.0, 99.0, TREMOLO_SPEED, CHORUSFX_POSITION},
    {"Tremolo depth", 0.0, 99.0, TREMOLO_DEPTH, CHORUSFX_POSITION},
    {"Tremolo wave", 0.0, 2.0, TREMOLO_WAVE, CHORUSFX_POSITION, wave_labels},
};

static EffectSettings chorusfx_panner_settings[] = {
    {"Panner speed", 0.0, 99.0, PANNER_SPEED, CHORUSFX_POSITION},
    {"Panner depth", 0.0, 99.0, PANNER_DEPTH, CHORUSFX_POSITION},
    {"Panner wave", 0.0, 2.0, PANNER_WAVE, CHORUSFX_POSITION, wave_labels},
};

static EffectSettings chorusfx_envelope_settings[] = {
    {"Envelope sensitivity", 0.0, 99.0, ENVELOPE_SENSITIVITY, CHORUSFX_POSITION},
    {"Envelope range", 0.0, 99.0, ENVELOPE_RANGE, CHORUSFX_POSITION},
};

static EffectSettings chorusfx_autoya_settings[] = {
    {"AutoYa speed", 0.0, 99.0, AUTOYA_SPEED, CHORUSFX_POSITION},
    {"AutoYa intensity", 0.0, 99.0, AUTOYA_INTENSITY, CHORUSFX_POSITION},
    {"AutoYa range", 0.0, 49.0, AUTOYA_RANGE, CHORUSFX_POSITION},
};

static EffectSettings chorusfx_yaya_settings[] = {
    {"YaYa pedal", 0.0, 99.0, YAYA_PEDAL, CHORUSFX_POSITION},
    {"YaYa intensity", 0.0, 99.0, YAYA_INTENSITY, CHORUSFX_POSITION},
    {"YaYa range", 0.0, 49.0, YAYA_RANGE, CHORUSFX_POSITION},
};

static EffectSettings chorusfx_step_filter_settings[] = {
    {"Step filter speed", 0.0, 99.0, STEP_FILTER_SPEED, CHORUSFX_POSITION},
    {"Step filter intensity", 0.0, 99.0, STEP_FILTER_INTENSITY, CHORUSFX_POSITION},
};

static EffectSettings chorusfx_whammy_settings[] = {
    {"Whammy pedal", 0.0, 99.0, WHAMMY_PEDAL, CHORUSFX_POSITION},
    {"Whammy mix", 0.0, 99.0, WHAMMY_MIX, CHORUSFX_POSITION},
    {"Whammy amount", 0.0, 15.0, WHAMMY_AMOUNT, CHORUSFX_POSITION, whammy_labels},
};

static EffectSettings chorusfx_pitch_shift_settings[] = {
    /* TODO: make this display propertly (display range -24 to 24) */
    {"Pitch amount", 0.0, 48.0, PITCH_AMOUNT, CHORUSFX_POSITION},
    {"Pitch mix", 0.0, 99.0, PITCH_MIX, CHORUSFX_POSITION},
};

static EffectSettings chorusfx_detune_settings[] = {
    {"Detune amount", 0.0, 48.0, DETUNE_AMOUNT, CHORUSFX_POSITION},
    {"Detune level", 0.0, 99.0, DETUNE_LEVEL, CHORUSFX_POSITION},
};

static EffectSettings chorusfx_ips_settings[] = {
    {"IPS shift", 0.0, 13.0, IPS_SHIFT_AMOUNT, CHORUSFX_POSITION, ips_shift_labels},
    {"IPS key", 0.0, 11.0, IPS_KEY, CHORUSFX_POSITION, ips_key_labels},
    {"IPS scale", 0.0, 5.0, IPS_SCALE, CHORUSFX_POSITION, ips_scale_labels},
    {"IPS level", 0.0, 99.0, IPS_LEVEL, CHORUSFX_POSITION},
};

static EffectSettings delay_analog_settings[] = {
    /* TODO: make this display propertly (10 msec to 5 sec) */
    {"Delay time", 0.0, 139.0, DELAY_TIME, DELAY_POSITION},
    {"Delay analog level", 0.0, 99.0, ANALOG_LEVEL, DELAY_POSITION},
    /* TODO: make last value display propertly */
    {"Delay analog repeats", 0.0, 100.0, ANALOG_REPEATS, DELAY_POSITION},
};

static EffectSettings delay_digital_settings[] = {
    /* TODO: make this display propertly (10 msec to 5 sec) */
    {"Delay time", 0.0, 139.0, DELAY_TIME, DELAY_POSITION},
    {"Delay digital level", 0.0, 99.0, DIGITAL_LEVEL, DELAY_POSITION},
    /* TODO: make last value display propertly */
    {"Delay digital repeats", 0.0, 100.0, DIGITAL_REPEATS, DELAY_POSITION},
    {"Delay digital ducker thresh", 0.0, 99.0, DIGITAL_DUCKER_THRESH, DELAY_POSITION},
    {"Delay digital ducker level", 0.0, 99.0, DIGITAL_DUCKER_LEVEL, DELAY_POSITION},
};

static EffectSettings delay_modulated_settings[] = {
    /* TODO: make this display propertly (10 msec to 5 sec) */
    {"Delay time", 0.0, 139.0, DELAY_TIME, DELAY_POSITION},
    {"Delay modulated level", 0.0, 99.0, MODULATED_LEVEL, DELAY_POSITION},
    /* TODO: make last value display propertly */
    {"Delay modulated repeats", 0.0, 100.0, MODULATED_REPEATS, DELAY_POSITION},
    {"Delay modulated depth", 0.0, 99.0, MODULATED_DEPTH, DELAY_POSITION},
};

static EffectSettings delay_pong_settings[] = {
    /* TODO: make this display propertly (10 msec to 5 sec) */
    {"Delay time", 0.0, 139.0, DELAY_TIME, DELAY_POSITION},
    {"Delay pong level", 0.0, 99.0, PONG_LEVEL, DELAY_POSITION},
    /* TODO: make last value display propertly */
    {"Delay pong repeats", 0.0, 100.0, PONG_REPEATS, DELAY_POSITION},
    {"Delay pong ducker thresh", 0.0, 99.0, PONG_DUCKER_THRESH, DELAY_POSITION},
    {"Delay pong ducker level", 0.0, 99.0, PONG_DUCKER_LEVEL, DELAY_POSITION},
};

static EffectSettings delay_tape_settings[] = {
    /* TODO: make this display propertly (10 msec to 5 sec) */
    {"Delay time", 0.0, 139.0, DELAY_TIME, DELAY_POSITION},
    {"Delay tape level", 0.0, 99.0, TAPE_LEVEL, DELAY_POSITION},
    /* TODO: make last value display propertly */
    {"Delay tape repeats", 0.0, 100.0, TAPE_REPEATS, DELAY_POSITION},
    {"Delay tape wow", 0.0, 99.0, TAPE_WOW, DELAY_POSITION},
    {"Delay tape flutter", 0.0, 99.0, TAPE_FLUTTER, DELAY_POSITION},
};

static EffectSettings reverb_twin_settings[] = {
    {"Twin reverb", 0.0, 99.0, TWIN_REVERB, REVERB_POSITION},
};

static EffectSettings reverb_lex_ambience_settings[] = {
    {"Predelay", 0.0, 15.0, LEX_AMBIENCE_PREDELAY, REVERB_POSITION},
    {"Decay", 0.0, 99.0, LEX_AMBIENCE_DECAY, REVERB_POSITION},
    {"Liveliness", 0.0, 99.0, LEX_AMBIENCE_LIVELINESS, REVERB_POSITION},
    {"Level", 0.0, 99.0, LEX_AMBIENCE_LEVEL, REVERB_POSITION},
};

static EffectSettings reverb_lex_studio_settings[] = {
    {"Predelay", 0.0, 15.0, LEX_STUDIO_PREDELAY, REVERB_POSITION},
    {"Decay", 0.0, 99.0, LEX_STUDIO_DECAY, REVERB_POSITION},
    {"Liveliness", 0.0, 99.0, LEX_STUDIO_LIVELINESS, REVERB_POSITION},
    {"Level", 0.0, 99.0, LEX_STUDIO_LEVEL, REVERB_POSITION},
};

static EffectSettings reverb_lex_room_settings[] = {
    {"Predelay", 0.0, 15.0, LEX_ROOM_PREDELAY, REVERB_POSITION},
    {"Decay", 0.0, 99.0, LEX_ROOM_DECAY, REVERB_POSITION},
    {"Liveliness", 0.0, 99.0, LEX_ROOM_LIVELINESS, REVERB_POSITION},
    {"Level", 0.0, 99.0, LEX_ROOM_LEVEL, REVERB_POSITION},
};

static EffectSettings reverb_lex_hall_settings[] = {
    {"Predelay", 0.0, 15.0, LEX_HALL_PREDELAY, REVERB_POSITION},
    {"Decay", 0.0, 99.0, LEX_HALL_DECAY, REVERB_POSITION},
    {"Liveliness", 0.0, 99.0, LEX_HALL_LIVELINESS, REVERB_POSITION},
    {"Level", 0.0, 99.0, LEX_HALL_LEVEL, REVERB_POSITION},
};

static EffectSettings reverb_emt240_plate_settings[] = {
    {"Predelay", 0.0, 15.0, EMT240_PLATE_PREDELAY, REVERB_POSITION},
    {"Decay", 0.0, 99.0, EMT240_PLATE_DECAY, REVERB_POSITION},
    {"Liveliness", 0.0, 99.0, EMT240_PLATE_LIVELINESS, REVERB_POSITION},
    {"Level", 0.0, 99.0, EMT240_PLATE_LEVEL, REVERB_POSITION},
};

static EffectGroup wah_group[] = {
    {WAH_TYPE_CRY, "Cry wah", WAH_TYPE, WAH_POSITION, wah_settings, G_N_ELEMENTS(wah_settings)},
    {WAH_TYPE_FULLRANGE, "Fullrange wah", WAH_TYPE, WAH_POSITION, wah_settings, G_N_ELEMENTS(wah_settings)},
    {WAH_TYPE_CLYDE, "Clyde wah", WAH_TYPE, WAH_POSITION, wah_settings, G_N_ELEMENTS(wah_settings)},
};

static EffectGroup comp_group[] = {
    {COMP_TYPE_DIGI, "Digital compressor", COMP_TYPE, COMP_POSITION, comp_digi_settings, G_N_ELEMENTS(comp_digi_settings)},
    {COMP_TYPE_CS, "CS compressor", COMP_TYPE, COMP_POSITION, comp_cs_settings, G_N_ELEMENTS(comp_cs_settings)},
};

static EffectGroup dist_group[] = {
    {DIST_TYPE_SCREAMER, "Screamer", DIST_TYPE, DIST_POSITION, dist_screamer_settings, G_N_ELEMENTS(dist_screamer_settings)},
    {DIST_TYPE_808, "808", DIST_TYPE, DIST_POSITION, dist_808_settings, G_N_ELEMENTS(dist_808_settings)},
    {DIST_TYPE_GUYOD, "GuyOD", DIST_TYPE, DIST_POSITION, dist_guyod_settings, G_N_ELEMENTS(dist_guyod_settings)},
    {DIST_TYPE_DOD250, "DOD250", DIST_TYPE, DIST_POSITION, dist_dod250_settings, G_N_ELEMENTS(dist_dod250_settings)},
    {DIST_TYPE_RODENT, "Rodent", DIST_TYPE, DIST_POSITION, dist_rodent_settings, G_N_ELEMENTS(dist_rodent_settings)},
    {DIST_TYPE_MX, "MX", DIST_TYPE, DIST_POSITION, dist_mx_settings, G_N_ELEMENTS(dist_mx_settings)},
    {DIST_TYPE_DS, "DS", DIST_TYPE, DIST_POSITION, dist_ds_settings, G_N_ELEMENTS(dist_ds_settings)},
    {DIST_TYPE_GRUNGE, "Grunge", DIST_TYPE, DIST_POSITION, dist_grunge_settings, G_N_ELEMENTS(dist_grunge_settings)},
    {DIST_TYPE_ZONE, "Zone", DIST_TYPE, DIST_POSITION, dist_zone_settings, G_N_ELEMENTS(dist_zone_settings)},
    {DIST_TYPE_DEATH, "Death", DIST_TYPE, DIST_POSITION, dist_death_settings, G_N_ELEMENTS(dist_death_settings)},
    {DIST_TYPE_GONK, "Gonk", DIST_TYPE, DIST_POSITION, dist_gonk_settings, G_N_ELEMENTS(dist_gonk_settings)},
    {DIST_TYPE_FUZZY, "Fuzzy", DIST_TYPE, DIST_POSITION, dist_fuzzy_settings, G_N_ELEMENTS(dist_fuzzy_settings)},
    {DIST_TYPE_MP, "MP", DIST_TYPE, DIST_POSITION, dist_mp_settings, G_N_ELEMENTS(dist_mp_settings)},
};

static EffectGroup noisegate_group[] = {
    {NOISEGATE_GATE, "Gate", NOISEGATE_TYPE, NOISEGATE_POSITION, noisegate_gate_settings, G_N_ELEMENTS(noisegate_gate_settings)},
    {NOISEGATE_SWELL, "Swell", NOISEGATE_TYPE, NOISEGATE_POSITION, noisegate_swell_settings, G_N_ELEMENTS(noisegate_swell_settings)},
};

static EffectGroup chorusfx_group[] = {
    {CHORUS_TYPE_CE, "CE Chorus", CHORUSFX_TYPE, CHORUSFX_POSITION, chorusfx_ce_settings, G_N_ELEMENTS(chorusfx_ce_settings)},
    {CHORUS_TYPE_DUAL, "Dual Chorus", CHORUSFX_TYPE, CHORUSFX_POSITION, chorusfx_dual_settings, G_N_ELEMENTS(chorusfx_dual_settings)},
    {CHORUS_TYPE_MULTI, "Multi Chorus", CHORUSFX_TYPE, CHORUSFX_POSITION, chorusfx_multi_settings, G_N_ELEMENTS(chorusfx_multi_settings)},
    {CHORUS_TYPE_FLANGER, "Flanger", CHORUSFX_TYPE, CHORUSFX_POSITION, chorusfx_flanger_settings, G_N_ELEMENTS(chorusfx_flanger_settings)},
    {CHORUS_TYPE_MXR_FLANGER, "MXR FLANGER", CHORUSFX_TYPE, CHORUSFX_POSITION, chorusfx_mxr_flanger_settings, G_N_ELEMENTS(chorusfx_mxr_flanger_settings)},
    {CHORUS_TYPE_PHASER, "Phaser", CHORUSFX_TYPE, CHORUSFX_POSITION, chorusfx_phaser_settings, G_N_ELEMENTS(chorusfx_phaser_settings)},
    {CHORUS_TYPE_VIBRATO, "Vibrato", CHORUSFX_TYPE, CHORUSFX_POSITION, chorusfx_vibrato_settings, G_N_ELEMENTS(chorusfx_vibrato_settings)},
    {CHORUS_TYPE_ROTARY, "Rotary", CHORUSFX_TYPE, CHORUSFX_POSITION, chorusfx_rotary_settings, G_N_ELEMENTS(chorusfx_rotary_settings)},
    {CHORUS_TYPE_VIBROPAN, "Vibropan", CHORUSFX_TYPE, CHORUSFX_POSITION, chorusfx_vibropan_settings, G_N_ELEMENTS(chorusfx_vibropan_settings)},
    {CHORUS_TYPE_TREMOLO, "Tremolo", CHORUSFX_TYPE, CHORUSFX_POSITION, chorusfx_tremolo_settings, G_N_ELEMENTS(chorusfx_tremolo_settings)},
    {CHORUS_TYPE_PANNER, "Panner", CHORUSFX_TYPE, CHORUSFX_POSITION, chorusfx_panner_settings, G_N_ELEMENTS(chorusfx_panner_settings)},
    {CHORUS_TYPE_ENVELOPE, "Envelope", CHORUSFX_TYPE, CHORUSFX_POSITION, chorusfx_envelope_settings, G_N_ELEMENTS(chorusfx_envelope_settings)},
    {CHORUS_TYPE_AUTOYA, "AutoYa", CHORUSFX_TYPE, CHORUSFX_POSITION, chorusfx_autoya_settings, G_N_ELEMENTS(chorusfx_autoya_settings)},
    {CHORUS_TYPE_YAYA, "YaYa", CHORUSFX_TYPE, CHORUSFX_POSITION, chorusfx_yaya_settings, G_N_ELEMENTS(chorusfx_yaya_settings)},
    {CHORUS_TYPE_STEP_FILTER, "Step Filter", CHORUSFX_TYPE, CHORUSFX_POSITION, chorusfx_step_filter_settings, G_N_ELEMENTS(chorusfx_step_filter_settings)},
    {CHORUS_TYPE_WHAMMY, "Whammy", CHORUSFX_TYPE, CHORUSFX_POSITION, chorusfx_whammy_settings, G_N_ELEMENTS(chorusfx_whammy_settings)},
    {CHORUS_TYPE_PITCH_SHIFT, "Pitch Shift", CHORUSFX_TYPE, CHORUSFX_POSITION, chorusfx_pitch_shift_settings, G_N_ELEMENTS(chorusfx_pitch_shift_settings)},
    {CHORUS_TYPE_DETUNE, "Detune", CHORUSFX_TYPE, CHORUSFX_POSITION, chorusfx_detune_settings, G_N_ELEMENTS(chorusfx_detune_settings)},
    {CHORUS_TYPE_IPS, "IPS", CHORUSFX_TYPE, CHORUSFX_POSITION, chorusfx_ips_settings, G_N_ELEMENTS(chorusfx_ips_settings)},
};

static EffectGroup delay_group[] = {
    {DELAY_TYPE_ANALOG, "Analog", DELAY_TYPE, DELAY_POSITION, delay_analog_settings, G_N_ELEMENTS(delay_analog_settings)},
    {DELAY_TYPE_DIGITAL, "Digital", DELAY_TYPE, DELAY_POSITION, delay_digital_settings, G_N_ELEMENTS(delay_digital_settings)},
    {DELAY_TYPE_MODULATED, "Modulated", DELAY_TYPE, DELAY_POSITION, delay_modulated_settings, G_N_ELEMENTS(delay_modulated_settings)},
    {DELAY_TYPE_PONG, "Pong", DELAY_TYPE, DELAY_POSITION, delay_pong_settings, G_N_ELEMENTS(delay_pong_settings)},
    {DELAY_TYPE_TAPE, "Tape", DELAY_TYPE, DELAY_POSITION, delay_tape_settings, G_N_ELEMENTS(delay_tape_settings)},
};

static EffectGroup reverb_group[] = {
    {REVERB_TYPE_TWIN, "Twin", REVERB_TYPE, REVERB_POSITION, reverb_twin_settings, G_N_ELEMENTS(reverb_twin_settings)},
    {REVERB_TYPE_LEX_AMBIENCE, "Lexicon ambience", REVERB_TYPE, REVERB_POSITION, reverb_lex_ambience_settings, G_N_ELEMENTS(reverb_lex_ambience_settings)},
    {REVERB_TYPE_LEX_STUDIO, "Lexicon studio", REVERB_TYPE, REVERB_POSITION, reverb_lex_studio_settings, G_N_ELEMENTS(reverb_lex_studio_settings)},
    {REVERB_TYPE_LEX_ROOM, "Lexicon room", REVERB_TYPE, REVERB_POSITION, reverb_lex_room_settings, G_N_ELEMENTS(reverb_lex_room_settings)},
    {REVERB_TYPE_LEX_HALL, "Lexicon hall", REVERB_TYPE, REVERB_POSITION, reverb_lex_hall_settings, G_N_ELEMENTS(reverb_lex_hall_settings)},
    {REVERB_TYPE_EMT240_PLATE, "EMT240 Plate", REVERB_TYPE, REVERB_POSITION, reverb_emt240_plate_settings, G_N_ELEMENTS(reverb_emt240_plate_settings)},
};

static EffectGroup amp_group[] = {
    {AMP_TYPE_TWEED_CHAMP, "Tweed Champ", AMP_TYPE, AMP_POSITION, amp_settings, G_N_ELEMENTS(amp_settings)},
    {AMP_TYPE_TWEED_DELUXE, "Tweed Deluxe", AMP_TYPE, AMP_POSITION, amp_settings, G_N_ELEMENTS(amp_settings)},
    {AMP_TYPE_TWEED_BASSMAN, "Tweed Bassman", AMP_TYPE, AMP_POSITION, amp_settings, G_N_ELEMENTS(amp_settings)},
    {AMP_TYPE_BLACKFACE_TWIN, "Blackface Twin", AMP_TYPE, AMP_POSITION, amp_settings, G_N_ELEMENTS(amp_settings)},
    {AMP_TYPE_BLACKFACE_DELUXE, "Blackface Deluxe", AMP_TYPE, AMP_POSITION, amp_settings, G_N_ELEMENTS(amp_settings)},
    {AMP_TYPE_SUPER_LEAD_PLEXI, "Super Lead Plexi", AMP_TYPE, AMP_POSITION, amp_settings, G_N_ELEMENTS(amp_settings)},
    {AMP_TYPE_MASTER_VOLUME, "Master Volume", AMP_TYPE, AMP_POSITION, amp_settings, G_N_ELEMENTS(amp_settings)},
    {AMP_TYPE_JCM800, "JCM800", AMP_TYPE, AMP_POSITION, amp_settings, G_N_ELEMENTS(amp_settings)},
    {AMP_TYPE_JCM900, "JCM900", AMP_TYPE, AMP_POSITION, amp_settings, G_N_ELEMENTS(amp_settings)},
    {AMP_TYPE_AC15, "AC15", AMP_TYPE, AMP_POSITION, amp_settings, G_N_ELEMENTS(amp_settings)},
    {AMP_TYPE_AC30TB, "AC30TB", AMP_TYPE, AMP_POSITION, amp_settings, G_N_ELEMENTS(amp_settings)},
    {AMP_TYPE_HIWATT_100, "Hiwatt 100", AMP_TYPE, AMP_POSITION, amp_settings, G_N_ELEMENTS(amp_settings)},
    {AMP_TYPE_BOOGIE_MARK_II, "Boogie Mark II", AMP_TYPE, AMP_POSITION, amp_settings, G_N_ELEMENTS(amp_settings)},
    {AMP_TYPE_DUAL_RECTIFIER, "Dual Rectifier", AMP_TYPE, AMP_POSITION, amp_settings, G_N_ELEMENTS(amp_settings)},
    {AMP_TYPE_MATCHLESS_HC30, "Matchless HC30", AMP_TYPE, AMP_POSITION, amp_settings, G_N_ELEMENTS(amp_settings)},
    {AMP_TYPE_SOLO, "Solo", AMP_TYPE, AMP_POSITION, amp_settings, G_N_ELEMENTS(amp_settings)},
    {AMP_TYPE_METAL, "Metal", AMP_TYPE, AMP_POSITION, amp_settings, G_N_ELEMENTS(amp_settings)},
    {AMP_TYPE_BRIGHT, "Bright", AMP_TYPE, AMP_POSITION, amp_settings, G_N_ELEMENTS(amp_settings)},
    {AMP_TYPE_CLEAN, "Clean", AMP_TYPE, AMP_POSITION, amp_settings, G_N_ELEMENTS(amp_settings)},
    {AMP_TYPE_HIGH_GAIN, "High Gain", AMP_TYPE, AMP_POSITION, amp_settings, G_N_ELEMENTS(amp_settings)},
    {AMP_TYPE_ACOUSTIC, "Acoustic", AMP_TYPE, AMP_POSITION, amp_settings2, G_N_ELEMENTS(amp_settings2)},
    {AMP_TYPE_DIRECT, "Direct", AMP_TYPE, AMP_POSITION, amp_settings2, G_N_ELEMENTS(amp_settings2)},
};

static EffectGroup eq_group[] = {
    {EQ_TYPE_BRIGHT, "Bright", EQ_TYPE, EQ_POSITION, eq_settings, G_N_ELEMENTS(eq_settings)},
    {EQ_TYPE_MIDBOOST, "Mid Boost", EQ_TYPE, EQ_POSITION, eq_settings, G_N_ELEMENTS(eq_settings)},
    {EQ_TYPE_SCOOP, "Scoop", EQ_TYPE, EQ_POSITION, eq_settings, G_N_ELEMENTS(eq_settings)},
    {EQ_TYPE_WARM, "Warm", EQ_TYPE, EQ_POSITION, eq_settings, G_N_ELEMENTS(eq_settings)},
};

static Effect wah_effect[] = {
    {"Wah", WAH_ON_OFF, WAH_POSITION, wah_group, G_N_ELEMENTS(wah_group)},
};

static Effect comp_effect[] = {
    {"Compressor", COMP_ON_OFF, COMP_POSITION, comp_group, G_N_ELEMENTS(comp_group)},
};

static Effect dist_effect[] = {
    {"Distortion", DIST_ON_OFF, DIST_POSITION, dist_group, G_N_ELEMENTS(dist_group)},
};

static Effect noisegate_effect[] = {
    {"Noisegate", NOISEGATE_ON_OFF, NOISEGATE_POSITION, noisegate_group, G_N_ELEMENTS(noisegate_group)},
};

static Effect chorusfx_effect[] = {
    {"Chorus/FX", CHORUSFX_ON_OFF, CHORUSFX_POSITION, chorusfx_group, G_N_ELEMENTS(chorusfx_group)},
};

static Effect delay_effect[] = {
    {"Delay", DELAY_ON_OFF, DELAY_POSITION, delay_group, G_N_ELEMENTS(delay_group)},
};

static Effect reverb_effect[] = {
    {"Reverb", REVERB_ON_OFF, REVERB_POSITION, reverb_group, G_N_ELEMENTS(reverb_group)},
};

static Effect amp_effect[] = {
    {"Amp", AMP_ON_OFF, AMP_POSITION, amp_group, G_N_ELEMENTS(amp_group)},
};

static Effect eq_effect[] = {
    {"EQ", EQ_ON_OFF, EQ_POSITION, eq_group, G_N_ELEMENTS(eq_group)},
};

EffectList effects[] = {
    {wah_effect, G_N_ELEMENTS(wah_effect)},
    {amp_effect, G_N_ELEMENTS(amp_effect)},
    {eq_effect, G_N_ELEMENTS(eq_effect)},
    {comp_effect, G_N_ELEMENTS(comp_effect)},
    {dist_effect, G_N_ELEMENTS(dist_effect)},
    {noisegate_effect, G_N_ELEMENTS(noisegate_effect)},
    {chorusfx_effect, G_N_ELEMENTS(chorusfx_effect)},
    {delay_effect, G_N_ELEMENTS(delay_effect)},
    {reverb_effect, G_N_ELEMENTS(reverb_effect)},
};

int n_effects = G_N_ELEMENTS(effects);
