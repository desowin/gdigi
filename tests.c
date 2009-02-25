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

void test_wah()
{
    int x;

    set_wah_type(WAH_TYPE_CRY);
    for (x=0; x<=99; x++)
        set_wah_min(x);

    for (x=0; x<=99; x++)
        set_wah_max(x);

    for (x=0; x<=12; x++)
        set_wah_level(x);

    set_wah_type(WAH_TYPE_FULLRANGE);
    for (x=0; x<=99; x++)
        set_wah_min(x);

    for (x=0; x<=99; x++)
        set_wah_max(x);

    for (x=0; x<=12; x++)
        set_wah_level(x);

    set_wah_type(WAH_TYPE_CLYDE);
    for (x=0; x<=99; x++)
        set_wah_min(x);

    for (x=0; x<=99; x++)
        set_wah_max(x);

    for (x=0; x<=12; x++)
        set_wah_level(x);

    set_wah_on_off(TRUE);
    set_wah_on_off(FALSE);
}

void test_compressor()
{
    int x;

    set_comp_type(COMP_TYPE_DIGI);
    for (x=0; x<=99; x++)
        set_comp_sustain(x);

    for (x=0; x<=99; x++)
        set_comp_tone(x);

    for (x=0; x<=99; x++)
        set_comp_attack(x);

    for (x=0; x<=99; x++)
        set_comp_level(x);

    set_comp_type(COMP_TYPE_CS);
    for (x=0; x<=99; x++)
        set_comp_sustain(x);

    for (x=0; x<=99; x++)
        set_comp_attack(x);

    for (x=0; x<=99; x++)
        set_comp_level(x);

    set_comp_on_off(TRUE);
    set_comp_on_off(FALSE);
}

void test_dist()
{
    int x;

    set_dist_type(DIST_TYPE_SCREAMER);
    for (x=0; x<=99; x++)
        set_dist_option(DIST_SCREAMER_DRIVE, x);
    for (x=0; x<=99; x++)
        set_dist_option(DIST_SCREAMER_TONE, x);
    for (x=0; x<=99; x++)
        set_dist_option(DIST_SCREAMER_LVL, x);

    set_dist_type(DIST_TYPE_808);
    for (x=0; x<=99; x++)
        set_dist_option(DIST_808_OVERDRIVE, x);
    for (x=0; x<=99; x++)
        set_dist_option(DIST_808_TONE, x);
    for (x=0; x<=99; x++)
        set_dist_option(DIST_808_LVL, x);

    set_dist_type(DIST_TYPE_GUYOD);
    for (x=0; x<=99; x++)
        set_dist_option(DIST_GUYOD_DRIVE, x);
    for (x=0; x<=99; x++)
        set_dist_option(DIST_GUYOD_LVL, x);

    set_dist_type(DIST_TYPE_DOD250);
    for (x=0; x<=99; x++)
        set_dist_option(DIST_DOD250_GAIN, x);
    for (x=0; x<=99; x++)
        set_dist_option(DIST_DOD250_LVL, x);

    set_dist_type(DIST_TYPE_RODENT);
    for (x=0; x<=99; x++)
        set_dist_option(DIST_RODENT_DIST, x);
    for (x=0; x<=99; x++)
        set_dist_option(DIST_RODENT_FILTER, x);
    for (x=0; x<=99; x++)
        set_dist_option(DIST_RODENT_LVL, x);

    set_dist_type(DIST_TYPE_MX);
    for (x=0; x<=99; x++)
        set_dist_option(DIST_MX_DIST, x);
    for (x=0; x<=99; x++)
        set_dist_option(DIST_MX_OUTPUT, x);

    set_dist_type(DIST_TYPE_DS);
    for (x=0; x<=99; x++)
        set_dist_option(DIST_DS_GAIN, x);
    for (x=0; x<=99; x++)
        set_dist_option(DIST_DS_TONE, x);
    for (x=0; x<=99; x++)
        set_dist_option(DIST_DS_LVL, x);

    set_dist_type(DIST_TYPE_GRUNGE);
    for (x=0; x<=99; x++)
        set_dist_option(DIST_GRUNGE_GRUNGE, x);
    for (x=0; x<=99; x++)
        set_dist_option(DIST_GRUNGE_FACE, x);
    for (x=0; x<=99; x++)
        set_dist_option(DIST_GRUNGE_LOUD, x);
    for (x=0; x<=99; x++)
        set_dist_option(DIST_GRUNGE_BUTT, x);

    set_dist_type(DIST_TYPE_ZONE);
    for (x=0; x<=99; x++)
        set_dist_option(DIST_ZONE_GAIN, x);
    for (x=0; x<=99; x++)
        set_dist_option(DIST_ZONE_LOW, x);
    for (x=0; x<=99; x++)
        set_dist_option(DIST_ZONE_MID_LVL, x);
    for (x=0; x<=99; x++)
        set_dist_option(DIST_ZONE_MID_FREQ, x);
    for (x=0; x<=99; x++)
        set_dist_option(DIST_ZONE_HIGH, x);
    for (x=0; x<=99; x++)
        set_dist_option(DIST_ZONE_LEVEL, x);

    set_dist_type(DIST_TYPE_DEATH);
    for (x=0; x<=99; x++)
        set_dist_option(DIST_DEATH_LOW, x);
    for (x=0; x<=99; x++)
        set_dist_option(DIST_DEATH_MID, x);
    for (x=0; x<=99; x++)
        set_dist_option(DIST_DEATH_HIGH, x);
    for (x=0; x<=99; x++)
        set_dist_option(DIST_DEATH_LVL, x);

    set_dist_type(DIST_TYPE_GONK);
    for (x=0; x<=99; x++)
        set_dist_option(DIST_GONK_GONK, x);
    for (x=0; x<=99; x++)
        set_dist_option(DIST_GONK_SMEAR, x);
    for (x=0; x<=99; x++)
        set_dist_option(DIST_GONK_SUCK, x);
    for (x=0; x<=99; x++)
        set_dist_option(DIST_GONK_HEAVE, x);

    set_dist_type(DIST_TYPE_FUZZY);
    for (x=0; x<=99; x++)
        set_dist_option(DIST_FUZZY_FUZZ, x);
    for (x=0; x<=99; x++)
        set_dist_option(DIST_FUZZY_VOLUME, x);

    set_dist_type(DIST_TYPE_MP);
    for (x=0; x<=99; x++)
        set_dist_option(DIST_MP_SUSTAIN, x);
    for (x=0; x<=99; x++)
        set_dist_option(DIST_MP_TONE, x);
    for (x=0; x<=99; x++)
        set_dist_option(DIST_MP_VOLUME, x);

    set_dist_on_off(TRUE);
    set_dist_on_off(FALSE);
}

void test_presets()
{
    int x;

    for (x=0; x<=60; x++)
        switch_user_preset(x);

    for (x=0; x<=60; x++)
        switch_system_preset(x);

    for (x=0; x<=99; x++)
        set_preset_level(x);
}

void test_pickups()
{
    set_pickup_type(PICKUP_TYPE_HB_SC);
    set_pickup_type(PICKUP_TYPE_SC_HB);

    set_pickup_on_off(TRUE);
    set_pickup_on_off(FALSE);
}

void test_eq()
{
    int x;

    set_eq_type(EQ_TYPE_BRIGHT);
    set_eq_type(EQ_TYPE_MIDBOOST);
    set_eq_type(EQ_TYPE_SCOOP);
    set_eq_type(EQ_TYPE_WARM);

    for (x=0; x<=99; x++)
        set_eq_gain(x);
    for (x=0; x<=99; x++)
        set_eq_level(x);

    for (x=0; x<=0x18; x++)
        set_eq_bass(x);
    for (x=0; x<=0x18; x++)
        set_eq_mid(x);
    for (x=0; x<=4700; x++)
        set_eq_mid_hz(x);
    for (x=0; x<=0x18; x++)
        set_eq_treble(x);
    for (x=0; x<=7500; x++)
        set_eq_treb_hz(x);

    set_eq_on_off(TRUE);
    set_eq_on_off(FALSE);
}

void test_noisegate()
{
    int x;

    set_noisegate_type(NOISEGATE_GATE);
    for (x=0; x<=99; x++)
        set_gate_option(NOISEGATE_GATE_TRESHOLD, x);
    for (x=0; x<=99; x++)
        set_gate_option(NOISEGATE_ATTACK, x);
    for (x=0; x<=99; x++)
        set_gate_option(NOISEGATE_RELEASE, x);
    for (x=0; x<=99; x++)
        set_gate_option(NOISEGATE_ATTN, x);

    set_noisegate_type(NOISEGATE_SWELL);
    for (x=0; x<=99; x++)
        set_gate_option(NOISEGATE_SWELL_SENS, x);
    for (x=0; x<=99; x++)
        set_gate_option(NOISEGATE_ATTACK, x);
    for (x=0; x<=99; x++)
        set_gate_option(NOISEGATE_RELEASE, x);
    for (x=0; x<=99; x++)
        set_gate_option(NOISEGATE_ATTN, x);

    set_noisegate_on_off(TRUE);
    set_noisegate_on_off(FALSE);
}

void test_chorusfx()
{
    int x;

    set_chorusfx_type(CHORUS_TYPE_CE);
    for (x=0; x<=99; x++)
        set_chorusfx_option(CE_CHORUS_SPEED, x);
    for (x=0; x<=99; x++)
        set_chorusfx_option(CE_CHORUS_DEPTH, x);

    set_chorusfx_type(CHORUS_TYPE_DUAL);
    for (x=0; x<=99; x++)
        set_chorusfx_option(DUAL_CHORUS_SPEED, x);
    for (x=0; x<=99; x++)
        set_chorusfx_option(DUAL_CHORUS_DEPTH, x);
    for (x=0; x<=99; x++)
        set_chorusfx_option(DUAL_CHORUS_LEVEL, x);
    set_chorusfx_option(DUAL_CHORUS_WAVE, WAVE_TRI);
    set_chorusfx_option(DUAL_CHORUS_WAVE, WAVE_SINE);
    set_chorusfx_option(DUAL_CHORUS_WAVE, WAVE_SQUARE);

    set_chorusfx_type(CHORUS_TYPE_MULTI);
    for (x=0; x<=99; x++)
        set_chorusfx_option(MULTI_CHORUS_SPEED, x);
    for (x=0; x<=99; x++)
        set_chorusfx_option(MULTI_CHORUS_DEPTH, x);
    for (x=0; x<=99; x++)
        set_chorusfx_option(MULTI_CHORUS_LEVEL, x);
    set_chorusfx_option(MULTI_CHORUS_WAVE, WAVE_TRI);
    set_chorusfx_option(MULTI_CHORUS_WAVE, WAVE_SINE);
    set_chorusfx_option(MULTI_CHORUS_WAVE, WAVE_SQUARE);

    set_chorusfx_type(CHORUS_TYPE_FLANGER);
    for (x=0; x<=99; x++)
        set_chorusfx_option(FLANGER_SPEED, x);
    for (x=0; x<=99; x++)
        set_chorusfx_option(FLANGER_DEPTH, x);
    for (x=0; x<=99; x++)
        set_chorusfx_option(FLANGER_REGEN, x);
    for (x=0; x<=99; x++)
        set_chorusfx_option(FLANGER_LEVEL, x);
    set_chorusfx_option(FLANGER_WAVE, WAVE_TRI);
    set_chorusfx_option(FLANGER_WAVE, WAVE_SINE);
    set_chorusfx_option(FLANGER_WAVE, WAVE_SQUARE);

    set_chorusfx_type(CHORUS_TYPE_MXR_FLANGER);
    for (x=0; x<=99; x++)
        set_chorusfx_option(MXR_FLANGER_SPEED, x);
    for (x=0; x<=99; x++)
        set_chorusfx_option(MXR_FLANGER_WIDTH, x);
    for (x=0; x<=99; x++)
        set_chorusfx_option(MXR_FLANGER_REGEN, x);
    for (x=0; x<=99; x++)
        set_chorusfx_option(MXR_FLANGER_MANUAL, x);

    set_chorusfx_type(CHORUS_TYPE_PHASER);
    for (x=0; x<=99; x++)
        set_chorusfx_option(PHASER_SPEED, x);
    for (x=0; x<=99; x++)
        set_chorusfx_option(PHASER_DEPTH, x);
    for (x=0; x<=99; x++)
        set_chorusfx_option(PHASER_REGEN, x);
    for (x=0; x<=99; x++)
        set_chorusfx_option(PHASER_LEVEL, x);
    set_chorusfx_option(PHASER_WAVE, WAVE_TRI);
    set_chorusfx_option(PHASER_WAVE, WAVE_SINE);
    set_chorusfx_option(PHASER_WAVE, WAVE_SQUARE);

    set_chorusfx_type(CHORUS_TYPE_VIBRATO);
    for (x=0; x<=99; x++)
        set_chorusfx_option(VIBRATO_SPEED, x);
    for (x=0; x<=99; x++)
        set_chorusfx_option(VIBRATO_DEPTH, x);

    set_chorusfx_type(CHORUS_TYPE_ROTARY);
    for (x=0; x<=99; x++)
        set_chorusfx_option(ROTARY_SPEED, x);
    for (x=0; x<=99; x++)
        set_chorusfx_option(ROTARY_INTENSITY, x);
    for (x=0; x<=99; x++)
        set_chorusfx_option(ROTARY_DOPPLER, x);
    for (x=0; x<=99; x++)
        set_chorusfx_option(ROTARY_CROSSOVER, x);

    set_chorusfx_type(CHORUS_TYPE_VIBROPAN);
    for (x=0; x<=99; x++)
        set_chorusfx_option(VIBROPAN_SPEED, x);
    for (x=0; x<=99; x++)
        set_chorusfx_option(VIBROPAN_DEPTH, x);
    for (x=0; x<=99; x++)
        set_chorusfx_option(VIBROPAN_VIBRA, x);
    set_chorusfx_option(VIBROPAN_WAVE, WAVE_TRI);
    set_chorusfx_option(VIBROPAN_WAVE, WAVE_SINE);
    set_chorusfx_option(VIBROPAN_WAVE, WAVE_SQUARE);

    set_chorusfx_type(CHORUS_TYPE_TREMOLO);
    for (x=0; x<=99; x++)
        set_chorusfx_option(TREMOLO_SPEED, x);
    for (x=0; x<=99; x++)
        set_chorusfx_option(TREMOLO_DEPTH, x);
    set_chorusfx_option(TREMOLO_WAVE, WAVE_TRI);
    set_chorusfx_option(TREMOLO_WAVE, WAVE_SINE);
    set_chorusfx_option(TREMOLO_WAVE, WAVE_SQUARE);

    set_chorusfx_type(CHORUS_TYPE_PANNER);
    for (x=0; x<=99; x++)
        set_chorusfx_option(PANNER_SPEED, x);
    for (x=0; x<=99; x++)
        set_chorusfx_option(PANNER_DEPTH, x);
    set_chorusfx_option(PANNER_WAVE, WAVE_TRI);
    set_chorusfx_option(PANNER_WAVE, WAVE_SINE);
    set_chorusfx_option(PANNER_WAVE, WAVE_SQUARE);

    set_chorusfx_type(CHORUS_TYPE_ENVELOPE);
    for (x=0; x<=99; x++)
        set_chorusfx_option(ENVELOPE_SENSITIVITY, x);
    for (x=0; x<=99; x++)
        set_chorusfx_option(ENVELOPE_RANGE, x);

    set_chorusfx_type(CHORUS_TYPE_AUTOYA);
    for (x=0; x<=99; x++)
        set_chorusfx_option(AUTOYA_SPEED, x);
    for (x=0; x<=99; x++)
        set_chorusfx_option(AUTOYA_INTENSITY, x);
    for (x=0; x<=0x31; x++)
        set_chorusfx_option(AUTOYA_RANGE, x);

    set_chorusfx_type(CHORUS_TYPE_YAYA);
    for (x=0; x<=99; x++)
        set_chorusfx_option(YAYA_PEDAL, x);
    for (x=0; x<=99; x++)
        set_chorusfx_option(YAYA_INTENSITY, x);
    for (x=0; x<=0x31; x++)
        set_chorusfx_option(YAYA_RANGE, x);

    set_chorusfx_type(CHORUS_TYPE_STEP_FILTER);
    for (x=0; x<=99; x++)
        set_chorusfx_option(STEP_FILTER_SPEED, x);
    for (x=0; x<=99; x++)
        set_chorusfx_option(STEP_FILTER_INTENSITY, x);

    set_chorusfx_type(CHORUS_TYPE_WHAMMY);
    set_chorusfx_option(WHAMMY_AMOUNT, WHAMMY_OCT_UP);
    set_chorusfx_option(WHAMMY_AMOUNT, WHAMMY_2OCT_UP);
    set_chorusfx_option(WHAMMY_AMOUNT, WHAMMY_2ND_DN);
    set_chorusfx_option(WHAMMY_AMOUNT, WHAMMY_RV_2ND);
    set_chorusfx_option(WHAMMY_AMOUNT, WHAMMY_4TH_DN);
    set_chorusfx_option(WHAMMY_AMOUNT, WHAMMY_OCT_DN);
    set_chorusfx_option(WHAMMY_AMOUNT, WHAMMY_2OCT_DN);
    set_chorusfx_option(WHAMMY_AMOUNT, WHAMMY_DIV_BMB);
    set_chorusfx_option(WHAMMY_AMOUNT, WHAMMY_M3_MA);
    set_chorusfx_option(WHAMMY_AMOUNT, WHAMMY_2ND_MA3);
    set_chorusfx_option(WHAMMY_AMOUNT, WHAMMY_3RD_4TH);
    set_chorusfx_option(WHAMMY_AMOUNT, WHAMMY_4TH_5TH);
    set_chorusfx_option(WHAMMY_AMOUNT, WHAMMY_5TH_OCT);
    set_chorusfx_option(WHAMMY_AMOUNT, WHAMMY_HOCT_UP);
    set_chorusfx_option(WHAMMY_AMOUNT, WHAMMY_HOCT_DN);
    set_chorusfx_option(WHAMMY_AMOUNT, WHAMMY_OCT_UD);
    for (x=0; x<=99; x++)
        set_chorusfx_option(WHAMMY_PEDAL, x);
    for (x=0; x<=99; x++)
        set_chorusfx_option(WHAMMY_MIX, x);

    set_chorusfx_type(CHORUS_TYPE_PITCH_SHIFT);
    for (x=0; x<=0x30; x++)
        set_chorusfx_option(PITCH_AMOUNT, x);
    for (x=0; x<=99; x++)
        set_chorusfx_option(PITCH_MIX, x);

    set_chorusfx_type(CHORUS_TYPE_DETUNE);
    for (x=0; x<=0x30; x++)
        set_chorusfx_option(DETUNE_AMOUNT, x);
    for (x=0; x<=99; x++)
        set_chorusfx_option(DETUNE_LEVEL, x);

    set_chorusfx_type(CHORUS_TYPE_IPS);
    set_chorusfx_option(IPS_SHIFT_AMOUNT, IPS_OCT_D);
    set_chorusfx_option(IPS_SHIFT_AMOUNT, IPS_7TH_DN);
    set_chorusfx_option(IPS_SHIFT_AMOUNT, IPS_6TH_DN);
    set_chorusfx_option(IPS_SHIFT_AMOUNT, IPS_5TH_DN);
    set_chorusfx_option(IPS_SHIFT_AMOUNT, IPS_4TH_DN);
    set_chorusfx_option(IPS_SHIFT_AMOUNT, IPS_3RD_DN);
    set_chorusfx_option(IPS_SHIFT_AMOUNT, IPS_2ND_DN);
    set_chorusfx_option(IPS_SHIFT_AMOUNT, IPS_2ND_UP);
    set_chorusfx_option(IPS_SHIFT_AMOUNT, IPS_3RD_UP);
    set_chorusfx_option(IPS_SHIFT_AMOUNT, IPS_4TH_UP);
    set_chorusfx_option(IPS_SHIFT_AMOUNT, IPS_5TH_UP);
    set_chorusfx_option(IPS_SHIFT_AMOUNT, IPS_6TH_UP);
    set_chorusfx_option(IPS_SHIFT_AMOUNT, IPS_7TH_UP);
    set_chorusfx_option(IPS_SHIFT_AMOUNT, IPS_OCT_U);
    set_chorusfx_option(IPS_KEY, IPS_E);
    set_chorusfx_option(IPS_KEY, IPS_F);
    set_chorusfx_option(IPS_KEY, IPS_GB);
    set_chorusfx_option(IPS_KEY, IPS_G);
    set_chorusfx_option(IPS_KEY, IPS_AB);
    set_chorusfx_option(IPS_KEY, IPS_A);
    set_chorusfx_option(IPS_KEY, IPS_BB);
    set_chorusfx_option(IPS_KEY, IPS_B);
    set_chorusfx_option(IPS_KEY, IPS_C);
    set_chorusfx_option(IPS_KEY, IPS_DD);
    set_chorusfx_option(IPS_KEY, IPS_D);
    set_chorusfx_option(IPS_KEY, IPS_EB);
    set_chorusfx_option(IPS_SCALE, IPS_MAJOR);
    set_chorusfx_option(IPS_SCALE, IPS_MINOR);
    set_chorusfx_option(IPS_SCALE, IPS_DORIA);
    set_chorusfx_option(IPS_SCALE, IPS_MIXLYD);
    set_chorusfx_option(IPS_SCALE, IPS_LYDIAN);
    set_chorusfx_option(IPS_SCALE, IPS_HMINO);
    for (x=0; x<=99; x++)
        set_chorusfx_option(IPS_LEVEL, x);

    set_chorusfx_on_off(TRUE);
    set_chorusfx_on_off(FALSE);
}

void test_delay()
{
    int x;

    set_delay_type(DELAY_TYPE_ANALOG);
    for (x=0; x<=139; x++)
        set_delay_time(x);
    for (x=0; x<=99; x++)
        set_delay_option(ANALOG_LEVEL, x);
    for (x=0; x<=100; x++)
        set_delay_option(ANALOG_REPEATS, x);

    set_delay_type(DELAY_TYPE_DIGITAL);
    for (x=0; x<=139; x++)
        set_delay_time(x);
    for (x=0; x<=99; x++)
        set_delay_option(DIGITAL_LEVEL, x);
    for (x=0; x<=100; x++)
        set_delay_option(DIGITAL_REPEATS, x);
    for (x=0; x<=99; x++)
        set_delay_option(DIGITAL_DUCKER_THRESH, x);
    for (x=0; x<=99; x++)
        set_delay_option(DIGITAL_DUCKER_LEVEL, x);

    set_delay_type(DELAY_TYPE_MODULATED);
    for (x=0; x<=139; x++)
        set_delay_time(x);
    for (x=0; x<=99; x++)
        set_delay_option(MODULATED_LEVEL, x);
    for (x=0; x<=100; x++)
        set_delay_option(MODULATED_REPEATS, x);
    for (x=0; x<=99; x++)
        set_delay_option(MODULATED_DEPTH, x);

    set_delay_type(DELAY_TYPE_PONG);
    for (x=0; x<=139; x++)
        set_delay_time(x);
    for (x=0; x<=99; x++)
        set_delay_option(PONG_LEVEL, x);
    for (x=0; x<=100; x++)
        set_delay_option(PONG_REPEATS, x);
    for (x=0; x<=99; x++)
        set_delay_option(PONG_DUCKER_THRESH, x);
    for (x=0; x<=99; x++)
        set_delay_option(PONG_DUCKER_LEVEL, x);

    set_delay_type(DELAY_TYPE_TAPE);
    for (x=0; x<=139; x++)
        set_delay_time(x);
    for (x=0; x<=99; x++)
        set_delay_option(TAPE_LEVEL, x);
    for (x=0; x<=100; x++)
        set_delay_option(TAPE_REPEATS, x);
    for (x=0; x<=99; x++)
        set_delay_option(TAPE_WOW, x);
    for (x=0; x<=99; x++)
        set_delay_option(TAPE_FLUTTER, x);

    set_delay_on_off(TRUE);
    set_delay_on_off(FALSE);
}

void test_reverb()
{
    int x;

    set_reverb_type(REVERB_TYPE_TWIN);
    for (x=0; x<=99; x++)
        set_reverb_option(TWIN_REVERB, x);

    set_reverb_type(REVERB_TYPE_LEX_AMBIENCE);
    for (x=0; x<=15; x++)
        set_reverb_option(LEX_AMBIENCE_PREDELAY, x);
    for (x=0; x<=99; x++)
        set_reverb_option(LEX_AMBIENCE_DECAY, x);
    for (x=0; x<=99; x++)
        set_reverb_option(LEX_AMBIENCE_LIVELINESS, x);
    for (x=0; x<=99; x++)
        set_reverb_option(LEX_AMBIENCE_LEVEL, x);

    set_reverb_type(REVERB_TYPE_LEX_STUDIO);
    for (x=0; x<=15; x++)
        set_reverb_option(LEX_STUDIO_PREDELAY, x);
    for (x=0; x<=99; x++)
        set_reverb_option(LEX_STUDIO_DECAY, x);
    for (x=0; x<=99; x++)
        set_reverb_option(LEX_STUDIO_LIVELINESS, x);
    for (x=0; x<=99; x++)
        set_reverb_option(LEX_STUDIO_LEVEL, x);

    set_reverb_type(REVERB_TYPE_LEX_ROOM);
    for (x=0; x<=15; x++)
        set_reverb_option(LEX_ROOM_PREDELAY, x);
    for (x=0; x<=99; x++)
        set_reverb_option(LEX_ROOM_DECAY, x);
    for (x=0; x<=99; x++)
        set_reverb_option(LEX_ROOM_LIVELINESS, x);
    for (x=0; x<=99; x++)
        set_reverb_option(LEX_ROOM_LEVEL, x);

    set_reverb_type(REVERB_TYPE_LEX_HALL);
    for (x=0; x<=15; x++)
        set_reverb_option(LEX_HALL_PREDELAY, x);
    for (x=0; x<=99; x++)
        set_reverb_option(LEX_HALL_DECAY, x);
    for (x=0; x<=99; x++)
        set_reverb_option(LEX_HALL_LIVELINESS, x);
    for (x=0; x<=99; x++)
        set_reverb_option(LEX_HALL_LEVEL, x);

    set_reverb_type(REVERB_TYPE_EMT240_PLATE);
    for (x=0; x<=15; x++)
        set_reverb_option(EMT240_PLATE_PREDELAY, x);
    for (x=0; x<=99; x++)
        set_reverb_option(EMT240_PLATE_DECAY, x);
    for (x=0; x<=99; x++)
        set_reverb_option(EMT240_PLATE_LIVELINESS, x);
    for (x=0; x<=99; x++)
        set_reverb_option(EMT240_PLATE_LEVEL, x);

    set_reverb_on_off(TRUE);
    set_reverb_on_off(FALSE);
}

void test_exp()
{
    int x;

    set_option(EXP_TYPE, EXP_POSITION, EXP_TYPE_NONE);

    set_option(EXP_TYPE, EXP_POSITION, EXP_TYPE_PICKUP_ENABLE);
    for (x=0; x<=1; x++)
        set_option(EXP_MIN, EXP_POSITION, x);
    for (x=0; x<=1; x++)
        set_option(EXP_MAX, EXP_POSITION, x);

    set_option(EXP_TYPE, EXP_POSITION, EXP_TYPE_PIKCUP_TYPE);
    for (x=0; x<=1; x++)
        set_option(EXP_MIN, EXP_POSITION, x);
    for (x=0; x<=1; x++)
        set_option(EXP_MAX, EXP_POSITION, x);

    set_option(EXP_TYPE, EXP_POSITION, EXP_TYPE_COMP_ENABLE);
    for (x=0; x<=1; x++)
        set_option(EXP_MIN, EXP_POSITION, x);
    for (x=0; x<=1; x++)
        set_option(EXP_MAX, EXP_POSITION, x);

    set_option(EXP_TYPE, EXP_POSITION, EXP_TYPE_COMP_SUST);
    for (x=0; x<=99; x++)
        set_option(EXP_MIN, EXP_POSITION, x);
    for (x=0; x<=99; x++)
        set_option(EXP_MAX, EXP_POSITION, x);

    set_option(EXP_TYPE, EXP_POSITION, EXP_TYPE_COMP_TONE);
    for (x=0; x<=99; x++)
        set_option(EXP_MIN, EXP_POSITION, x);
    for (x=0; x<=99; x++)
        set_option(EXP_MAX, EXP_POSITION, x);

    set_option(EXP_TYPE, EXP_POSITION, EXP_TYPE_COMP_LEVEL);
    for (x=0; x<=99; x++)
        set_option(EXP_MIN, EXP_POSITION, x);
    for (x=0; x<=99; x++)
        set_option(EXP_MAX, EXP_POSITION, x);

    set_option(EXP_TYPE, EXP_POSITION, EXP_TYPE_COMP_ATTACK);
    for (x=0; x<=99; x++)
        set_option(EXP_MIN, EXP_POSITION, x);
    for (x=0; x<=99; x++)
        set_option(EXP_MAX, EXP_POSITION, x);

    set_option(EXP_TYPE, EXP_POSITION, EXP_TYPE_DIST_ENABLE);
    for (x=0; x<=1; x++)
        set_option(EXP_MIN, EXP_POSITION, x);
    for (x=0; x<=1; x++)
        set_option(EXP_MAX, EXP_POSITION, x);

    set_option(EXP_TYPE, EXP_POSITION, EXP_TYPE_DIST_DISTORTION);
    for (x=0; x<=99; x++)
        set_option(EXP_MIN, EXP_POSITION, x);
    for (x=0; x<=99; x++)
        set_option(EXP_MAX, EXP_POSITION, x);

    set_option(EXP_TYPE, EXP_POSITION, EXP_TYPE_DIST_FILTER);
    for (x=0; x<=99; x++)
        set_option(EXP_MIN, EXP_POSITION, x);
    for (x=0; x<=99; x++)
        set_option(EXP_MAX, EXP_POSITION, x);

    set_option(EXP_TYPE, EXP_POSITION, EXP_TYPE_DIST_VOLUME);
    for (x=0; x<=99; x++)
        set_option(EXP_MIN, EXP_POSITION, x);
    for (x=0; x<=99; x++)
        set_option(EXP_MAX, EXP_POSITION, x);

    set_option(EXP_TYPE, EXP_POSITION, EXP_TYPE_AMP_ENABLE);
    for (x=0; x<=1; x++)
        set_option(EXP_MIN, EXP_POSITION, x);
    for (x=0; x<=1; x++)
        set_option(EXP_MAX, EXP_POSITION, x);

    set_option(EXP_TYPE, EXP_POSITION, EXP_TYPE_AMP_GAIN);
    for (x=0; x<=99; x++)
        set_option(EXP_MIN, EXP_POSITION, x);
    for (x=0; x<=99; x++)
        set_option(EXP_MAX, EXP_POSITION, x);

    set_option(EXP_TYPE, EXP_POSITION, EXP_TYPE_AMP_LEVEL);
    for (x=0; x<=99; x++)
        set_option(EXP_MIN, EXP_POSITION, x);
    for (x=0; x<=99; x++)
        set_option(EXP_MAX, EXP_POSITION, x);

    set_option(EXP_TYPE, EXP_POSITION, EXP_TYPE_EQ_ENABLE);
    for (x=0; x<=1; x++)
        set_option(EXP_MIN, EXP_POSITION, x);
    for (x=0; x<=1; x++)
        set_option(EXP_MAX, EXP_POSITION, x);

    set_option(EXP_TYPE, EXP_POSITION, EXP_TYPE_EQ_BASS);
    for (x=0; x<=24; x++)
        set_option(EXP_MIN, EXP_POSITION, x);
    for (x=0; x<=24; x++)
        set_option(EXP_MAX, EXP_POSITION, x);

    set_option(EXP_TYPE, EXP_POSITION, EXP_TYPE_EQ_MID);
    for (x=0; x<=24; x++)
        set_option(EXP_MIN, EXP_POSITION, x);
    for (x=0; x<=24; x++)
        set_option(EXP_MAX, EXP_POSITION, x);

    set_option(EXP_TYPE, EXP_POSITION, EXP_TYPE_EQ_TREB);
    for (x=0; x<=24; x++)
        set_option(EXP_MIN, EXP_POSITION, x);
    for (x=0; x<=24; x++)
        set_option(EXP_MAX, EXP_POSITION, x);

    set_option(EXP_TYPE, EXP_POSITION, EXP_TYPE_GATE_ENABLE);
    for (x=0; x<=1; x++)
        set_option(EXP_MIN, EXP_POSITION, x);
    for (x=0; x<=1; x++)
        set_option(EXP_MAX, EXP_POSITION, x);

    set_option(EXP_TYPE, EXP_POSITION, EXP_TYPE_GATE_THRESHOLD);
    for (x=0; x<=99; x++)
        set_option(EXP_MIN, EXP_POSITION, x);
    for (x=0; x<=99; x++)
        set_option(EXP_MAX, EXP_POSITION, x);

    set_option(EXP_TYPE, EXP_POSITION, EXP_TYPE_GATE_ATTACK);
    for (x=0; x<=99; x++)
        set_option(EXP_MIN, EXP_POSITION, x);
    for (x=0; x<=99; x++)
        set_option(EXP_MAX, EXP_POSITION, x);

    set_option(EXP_TYPE, EXP_POSITION, EXP_TYPE_GATE_RELEASE);
    for (x=0; x<=99; x++)
        set_option(EXP_MIN, EXP_POSITION, x);
    for (x=0; x<=99; x++)
        set_option(EXP_MAX, EXP_POSITION, x);

    set_option(EXP_TYPE, EXP_POSITION, EXP_TYPE_GATE_ATTN);
    for (x=0; x<=99; x++)
        set_option(EXP_MIN, EXP_POSITION, x);
    for (x=0; x<=99; x++)
        set_option(EXP_MAX, EXP_POSITION, x);

    set_option(EXP_TYPE, EXP_POSITION, EXP_TYPE_CHORUSFX_ENABLE);
    for (x=0; x<=1; x++)
        set_option(EXP_MIN, EXP_POSITION, x);
    for (x=0; x<=1; x++)
        set_option(EXP_MAX, EXP_POSITION, x);

    set_option(EXP_TYPE, EXP_POSITION, EXP_TYPE_PHASER_SPEED);
    for (x=0; x<=99; x++)
        set_option(EXP_MIN, EXP_POSITION, x);
    for (x=0; x<=99; x++)
        set_option(EXP_MAX, EXP_POSITION, x);

    set_option(EXP_TYPE, EXP_POSITION, EXP_TYPE_PHASER_DEPTH);
    for (x=0; x<=99; x++)
        set_option(EXP_MIN, EXP_POSITION, x);
    for (x=0; x<=99; x++)
        set_option(EXP_MAX, EXP_POSITION, x);

    set_option(EXP_TYPE, EXP_POSITION, EXP_TYPE_PHASER_REGEN);
    for (x=0; x<=99; x++)
        set_option(EXP_MIN, EXP_POSITION, x);
    for (x=0; x<=99; x++)
        set_option(EXP_MAX, EXP_POSITION, x);

    set_option(EXP_TYPE, EXP_POSITION, EXP_TYPE_PHASER_WAVEFORM);
    for (x=0; x<=2; x++)
        set_option(EXP_MIN, EXP_POSITION, x);
    for (x=0; x<=2; x++)
        set_option(EXP_MAX, EXP_POSITION, x);

    set_option(EXP_TYPE, EXP_POSITION, EXP_TYPE_PHASER_LEVEL);
    for (x=0; x<=99; x++)
        set_option(EXP_MIN, EXP_POSITION, x);
    for (x=0; x<=99; x++)
        set_option(EXP_MAX, EXP_POSITION, x);

    set_option(EXP_TYPE, EXP_POSITION, EXP_TYPE_DELAY_ENABLE);
    for (x=0; x<=1; x++)
        set_option(EXP_MIN, EXP_POSITION, x);
    for (x=0; x<=1; x++)
        set_option(EXP_MAX, EXP_POSITION, x);

    set_option(EXP_TYPE, EXP_POSITION, EXP_TYPE_DELAY_TIME);
    for (x=0; x<=139; x++)
        set_option(EXP_MIN, EXP_POSITION, x);
    for (x=0; x<=139; x++)
        set_option(EXP_MAX, EXP_POSITION, x);

    set_option(EXP_TYPE, EXP_POSITION, EXP_TYPE_DELAY_REPEATS);
    for (x=0; x<=100; x++)
        set_option(EXP_MIN, EXP_POSITION, x);
    for (x=0; x<=100; x++)
        set_option(EXP_MAX, EXP_POSITION, x);

    set_option(EXP_TYPE, EXP_POSITION, EXP_TYPE_DELAY_LEVEL);
    for (x=0; x<=99; x++)
        set_option(EXP_MIN, EXP_POSITION, x);
    for (x=0; x<=99; x++)
        set_option(EXP_MAX, EXP_POSITION, x);

    set_option(EXP_TYPE, EXP_POSITION, EXP_TYPE_DELAY_DUCK_THRESH);
    for (x=0; x<=99; x++)
        set_option(EXP_MIN, EXP_POSITION, x);
    for (x=0; x<=99; x++)
        set_option(EXP_MAX, EXP_POSITION, x);

    set_option(EXP_TYPE, EXP_POSITION, EXP_TYPE_DELAY_DUCK_LEVEL);
    for (x=0; x<=99; x++)
        set_option(EXP_MIN, EXP_POSITION, x);
    for (x=0; x<=99; x++)
        set_option(EXP_MAX, EXP_POSITION, x);

    set_option(EXP_TYPE, EXP_POSITION, EXP_TYPE_REVERB_ENABLE);
    for (x=0; x<=1; x++)
        set_option(EXP_MIN, EXP_POSITION, x);
    for (x=0; x<=1; x++)
        set_option(EXP_MAX, EXP_POSITION, x);

    set_option(EXP_TYPE, EXP_POSITION, EXP_TYPE_REVERB_LEVEL);
    for (x=0; x<=99; x++)
        set_option(EXP_MIN, EXP_POSITION, x);
    for (x=0; x<=99; x++)
        set_option(EXP_MAX, EXP_POSITION, x);

    set_option(EXP_TYPE, EXP_POSITION, EXP_TYPE_VOLUME_PRE_FX);
    for (x=0; x<=99; x++)
        set_option(EXP_MIN, EXP_POSITION, x);
    for (x=0; x<=99; x++)
        set_option(EXP_MAX, EXP_POSITION, x);

    set_option(EXP_TYPE, EXP_POSITION, EXP_TYPE_VOLUME_POST_FX);
    for (x=0; x<=99; x++)
        set_option(EXP_MIN, EXP_POSITION, x);
    for (x=0; x<=99; x++)
        set_option(EXP_MAX, EXP_POSITION, x);
}

void test_all()
{
    test_wah();
    test_compressor();
    test_dist();
    test_presets();
    test_pickups();
    test_eq();
    test_noisegate();
    test_chorusfx();
    test_delay();
    test_reverb();
    test_exp();
}
