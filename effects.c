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

static SettingsWidget wah_widgets[] = {
    {"Wah min", 0.0, 99.0, WAH_MIN, WAH_POSITION_MIN_MAX},
    {"Wah max", 0.0, 99.0, WAH_MAX, WAH_POSITION_MIN_MAX},
    {"Wah level", 0.0, 12.0, WAH_LEVEL, WAH_POSITION},
};

static SettingsWidget comp_digi_widgets[] = {
    {"Compressor sustain", 0.0, 99.0, COMP_SUSTAIN, COMP_POSITION},
    {"Compressor tone", 0.0, 99.0, COMP_TONE, COMP_POSITION},
    {"Compressor attack", 0.0, 99.0, COMP_ATTACK, COMP_POSITION},
    {"Compressor level", 0.0, 99.0, COMP_LEVEL, COMP_POSITION},
};

static SettingsWidget comp_cs_widgets[] = {
    {"Compressor sustain", 0.0, 99.0, COMP_SUSTAIN, COMP_POSITION},
    {"Compressor attack", 0.0, 99.0, COMP_ATTACK, COMP_POSITION},
    {"Compressor level", 0.0, 99.0, COMP_LEVEL, COMP_POSITION},
};

static SettingsWidget dist_screamer_widgets[] = {
    {"Screamer drive", 0.0, 99.0, DIST_SCREAMER_DRIVE, DIST_POSITION},
    {"Screamer tone", 0.0, 99.0, DIST_SCREAMER_TONE, DIST_POSITION},
    {"Screamer level", 0.0, 99.0, DIST_SCREAMER_LVL, DIST_POSITION},
};

static SettingsWidget dist_808_widgets[] = {
    {"808 overdrive", 0.0, 99.0, DIST_808_OVERDRIVE, DIST_POSITION},
    {"808 tone", 0.0, 99.0, DIST_808_TONE, DIST_POSITION},
    {"808 level", 0.0, 99.0, DIST_808_LVL, DIST_POSITION},
};

static SettingsWidget dist_guyod_widgets[] = {
    {"GuyOD drive", 0.0, 99.0, DIST_GUYOD_DRIVE, DIST_POSITION},
    {"GuyOD level", 0.0, 99.0, DIST_GUYOD_LVL, DIST_POSITION},
};

static SettingsWidget dist_dod250_widgets[] = {
    {"DOD250 gain", 0.0, 99.0, DIST_DOD250_GAIN, DIST_POSITION},
    {"DOD250 level", 0.0, 99.0, DIST_DOD250_LVL, DIST_POSITION},
};

static SettingsWidget dist_rodent_widgets[] = {
    {"Rodent dist", 0.0, 99.0, DIST_RODENT_DIST, DIST_POSITION},
    {"Rodent filter", 0.0, 99.0, DIST_RODENT_FILTER, DIST_POSITION},
    {"Rodent level", 0.0, 99.0, DIST_RODENT_LVL, DIST_POSITION},
};

static SettingsWidget dist_mx_widgets[] = {
    {"MX dist", 0.0, 99.0, DIST_MX_DIST, DIST_POSITION},
    {"MX output", 0.0, 99.0, DIST_MX_OUTPUT, DIST_POSITION},
};

static SettingsWidget dist_ds_widgets[] = {
    {"DS gain", 0.0, 99.0, DIST_DS_GAIN, DIST_POSITION},
    {"DS tone", 0.0, 99.0, DIST_DS_TONE, DIST_POSITION},
    {"DS level", 0.0, 99.0, DIST_DS_LVL, DIST_POSITION},
};

static SettingsWidget dist_grunge_widgets[] = {
    {"Grunge", 0.0, 99.0, DIST_GRUNGE_GRUNGE, DIST_POSITION},
    {"Grunge face", 0.0, 99.0, DIST_GRUNGE_FACE, DIST_POSITION},
    {"Grunge loud", 0.0, 99.0, DIST_GRUNGE_LOUD, DIST_POSITION},
    {"Grunge butt", 0.0, 99.0, DIST_GRUNGE_BUTT, DIST_POSITION},
};

static SettingsWidget dist_zone_widgets[] = {
    {"Zone gain", 0.0, 99.0, DIST_ZONE_GAIN, DIST_POSITION},
    {"Zone low", 0.0, 99.0, DIST_ZONE_LOW, DIST_POSITION},
    {"Zone mid level", 0.0, 99.0, DIST_ZONE_MID_LVL, DIST_POSITION},
    {"Zone mid freq", 0.0, 99.0, DIST_ZONE_MID_FREQ, DIST_POSITION},
    {"Zone high", 0.0, 99.0, DIST_ZONE_HIGH, DIST_POSITION},
    {"Zone level", 0.0, 99.0, DIST_ZONE_LEVEL, DIST_POSITION},
};

static SettingsWidget dist_death_widgets[] = {
    {"Death low", 0.0, 99.0, DIST_DEATH_LOW, DIST_POSITION},
    {"Death mid", 0.0, 99.0, DIST_DEATH_MID, DIST_POSITION},
    {"Death high", 0.0, 99.0, DIST_DEATH_HIGH, DIST_POSITION},
    {"Death level", 0.0, 99.0, DIST_DEATH_LVL, DIST_POSITION},
};

static SettingsWidget dist_gonk_widgets[] = {
    {"Gonk gonk", 0.0, 99.0, DIST_GONK_GONK, DIST_POSITION},
    {"Gonk smear", 0.0, 99.0, DIST_GONK_SMEAR, DIST_POSITION},
    {"Gonk suck", 0.0, 99.0, DIST_GONK_SUCK, DIST_POSITION},
    {"Gonk heave", 0.0, 99.0, DIST_GONK_HEAVE, DIST_POSITION},
};

static SettingsWidget dist_fuzzy_widgets[] = {
    {"Fuzzy fuzz", 0.0, 99.0, DIST_FUZZY_FUZZ, DIST_POSITION},
    {"Fuzzy volume", 0.0, 99.0, DIST_FUZZY_VOLUME, DIST_POSITION},
};

static SettingsWidget dist_mp_widgets[] = {
    {"MP sustain", 0.0, 99.0, DIST_MP_SUSTAIN, DIST_POSITION},
    {"MP tone", 0.0, 99.0, DIST_MP_TONE, DIST_POSITION},
    {"MP volume", 0.0, 99.0, DIST_MP_VOLUME, DIST_POSITION},
};

static SettingsWidget eq_widgets[] = {
    {"EQ gain", 0.0, 99.0, AMP_GAIN, AMP_POSITION},
    {"EQ level", 0.0, 99.0, AMP_LEVEL, AMP_POSITION},
    // TODO: make those display propertly (display range -12 to 12)
    {"EQ bass", 0.0, 24.0, EQ_BASS, EQ_POSITION},
    {"EQ mid", 0.0, 24.0, EQ_MID, EQ_POSITION},
    {"EQ treble", 0.0, 24.0, EQ_TREBLE, EQ_POSITION},
    // TODO: make this display propertly (display range 300 to 5000)
    {"EQ mid Hz", 0.0, 4700.0, EQ_MID_HZ, EQ_POSITION},
    // TODO: make this display propertly (display range 500 to 8000)
    {"EQ treb Hz", 0.0, 7500.0, EQ_TREBLE_HZ, EQ_POSITION},
};

static SettingsWidget noisegate_gate_widgets[] = {
    {"Gate treshold", 0.0, 99.0, NOISEGATE_GATE_TRESHOLD, NOISEGATE_POSITION},
    {"Gate attack", 0.0, 99.0, NOISEGATE_ATTACK, NOISEGATE_POSITION},
    {"Gate release", 0.0, 99.0, NOISEGATE_RELEASE, NOISEGATE_POSITION},
    {"Gate attn", 0.0, 99.0, NOISEGATE_ATTN, NOISEGATE_POSITION},
};

static SettingsWidget noisegate_swell_widgets[] = {
    {"Swell sens", 0.0, 99.0, NOISEGATE_SWELL_SENS, NOISEGATE_POSITION},
    {"Swell attack", 0.0, 99.0, NOISEGATE_ATTACK, NOISEGATE_POSITION},
    {"Swell release", 0.0, 99.0, NOISEGATE_RELEASE, NOISEGATE_POSITION},
    {"Swell attn", 0.0, 99.0, NOISEGATE_ATTN, NOISEGATE_POSITION},
};

static SettingsWidget chorusfx_ce_widgets[] = {
    {"CE chorus speed", 0.0, 99.0, CE_CHORUS_SPEED, CHORUSFX_POSITION},
    {"CE chorus depth", 0.0, 99.0, CE_CHORUS_DEPTH, CHORUSFX_POSITION},
};

static SettingsWidget chorusfx_dual_widgets[] = {
    {"Dual chorus speed", 0.0, 99.0, DUAL_CHORUS_SPEED, CHORUSFX_POSITION},
    {"Dual chorus depth", 0.0, 99.0, DUAL_CHORUS_DEPTH, CHORUSFX_POSITION},
    {"Dual chorus level", 0.0, 99.0, DUAL_CHORUS_LEVEL, CHORUSFX_POSITION},
    // TODO: DUAL_CHORUS_WAVE with valid options WAVE_TRI, WAVE_SINE, WAVE_SQUARE
};

static SettingsWidget chorusfx_multi_widgets[] = {
    {"Multi chorus speed", 0.0, 99.0, MULTI_CHORUS_SPEED, CHORUSFX_POSITION},
    {"Multi chorus depth", 0.0, 99.0, MULTI_CHORUS_DEPTH, CHORUSFX_POSITION},
    {"Multi chorus level", 0.0, 99.0, MULTI_CHORUS_LEVEL, CHORUSFX_POSITION},
    // TODO: MULTI_CHORUS_WAVE with valid options WAVE_TRI, WAVE_SINE, WAVE_SQUARE
};

static SettingsWidget chorusfx_flanger_widgets[] = {
    {"Flanger speed", 0.0, 99.0, FLANGER_SPEED, CHORUSFX_POSITION},
    {"Flanger depth", 0.0, 99.0, FLANGER_DEPTH, CHORUSFX_POSITION},
    {"Flanger regen", 0.0, 99.0, FLANGER_REGEN, CHORUSFX_POSITION},
    {"Flanger level", 0.0, 99.0, FLANGER_LEVEL, CHORUSFX_POSITION},
    // TODO: FLANGER_WAVE with valid options WAVE_TRI, WAVE_SINE, WAVE_SQUARE
};

static SettingsWidget chorusfx_mxr_flanger_widgets[] = {
    {"MXR flanger speed", 0.0, 99.0, MXR_FLANGER_SPEED, CHORUSFX_POSITION},
    {"MXR flanger width", 0.0, 99.0, MXR_FLANGER_WIDTH, CHORUSFX_POSITION},
    {"MXR flanger regen", 0.0, 99.0, MXR_FLANGER_REGEN, CHORUSFX_POSITION},
    {"MXR flanger manual", 0.0, 99.0, MXR_FLANGER_MANUAL, CHORUSFX_POSITION},
};

static SettingsWidget chorusfx_phaser_widgets[] = {
    {"Phaser speed", 0.0, 99.0, PHASER_SPEED, CHORUSFX_POSITION},
    {"Phaser depth", 0.0, 99.0, PHASER_DEPTH, CHORUSFX_POSITION},
    {"Phaser regen", 0.0, 99.0, PHASER_REGEN, CHORUSFX_POSITION},
    {"Phaser level", 0.0, 99.0, PHASER_LEVEL, CHORUSFX_POSITION},
    // TODO: PHASER_WAVE with valid options WAVE_TRI, WAVE_SINE, WAVE_SQUARE
};

static SettingsWidget chorusfx_vibrato_widgets[] = {
    {"Vibrato speed", 0.0, 99.0, VIBRATO_SPEED, CHORUSFX_POSITION},
    {"Vibrato depth", 0.0, 99.0, VIBRATO_DEPTH, CHORUSFX_POSITION},
};

static SettingsWidget chorusfx_rotary_widgets[] = {
    {"Rotary speed", 0.0, 99.0, ROTARY_SPEED, CHORUSFX_POSITION},
    {"Rotary intensity", 0.0, 99.0, ROTARY_INTENSITY, CHORUSFX_POSITION},
    {"Rotary doppler", 0.0, 99.0, ROTARY_DOPPLER, CHORUSFX_POSITION},
    {"Rotary crossover", 0.0, 99.0, ROTARY_CROSSOVER, CHORUSFX_POSITION},
};

static SettingsWidget chorusfx_vibropan_widgets[] = {
    {"Vibropan speed", 0.0, 99.0, VIBROPAN_SPEED, CHORUSFX_POSITION},
    {"Vibropan depth", 0.0, 99.0, VIBROPAN_DEPTH, CHORUSFX_POSITION},
    {"Vibropan vibra", 0.0, 99.0, VIBROPAN_VIBRA, CHORUSFX_POSITION},
    // TODO: VIBROPAN_WAVE with valid options WAVE_TRI, WAVE_SINE, WAVE_SQUARE
};

static SettingsWidget chorusfx_tremolo_widgets[] = {
    {"Tremolo speed", 0.0, 99.0, TREMOLO_SPEED, CHORUSFX_POSITION},
    {"Tremolo depth", 0.0, 99.0, TREMOLO_DEPTH, CHORUSFX_POSITION},
    // TODO: TREMOLO_WAVE with valid options WAVE_TRI, WAVE_SINE, WAVE_SQUARE
};

static SettingsWidget chorusfx_panner_widgets[] = {
    {"Panner speed", 0.0, 99.0, PANNER_SPEED, CHORUSFX_POSITION},
    {"Panner depth", 0.0, 99.0, PANNER_DEPTH, CHORUSFX_POSITION},
    // TODO: PANNER_WAVE with valid options WAVE_TRI, WAVE_SINE, WAVE_SQUARE
};

static SettingsWidget chorusfx_envelope_widgets[] = {
    {"Envelope sensitivity", 0.0, 99.0, ENVELOPE_SENSITIVITY, CHORUSFX_POSITION},
    {"Envelope range", 0.0, 99.0, ENVELOPE_RANGE, CHORUSFX_POSITION},
};

static SettingsWidget chorusfx_autoya_widgets[] = {
    {"AutoYa speed", 0.0, 99.0, AUTOYA_SPEED, CHORUSFX_POSITION},
    {"AutoYa intensity", 0.0, 99.0, AUTOYA_INTENSITY, CHORUSFX_POSITION},
    {"AutoYa range", 0.0, 49.0, AUTOYA_RANGE, CHORUSFX_POSITION},
};

static SettingsWidget chorusfx_yaya_widgets[] = {
    {"YaYa pedal", 0.0, 99.0, YAYA_PEDAL, CHORUSFX_POSITION},
    {"YaYa intensity", 0.0, 99.0, YAYA_INTENSITY, CHORUSFX_POSITION},
    {"YaYa range", 0.0, 49.0, YAYA_RANGE, CHORUSFX_POSITION},
};

static SettingsWidget chorusfx_step_filter_widgets[] = {
    {"Step filter speed", 0.0, 99.0, STEP_FILTER_SPEED, CHORUSFX_POSITION},
    {"Step filter intensity", 0.0, 99.0, STEP_FILTER_INTENSITY, CHORUSFX_POSITION},
};

static SettingsWidget chorusfx_whammy_widgets[] = {
    {"Whammy pedal", 0.0, 99.0, WHAMMY_PEDAL, CHORUSFX_POSITION},
    {"Whammy mix", 0.0, 99.0, WHAMMY_MIX, CHORUSFX_POSITION},
    //TODO: WHAMMY_AMOUNT with valid options:
    //      WHAMMY_OCT_UP, WHAMMY_2OCT_UP, WHAMMY_2ND_DN, WHAMMY_RV_2ND,
    //      WHAMMY_4TH_DN, WHAMMY_OCT_DN, WHAMMY_2OCT_DN, WHAMMY_DIV_BMB,
    //      WHAMMY_M3_MA, WHAMMY_2ND_MA3, WHAMMY_3RD_4TH, WHAMMY_4TH_5TH,
    //      WHAMMY_5TH_OCT, WHAMMY_HOCT_UP, WHAMMY_HOCT_DN, WHAMMY_OCT_UD
};

static SettingsWidget chorusfx_pitch_shift_widgets[] = {
    // TODO: make this display propertly (display range -24 to 24)
    {"Pitch amount", 0.0, 48.0, PITCH_AMOUNT, CHORUSFX_POSITION},
    {"Pitch mix", 0.0, 99.0, PITCH_MIX, CHORUSFX_POSITION},
};

static SettingsWidget chorusfx_detune_widgets[] = {
    {"Detune amount", 0.0, 48.0, DETUNE_AMOUNT, CHORUSFX_POSITION},
    {"Detune level", 0.0, 99.0, DETUNE_LEVEL, CHORUSFX_POSITION},
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

    {"IPS level", 0.0, 99.0, IPS_LEVEL, CHORUSFX_POSITION},
};

static SettingsWidget delay_analog_widgets[] = {
    // TODO: make this display propertly (10 msec to 5 sec)
    {"Delay time", 0.0, 139.0, DELAY_TIME, DELAY_POSITION},
    {"Delay analog level", 0.0, 99.0, ANALOG_LEVEL, DELAY_POSITION},
    // TODO: make last value display propertly
    {"Delay analog repeats", 0.0, 100.0, ANALOG_REPEATS, DELAY_POSITION},
};

static SettingsWidget delay_digital_widgets[] = {
    // TODO: make this display propertly (10 msec to 5 sec)
    {"Delay time", 0.0, 139.0, DELAY_TIME, DELAY_POSITION},
    {"Delay digital level", 0.0, 99.0, DIGITAL_LEVEL, DELAY_POSITION},
    // TODO: make last value display propertly
    {"Delay digital repeats", 0.0, 100.0, DIGITAL_REPEATS, DELAY_POSITION},
    {"Delay digital ducker thresh", 0.0, 99.0, DIGITAL_DUCKER_THRESH, DELAY_POSITION},
    {"Delay digital ducker level", 0.0, 99.0, DIGITAL_DUCKER_LEVEL, DELAY_POSITION},
};

static SettingsWidget delay_modulated_widgets[] = {
    // TODO: make this display propertly (10 msec to 5 sec)
    {"Delay time", 0.0, 139.0, DELAY_TIME, DELAY_POSITION},
    {"Delay modulated level", 0.0, 99.0, MODULATED_LEVEL, DELAY_POSITION},
    // TODO: make last value display propertly
    {"Delay modulated repeats", 0.0, 100.0, MODULATED_REPEATS, DELAY_POSITION},
    {"Delay modulated depth", 0.0, 99.0, MODULATED_DEPTH, DELAY_POSITION},
};

static SettingsWidget delay_pong_widgets[] = {
    // TODO: make this display propertly (10 msec to 5 sec)
    {"Delay time", 0.0, 139.0, DELAY_TIME, DELAY_POSITION},
    {"Delay pong level", 0.0, 99.0, PONG_LEVEL, DELAY_POSITION},
    // TODO: make last value display propertly
    {"Delay pong repeats", 0.0, 100.0, PONG_REPEATS, DELAY_POSITION},
    {"Delay pong ducker thresh", 0.0, 99.0, PONG_DUCKER_THRESH, DELAY_POSITION},
    {"Delay pong ducker level", 0.0, 99.0, PONG_DUCKER_LEVEL, DELAY_POSITION},
};

static SettingsWidget delay_tape_widgets[] = {
    // TODO: make this display propertly (10 msec to 5 sec)
    {"Delay time", 0.0, 139.0, DELAY_TIME, DELAY_POSITION},
    {"Delay tape level", 0.0, 99.0, TAPE_LEVEL, DELAY_POSITION},
    // TODO: make last value display propertly
    {"Delay tape repeats", 0.0, 100.0, TAPE_REPEATS, DELAY_POSITION},
    {"Delay tape wow", 0.0, 99.0, TAPE_WOW, DELAY_POSITION},
    {"Delay tape flutter", 0.0, 99.0, TAPE_FLUTTER, DELAY_POSITION},
};

static SettingsWidget reverb_twin_widgets[] = {
    {"Twin reverb", 0.0, 99.0, TWIN_REVERB, REVERB_POSITION},
};

static SettingsWidget reverb_lex_ambience_widgets[] = {
    {"Predelay", 0.0, 15.0, LEX_AMBIENCE_PREDELAY, REVERB_POSITION},
    {"Decay", 0.0, 99.0, LEX_AMBIENCE_DECAY, REVERB_POSITION},
    {"Liveliness", 0.0, 99.0, LEX_AMBIENCE_LIVELINESS, REVERB_POSITION},
    {"Level", 0.0, 99.0, LEX_AMBIENCE_LEVEL, REVERB_POSITION},
};

static SettingsWidget reverb_lex_studio_widgets[] = {
    {"Predelay", 0.0, 15.0, LEX_STUDIO_PREDELAY, REVERB_POSITION},
    {"Decay", 0.0, 99.0, LEX_STUDIO_DECAY, REVERB_POSITION},
    {"Liveliness", 0.0, 99.0, LEX_STUDIO_LIVELINESS, REVERB_POSITION},
    {"Level", 0.0, 99.0, LEX_STUDIO_LEVEL, REVERB_POSITION},
};

static SettingsWidget reverb_lex_room_widgets[] = {
    {"Predelay", 0.0, 15.0, LEX_ROOM_PREDELAY, REVERB_POSITION},
    {"Decay", 0.0, 99.0, LEX_ROOM_DECAY, REVERB_POSITION},
    {"Liveliness", 0.0, 99.0, LEX_ROOM_LIVELINESS, REVERB_POSITION},
    {"Level", 0.0, 99.0, LEX_ROOM_LEVEL, REVERB_POSITION},
};

static SettingsWidget reverb_lex_hall_widgets[] = {
    {"Predelay", 0.0, 15.0, LEX_HALL_PREDELAY, REVERB_POSITION},
    {"Decay", 0.0, 99.0, LEX_HALL_DECAY, REVERB_POSITION},
    {"Liveliness", 0.0, 99.0, LEX_HALL_LIVELINESS, REVERB_POSITION},
    {"Level", 0.0, 99.0, LEX_HALL_LEVEL, REVERB_POSITION},
};

static SettingsWidget reverb_emt240_plate_widgets[] = {
    {"Predelay", 0.0, 15.0, EMT240_PLATE_PREDELAY, REVERB_POSITION},
    {"Decay", 0.0, 99.0, EMT240_PLATE_DECAY, REVERB_POSITION},
    {"Liveliness", 0.0, 99.0, EMT240_PLATE_LIVELINESS, REVERB_POSITION},
    {"Level", 0.0, 99.0, EMT240_PLATE_LEVEL, REVERB_POSITION},
};

static WidgetContainer wah_container[] = {
    {WAH_TYPE_CRY, "Cry wah", WAH_TYPE, WAH_POSITION, wah_widgets, G_N_ELEMENTS(wah_widgets)},
    {WAH_TYPE_FULLRANGE, "Fullrange wah", WAH_TYPE, WAH_POSITION, wah_widgets, G_N_ELEMENTS(wah_widgets)},
    {WAH_TYPE_CLYDE, "Clyde wah", WAH_TYPE, WAH_POSITION, wah_widgets, G_N_ELEMENTS(wah_widgets)},
};

static WidgetContainer comp_container[] = {
    {COMP_TYPE_DIGI, "Digital compressor", COMP_TYPE, COMP_POSITION, comp_digi_widgets, G_N_ELEMENTS(comp_digi_widgets)},
    {COMP_TYPE_CS, "CS compressor", COMP_TYPE, COMP_POSITION, comp_cs_widgets, G_N_ELEMENTS(comp_cs_widgets)},
};

static WidgetContainer dist_container[] = {
    {DIST_TYPE_SCREAMER, "Screamer", DIST_TYPE, DIST_POSITION, dist_screamer_widgets, G_N_ELEMENTS(dist_screamer_widgets)},
    {DIST_TYPE_808, "808", DIST_TYPE, DIST_POSITION, dist_808_widgets, G_N_ELEMENTS(dist_808_widgets)},
    {DIST_TYPE_GUYOD, "GuyOD", DIST_TYPE, DIST_POSITION, dist_guyod_widgets, G_N_ELEMENTS(dist_guyod_widgets)},
    {DIST_TYPE_DOD250, "DOD250", DIST_TYPE, DIST_POSITION, dist_dod250_widgets, G_N_ELEMENTS(dist_dod250_widgets)},
    {DIST_TYPE_RODENT, "Rodent", DIST_TYPE, DIST_POSITION, dist_rodent_widgets, G_N_ELEMENTS(dist_rodent_widgets)},
    {DIST_TYPE_MX, "MX", DIST_TYPE, DIST_POSITION, dist_mx_widgets, G_N_ELEMENTS(dist_mx_widgets)},
    {DIST_TYPE_DS, "DS", DIST_TYPE, DIST_POSITION, dist_ds_widgets, G_N_ELEMENTS(dist_ds_widgets)},
    {DIST_TYPE_GRUNGE, "Grunge", DIST_TYPE, DIST_POSITION, dist_grunge_widgets, G_N_ELEMENTS(dist_grunge_widgets)},
    {DIST_TYPE_ZONE, "Zone", DIST_TYPE, DIST_POSITION, dist_zone_widgets, G_N_ELEMENTS(dist_zone_widgets)},
    {DIST_TYPE_DEATH, "Death", DIST_TYPE, DIST_POSITION, dist_death_widgets, G_N_ELEMENTS(dist_death_widgets)},
    {DIST_TYPE_GONK, "Gonk", DIST_TYPE, DIST_POSITION, dist_gonk_widgets, G_N_ELEMENTS(dist_gonk_widgets)},
    {DIST_TYPE_FUZZY, "Fuzzy", DIST_TYPE, DIST_POSITION, dist_fuzzy_widgets, G_N_ELEMENTS(dist_fuzzy_widgets)},
    {DIST_TYPE_MP, "MP", DIST_TYPE, DIST_POSITION, dist_mp_widgets, G_N_ELEMENTS(dist_mp_widgets)},
};

static WidgetContainer noisegate_container[] = {
    {NOISEGATE_GATE, "Gate", NOISEGATE_TYPE, NOISEGATE_POSITION, noisegate_gate_widgets, G_N_ELEMENTS(noisegate_gate_widgets)},
    {NOISEGATE_SWELL, "Swell", NOISEGATE_TYPE, NOISEGATE_POSITION, noisegate_swell_widgets, G_N_ELEMENTS(noisegate_swell_widgets)},
};

static WidgetContainer chorusfx_container[] = {
    {CHORUS_TYPE_CE, "CE Chorus", CHORUSFX_TYPE, CHORUSFX_POSITION, chorusfx_ce_widgets, G_N_ELEMENTS(chorusfx_ce_widgets)},
    {CHORUS_TYPE_DUAL, "Dual Chorus", CHORUSFX_TYPE, CHORUSFX_POSITION, chorusfx_dual_widgets, G_N_ELEMENTS(chorusfx_dual_widgets)},
    {CHORUS_TYPE_MULTI, "Multi Chorus", CHORUSFX_TYPE, CHORUSFX_POSITION, chorusfx_multi_widgets, G_N_ELEMENTS(chorusfx_multi_widgets)},
    {CHORUS_TYPE_FLANGER, "Flanger", CHORUSFX_TYPE, CHORUSFX_POSITION, chorusfx_flanger_widgets, G_N_ELEMENTS(chorusfx_flanger_widgets)},
    {CHORUS_TYPE_MXR_FLANGER, "MXR FLANGER", CHORUSFX_TYPE, CHORUSFX_POSITION, chorusfx_mxr_flanger_widgets, G_N_ELEMENTS(chorusfx_mxr_flanger_widgets)},
    {CHORUS_TYPE_PHASER, "Phaser", CHORUSFX_TYPE, CHORUSFX_POSITION, chorusfx_phaser_widgets, G_N_ELEMENTS(chorusfx_phaser_widgets)},
    {CHORUS_TYPE_VIBRATO, "Vibrato", CHORUSFX_TYPE, CHORUSFX_POSITION, chorusfx_vibrato_widgets, G_N_ELEMENTS(chorusfx_vibrato_widgets)},
    {CHORUS_TYPE_ROTARY, "Rotary", CHORUSFX_TYPE, CHORUSFX_POSITION, chorusfx_rotary_widgets, G_N_ELEMENTS(chorusfx_rotary_widgets)},
    {CHORUS_TYPE_VIBROPAN, "Vibropan", CHORUSFX_TYPE, CHORUSFX_POSITION, chorusfx_vibropan_widgets, G_N_ELEMENTS(chorusfx_vibropan_widgets)},
    {CHORUS_TYPE_TREMOLO, "Tremolo", CHORUSFX_TYPE, CHORUSFX_POSITION, chorusfx_tremolo_widgets, G_N_ELEMENTS(chorusfx_tremolo_widgets)},
    {CHORUS_TYPE_PANNER, "Panner", CHORUSFX_TYPE, CHORUSFX_POSITION, chorusfx_panner_widgets, G_N_ELEMENTS(chorusfx_panner_widgets)},
    {CHORUS_TYPE_ENVELOPE, "Envelope", CHORUSFX_TYPE, CHORUSFX_POSITION, chorusfx_envelope_widgets, G_N_ELEMENTS(chorusfx_envelope_widgets)},
    {CHORUS_TYPE_AUTOYA, "AutoYa", CHORUSFX_TYPE, CHORUSFX_POSITION, chorusfx_autoya_widgets, G_N_ELEMENTS(chorusfx_autoya_widgets)},
    {CHORUS_TYPE_YAYA, "YaYa", CHORUSFX_TYPE, CHORUSFX_POSITION, chorusfx_yaya_widgets, G_N_ELEMENTS(chorusfx_yaya_widgets)},
    {CHORUS_TYPE_STEP_FILTER, "Step Filter", CHORUSFX_TYPE, CHORUSFX_POSITION, chorusfx_step_filter_widgets, G_N_ELEMENTS(chorusfx_step_filter_widgets)},
    {CHORUS_TYPE_WHAMMY, "Whammy", CHORUSFX_TYPE, CHORUSFX_POSITION, chorusfx_whammy_widgets, G_N_ELEMENTS(chorusfx_whammy_widgets)},
    {CHORUS_TYPE_PITCH_SHIFT, "Pitch Shift", CHORUSFX_TYPE, CHORUSFX_POSITION, chorusfx_pitch_shift_widgets, G_N_ELEMENTS(chorusfx_pitch_shift_widgets)},
    {CHORUS_TYPE_DETUNE, "Detune", CHORUSFX_TYPE, CHORUSFX_POSITION, chorusfx_detune_widgets, G_N_ELEMENTS(chorusfx_detune_widgets)},
    {CHORUS_TYPE_IPS, "IPS", CHORUSFX_TYPE, CHORUSFX_POSITION, chorusfx_ips_widgets, G_N_ELEMENTS(chorusfx_ips_widgets)},
};

static WidgetContainer delay_container[] = {
    {DELAY_TYPE_ANALOG, "Analog", DELAY_TYPE, DELAY_POSITION, delay_analog_widgets, G_N_ELEMENTS(delay_analog_widgets)},
    {DELAY_TYPE_DIGITAL, "Digital", DELAY_TYPE, DELAY_POSITION, delay_digital_widgets, G_N_ELEMENTS(delay_digital_widgets)},
    {DELAY_TYPE_MODULATED, "Modulated", DELAY_TYPE, DELAY_POSITION, delay_modulated_widgets, G_N_ELEMENTS(delay_modulated_widgets)},
    {DELAY_TYPE_PONG, "Pong", DELAY_TYPE, DELAY_POSITION, delay_pong_widgets, G_N_ELEMENTS(delay_pong_widgets)},
    {DELAY_TYPE_TAPE, "Tape", DELAY_TYPE, DELAY_POSITION, delay_tape_widgets, G_N_ELEMENTS(delay_tape_widgets)},
};

static WidgetContainer reverb_container[] = {
    {REVERB_TYPE_TWIN, "Twin", REVERB_TYPE, REVERB_POSITION, reverb_twin_widgets, G_N_ELEMENTS(reverb_twin_widgets)},
    {REVERB_TYPE_LEX_AMBIENCE, "Lexicon ambience", REVERB_TYPE, REVERB_POSITION, reverb_lex_ambience_widgets, G_N_ELEMENTS(reverb_lex_ambience_widgets)},
    {REVERB_TYPE_LEX_STUDIO, "Lexicon studio", REVERB_TYPE, REVERB_POSITION, reverb_lex_studio_widgets, G_N_ELEMENTS(reverb_lex_studio_widgets)},
    {REVERB_TYPE_LEX_ROOM, "Lexicon room", REVERB_TYPE, REVERB_POSITION, reverb_lex_room_widgets, G_N_ELEMENTS(reverb_lex_room_widgets)},
    {REVERB_TYPE_LEX_HALL, "Lexicon hall", REVERB_TYPE, REVERB_POSITION, reverb_lex_hall_widgets, G_N_ELEMENTS(reverb_lex_hall_widgets)},
    {REVERB_TYPE_EMT240_PLATE, "EMT240 Plate", REVERB_TYPE, REVERB_POSITION, reverb_emt240_plate_widgets, G_N_ELEMENTS(reverb_emt240_plate_widgets)},
};

static WidgetContainer eq_container[] = {
    {EQ_TYPE_BRIGHT, "Bright", EQ_TYPE, EQ_POSITION, eq_widgets, G_N_ELEMENTS(eq_widgets)},
    {EQ_TYPE_MIDBOOST, "Mid Boost", EQ_TYPE, EQ_POSITION, eq_widgets, G_N_ELEMENTS(eq_widgets)},
    {EQ_TYPE_SCOOP, "Scoop", EQ_TYPE, EQ_POSITION, eq_widgets, G_N_ELEMENTS(eq_widgets)},
    {EQ_TYPE_WARM, "Warm", EQ_TYPE, EQ_POSITION, eq_widgets, G_N_ELEMENTS(eq_widgets)},
};

static VBoxWidget wah_vbox[] = {
    {"Wah", FALSE, set_wah_on_off, wah_container, G_N_ELEMENTS(wah_container)},
};

static VBoxWidget comp_vbox[] = {
    {"Compressor", FALSE, set_comp_on_off, comp_container, G_N_ELEMENTS(comp_container)},
};

static VBoxWidget dist_vbox[] = {
    {"Distortion", FALSE, set_dist_on_off, dist_container, G_N_ELEMENTS(dist_container)},
};

static VBoxWidget noisegate_vbox[] = {
    {"Noisegate", FALSE, set_noisegate_on_off, noisegate_container, G_N_ELEMENTS(noisegate_container)},
};

static VBoxWidget chorusfx_vbox[] = {
    {"Chorus/FX", FALSE, set_chorusfx_on_off, chorusfx_container, G_N_ELEMENTS(chorusfx_container)},
};

static VBoxWidget delay_vbox[] = {
    {"Delay", FALSE, set_delay_on_off, delay_container, G_N_ELEMENTS(delay_container)},
};

static VBoxWidget reverb_vbox[] = {
    {"Reverb", FALSE, set_reverb_on_off, reverb_container, G_N_ELEMENTS(reverb_container)},
};

static VBoxWidget eq_vbox[] = {
    {"EQ", FALSE, set_eq_on_off, eq_container, G_N_ELEMENTS(eq_container)},
};

VBoxes vboxes[] = {
    {wah_vbox, G_N_ELEMENTS(wah_vbox)},
    {eq_vbox, G_N_ELEMENTS(eq_vbox)},
    {comp_vbox, G_N_ELEMENTS(comp_vbox)},
    {dist_vbox, G_N_ELEMENTS(dist_vbox)},
    {noisegate_vbox, G_N_ELEMENTS(noisegate_vbox)},
    {chorusfx_vbox, G_N_ELEMENTS(chorusfx_vbox)},
    {delay_vbox, G_N_ELEMENTS(delay_vbox)},
    {reverb_vbox, G_N_ELEMENTS(reverb_vbox)},
};

int n_vboxes = G_N_ELEMENTS(vboxes);
