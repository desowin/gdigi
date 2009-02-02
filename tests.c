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

void test_wah(struct usb_dev_handle *handle)
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

    set_wah_on_off(handle, TRUE);
    set_wah_on_off(handle, FALSE);
}

void test_compressor(struct usb_dev_handle *handle)
{
    int x;

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

    set_comp_on_off(handle, TRUE);
    set_comp_on_off(handle, FALSE);
}

void test_dist(struct usb_dev_handle *handle)
{
    int x;

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

    set_dist_on_off(handle, TRUE);
    set_dist_on_off(handle, FALSE);
}

void test_presets(struct usb_dev_handle *handle)
{
    int x;

    for (x=0; x<=60; x++)
        switch_user_preset(handle, x);

    for (x=0; x<=60; x++)
        switch_system_preset(handle, x);

    for (x=0; x<=99; x++)
        set_preset_level(handle, x);
}

void test_pickups(struct usb_dev_handle *handle)
{
    set_pickup_type(handle, PICKUP_TYPE_HB_SC);
    set_pickup_type(handle, PICKUP_TYPE_SC_HB);

    set_pickup_on_off(handle, TRUE);
    set_pickup_on_off(handle, FALSE);
}

void test_eq(struct usb_dev_handle *handle)
{
    int x;

    set_eq_type(handle, EQ_TYPE_BRIGHT);
    set_eq_type(handle, EQ_TYPE_MIDBOOST);
    set_eq_type(handle, EQ_TYPE_SCOOP);
    set_eq_type(handle, EQ_TYPE_WARM);

    for (x=0; x<=99; x++)
        set_eq_gain(handle, x);
    for (x=0; x<=99; x++)
        set_eq_level(handle, x);

    for (x=0; x<=0x18; x++)
        set_eq_bass(handle, x);
    for (x=0; x<=0x18; x++)
        set_eq_mid(handle, x);
    for (x=0; x<=4700; x++)
        set_eq_mid_hz(handle, x);
    for (x=0; x<=0x18; x++)
        set_eq_treble(handle, x);
    for (x=0; x<=7500; x++)
        set_eq_treb_hz(handle, x);

    set_eq_on_off(handle, TRUE);
    set_eq_on_off(handle, FALSE);
}

void test_noisegate(struct usb_dev_handle *handle)
{
    int x;

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
}

void test_chorusfx(struct usb_dev_handle *handle)
{
    int x;

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
}

void test_delay(struct usb_dev_handle *handle)
{
    int x;

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
}

void test_reverb(struct usb_dev_handle *handle)
{
    int x;

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

void test_all(struct usb_dev_handle *handle)
{
    test_wah(handle);
    test_compressor(handle);
    test_dist(handle);
    test_presets(handle);
    test_pickups(handle);
    test_eq(handle);
    test_noisegate(handle);
    test_chorusfx(handle);
    test_delay(handle);
    test_reverb(handle);
}
