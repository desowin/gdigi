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

    set_option(WAH_TYPE, WAH_POSITION, WAH_TYPE_CRY);
    for (x=0; x<=99; x++)
        set_option(WAH_MIN, WAH_POSITION_MIN_MAX, x);

    for (x=0; x<=99; x++)
        set_option(WAH_MAX, WAH_POSITION_MIN_MAX, x);

    for (x=0; x<=12; x++)
        set_option(WAH_LEVEL, WAH_POSITION, x);

    set_option(WAH_TYPE, WAH_POSITION, WAH_TYPE_FULLRANGE);
    for (x=0; x<=99; x++)
        set_option(WAH_MIN, WAH_POSITION_MIN_MAX, x);

    for (x=0; x<=99; x++)
        set_option(WAH_MAX, WAH_POSITION_MIN_MAX, x);

    for (x=0; x<=12; x++)
        set_option(WAH_LEVEL, WAH_POSITION, x);

    set_option(WAH_TYPE, WAH_POSITION, WAH_TYPE_CLYDE);
    for (x=0; x<=99; x++)
        set_option(WAH_MIN, WAH_POSITION_MIN_MAX, x);

    for (x=0; x<=99; x++)
        set_option(WAH_MAX, WAH_POSITION_MIN_MAX, x);

    for (x=0; x<=12; x++)
        set_option(WAH_LEVEL, WAH_POSITION, x);

    set_option(WAH_ON_OFF, WAH_POSITION, 1);
    set_option(WAH_ON_OFF, WAH_POSITION, 0);
}

void test_compressor()
{
    int x;

    set_option(COMP_TYPE, COMP_POSITION, COMP_TYPE_DIGI);
    for (x=0; x<=99; x++)
        set_option(COMP_SUSTAIN, COMP_POSITION, x);

    for (x=0; x<=99; x++)
        set_option(COMP_TONE, COMP_POSITION, x);

    for (x=0; x<=99; x++)
        set_option(COMP_ATTACK, COMP_POSITION, x);

    for (x=0; x<=99; x++)
        set_option(COMP_LEVEL, COMP_POSITION, x);

    set_option(COMP_TYPE, COMP_POSITION, COMP_TYPE_CS);
    for (x=0; x<=99; x++)
        set_option(COMP_SUSTAIN, COMP_POSITION, x);

    for (x=0; x<=99; x++)
        set_option(COMP_ATTACK, COMP_POSITION, x);

    for (x=0; x<=99; x++)
        set_option(COMP_LEVEL, COMP_POSITION, x);

    set_option(COMP_ON_OFF, COMP_POSITION, 1);
    set_option(COMP_ON_OFF, COMP_POSITION, 0);
}

void test_dist()
{
    int x;

    set_option(DIST_TYPE, DIST_POSITION, DIST_TYPE_SCREAMER);
    for (x=0; x<=99; x++)
        set_option(DIST_SCREAMER_DRIVE, DIST_POSITION, x);
    for (x=0; x<=99; x++)
        set_option(DIST_SCREAMER_TONE, DIST_POSITION, x);
    for (x=0; x<=99; x++)
        set_option(DIST_SCREAMER_LVL, DIST_POSITION, x);

    set_option(DIST_TYPE, DIST_POSITION, DIST_TYPE_808);
    for (x=0; x<=99; x++)
        set_option(DIST_808_OVERDRIVE, DIST_POSITION, x);
    for (x=0; x<=99; x++)
        set_option(DIST_808_TONE, DIST_POSITION, x);
    for (x=0; x<=99; x++)
        set_option(DIST_808_LVL, DIST_POSITION, x);

    set_option(DIST_TYPE, DIST_POSITION, DIST_TYPE_GUYOD);
    for (x=0; x<=99; x++)
        set_option(DIST_GUYOD_DRIVE, DIST_POSITION, x);
    for (x=0; x<=99; x++)
        set_option(DIST_GUYOD_LVL, DIST_POSITION, x);

    set_option(DIST_TYPE, DIST_POSITION, DIST_TYPE_DOD250);
    for (x=0; x<=99; x++)
        set_option(DIST_DOD250_GAIN, DIST_POSITION, x);
    for (x=0; x<=99; x++)
        set_option(DIST_DOD250_LVL, DIST_POSITION, x);

    set_option(DIST_TYPE, DIST_POSITION, DIST_TYPE_RODENT);
    for (x=0; x<=99; x++)
        set_option(DIST_RODENT_DIST, DIST_POSITION, x);
    for (x=0; x<=99; x++)
        set_option(DIST_RODENT_FILTER, DIST_POSITION, x);
    for (x=0; x<=99; x++)
        set_option(DIST_RODENT_LVL, DIST_POSITION, x);

    set_option(DIST_TYPE, DIST_POSITION, DIST_TYPE_MX);
    for (x=0; x<=99; x++)
        set_option(DIST_MX_DIST, DIST_POSITION, x);
    for (x=0; x<=99; x++)
        set_option(DIST_MX_OUTPUT, DIST_POSITION, x);

    set_option(DIST_TYPE, DIST_POSITION, DIST_TYPE_DS);
    for (x=0; x<=99; x++)
        set_option(DIST_DS_GAIN, DIST_POSITION, x);
    for (x=0; x<=99; x++)
        set_option(DIST_DS_TONE, DIST_POSITION, x);
    for (x=0; x<=99; x++)
        set_option(DIST_DS_LVL, DIST_POSITION, x);

    set_option(DIST_TYPE, DIST_POSITION, DIST_TYPE_GRUNGE);
    for (x=0; x<=99; x++)
        set_option(DIST_GRUNGE_GRUNGE, DIST_POSITION, x);
    for (x=0; x<=99; x++)
        set_option(DIST_GRUNGE_FACE, DIST_POSITION, x);
    for (x=0; x<=99; x++)
        set_option(DIST_GRUNGE_LOUD, DIST_POSITION, x);
    for (x=0; x<=99; x++)
        set_option(DIST_GRUNGE_BUTT, DIST_POSITION, x);

    set_option(DIST_TYPE, DIST_POSITION, DIST_TYPE_ZONE);
    for (x=0; x<=99; x++)
        set_option(DIST_ZONE_GAIN, DIST_POSITION, x);
    for (x=0; x<=99; x++)
        set_option(DIST_ZONE_LOW, DIST_POSITION, x);
    for (x=0; x<=99; x++)
        set_option(DIST_ZONE_MID_LVL, DIST_POSITION, x);
    for (x=0; x<=99; x++)
        set_option(DIST_ZONE_MID_FREQ, DIST_POSITION, x);
    for (x=0; x<=99; x++)
        set_option(DIST_ZONE_HIGH, DIST_POSITION, x);
    for (x=0; x<=99; x++)
        set_option(DIST_ZONE_LEVEL, DIST_POSITION, x);

    set_option(DIST_TYPE, DIST_POSITION, DIST_TYPE_DEATH);
    for (x=0; x<=99; x++)
        set_option(DIST_DEATH_LOW, DIST_POSITION, x);
    for (x=0; x<=99; x++)
        set_option(DIST_DEATH_MID, DIST_POSITION, x);
    for (x=0; x<=99; x++)
        set_option(DIST_DEATH_HIGH, DIST_POSITION, x);
    for (x=0; x<=99; x++)
        set_option(DIST_DEATH_LVL, DIST_POSITION, x);

    set_option(DIST_TYPE, DIST_POSITION, DIST_TYPE_GONK);
    for (x=0; x<=99; x++)
        set_option(DIST_GONK_GONK, DIST_POSITION, x);
    for (x=0; x<=99; x++)
        set_option(DIST_GONK_SMEAR, DIST_POSITION, x);
    for (x=0; x<=99; x++)
        set_option(DIST_GONK_SUCK, DIST_POSITION, x);
    for (x=0; x<=99; x++)
        set_option(DIST_GONK_HEAVE, DIST_POSITION, x);

    set_option(DIST_TYPE, DIST_POSITION, DIST_TYPE_FUZZY);
    for (x=0; x<=99; x++)
        set_option(DIST_FUZZY_FUZZ, DIST_POSITION, x);
    for (x=0; x<=99; x++)
        set_option(DIST_FUZZY_VOLUME, DIST_POSITION, x);

    set_option(DIST_TYPE, DIST_POSITION, DIST_TYPE_MP);
    for (x=0; x<=99; x++)
        set_option(DIST_MP_SUSTAIN, DIST_POSITION, x);
    for (x=0; x<=99; x++)
        set_option(DIST_MP_TONE, DIST_POSITION, x);
    for (x=0; x<=99; x++)
        set_option(DIST_MP_VOLUME, DIST_POSITION, x);

    set_option(DIST_ON_OFF, DIST_POSITION, 1);
    set_option(DIST_ON_OFF, DIST_POSITION, 0);
}

void test_presets()
{
    int x;

    for (x=0; x<=60; x++)
        switch_preset(PRESETS_USER, x);

    for (x=0; x<=60; x++)
        switch_preset(PRESETS_SYSTEM, x);

    for (x=0; x<=99; x++)
        set_preset_level(x);
}

void test_pickups()
{
    set_option(PICKUP_TYPE, PICKUP_POSITION, PICKUP_TYPE_HB_SC);
    set_option(PICKUP_TYPE, PICKUP_POSITION, PICKUP_TYPE_SC_HB);

    set_option(PICKUP_ON_OFF, PICKUP_POSITION, 1);
    set_option(PICKUP_ON_OFF, PICKUP_POSITION, 0);
}

void test_eq()
{
    int x;

    set_option(EQ_TYPE, EQ_POSITION, EQ_TYPE_BRIGHT);
    set_option(EQ_TYPE, EQ_POSITION, EQ_TYPE_MIDBOOST);
    set_option(EQ_TYPE, EQ_POSITION, EQ_TYPE_SCOOP);
    set_option(EQ_TYPE, EQ_POSITION, EQ_TYPE_WARM);

    for (x=0; x<=99; x++)
        set_option(AMP_GAIN, AMP_POSITION, x);
    for (x=0; x<=99; x++)
        set_option(AMP_LEVEL, AMP_POSITION, x);

    for (x=0; x<=0x18; x++)
        set_option(EQ_BASS, EQ_POSITION, x);
    for (x=0; x<=0x18; x++)
        set_option(EQ_MID, EQ_POSITION, x);
    for (x=0; x<=4700; x++)
        set_option(EQ_MID_HZ, EQ_POSITION, x);
    for (x=0; x<=0x18; x++)
        set_option(EQ_TREBLE, EQ_POSITION, x);
    for (x=0; x<=7500; x++)
        set_option(EQ_TREBLE_HZ, EQ_POSITION, x);

    set_option(EQ_ON_OFF, EQ_POSITION, 1);
    set_option(EQ_ON_OFF, EQ_POSITION, 0);
}

void test_noisegate()
{
    int x;

    set_option(NOISEGATE_TYPE, NOISEGATE_POSITION, NOISEGATE_GATE);
    for (x=0; x<=99; x++)
        set_option(NOISEGATE_GATE_TRESHOLD, NOISEGATE_POSITION, x);
    for (x=0; x<=99; x++)
        set_option(NOISEGATE_ATTACK, NOISEGATE_POSITION, x);
    for (x=0; x<=99; x++)
        set_option(NOISEGATE_RELEASE, NOISEGATE_POSITION, x);
    for (x=0; x<=99; x++)
        set_option(NOISEGATE_ATTN, NOISEGATE_POSITION, x);

    set_option(NOISEGATE_TYPE, NOISEGATE_POSITION, NOISEGATE_SWELL);
    for (x=0; x<=99; x++)
        set_option(NOISEGATE_SWELL_SENS, NOISEGATE_POSITION, x);
    for (x=0; x<=99; x++)
        set_option(NOISEGATE_ATTACK, NOISEGATE_POSITION, x);
    for (x=0; x<=99; x++)
        set_option(NOISEGATE_RELEASE, NOISEGATE_POSITION, x);
    for (x=0; x<=99; x++)
        set_option(NOISEGATE_ATTN, NOISEGATE_POSITION, x);

    set_option(NOISEGATE_ON_OFF, NOISEGATE_POSITION, 1);
    set_option(NOISEGATE_ON_OFF, NOISEGATE_POSITION, 0);
}

void test_chorusfx()
{
    int x;

    set_option(CHORUSFX_TYPE, CHORUSFX_POSITION, CHORUS_TYPE_CE);
    for (x=0; x<=99; x++)
        set_option(CE_CHORUS_SPEED, CHORUSFX_POSITION, x);
    for (x=0; x<=99; x++)
        set_option(CE_CHORUS_DEPTH, CHORUSFX_POSITION, x);

    set_option(CHORUSFX_TYPE, CHORUSFX_POSITION, CHORUS_TYPE_DUAL);
    for (x=0; x<=99; x++)
        set_option(DUAL_CHORUS_SPEED, CHORUSFX_POSITION, x);
    for (x=0; x<=99; x++)
        set_option(DUAL_CHORUS_DEPTH, CHORUSFX_POSITION, x);
    for (x=0; x<=99; x++)
        set_option(DUAL_CHORUS_LEVEL, CHORUSFX_POSITION, x);
    set_option(DUAL_CHORUS_WAVE, CHORUSFX_POSITION, WAVE_TRI);
    set_option(DUAL_CHORUS_WAVE, CHORUSFX_POSITION, WAVE_SINE);
    set_option(DUAL_CHORUS_WAVE, CHORUSFX_POSITION, WAVE_SQUARE);

    set_option(CHORUSFX_TYPE, CHORUSFX_POSITION, CHORUS_TYPE_MULTI);
    for (x=0; x<=99; x++)
        set_option(MULTI_CHORUS_SPEED, CHORUSFX_POSITION, x);
    for (x=0; x<=99; x++)
        set_option(MULTI_CHORUS_DEPTH, CHORUSFX_POSITION, x);
    for (x=0; x<=99; x++)
        set_option(MULTI_CHORUS_LEVEL, CHORUSFX_POSITION, x);
    set_option(MULTI_CHORUS_WAVE, CHORUSFX_POSITION, WAVE_TRI);
    set_option(MULTI_CHORUS_WAVE, CHORUSFX_POSITION, WAVE_SINE);
    set_option(MULTI_CHORUS_WAVE, CHORUSFX_POSITION, WAVE_SQUARE);

    set_option(CHORUSFX_TYPE, CHORUSFX_POSITION, CHORUS_TYPE_FLANGER);
    for (x=0; x<=99; x++)
        set_option(FLANGER_SPEED, CHORUSFX_POSITION, x);
    for (x=0; x<=99; x++)
        set_option(FLANGER_DEPTH, CHORUSFX_POSITION, x);
    for (x=0; x<=99; x++)
        set_option(FLANGER_REGEN, CHORUSFX_POSITION, x);
    for (x=0; x<=99; x++)
        set_option(FLANGER_LEVEL, CHORUSFX_POSITION, x);
    set_option(FLANGER_WAVE, CHORUSFX_POSITION, WAVE_TRI);
    set_option(FLANGER_WAVE, CHORUSFX_POSITION, WAVE_SINE);
    set_option(FLANGER_WAVE, CHORUSFX_POSITION, WAVE_SQUARE);

    set_option(CHORUSFX_TYPE, CHORUSFX_POSITION, CHORUS_TYPE_MXR_FLANGER);
    for (x=0; x<=99; x++)
        set_option(MXR_FLANGER_SPEED, CHORUSFX_POSITION, x);
    for (x=0; x<=99; x++)
        set_option(MXR_FLANGER_WIDTH, CHORUSFX_POSITION, x);
    for (x=0; x<=99; x++)
        set_option(MXR_FLANGER_REGEN, CHORUSFX_POSITION, x);
    for (x=0; x<=99; x++)
        set_option(MXR_FLANGER_MANUAL, CHORUSFX_POSITION, x);

    set_option(CHORUSFX_TYPE, CHORUSFX_POSITION, CHORUS_TYPE_PHASER);
    for (x=0; x<=99; x++)
        set_option(PHASER_SPEED, CHORUSFX_POSITION, x);
    for (x=0; x<=99; x++)
        set_option(PHASER_DEPTH, CHORUSFX_POSITION, x);
    for (x=0; x<=99; x++)
        set_option(PHASER_REGEN, CHORUSFX_POSITION, x);
    for (x=0; x<=99; x++)
        set_option(PHASER_LEVEL, CHORUSFX_POSITION, x);
    set_option(PHASER_WAVE, CHORUSFX_POSITION, WAVE_TRI);
    set_option(PHASER_WAVE, CHORUSFX_POSITION, WAVE_SINE);
    set_option(PHASER_WAVE, CHORUSFX_POSITION, WAVE_SQUARE);

    set_option(CHORUSFX_TYPE, CHORUSFX_POSITION, CHORUS_TYPE_VIBRATO);
    for (x=0; x<=99; x++)
        set_option(VIBRATO_SPEED, CHORUSFX_POSITION, x);
    for (x=0; x<=99; x++)
        set_option(VIBRATO_DEPTH, CHORUSFX_POSITION, x);

    set_option(CHORUSFX_TYPE, CHORUSFX_POSITION, CHORUS_TYPE_ROTARY);
    for (x=0; x<=99; x++)
        set_option(ROTARY_SPEED, CHORUSFX_POSITION, x);
    for (x=0; x<=99; x++)
        set_option(ROTARY_INTENSITY, CHORUSFX_POSITION, x);
    for (x=0; x<=99; x++)
        set_option(ROTARY_DOPPLER, CHORUSFX_POSITION, x);
    for (x=0; x<=99; x++)
        set_option(ROTARY_CROSSOVER, CHORUSFX_POSITION, x);

    set_option(CHORUSFX_TYPE, CHORUSFX_POSITION, CHORUS_TYPE_VIBROPAN);
    for (x=0; x<=99; x++)
        set_option(VIBROPAN_SPEED, CHORUSFX_POSITION, x);
    for (x=0; x<=99; x++)
        set_option(VIBROPAN_DEPTH, CHORUSFX_POSITION, x);
    for (x=0; x<=99; x++)
        set_option(VIBROPAN_VIBRA, CHORUSFX_POSITION, x);
    set_option(VIBROPAN_WAVE, CHORUSFX_POSITION, WAVE_TRI);
    set_option(VIBROPAN_WAVE, CHORUSFX_POSITION, WAVE_SINE);
    set_option(VIBROPAN_WAVE, CHORUSFX_POSITION, WAVE_SQUARE);

    set_option(CHORUSFX_TYPE, CHORUSFX_POSITION, CHORUS_TYPE_TREMOLO);
    for (x=0; x<=99; x++)
        set_option(TREMOLO_SPEED, CHORUSFX_POSITION, x);
    for (x=0; x<=99; x++)
        set_option(TREMOLO_DEPTH, CHORUSFX_POSITION, x);
    set_option(TREMOLO_WAVE, CHORUSFX_POSITION, WAVE_TRI);
    set_option(TREMOLO_WAVE, CHORUSFX_POSITION, WAVE_SINE);
    set_option(TREMOLO_WAVE, CHORUSFX_POSITION, WAVE_SQUARE);

    set_option(CHORUSFX_TYPE, CHORUSFX_POSITION, CHORUS_TYPE_PANNER);
    for (x=0; x<=99; x++)
        set_option(PANNER_SPEED, CHORUSFX_POSITION, x);
    for (x=0; x<=99; x++)
        set_option(PANNER_DEPTH, CHORUSFX_POSITION, x);
    set_option(PANNER_WAVE, CHORUSFX_POSITION, WAVE_TRI);
    set_option(PANNER_WAVE, CHORUSFX_POSITION, WAVE_SINE);
    set_option(PANNER_WAVE, CHORUSFX_POSITION, WAVE_SQUARE);

    set_option(CHORUSFX_TYPE, CHORUSFX_POSITION, CHORUS_TYPE_ENVELOPE);
    for (x=0; x<=99; x++)
        set_option(ENVELOPE_SENSITIVITY, CHORUSFX_POSITION, x);
    for (x=0; x<=99; x++)
        set_option(ENVELOPE_RANGE, CHORUSFX_POSITION, x);

    set_option(CHORUSFX_TYPE, CHORUSFX_POSITION, CHORUS_TYPE_AUTOYA);
    for (x=0; x<=99; x++)
        set_option(AUTOYA_SPEED, CHORUSFX_POSITION, x);
    for (x=0; x<=99; x++)
        set_option(AUTOYA_INTENSITY, CHORUSFX_POSITION, x);
    for (x=0; x<=0x31; x++)
        set_option(AUTOYA_RANGE, CHORUSFX_POSITION, x);

    set_option(CHORUSFX_TYPE, CHORUSFX_POSITION, CHORUS_TYPE_YAYA);
    for (x=0; x<=99; x++)
        set_option(YAYA_PEDAL, CHORUSFX_POSITION, x);
    for (x=0; x<=99; x++)
        set_option(YAYA_INTENSITY, CHORUSFX_POSITION, x);
    for (x=0; x<=0x31; x++)
        set_option(YAYA_RANGE, CHORUSFX_POSITION, x);

    set_option(CHORUSFX_TYPE, CHORUSFX_POSITION, CHORUS_TYPE_STEP_FILTER);
    for (x=0; x<=99; x++)
        set_option(STEP_FILTER_SPEED, CHORUSFX_POSITION, x);
    for (x=0; x<=99; x++)
        set_option(STEP_FILTER_INTENSITY, CHORUSFX_POSITION, x);

    set_option(CHORUSFX_TYPE, CHORUSFX_POSITION, CHORUS_TYPE_WHAMMY);
    set_option(WHAMMY_AMOUNT, CHORUSFX_POSITION, WHAMMY_OCT_UP);
    set_option(WHAMMY_AMOUNT, CHORUSFX_POSITION, WHAMMY_2OCT_UP);
    set_option(WHAMMY_AMOUNT, CHORUSFX_POSITION, WHAMMY_2ND_DN);
    set_option(WHAMMY_AMOUNT, CHORUSFX_POSITION, WHAMMY_RV_2ND);
    set_option(WHAMMY_AMOUNT, CHORUSFX_POSITION, WHAMMY_4TH_DN);
    set_option(WHAMMY_AMOUNT, CHORUSFX_POSITION, WHAMMY_OCT_DN);
    set_option(WHAMMY_AMOUNT, CHORUSFX_POSITION, WHAMMY_2OCT_DN);
    set_option(WHAMMY_AMOUNT, CHORUSFX_POSITION, WHAMMY_DIV_BMB);
    set_option(WHAMMY_AMOUNT, CHORUSFX_POSITION, WHAMMY_M3_MA);
    set_option(WHAMMY_AMOUNT, CHORUSFX_POSITION, WHAMMY_2ND_MA3);
    set_option(WHAMMY_AMOUNT, CHORUSFX_POSITION, WHAMMY_3RD_4TH);
    set_option(WHAMMY_AMOUNT, CHORUSFX_POSITION, WHAMMY_4TH_5TH);
    set_option(WHAMMY_AMOUNT, CHORUSFX_POSITION, WHAMMY_5TH_OCT);
    set_option(WHAMMY_AMOUNT, CHORUSFX_POSITION, WHAMMY_HOCT_UP);
    set_option(WHAMMY_AMOUNT, CHORUSFX_POSITION, WHAMMY_HOCT_DN);
    set_option(WHAMMY_AMOUNT, CHORUSFX_POSITION, WHAMMY_OCT_UD);
    for (x=0; x<=99; x++)
        set_option(WHAMMY_PEDAL, CHORUSFX_POSITION, x);
    for (x=0; x<=99; x++)
        set_option(WHAMMY_MIX, CHORUSFX_POSITION, x);

    set_option(CHORUSFX_TYPE, CHORUSFX_POSITION, CHORUS_TYPE_PITCH_SHIFT);
    for (x=0; x<=0x30; x++)
        set_option(PITCH_AMOUNT, CHORUSFX_POSITION, x);
    for (x=0; x<=99; x++)
        set_option(PITCH_MIX, CHORUSFX_POSITION, x);

    set_option(CHORUSFX_TYPE, CHORUSFX_POSITION, CHORUS_TYPE_DETUNE);
    for (x=0; x<=0x30; x++)
        set_option(DETUNE_AMOUNT, CHORUSFX_POSITION, x);
    for (x=0; x<=99; x++)
        set_option(DETUNE_LEVEL, CHORUSFX_POSITION, x);

    set_option(CHORUSFX_TYPE, CHORUSFX_POSITION, CHORUS_TYPE_IPS);
    set_option(IPS_SHIFT_AMOUNT, CHORUSFX_POSITION, IPS_OCT_D);
    set_option(IPS_SHIFT_AMOUNT, CHORUSFX_POSITION, IPS_7TH_DN);
    set_option(IPS_SHIFT_AMOUNT, CHORUSFX_POSITION, IPS_6TH_DN);
    set_option(IPS_SHIFT_AMOUNT, CHORUSFX_POSITION, IPS_5TH_DN);
    set_option(IPS_SHIFT_AMOUNT, CHORUSFX_POSITION, IPS_4TH_DN);
    set_option(IPS_SHIFT_AMOUNT, CHORUSFX_POSITION, IPS_3RD_DN);
    set_option(IPS_SHIFT_AMOUNT, CHORUSFX_POSITION, IPS_2ND_DN);
    set_option(IPS_SHIFT_AMOUNT, CHORUSFX_POSITION, IPS_2ND_UP);
    set_option(IPS_SHIFT_AMOUNT, CHORUSFX_POSITION, IPS_3RD_UP);
    set_option(IPS_SHIFT_AMOUNT, CHORUSFX_POSITION, IPS_4TH_UP);
    set_option(IPS_SHIFT_AMOUNT, CHORUSFX_POSITION, IPS_5TH_UP);
    set_option(IPS_SHIFT_AMOUNT, CHORUSFX_POSITION, IPS_6TH_UP);
    set_option(IPS_SHIFT_AMOUNT, CHORUSFX_POSITION, IPS_7TH_UP);
    set_option(IPS_SHIFT_AMOUNT, CHORUSFX_POSITION, IPS_OCT_U);
    set_option(IPS_KEY, CHORUSFX_POSITION, IPS_E);
    set_option(IPS_KEY, CHORUSFX_POSITION, IPS_F);
    set_option(IPS_KEY, CHORUSFX_POSITION, IPS_GB);
    set_option(IPS_KEY, CHORUSFX_POSITION, IPS_G);
    set_option(IPS_KEY, CHORUSFX_POSITION, IPS_AB);
    set_option(IPS_KEY, CHORUSFX_POSITION, IPS_A);
    set_option(IPS_KEY, CHORUSFX_POSITION, IPS_BB);
    set_option(IPS_KEY, CHORUSFX_POSITION, IPS_B);
    set_option(IPS_KEY, CHORUSFX_POSITION, IPS_C);
    set_option(IPS_KEY, CHORUSFX_POSITION, IPS_DB);
    set_option(IPS_KEY, CHORUSFX_POSITION, IPS_D);
    set_option(IPS_KEY, CHORUSFX_POSITION, IPS_EB);
    set_option(IPS_SCALE, CHORUSFX_POSITION, IPS_MAJOR);
    set_option(IPS_SCALE, CHORUSFX_POSITION, IPS_MINOR);
    set_option(IPS_SCALE, CHORUSFX_POSITION, IPS_DORIA);
    set_option(IPS_SCALE, CHORUSFX_POSITION, IPS_MIXLYD);
    set_option(IPS_SCALE, CHORUSFX_POSITION, IPS_LYDIAN);
    set_option(IPS_SCALE, CHORUSFX_POSITION, IPS_HMINO);
    for (x=0; x<=99; x++)
        set_option(IPS_LEVEL, CHORUSFX_POSITION, x);

    set_option(CHORUSFX_ON_OFF, CHORUSFX_POSITION, 1);
    set_option(CHORUSFX_ON_OFF, CHORUSFX_POSITION, 0);
}

void test_delay()
{
    int x;

    set_option(DELAY_TYPE, DELAY_POSITION, DELAY_TYPE_ANALOG);
    for (x=0; x<=139; x++)
        set_option(DELAY_TIME, DELAY_POSITION, x);
    for (x=0; x<=99; x++)
        set_option(ANALOG_LEVEL, DELAY_POSITION, x);
    for (x=0; x<=100; x++)
        set_option(ANALOG_REPEATS, DELAY_POSITION, x);

    set_option(DELAY_TYPE, DELAY_POSITION, DELAY_TYPE_DIGITAL);
    for (x=0; x<=139; x++)
        set_option(DELAY_TIME, DELAY_POSITION, x);
    for (x=0; x<=99; x++)
        set_option(DIGITAL_LEVEL, DELAY_POSITION, x);
    for (x=0; x<=100; x++)
        set_option(DIGITAL_REPEATS, DELAY_POSITION, x);
    for (x=0; x<=99; x++)
        set_option(DIGITAL_DUCKER_THRESH, DELAY_POSITION, x);
    for (x=0; x<=99; x++)
        set_option(DIGITAL_DUCKER_LEVEL, DELAY_POSITION, x);

    set_option(DELAY_TYPE, DELAY_POSITION, DELAY_TYPE_MODULATED);
    for (x=0; x<=139; x++)
        set_option(DELAY_TIME, DELAY_POSITION, x);
    for (x=0; x<=99; x++)
        set_option(MODULATED_LEVEL, DELAY_POSITION, x);
    for (x=0; x<=100; x++)
        set_option(MODULATED_REPEATS, DELAY_POSITION, x);
    for (x=0; x<=99; x++)
        set_option(MODULATED_DEPTH, DELAY_POSITION, x);

    set_option(DELAY_TYPE, DELAY_POSITION, DELAY_TYPE_PONG);
    for (x=0; x<=139; x++)
        set_option(DELAY_TIME, DELAY_POSITION, x);
    for (x=0; x<=99; x++)
        set_option(PONG_LEVEL, DELAY_POSITION, x);
    for (x=0; x<=100; x++)
        set_option(PONG_REPEATS, DELAY_POSITION, x);
    for (x=0; x<=99; x++)
        set_option(PONG_DUCKER_THRESH, DELAY_POSITION, x);
    for (x=0; x<=99; x++)
        set_option(PONG_DUCKER_LEVEL, DELAY_POSITION, x);

    set_option(DELAY_TYPE, DELAY_POSITION, DELAY_TYPE_TAPE);
    for (x=0; x<=139; x++)
        set_option(DELAY_TIME, DELAY_POSITION, x);
    for (x=0; x<=99; x++)
        set_option(TAPE_LEVEL, DELAY_POSITION, x);
    for (x=0; x<=100; x++)
        set_option(TAPE_REPEATS, DELAY_POSITION, x);
    for (x=0; x<=99; x++)
        set_option(TAPE_WOW, DELAY_POSITION, x);
    for (x=0; x<=99; x++)
        set_option(TAPE_FLUTTER, DELAY_POSITION, x);

    set_option(DELAY_ON_OFF, DELAY_POSITION, 1);
    set_option(DELAY_ON_OFF, DELAY_POSITION, 0);
}

void test_reverb()
{
    int x;

    set_option(REVERB_TYPE, REVERB_POSITION, REVERB_TYPE_TWIN);
    for (x=0; x<=99; x++)
        set_option(TWIN_REVERB, REVERB_POSITION, x);

    set_option(REVERB_TYPE, REVERB_POSITION, REVERB_TYPE_LEX_AMBIENCE);
    for (x=0; x<=15; x++)
        set_option(LEX_AMBIENCE_PREDELAY, REVERB_POSITION, x);
    for (x=0; x<=99; x++)
        set_option(LEX_AMBIENCE_DECAY, REVERB_POSITION, x);
    for (x=0; x<=99; x++)
        set_option(LEX_AMBIENCE_LIVELINESS, REVERB_POSITION, x);
    for (x=0; x<=99; x++)
        set_option(LEX_AMBIENCE_LEVEL, REVERB_POSITION, x);

    set_option(REVERB_TYPE, REVERB_POSITION, REVERB_TYPE_LEX_STUDIO);
    for (x=0; x<=15; x++)
        set_option(LEX_STUDIO_PREDELAY, REVERB_POSITION, x);
    for (x=0; x<=99; x++)
        set_option(LEX_STUDIO_DECAY, REVERB_POSITION, x);
    for (x=0; x<=99; x++)
        set_option(LEX_STUDIO_LIVELINESS, REVERB_POSITION, x);
    for (x=0; x<=99; x++)
        set_option(LEX_STUDIO_LEVEL, REVERB_POSITION, x);

    set_option(REVERB_TYPE, REVERB_POSITION, REVERB_TYPE_LEX_ROOM);
    for (x=0; x<=15; x++)
        set_option(LEX_ROOM_PREDELAY, REVERB_POSITION, x);
    for (x=0; x<=99; x++)
        set_option(LEX_ROOM_DECAY, REVERB_POSITION, x);
    for (x=0; x<=99; x++)
        set_option(LEX_ROOM_LIVELINESS, REVERB_POSITION, x);
    for (x=0; x<=99; x++)
        set_option(LEX_ROOM_LEVEL, REVERB_POSITION, x);

    set_option(REVERB_TYPE, REVERB_POSITION, REVERB_TYPE_LEX_HALL);
    for (x=0; x<=15; x++)
        set_option(LEX_HALL_PREDELAY, REVERB_POSITION, x);
    for (x=0; x<=99; x++)
        set_option(LEX_HALL_DECAY, REVERB_POSITION, x);
    for (x=0; x<=99; x++)
        set_option(LEX_HALL_LIVELINESS, REVERB_POSITION, x);
    for (x=0; x<=99; x++)
        set_option(LEX_HALL_LEVEL, REVERB_POSITION, x);

    set_option(REVERB_TYPE, REVERB_POSITION, REVERB_TYPE_EMT240_PLATE);
    for (x=0; x<=15; x++)
        set_option(EMT240_PLATE_PREDELAY, REVERB_POSITION, x);
    for (x=0; x<=99; x++)
        set_option(EMT240_PLATE_DECAY, REVERB_POSITION, x);
    for (x=0; x<=99; x++)
        set_option(EMT240_PLATE_LIVELINESS, REVERB_POSITION, x);
    for (x=0; x<=99; x++)
        set_option(EMT240_PLATE_LEVEL, REVERB_POSITION, x);

    set_option(REVERB_ON_OFF, REVERB_POSITION, 1);
    set_option(REVERB_ON_OFF, REVERB_POSITION, 0);
}

void test_usb()
{
    int x;

    for (x=0; x<=100; x++)
        set_option(USB_AUDIO_PLAYBACK_MIX, USB_POSITION, x);

    for (x=0; x<=36; x++)
        set_option(USB_AUDIO_LEVEL, USB_POSITION, x);
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
    test_usb();
}
