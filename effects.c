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

static gchar *on_off_labels[] = {
    "Off",
    "On",
};

static gchar *pickup_type_labels[] = {
    "SC>HB",
    "HB>SC",
};

static EffectValues values_0_to_99 = {
    0.0, 99.0, NULL,
};

static EffectValues values_0_to_49 = {
    0.0, 49.0, NULL,
};

static EffectValues values_0_to_12 = {
    0.0, 12.0, NULL,
};

static EffectValues values_0_to_15 = {
    0.0, 15.0, NULL,
};

static EffectValues values_m24_to_24 = {
    /* TODO: make this display propertly (display range -24 to 24) */
    0.0, 48.0, NULL,
};

static EffectValues values_eq_db = {
    /* TODO: make those display propertly (display range -12 to 12) */
    0.0, 24.0, NULL,
};

static EffectValues values_eq_mid_hz = {
    /* TODO: make this display propertly (display range 300 to 5000) */
    0.0, 4700.0, NULL,
};

static EffectValues values_eq_treb_hz = {
    /* TODO: make this display propertly (display range 500 to 8000) */
    0.0, 7500.0, NULL,
};

static EffectValues values_waveform = {
    0.0, 2.0, wave_labels,
};

static EffectValues values_whammy_amount = {
    0.0, 15.0, whammy_labels,
};

static EffectValues values_ips_shift = {
    0.0, 13.0, ips_shift_labels,
};

static EffectValues values_ips_key = {
    0.0, 11.0, ips_key_labels,
};

static EffectValues values_ips_scale = {
    0.0, 5.0, ips_scale_labels,
};

static EffectValues values_delay_time = {
    /* TODO: make this display propertly (10 msec to 5 sec) */
    0.0, 139.0, NULL,
};

static EffectValues values_delay_repeats = {
    /* TODO: make last value display propertly */
    0.0, 100.0, NULL,
};

static EffectValues values_on_off = {
    0.0, 1.0, on_off_labels,
};

static EffectValues values_pickup_type = {
    0.0, 1.0, pickup_type_labels,
};

static EffectSettings wah_settings[] = {
    {"Wah min", WAH_MIN, WAH_POSITION_MIN_MAX, &values_0_to_99},
    {"Wah max", WAH_MAX, WAH_POSITION_MIN_MAX, &values_0_to_99},
    {"Wah level", WAH_LEVEL, WAH_POSITION, &values_0_to_12},
};

static EffectSettings comp_digi_settings[] = {
    {"Compressor sustain", COMP_SUSTAIN, COMP_POSITION, &values_0_to_99},
    {"Compressor tone", COMP_TONE, COMP_POSITION, &values_0_to_99},
    {"Compressor attack", COMP_ATTACK, COMP_POSITION, &values_0_to_99},
    {"Compressor level", COMP_LEVEL, COMP_POSITION, &values_0_to_99},
};

static EffectSettings comp_cs_settings[] = {
    {"Compressor sustain", COMP_SUSTAIN, COMP_POSITION, &values_0_to_99},
    {"Compressor attack", COMP_ATTACK, COMP_POSITION, &values_0_to_99},
    {"Compressor level", COMP_LEVEL, COMP_POSITION, &values_0_to_99},
};

static EffectSettings dist_screamer_settings[] = {
    {"Screamer drive", DIST_SCREAMER_DRIVE, DIST_POSITION, &values_0_to_99},
    {"Screamer tone", DIST_SCREAMER_TONE, DIST_POSITION, &values_0_to_99},
    {"Screamer level", DIST_SCREAMER_LVL, DIST_POSITION, &values_0_to_99},
};

static EffectSettings dist_808_settings[] = {
    {"808 overdrive", DIST_808_OVERDRIVE, DIST_POSITION, &values_0_to_99},
    {"808 tone", DIST_808_TONE, DIST_POSITION, &values_0_to_99},
    {"808 level", DIST_808_LVL, DIST_POSITION, &values_0_to_99},
};

static EffectSettings dist_guyod_settings[] = {
    {"GuyOD drive", DIST_GUYOD_DRIVE, DIST_POSITION, &values_0_to_99},
    {"GuyOD level", DIST_GUYOD_LVL, DIST_POSITION, &values_0_to_99},
};

static EffectSettings dist_dod250_settings[] = {
    {"DOD250 gain", DIST_DOD250_GAIN, DIST_POSITION, &values_0_to_99},
    {"DOD250 level", DIST_DOD250_LVL, DIST_POSITION, &values_0_to_99},
};

static EffectSettings dist_rodent_settings[] = {
    {"Rodent dist", DIST_RODENT_DIST, DIST_POSITION, &values_0_to_99},
    {"Rodent filter", DIST_RODENT_FILTER, DIST_POSITION, &values_0_to_99},
    {"Rodent level", DIST_RODENT_LVL, DIST_POSITION, &values_0_to_99},
};

static EffectSettings dist_mx_settings[] = {
    {"MX dist", DIST_MX_DIST, DIST_POSITION, &values_0_to_99},
    {"MX output", DIST_MX_OUTPUT, DIST_POSITION, &values_0_to_99},
};

static EffectSettings dist_ds_settings[] = {
    {"DS gain", DIST_DS_GAIN, DIST_POSITION, &values_0_to_99},
    {"DS tone", DIST_DS_TONE, DIST_POSITION, &values_0_to_99},
    {"DS level", DIST_DS_LVL, DIST_POSITION, &values_0_to_99},
};

static EffectSettings dist_grunge_settings[] = {
    {"Grunge", DIST_GRUNGE_GRUNGE, DIST_POSITION, &values_0_to_99},
    {"Grunge face", DIST_GRUNGE_FACE, DIST_POSITION, &values_0_to_99},
    {"Grunge loud", DIST_GRUNGE_LOUD, DIST_POSITION, &values_0_to_99},
    {"Grunge butt", DIST_GRUNGE_BUTT, DIST_POSITION, &values_0_to_99},
};

static EffectSettings dist_zone_settings[] = {
    {"Zone gain", DIST_ZONE_GAIN, DIST_POSITION, &values_0_to_99},
    {"Zone low", DIST_ZONE_LOW, DIST_POSITION, &values_0_to_99},
    {"Zone mid level", DIST_ZONE_MID_LVL, DIST_POSITION, &values_0_to_99},
    {"Zone mid freq", DIST_ZONE_MID_FREQ, DIST_POSITION, &values_0_to_99},
    {"Zone high", DIST_ZONE_HIGH, DIST_POSITION, &values_0_to_99},
    {"Zone level", DIST_ZONE_LEVEL, DIST_POSITION, &values_0_to_99},
};

static EffectSettings dist_death_settings[] = {
    {"Death low", DIST_DEATH_LOW, DIST_POSITION, &values_0_to_99},
    {"Death mid", DIST_DEATH_MID, DIST_POSITION, &values_0_to_99},
    {"Death high", DIST_DEATH_HIGH, DIST_POSITION, &values_0_to_99},
    {"Death level", DIST_DEATH_LVL, DIST_POSITION, &values_0_to_99},
};

static EffectSettings dist_gonk_settings[] = {
    {"Gonk gonk", DIST_GONK_GONK, DIST_POSITION, &values_0_to_99},
    {"Gonk smear", DIST_GONK_SMEAR, DIST_POSITION, &values_0_to_99},
    {"Gonk suck", DIST_GONK_SUCK, DIST_POSITION, &values_0_to_99},
    {"Gonk heave", DIST_GONK_HEAVE, DIST_POSITION, &values_0_to_99},
};

static EffectSettings dist_fuzzy_settings[] = {
    {"Fuzzy fuzz", DIST_FUZZY_FUZZ, DIST_POSITION, &values_0_to_99},
    {"Fuzzy volume", DIST_FUZZY_VOLUME, DIST_POSITION, &values_0_to_99},
};

static EffectSettings dist_mp_settings[] = {
    {"MP sustain", DIST_MP_SUSTAIN, DIST_POSITION, &values_0_to_99},
    {"MP tone", DIST_MP_TONE, DIST_POSITION, &values_0_to_99},
    {"MP volume", DIST_MP_VOLUME, DIST_POSITION, &values_0_to_99},
};

static EffectSettings amp_settings[] = {
    {"AMP gain", AMP_GAIN, AMP_POSITION, &values_0_to_99},
    {"AMP level", AMP_LEVEL, AMP_POSITION, &values_0_to_99},
};

static EffectSettings amp_settings2[] = {
    {"AMP level", AMP_LEVEL, AMP_POSITION, &values_0_to_99},
};

static EffectSettings eq_settings[] = {
    {"EQ bass", EQ_BASS, EQ_POSITION, &values_eq_db},
    {"EQ mid", EQ_MID, EQ_POSITION, &values_eq_db},
    {"EQ treble", EQ_TREBLE, EQ_POSITION, &values_eq_db},
    {"EQ mid Hz", EQ_MID_HZ, EQ_POSITION, &values_eq_mid_hz},
    {"EQ treb Hz", EQ_TREBLE_HZ, EQ_POSITION, &values_eq_treb_hz},
};

static EffectSettings noisegate_gate_settings[] = {
    {"Gate treshold", NOISEGATE_GATE_TRESHOLD, NOISEGATE_POSITION, &values_0_to_99},
    {"Gate attack", NOISEGATE_ATTACK, NOISEGATE_POSITION, &values_0_to_99},
    {"Gate release", NOISEGATE_RELEASE, NOISEGATE_POSITION, &values_0_to_99},
    {"Gate attn", NOISEGATE_ATTN, NOISEGATE_POSITION, &values_0_to_99},
};

static EffectSettings noisegate_swell_settings[] = {
    {"Swell sens", NOISEGATE_SWELL_SENS, NOISEGATE_POSITION, &values_0_to_99},
    {"Swell attack", NOISEGATE_ATTACK, NOISEGATE_POSITION, &values_0_to_99},
    {"Swell release", NOISEGATE_RELEASE, NOISEGATE_POSITION, &values_0_to_99},
    {"Swell attn", NOISEGATE_ATTN, NOISEGATE_POSITION, &values_0_to_99},
};

static EffectSettings chorusfx_ce_settings[] = {
    {"CE chorus speed", CE_CHORUS_SPEED, CHORUSFX_POSITION, &values_0_to_99},
    {"CE chorus depth", CE_CHORUS_DEPTH, CHORUSFX_POSITION, &values_0_to_99},
};

static EffectSettings chorusfx_dual_settings[] = {
    {"Dual chorus speed", DUAL_CHORUS_SPEED, CHORUSFX_POSITION, &values_0_to_99},
    {"Dual chorus depth", DUAL_CHORUS_DEPTH, CHORUSFX_POSITION, &values_0_to_99},
    {"Dual chorus level", DUAL_CHORUS_LEVEL, CHORUSFX_POSITION, &values_0_to_99},
    {"Dual chorus wave", DUAL_CHORUS_WAVE, CHORUSFX_POSITION, &values_waveform},
};

static EffectSettings chorusfx_multi_settings[] = {
    {"Multi chorus speed", MULTI_CHORUS_SPEED, CHORUSFX_POSITION, &values_0_to_99},
    {"Multi chorus depth", MULTI_CHORUS_DEPTH, CHORUSFX_POSITION, &values_0_to_99},
    {"Multi chorus level", MULTI_CHORUS_LEVEL, CHORUSFX_POSITION, &values_0_to_99},
    {"Multi chorus wave", MULTI_CHORUS_WAVE, CHORUSFX_POSITION, &values_waveform},
};

static EffectSettings chorusfx_flanger_settings[] = {
    {"Flanger speed", FLANGER_SPEED, CHORUSFX_POSITION, &values_0_to_99},
    {"Flanger depth", FLANGER_DEPTH, CHORUSFX_POSITION, &values_0_to_99},
    {"Flanger regen", FLANGER_REGEN, CHORUSFX_POSITION, &values_0_to_99},
    {"Flanger level", FLANGER_LEVEL, CHORUSFX_POSITION, &values_0_to_99},
    {"Flanger wave", FLANGER_WAVE, CHORUSFX_POSITION, &values_waveform},
};

static EffectSettings chorusfx_mxr_flanger_settings[] = {
    {"MXR flanger speed", MXR_FLANGER_SPEED, CHORUSFX_POSITION, &values_0_to_99},
    {"MXR flanger width", MXR_FLANGER_WIDTH, CHORUSFX_POSITION, &values_0_to_99},
    {"MXR flanger regen", MXR_FLANGER_REGEN, CHORUSFX_POSITION, &values_0_to_99},
    {"MXR flanger manual", MXR_FLANGER_MANUAL, CHORUSFX_POSITION, &values_0_to_99},
};

static EffectSettings chorusfx_phaser_settings[] = {
    {"Phaser speed", PHASER_SPEED, CHORUSFX_POSITION, &values_0_to_99},
    {"Phaser depth", PHASER_DEPTH, CHORUSFX_POSITION, &values_0_to_99},
    {"Phaser regen", PHASER_REGEN, CHORUSFX_POSITION, &values_0_to_99},
    {"Phaser level", PHASER_LEVEL, CHORUSFX_POSITION, &values_0_to_99},
    {"Phaser wave", PHASER_WAVE, CHORUSFX_POSITION, &values_waveform},
};

static EffectSettings chorusfx_vibrato_settings[] = {
    {"Vibrato speed", VIBRATO_SPEED, CHORUSFX_POSITION, &values_0_to_99},
    {"Vibrato depth", VIBRATO_DEPTH, CHORUSFX_POSITION, &values_0_to_99},
};

static EffectSettings chorusfx_rotary_settings[] = {
    {"Rotary speed", ROTARY_SPEED, CHORUSFX_POSITION, &values_0_to_99},
    {"Rotary intensity", ROTARY_INTENSITY, CHORUSFX_POSITION, &values_0_to_99},
    {"Rotary doppler", ROTARY_DOPPLER, CHORUSFX_POSITION, &values_0_to_99},
    {"Rotary crossover", ROTARY_CROSSOVER, CHORUSFX_POSITION, &values_0_to_99},
};

static EffectSettings chorusfx_vibropan_settings[] = {
    {"Vibropan speed", VIBROPAN_SPEED, CHORUSFX_POSITION, &values_0_to_99},
    {"Vibropan depth", VIBROPAN_DEPTH, CHORUSFX_POSITION, &values_0_to_99},
    {"Vibropan vibra", VIBROPAN_VIBRA, CHORUSFX_POSITION, &values_0_to_99},
    {"Vibropan wave", VIBROPAN_WAVE, CHORUSFX_POSITION, &values_waveform},
};

static EffectSettings chorusfx_tremolo_settings[] = {
    {"Tremolo speed", TREMOLO_SPEED, CHORUSFX_POSITION, &values_0_to_99},
    {"Tremolo depth", TREMOLO_DEPTH, CHORUSFX_POSITION, &values_0_to_99},
    {"Tremolo wave", TREMOLO_WAVE, CHORUSFX_POSITION, &values_waveform},
};

static EffectSettings chorusfx_panner_settings[] = {
    {"Panner speed", PANNER_SPEED, CHORUSFX_POSITION, &values_0_to_99},
    {"Panner depth", PANNER_DEPTH, CHORUSFX_POSITION, &values_0_to_99},
    {"Panner wave", PANNER_WAVE, CHORUSFX_POSITION, &values_waveform},
};

static EffectSettings chorusfx_envelope_settings[] = {
    {"Envelope sensitivity", ENVELOPE_SENSITIVITY, CHORUSFX_POSITION, &values_0_to_99},
    {"Envelope range", ENVELOPE_RANGE, CHORUSFX_POSITION, &values_0_to_99},
};

static EffectSettings chorusfx_autoya_settings[] = {
    {"AutoYa speed", AUTOYA_SPEED, CHORUSFX_POSITION, &values_0_to_99},
    {"AutoYa intensity", AUTOYA_INTENSITY, CHORUSFX_POSITION, &values_0_to_99},
    {"AutoYa range", AUTOYA_RANGE, CHORUSFX_POSITION, &values_0_to_49},
};

static EffectSettings chorusfx_yaya_settings[] = {
    {"YaYa pedal", YAYA_PEDAL, CHORUSFX_POSITION, &values_0_to_99},
    {"YaYa intensity", YAYA_INTENSITY, CHORUSFX_POSITION, &values_0_to_99},
    {"YaYa range", YAYA_RANGE, CHORUSFX_POSITION, &values_0_to_49},
};

static EffectSettings chorusfx_step_filter_settings[] = {
    {"Step filter speed", STEP_FILTER_SPEED, CHORUSFX_POSITION, &values_0_to_99},
    {"Step filter intensity", STEP_FILTER_INTENSITY, CHORUSFX_POSITION, &values_0_to_99},
};

static EffectSettings chorusfx_whammy_settings[] = {
    {"Whammy pedal", WHAMMY_PEDAL, CHORUSFX_POSITION, &values_0_to_99},
    {"Whammy mix", WHAMMY_MIX, CHORUSFX_POSITION, &values_0_to_99},
    {"Whammy amount", WHAMMY_AMOUNT, CHORUSFX_POSITION, &values_whammy_amount},
};

static EffectSettings chorusfx_pitch_shift_settings[] = {
    /* TODO: make this display propertly (display range -24 to 24) */
    {"Pitch amount", PITCH_AMOUNT, CHORUSFX_POSITION, &values_m24_to_24},
    {"Pitch mix", PITCH_MIX, CHORUSFX_POSITION, &values_0_to_99},
};

static EffectSettings chorusfx_detune_settings[] = {
    {"Detune amount", DETUNE_AMOUNT, CHORUSFX_POSITION, &values_m24_to_24},
    {"Detune level", DETUNE_LEVEL, CHORUSFX_POSITION, &values_0_to_99},
};

static EffectSettings chorusfx_ips_settings[] = {
    {"IPS shift", IPS_SHIFT_AMOUNT, CHORUSFX_POSITION, &values_ips_shift},
    {"IPS key", IPS_KEY, CHORUSFX_POSITION, &values_ips_key},
    {"IPS scale", IPS_SCALE, CHORUSFX_POSITION, &values_ips_scale},
    {"IPS level", IPS_LEVEL, CHORUSFX_POSITION, &values_0_to_99},
};

static EffectSettings delay_analog_settings[] = {
    {"Delay time", DELAY_TIME, DELAY_POSITION, &values_delay_time},
    {"Delay analog level", ANALOG_LEVEL, DELAY_POSITION, &values_0_to_99},
    {"Delay analog repeats", ANALOG_REPEATS, DELAY_POSITION, &values_delay_repeats},
};

static EffectSettings delay_digital_settings[] = {
    {"Delay time", DELAY_TIME, DELAY_POSITION, &values_delay_time},
    {"Delay digital level", DIGITAL_LEVEL, DELAY_POSITION, &values_0_to_99},
    {"Delay digital repeats", DIGITAL_REPEATS, DELAY_POSITION, &values_delay_repeats},
    {"Delay digital ducker thresh", DIGITAL_DUCKER_THRESH, DELAY_POSITION, &values_0_to_99},
    {"Delay digital ducker level", DIGITAL_DUCKER_LEVEL, DELAY_POSITION, &values_0_to_99},
};

static EffectSettings delay_modulated_settings[] = {
    {"Delay time", DELAY_TIME, DELAY_POSITION, &values_delay_time},
    {"Delay modulated level", MODULATED_LEVEL, DELAY_POSITION, &values_0_to_99},
    {"Delay modulated repeats", MODULATED_REPEATS, DELAY_POSITION, &values_delay_repeats},
    {"Delay modulated depth", MODULATED_DEPTH, DELAY_POSITION, &values_0_to_99},
};

static EffectSettings delay_pong_settings[] = {
    {"Delay time", DELAY_TIME, DELAY_POSITION, &values_delay_time},
    {"Delay pong level", PONG_LEVEL, DELAY_POSITION, &values_0_to_99},
    {"Delay pong repeats", PONG_REPEATS, DELAY_POSITION, &values_delay_repeats},
    {"Delay pong ducker thresh", PONG_DUCKER_THRESH, DELAY_POSITION, &values_0_to_99},
    {"Delay pong ducker level", PONG_DUCKER_LEVEL, DELAY_POSITION, &values_0_to_99},
};

static EffectSettings delay_tape_settings[] = {
    {"Delay time", DELAY_TIME, DELAY_POSITION, &values_delay_time},
    {"Delay tape level", TAPE_LEVEL, DELAY_POSITION, &values_0_to_99},
    {"Delay tape repeats", TAPE_REPEATS, DELAY_POSITION, &values_delay_repeats},
    {"Delay tape wow", TAPE_WOW, DELAY_POSITION, &values_0_to_99},
    {"Delay tape flutter", TAPE_FLUTTER, DELAY_POSITION, &values_0_to_99},
};

static EffectSettings reverb_twin_settings[] = {
    {"Twin reverb", TWIN_REVERB, REVERB_POSITION, &values_0_to_99},
};

static EffectSettings reverb_lex_ambience_settings[] = {
    {"Predelay", LEX_AMBIENCE_PREDELAY, REVERB_POSITION, &values_0_to_15},
    {"Decay", LEX_AMBIENCE_DECAY, REVERB_POSITION, &values_0_to_99},
    {"Liveliness", LEX_AMBIENCE_LIVELINESS, REVERB_POSITION, &values_0_to_99},
    {"Level", LEX_AMBIENCE_LEVEL, REVERB_POSITION, &values_0_to_99},
};

static EffectSettings reverb_lex_studio_settings[] = {
    {"Predelay", LEX_STUDIO_PREDELAY, REVERB_POSITION, &values_0_to_15},
    {"Decay", LEX_STUDIO_DECAY, REVERB_POSITION, &values_0_to_99},
    {"Liveliness", LEX_STUDIO_LIVELINESS, REVERB_POSITION, &values_0_to_99},
    {"Level", LEX_STUDIO_LEVEL, REVERB_POSITION, &values_0_to_99},
};

static EffectSettings reverb_lex_room_settings[] = {
    {"Predelay", LEX_ROOM_PREDELAY, REVERB_POSITION, &values_0_to_15},
    {"Decay", LEX_ROOM_DECAY, REVERB_POSITION, &values_0_to_99},
    {"Liveliness", LEX_ROOM_LIVELINESS, REVERB_POSITION, &values_0_to_99},
    {"Level", LEX_ROOM_LEVEL, REVERB_POSITION, &values_0_to_99},
};

static EffectSettings reverb_lex_hall_settings[] = {
    {"Predelay", LEX_HALL_PREDELAY, REVERB_POSITION, &values_0_to_15},
    {"Decay", LEX_HALL_DECAY, REVERB_POSITION, &values_0_to_99},
    {"Liveliness", LEX_HALL_LIVELINESS, REVERB_POSITION, &values_0_to_99},
    {"Level", LEX_HALL_LEVEL, REVERB_POSITION, &values_0_to_99},
};

static EffectSettings reverb_emt240_plate_settings[] = {
    {"Predelay", EMT240_PLATE_PREDELAY, REVERB_POSITION, &values_0_to_15},
    {"Decay", EMT240_PLATE_DECAY, REVERB_POSITION, &values_0_to_99},
    {"Liveliness", EMT240_PLATE_LIVELINESS, REVERB_POSITION, &values_0_to_99},
    {"Level", EMT240_PLATE_LEVEL, REVERB_POSITION, &values_0_to_99},
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

static EffectGroup amp_cab_group[] = {
    {AMP_CAB_DIRECT, "Direct", AMP_CAB_TYPE, AMP_CAB_POSITION, NULL, -1},
    {AMP_CAB_CHAMP, "Champ 1x8", AMP_CAB_TYPE, AMP_CAB_POSITION, NULL, -1},
    {AMP_CAB_DELUXE, "Deluxe 1x12", AMP_CAB_TYPE, AMP_CAB_POSITION, NULL, -1},
    {AMP_CAB_TWIN, "Twin 2x12", AMP_CAB_TYPE, AMP_CAB_POSITION, NULL, -1},
    {AMP_CAB_BRITISH2_12, "British 2x12", AMP_CAB_TYPE, AMP_CAB_POSITION, NULL, -1},
    {AMP_CAB_BASSMAN, "Bassman 4x10", AMP_CAB_TYPE, AMP_CAB_POSITION, NULL, -1},
    {AMP_CAB_BRITISH4_12, "British 4x12", AMP_CAB_TYPE, AMP_CAB_POSITION, NULL, -1},
    {AMP_CAB_BRITISH_GREEN, "British Green 4x12", AMP_CAB_TYPE, AMP_CAB_POSITION, NULL, -1},
    {AMP_CAB_VINTAGE, "Vintage V30 4x12", AMP_CAB_TYPE, AMP_CAB_POSITION, NULL, -1},
    {AMP_CAB_DIGI_SOLO, "DigiTech Solo 4x12", AMP_CAB_TYPE, AMP_CAB_POSITION, NULL, -1},
    {AMP_CAB_DIGI_BRIGHT, "DigiTech Bright 2x12", AMP_CAB_TYPE, AMP_CAB_POSITION, NULL, -1},
    {AMP_CAB_DIGI_METAL, "DigiTech Metal 4x12", AMP_CAB_TYPE, AMP_CAB_POSITION, NULL, -1},
    {AMP_CAB_DIGI_ROCK, "DigiTech Rock 4x12", AMP_CAB_TYPE, AMP_CAB_POSITION, NULL, -1},
    {AMP_CAB_DIGI_ALT, "DigiTech Alt 4x12", AMP_CAB_TYPE, AMP_CAB_POSITION, NULL, -1},
    {AMP_CAB_DIGI_VNTG, "DigiTech Vntg 4x12", AMP_CAB_TYPE, AMP_CAB_POSITION, NULL, -1},
};

static EffectGroup tone_lib_group[] = {
    {TONE_LIB_OVERDRIVE, "Overdrive", LIBRARY_TONE, LIBRARY_POSITION, NULL, -1},
    {TONE_LIB_ROCK1, "Rock 1", LIBRARY_TONE, LIBRARY_POSITION, NULL, -1},
    {TONE_LIB_ROCK2, "Rock 2", LIBRARY_TONE, LIBRARY_POSITION, NULL, -1},
    {TONE_LIB_BLUES1, "Blues 1", LIBRARY_TONE, LIBRARY_POSITION, NULL, -1},
    {TONE_LIB_BLUES2, "Blues 2", LIBRARY_TONE, LIBRARY_POSITION, NULL, -1},
    {TONE_LIB_METAL1, "Metal 1", LIBRARY_TONE, LIBRARY_POSITION, NULL, -1},
    {TONE_LIB_METAL2, "Metal 2", LIBRARY_TONE, LIBRARY_POSITION, NULL, -1},
    {TONE_LIB_COUNTRY1, "Country 1", LIBRARY_TONE, LIBRARY_POSITION, NULL, -1},
    {TONE_LIB_COUNTRY2, "Country 2", LIBRARY_TONE, LIBRARY_POSITION, NULL, -1},
    {TONE_LIB_WARM_DRIVE, "Warm Drive", LIBRARY_TONE, LIBRARY_POSITION, NULL, -1},
    {TONE_LIB_CRUNCH, "Crunch", LIBRARY_TONE, LIBRARY_POSITION, NULL, -1},
    {TONE_LIB_TEXAS_TONE, "Texas Tone", LIBRARY_TONE, LIBRARY_POSITION, NULL, -1},
    {TONE_LIB_ROCKABILLY, "Rockabilly", LIBRARY_TONE, LIBRARY_POSITION, NULL, -1},
    {TONE_LIB_SOLO1, "Solo 1", LIBRARY_TONE, LIBRARY_POSITION, NULL, -1},
    {TONE_LIB_SOLO2, "Solo 2", LIBRARY_TONE, LIBRARY_POSITION, NULL, -1},
    {TONE_LIB_ROCKWAH, "Rock Wah", LIBRARY_TONE, LIBRARY_POSITION, NULL, -1},
    {TONE_LIB_CHUNKY, "Chunky", LIBRARY_TONE, LIBRARY_POSITION, NULL, -1},
    {TONE_LIB_SMOOTH, "Smooth", LIBRARY_TONE, LIBRARY_POSITION, NULL, -1},
    {TONE_LIB_HEAVY, "Heavy", LIBRARY_TONE, LIBRARY_POSITION, NULL, -1},
    {TONE_LIB_CLEAN1, "Clean 1", LIBRARY_TONE, LIBRARY_POSITION, NULL, -1},
    {TONE_LIB_CLEAN2, "Clean 2", LIBRARY_TONE, LIBRARY_POSITION, NULL, -1},
    {TONE_LIB_BRITISH1, "British 1", LIBRARY_TONE, LIBRARY_POSITION, NULL, -1},
    {TONE_LIB_BRITISH2, "British 2", LIBRARY_TONE, LIBRARY_POSITION, NULL, -1},
    {TONE_LIB_AMERICAN1, "American 1", LIBRARY_TONE, LIBRARY_POSITION, NULL, -1},
    {TONE_LIB_AMERICAN2, "American 2", LIBRARY_TONE, LIBRARY_POSITION, NULL, -1},
    {TONE_LIB_TUBE_DRIVE, "Tube Drive", LIBRARY_TONE, LIBRARY_POSITION, NULL, -1},
    {TONE_LIB_DISTORTION, "Distortion", LIBRARY_TONE, LIBRARY_POSITION, NULL, -1},
    {TONE_LIB_SCOOPED, "Scooped", LIBRARY_TONE, LIBRARY_POSITION, NULL, -1},
    {TONE_LIB_PUNCHY, "Punchy", LIBRARY_TONE, LIBRARY_POSITION, NULL, -1},
    {TONE_LIB_BRIGHT_CLEAN, "Bright Clean", LIBRARY_TONE, LIBRARY_POSITION, NULL, -1},
};

static EffectGroup effects_lib_group[] = {
    {EFFECTS_LIB_CHORUS, "Chorus", LIBRARY_EFFECTS, LIBRARY_POSITION, NULL, -1},
    {EFFECTS_LIB_PHASER, "Phaser", LIBRARY_EFFECTS, LIBRARY_POSITION, NULL, -1},
    {EFFECTS_LIB_FLANGER, "Flanger", LIBRARY_EFFECTS, LIBRARY_POSITION, NULL, -1},
    {EFFECTS_LIB_PITCH, "Pitch", LIBRARY_EFFECTS, LIBRARY_POSITION, NULL, -1},
    {EFFECTS_LIB_TREMOLO, "Tremolo", LIBRARY_EFFECTS, LIBRARY_POSITION, NULL, -1},
    {EFFECTS_LIB_ROTARY, "Rotary", LIBRARY_EFFECTS, LIBRARY_POSITION, NULL, -1},
    {EFFECTS_LIB_ENVELOPE, "Envelope Filter", LIBRARY_EFFECTS, LIBRARY_POSITION, NULL, -1},
    {EFFECTS_LIB_DIGITAL, "Digital Delay", LIBRARY_EFFECTS, LIBRARY_POSITION, NULL, -1},
    {EFFECTS_LIB_ANALOG, "Analog Delay", LIBRARY_EFFECTS, LIBRARY_POSITION, NULL, -1},
    {EFFECTS_LIB_PONG, "Pong Delay", LIBRARY_EFFECTS, LIBRARY_POSITION, NULL, -1},
    {EFFECTS_LIB_MOD, "Mod Delay", LIBRARY_EFFECTS, LIBRARY_POSITION, NULL, -1},
    {EFFECTS_LIB_TAPE, "Tape Delay", LIBRARY_EFFECTS, LIBRARY_POSITION, NULL, -1},
    {EFFECTS_LIB_HALL, "Hall Reverb", LIBRARY_EFFECTS, LIBRARY_POSITION, NULL, -1},
    {EFFECTS_LIB_PLATE, "Plate Reverb", LIBRARY_EFFECTS, LIBRARY_POSITION, NULL, -1},
    {EFFECTS_LIB_SPRING, "Spring Reverb", LIBRARY_EFFECTS, LIBRARY_POSITION, NULL, -1},
    {EFFECTS_LIB_CHORUS_DIGITAL, "Chorus + Digital Delay", LIBRARY_EFFECTS, LIBRARY_POSITION, NULL, -1},
    {EFFECTS_LIB_CHORUS_DELAY_REVERB, "Chorus + Delay + Reverb", LIBRARY_EFFECTS, LIBRARY_POSITION, NULL, -1},
    {EFFECTS_LIB_FLANGER_ANALOG, "Flanger + Analog Delay", LIBRARY_EFFECTS, LIBRARY_POSITION, NULL, -1},
    {EFFECTS_LIB_PHASER_TAPE, "Phaser + Tape Delay", LIBRARY_EFFECTS, LIBRARY_POSITION, NULL, -1},
    {EFFECTS_LIB_PHASER_MOD, "Phaser + Mod Delay", LIBRARY_EFFECTS, LIBRARY_POSITION, NULL, -1},
    {EFFECTS_LIB_PHASER_ROOM, "Phaser + Room Reverb", LIBRARY_EFFECTS, LIBRARY_POSITION, NULL, -1},
    {EFFECTS_LIB_DIGITAL_HALL, "Digital Delay + Hall Reverb", LIBRARY_EFFECTS, LIBRARY_POSITION, NULL, -1},
    {EFFECTS_LIB_ANALOG_SPRING, "Analog Delay + Spring Reverb", LIBRARY_EFFECTS, LIBRARY_POSITION, NULL, -1},
    {EFFECTS_LIB_CHORUS_HALL, "Chorus + Hall Reverb", LIBRARY_EFFECTS, LIBRARY_POSITION, NULL, -1},
    {EFFECTS_LIB_PONG_HALL, "Pong Delay + Hall Reverb", LIBRARY_EFFECTS, LIBRARY_POSITION, NULL, -1},
    {EFFECTS_LIB_TAPE_SPRING, "Tape Delay + Spring Reverb", LIBRARY_EFFECTS, LIBRARY_POSITION, NULL, -1},
    {EFFECTS_LIB_TREMOLO_TAPE, "Tremolo + Tape Delay", LIBRARY_EFFECTS, LIBRARY_POSITION, NULL, -1},
    {EFFECTS_LIB_PITCH_DIGITAL, "Pitch + Digital Delay", LIBRARY_EFFECTS, LIBRARY_POSITION, NULL, -1},
    {EFFECTS_LIB_MOD_PLATE, "Mod Delay + Plate Reverb", LIBRARY_EFFECTS, LIBRARY_POSITION, NULL, -1},
    {EFFECTS_LIB_ROTARY_TAPE, "Rotary + Tape Delay", LIBRARY_EFFECTS, LIBRARY_POSITION, NULL, -1},
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

typedef struct {
    gchar *label;
    guint id;
    guint position;
    EffectValues *values;
} Modifier;

static Modifier modifiers[] = {
    {"None", 0, 0, NULL},
    {"Pickup Enable", PICKUP_ON_OFF, PICKUP_POSITION, &values_on_off},
    {"Pickup Type", PICKUP_TYPE, PICKUP_POSITION, &values_pickup_type},
    {"Compressor Enable", COMP_ON_OFF, COMP_POSITION, &values_on_off},
    {"Compressor Sustain", COMP_SUSTAIN, COMP_POSITION, &values_0_to_99},
    {"Compressor Tone", COMP_TONE, COMP_POSITION, &values_0_to_99},
    {"Compressor Level", COMP_LEVEL, COMP_POSITION, &values_0_to_99},
    {"Compressor Attack", COMP_ATTACK, COMP_POSITION, &values_0_to_99},
    {"Dist Enable", DIST_ON_OFF, DIST_POSITION, &values_on_off},
    {"Dist Drive", DIST_SCREAMER_DRIVE, DIST_POSITION, &values_0_to_99},
    {"Dist Tone", DIST_SCREAMER_TONE, DIST_POSITION, &values_0_to_99},
    {"Dist Level", DIST_SCREAMER_LVL, DIST_POSITION, &values_0_to_99},
    {"Dist Overdrive", DIST_808_OVERDRIVE, DIST_POSITION, &values_0_to_99},
    {"Dist Tone", DIST_808_TONE, DIST_POSITION, &values_0_to_99},
    {"Dist Level", DIST_808_LVL, DIST_POSITION, &values_0_to_99},
    {"Dist Drive", DIST_GUYOD_DRIVE, DIST_POSITION, &values_0_to_99},
    {"Dist Level", DIST_GUYOD_LVL, DIST_POSITION, &values_0_to_99},
    {"Dist Gain", DIST_DOD250_GAIN, DIST_POSITION, &values_0_to_99},
    {"Dist Level", DIST_DOD250_LVL, DIST_POSITION, &values_0_to_99},
    {"Dist Distortion", DIST_RODENT_DIST, DIST_POSITION, &values_0_to_99},
    {"Dist Filter", DIST_RODENT_FILTER, DIST_POSITION, &values_0_to_99},
    {"Dist Volume", DIST_RODENT_LVL, DIST_POSITION, &values_0_to_99},
    {"Dist Distortion", DIST_MX_DIST, DIST_POSITION, &values_0_to_99},
    {"Dist Output", DIST_MX_OUTPUT, DIST_POSITION, &values_0_to_99},
    {"Dist Distortion", DIST_DS_GAIN, DIST_POSITION, &values_0_to_99},
    {"Dist Tone", DIST_DS_TONE, DIST_POSITION, &values_0_to_99},
    {"Dist Level", DIST_DS_LVL, DIST_POSITION, &values_0_to_99},
    {"Dist Gain", DIST_GRUNGE_GRUNGE, DIST_POSITION, &values_0_to_99},
    {"Dist Face", DIST_GRUNGE_FACE, DIST_POSITION, &values_0_to_99},
    {"Dist Loud", DIST_GRUNGE_LOUD, DIST_POSITION, &values_0_to_99},
    {"Dist Butt", DIST_GRUNGE_BUTT, DIST_POSITION, &values_0_to_99},
    {"Dist Gain", DIST_ZONE_GAIN, DIST_POSITION, &values_0_to_99},
    {"Dist Mid Level", DIST_ZONE_MID_LVL, DIST_POSITION, &values_0_to_99},
    {"Dist Level", DIST_ZONE_LEVEL, DIST_POSITION, &values_0_to_99},
    {"Dist Low", DIST_ZONE_LOW, DIST_POSITION, &values_0_to_99},
    {"Dist Mid Freq", DIST_ZONE_MID_FREQ, DIST_POSITION, &values_0_to_99},
    {"Dist High", DIST_ZONE_HIGH, DIST_POSITION, &values_0_to_99},
    {"Dist Low", DIST_DEATH_LOW, DIST_POSITION, &values_0_to_99},
    {"Dist Mid", DIST_DEATH_MID, DIST_POSITION, &values_0_to_99},
    {"Dist Level", DIST_DEATH_LVL, DIST_POSITION, &values_0_to_99},
    {"Dist High", DIST_DEATH_HIGH, DIST_POSITION, &values_0_to_99},
    {"Dist Gunk", DIST_GONK_GONK, DIST_POSITION, &values_0_to_99},
    {"Dist Smear", DIST_GONK_SMEAR, DIST_POSITION, &values_0_to_99},
    {"Dist Suck", DIST_GONK_SUCK, DIST_POSITION, &values_0_to_99},
    {"Dist Heave", DIST_GONK_HEAVE, DIST_POSITION, &values_0_to_99},
    {"Dist Fuzz", DIST_FUZZY_FUZZ, DIST_POSITION, &values_0_to_99},
    {"Dist Volume", DIST_FUZZY_VOLUME, DIST_POSITION, &values_0_to_99},
    {"Dist Sustain", DIST_MP_SUSTAIN, DIST_POSITION, &values_0_to_99},
    {"Dist Tone", DIST_MP_TONE, DIST_POSITION, &values_0_to_99},
    {"Dist Volume", DIST_MP_VOLUME, DIST_POSITION, &values_0_to_99},
    {"Amp Channel Enable", AMP_ON_OFF, AMP_POSITION, &values_on_off},
    {"Amp Gain", AMP_GAIN, AMP_POSITION, &values_0_to_99},
    {"Amp Level", AMP_LEVEL, AMP_POSITION, &values_0_to_99},
    {"EQ Enable", EQ_ON_OFF, EQ_POSITION, &values_on_off},
    {"EQ Bass", EQ_BASS, EQ_POSITION, &values_eq_db},
    {"EQ Mid", EQ_MID, EQ_POSITION, &values_eq_db},
    {"EQ Treb", EQ_TREBLE, EQ_POSITION, &values_eq_db},
    {"Gate Enable", NOISEGATE_ON_OFF, NOISEGATE_POSITION, &values_on_off},
    {"Gate Pluck Sens", NOISEGATE_SWELL_SENS, NOISEGATE_POSITION, &values_0_to_99},
    {"Gate Threshold", NOISEGATE_GATE_TRESHOLD, NOISEGATE_POSITION, &values_0_to_99},
    {"Gate Attack", NOISEGATE_ATTACK, NOISEGATE_POSITION, &values_0_to_99},
    {"Gate Release", NOISEGATE_RELEASE, NOISEGATE_POSITION, &values_0_to_99},
    {"Gate Attenuation", NOISEGATE_ATTN, NOISEGATE_POSITION, &values_0_to_99},
    {"Chorus/FX Enable", CHORUSFX_ON_OFF, CHORUSFX_POSITION, &values_on_off},
    {"Phaser Speed", PHASER_SPEED, CHORUSFX_POSITION, &values_0_to_99},
    {"Phaser Depth", PHASER_DEPTH, CHORUSFX_POSITION, &values_0_to_99},
    {"Phaser Regen", PHASER_REGEN, CHORUSFX_POSITION, &values_0_to_99},
    {"Phaser Waveform", PHASER_WAVE, CHORUSFX_POSITION, &values_waveform},
    {"Phaser Level", PHASER_LEVEL, CHORUSFX_POSITION, &values_0_to_99},
    {"Chorus Speed", CE_CHORUS_SPEED, CHORUSFX_POSITION, &values_0_to_99},
    {"Chorus Depth", CE_CHORUS_DEPTH, CHORUSFX_POSITION, &values_0_to_99},
    {"Chorus Level", DUAL_CHORUS_LEVEL, CHORUSFX_POSITION, &values_0_to_99},
    {"Chorus Waveform", DUAL_CHORUS_WAVE, CHORUSFX_POSITION, &values_waveform},
    {"Flanger Speed", FLANGER_SPEED, CHORUSFX_POSITION, &values_0_to_99},
    {"Flanger Depth", FLANGER_DEPTH, CHORUSFX_POSITION, &values_0_to_99},
    {"Flanger Regen", FLANGER_REGEN, CHORUSFX_POSITION, &values_0_to_99},
    {"Flanger Waveform", FLANGER_WAVE, CHORUSFX_POSITION, &values_waveform},
    {"Flanger Level", FLANGER_LEVEL, CHORUSFX_POSITION, &values_0_to_99},
    {"Flanger Width", MXR_FLANGER_WIDTH, CHORUSFX_POSITION, &values_0_to_99},
    {"Flanger Manual", MXR_FLANGER_MANUAL, CHORUSFX_POSITION, &values_0_to_99},
    {"Vibrato Speed", VIBRATO_SPEED, CHORUSFX_POSITION, &values_0_to_99},
    {"Vibrato Depth", VIBRATO_DEPTH, CHORUSFX_POSITION, &values_0_to_99},
    {"Rotary Speed", ROTARY_SPEED, CHORUSFX_POSITION, &values_0_to_99},
    {"Rotary Intensity", ROTARY_INTENSITY, CHORUSFX_POSITION, &values_0_to_99},
    {"Rotary Doppler", ROTARY_DOPPLER, CHORUSFX_POSITION, &values_0_to_99},
    {"Rotary Crossover", ROTARY_CROSSOVER, CHORUSFX_POSITION, &values_0_to_99},
    {"VibroPan Speed", VIBROPAN_SPEED, CHORUSFX_POSITION, &values_0_to_99},
    {"VibroPan Depth", VIBROPAN_DEPTH, CHORUSFX_POSITION, &values_0_to_99},
    {"VibroPan VibratoPan", VIBROPAN_VIBRA, CHORUSFX_POSITION, &values_0_to_99},
    {"VibroPan Waveform", VIBROPAN_WAVE, CHORUSFX_POSITION, &values_waveform},
    {"Tremolo Speed", TREMOLO_SPEED, CHORUSFX_POSITION, &values_0_to_99},
    {"Tremolo Depth", TREMOLO_DEPTH, CHORUSFX_POSITION, &values_0_to_99},
    {"Tremolo Waveform", TREMOLO_WAVE, CHORUSFX_POSITION, &values_waveform},
    {"Panner Speed", PANNER_SPEED, CHORUSFX_POSITION, &values_0_to_99},
    {"Panner Depth", PANNER_DEPTH, CHORUSFX_POSITION, &values_0_to_99},
    {"Panner Waveform", PANNER_WAVE, CHORUSFX_POSITION, &values_waveform},
    {"Envelope Sens.", ENVELOPE_SENSITIVITY, CHORUSFX_POSITION, &values_0_to_99},
    {"Envelope Range", ENVELOPE_RANGE, CHORUSFX_POSITION, &values_0_to_99},
    {"AutoYa Speed", AUTOYA_SPEED, CHORUSFX_POSITION, &values_0_to_99},
    {"AutoYa Intensity", AUTOYA_INTENSITY, CHORUSFX_POSITION, &values_0_to_99},
    {"AutoYa Range", AUTOYA_RANGE, CHORUSFX_POSITION, &values_0_to_49},
    {"YaYa Pedal", YAYA_PEDAL, CHORUSFX_POSITION, &values_0_to_99},
    {"YaYa Intensity", YAYA_INTENSITY, CHORUSFX_POSITION, &values_0_to_99},
    {"YaYa Range", YAYA_RANGE, CHORUSFX_POSITION, &values_0_to_49},
    {"Step Filter Speed", STEP_FILTER_SPEED, CHORUSFX_POSITION, &values_0_to_99},
    {"Step Filter Intensity", STEP_FILTER_INTENSITY, CHORUSFX_POSITION, &values_0_to_99},
    {"Whammy Amount", WHAMMY_AMOUNT, CHORUSFX_POSITION, &values_whammy_amount},
    {"Whammy Pedal", WHAMMY_PEDAL, CHORUSFX_POSITION, &values_0_to_99},
    {"Whammy Mix", WHAMMY_MIX, CHORUSFX_POSITION, &values_0_to_99},
    {"Pitch Shift Amount", PITCH_AMOUNT, CHORUSFX_POSITION, &values_m24_to_24},
    {"Pitch Shift Mix", PITCH_MIX, CHORUSFX_POSITION, &values_0_to_99},
    {"Detune Amount", DETUNE_AMOUNT, CHORUSFX_POSITION, &values_m24_to_24},
    {"Detune Level", DETUNE_LEVEL, CHORUSFX_POSITION, &values_0_to_99},
    {"IPS Amount", IPS_SHIFT_AMOUNT, CHORUSFX_POSITION, &values_ips_shift},
    {"IPS Key", IPS_KEY, CHORUSFX_POSITION, &values_ips_key},
    {"IPS Scale", IPS_SCALE, CHORUSFX_POSITION, &values_ips_scale},
    {"IPS Level", IPS_LEVEL, CHORUSFX_POSITION, &values_0_to_99},
    {"Delay Enable", DELAY_ON_OFF, DELAY_POSITION, &values_on_off},
    {"Delay Time", DELAY_TIME, DELAY_POSITION, &values_delay_time},
    {"Delay Repeats", ANALOG_REPEATS, DELAY_POSITION, &values_delay_repeats},
    {"Delay Level", ANALOG_LEVEL, DELAY_POSITION, &values_0_to_99},
    {"Delay Duck Thresh", DIGITAL_DUCKER_THRESH, DELAY_POSITION, &values_0_to_99},
    {"Delay Duck Level", DIGITAL_DUCKER_LEVEL, DELAY_POSITION, &values_0_to_99},
    {"Delay Mod Depth", MODULATED_DEPTH, DELAY_POSITION, &values_0_to_99},
    {"Delay Tape Wow", TAPE_WOW, DELAY_POSITION, &values_0_to_99},
    {"Delay Tape Flut", TAPE_FLUTTER, DELAY_POSITION, &values_0_to_99},
    {"Reverb Enable", REVERB_ON_OFF, REVERB_POSITION, &values_on_off},
    {"Reverb Decay", LEX_AMBIENCE_DECAY, REVERB_POSITION, &values_0_to_99},
    {"Reverb Liveliness", LEX_STUDIO_LIVELINESS, REVERB_POSITION, &values_0_to_99},
    {"Reverb Level", LEX_STUDIO_LEVEL, REVERB_POSITION, &values_0_to_99},
    {"Reverb Predelay", LEX_STUDIO_PREDELAY, REVERB_POSITION, &values_0_to_15},
    {"Volume Pre FX", 2626, 13, &values_0_to_99},
    {"Volume Post FX", 2626, 17, &values_0_to_99},
};

int n_modifiers = G_N_ELEMENTS(modifiers);

/**
 *  get_modifier:
 *  @id: modifier ID
 *  @position: modifier position
 *
 *  Gets modifier info.
 *
 *  Return value: Modifier which must not be freed, or NULL if no matching Modifier has been found.
*/
static Modifier *get_modifier(guint id, guint position)
{
    gint x;

    for (x=0; x<n_modifiers; x++)
        if ((modifiers[x].id == id) && (modifiers[x].position == position))
            return &(modifiers[x]);

    return NULL;
}

/**
 *  get_modifier_settings:
 *  @values: possible setting values
 *
 *  Creates EffectSettings containing expression pedal min and max settings.
 *
 *  Return value: EffectSettings which must be freed using effect_settings_free.
 **/
static EffectSettings *get_modifier_settings(EffectValues *values)
{
    if (values == NULL)
        return NULL;

    /* TODO: reuse exsisting settings if values is the same */
    EffectSettings *settings = g_slice_alloc0(2 * sizeof(EffectSettings));
    settings[0].id = EXP_MIN;
    settings[1].id = EXP_MAX;

    settings[0].label = "Min";
    settings[1].label = "Max";

    settings[0].position = settings[1].position = EXP_POSITION;

    settings[0].values = settings[1].values = values;

    return settings;
}

/**
 *  effect_settings_free:
 *  @settings: settings to be freed
 *
 *  Frees all memory used by EffectSettings.
 **/
static void effect_settings_free(EffectSettings *settings)
{
    g_slice_free1(2 * sizeof(EffectSettings), settings);
}

/**
 *  modifier_linkable_list:
 *
 *  Retrieves modifier linkable gruop from device.
 *
 *  Return value: ModifierGroup which must be freed using modifier_group_free.
 **/
ModifierGroup *modifier_linkable_list()
{
    guint group_id;
    guint count;
    guint i;

    send_message(REQUEST_MODIFIER_LINKABLE_LIST, "\x00\x01", 2);

    GString *data = get_message_by_id(RECEIVE_MODIFIER_LINKABLE_LIST);

    unsigned char *str = (unsigned char*)data->str;

    group_id = (str[8] << 8) | str[9];
    count = (str[10] << 8) | str[11];

    ModifierGroup *modifier_group = g_slice_new(ModifierGroup);

    g_message("Group %d count %d", group_id, count);
    EffectGroup *group = g_slice_alloc(count * sizeof(EffectGroup));

    for (i=0; i<count; i++) {
        guint id = (str[12 + (i*3)] << 8) | str[13 + (i*3)];
        guint position = str[14 + (i*3)];

        group[i].type = (position << 16) | id;
        group[i].id = EXP_TYPE;
        group[i].position = EXP_POSITION;

        Modifier *modifier = get_modifier(id, position);
        if (modifier) {
            group[i].label = modifier->label;
            group[i].settings = get_modifier_settings(modifier->values);
            group[i].settings_amt = 2;
        } else {
            group[i].label = NULL;
            group[i].settings = NULL;
        }

        if (group[i].settings == NULL)
            group[i].settings_amt = 0;

        g_message("ID: %d Position: %d Label: %s", id, position, modifier ? modifier->label : NULL);
    }

    modifier_group->group = group;
    modifier_group->group_amt = count;

    return modifier_group;
}

/**
 *  modifier_group_free:
 *  @modifier_group: group to be freed
 *
 *  Frees all memory used by ModifierGroup.
 **/
void modifier_group_free(ModifierGroup *modifier_group)
{
    g_return_if_fail(modifier_group != NULL);

    int x;
    for (x=0; x<modifier_group->group_amt; x++) {
        if (modifier_group->group[x].settings)
            effect_settings_free(modifier_group->group[x].settings);
    }
    g_slice_free1(modifier_group->group_amt * sizeof(EffectGroup),
                  modifier_group->group);
    g_slice_free(ModifierGroup, modifier_group);
}
