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

#ifndef DOXYGEN_SHOULD_SKIP_THIS

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

static gchar *odd_even_labels[] = {
    "Odd",
    "Even",
};

static gchar *chorus_vibrato_labels[] = {
    "Chorus",
    "Vibrato",
};

static gchar *loose_tight_labels[] = {
    "Loose",
    "Tight",
};

static gchar *pickup_type_labels[] = {
    "SC>HB",
    "HB>SC",
};

static gchar *fast_medium_slow_labels[] = {
    "Fast",
    "Medium",
    "Slow",
    NULL,
};

static gchar *comp_ratio_labels[] = {
    "1.2:1",
    "1.5:1",
    "1.8:1",
    "2.0:1",
    "2.5:1",
    "3:1",
    "4:1",
    "5:1",
    "8:1",
    "10:1",
    "20:1",
    "Inf:1",
};

static gchar *eq_bandwidth_labels[] = {
    "Wide",
    "Medium",
    "Narrow",
};

static gchar *eq_low_freq_labels[] = {
    "60Hz",
    "63Hz",
    "67Hz",
    "71Hz",
    "75Hz",
    "80Hz",
    "85Hz",
    "90Hz",
    "95Hz",
    "100Hz",
    "106Hz",
    "112Hz",
    "118Hz",
    "125Hz",
    "132Hz",
    "140Hz",
    "150Hz",
    "160Hz",
    "170Hz",
    "180Hz",
    "190Hz",
    "200Hz",
    "212Hz",
    "224Hz",
    "236Hz",
    "250Hz",
    "265Hz",
    "280Hz",
    "300Hz",
    "315Hz",
    "335Hz",
    "355Hz",
    "375Hz",
    "400Hz",
    "425Hz",
    "450Hz",
    "475Hz",
    "500Hz",
};

static gchar *eq_mid_freq_labels[] = {
    "300Hz",
    "315Hz",
    "335Hz",
    "355Hz",
    "375Hz",
    "400Hz",
    "425Hz",
    "450Hz",
    "475Hz",
    "500Hz",
    "530Hz",
    "560Hz",
    "600Hz",
    "630Hz",
    "670Hz",
    "710Hz",
    "750Hz",
    "800Hz",
    "850Hz",
    "900Hz",
    "950Hz",
    "1kHz",
    "1.06kHz",
    "1.12kHz",
    "1.18kHz",
    "1.25kHz",
    "1.32kHz",
    "1.4kHz",
    "1.5kHz",
    "1.6kHz",
    "1.7kHz",
    "1.8kHz",
    "1.9kHz",
    "2kHz",
    "2.12kHz",
    "2.24kHz",
    "2.36kHz",
    "2.5kHz",
    "2.65kHz",
    "2.8kHz",
    "3kHz",
    "3.15kHz",
    "3.35kHz",
    "3.55kHz",
    "3.75kHz",
    "4kHz",
};

static gchar *eq_high_freq_labels[] = {
    "2kHz",
    "2.12kHz",
    "2.24kHz",
    "2.36kHz",
    "2.5kHz",
    "2.65kHz",
    "2.8kHz",
    "3kHz",
    "3.15kHz",
    "3.35kHz",
    "3.55kHz",
    "3.75kHz",
    "4kHz",
    "4.25kHz",
    "4.5kHz",
    "4.75kHz",
    "5kHz",
    "5.3kHz",
    "5.6kHz",
    "6kHz",
    "6.3kHz",
    "6.7kHz",
    "7.1kHz",
    "7.5kHz",
    "8kHz",
};

static gchar *amp_channel_labels[] = {
    "1",
    "2",
    "Warped",
};

static EffectValues values_1_2_warp = {
    0.0, 2.0, amp_channel_labels,
};

static EffectValues values_0_to_99 = {
    0.0, 99.0, NULL,
};

static EffectValues values_1_to_4 = {
    0.0, 3.0, NULL,
};

static EffectValues values_warp_0_to_98 = {
    /** \todo make this display properly (display text is value + 1) */
    0.0, 98.0, NULL,
};

static EffectValues values_0_to_9 = {
    0.0, 9.0, NULL,
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

static EffectValues values_0_to_20 = {
    0.0, 20.0, NULL,
};

static EffectValues values_m12_to_12_semitones = {
    /** \todo make this display properly (display range -12.0 to 12.0 semitones) */
    0.0, 48.0, NULL,
};

static EffectValues values_m24_to_24_step2 = {
    /** \todo make this display properly (display range -24 to 24) */
    0.0, 24.0, NULL,
};
static EffectValues values_m24_to_24 = {
    /** \todo make this display properly (display range -24 to 24) */
    0.0, 48.0, NULL,
};

static EffectValues values_m12_to_24 = {
    /** \todo make this display properly (display range -12 to 24) */
    0.0, 36.0, NULL,
};

static EffectValues values_comp_ratio = {
    0.0, 12.0, comp_ratio_labels,
};

static EffectValues values_fast_medium_slow = {
    0.0, 2.0, fast_medium_slow_labels,
};

static EffectValues values_eq_db = {
    /** \todo make those display properly (display range -12dB to 12dB) */
    0.0, 24.0, NULL,
};

static EffectValues values_eq_bass_hz = {
    /** \todo make those display properly (display range 50hz to 300hz) */
    0.0, 250.0, NULL,
};

static EffectValues values_eq_mid_hz = {
    /** \todo make this display properly (display range 300 to 5000) */
    0.0, 4700.0, NULL,
};

static EffectValues values_eq_treb_hz = {
    /** \todo make this display properly (display range 500 to 8000) */
    0.0, 7500.0, NULL,
};

static EffectValues values_eq_bandwidth = {
    0.0, 2.0, eq_bandwidth_labels,
};

static EffectValues values_eq_low_freq = {
    0.0, 37.0, eq_low_freq_labels,
};

static EffectValues values_eq_mid_freq = {
    0.0, 45.0, eq_mid_freq_labels,
};

static EffectValues values_eq_high_freq = {
    0.0, 24.0, eq_high_freq_labels,
};

static EffectValues values_200hz_to_1_5khz = {
    /** \todo make this display properly (display range 200 Hz to 1.5 kHz) */
    0.0, 130.0, NULL,
};

static EffectValues values_waveform = {
    0.0, 2.0, wave_labels,
};

static EffectValues values_balance = {
    /** \todo make this display properly */
    0.0, 198.0, NULL,
};

static EffectValues values_synth_talk_release = {
    /** \todo make last value display properly */
    0.0, 100.0, NULL,
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

static EffectValues values_predelay_time = {
    /** \todo make this display properly (1ms to 20ms) */
    0.0, 19.0, NULL,
};

static EffectValues values_delay_time = {
    /** \todo make this display properly (10 msec to 5 sec) */
    0.0, 139.0, NULL,
};

static EffectValues values_delay_time_0_760 = {
    /** \todo make this display properly */
    0.0, 760.0, NULL,
};

static EffectValues values_delay_time_0_2000 = {
    /** \todo make this display properly */
    0.0, 2000.0, NULL,
};

static EffectValues values_delay_time_0_4650 = {
    /** \todo make this display properly */
    0.0, 4650.0, NULL,
};

static EffectValues values_delay_time_0_4990 = {
    /** \todo make this display properly */
    0.0, 4990.0, NULL,
};

static EffectValues values_delay_time_0_5000 = {
    /** \todo make this display properly */
    0.0, 5000.0, NULL,
};

static EffectValues values_delay_repeats = {
    /** \todo make last value display properly */
    0.0, 100.0, NULL,
};

static EffectValues values_delay_thresh = {
    /** \todo make last value display properly */
    0.0, 100.0, NULL,
};

static EffectValues values_delay_repeat_rate_0_286 = {
    /** \todo make this display properly */
    0.0, 286.0, NULL,
};

static EffectValues values_delay_spread_0_49 = {
    /** \todo make this display properly */
    0.0, 49.0, NULL,
};

static EffectValues values_on_off = {
    0.0, 1.0, on_off_labels,
};

static EffectValues values_odd_even = {
    0.0, 1.0, odd_even_labels,
};

static EffectValues values_chorus_vibrato = {
    0.0, 1.0, chorus_vibrato_labels,
};

static EffectValues values_loose_tight = {
    0.0, 1.0, loose_tight_labels,
};

static EffectValues values_pickup_type = {
    0.0, 1.0, pickup_type_labels,
};

static EffectValues values_1_to_10_step_0p1 = {
    /** \todo make this display properly (1.0, 1.1, ...., 10.0) */
    0.0, 90.0, NULL,
};

static EffectValues values_rp_mix = {
    /** \todo make this display properly (USB 0, USB 1, USB 2 ... USB 49, USB RP, RP 49, RP 48 ...  RP 1, RP 0) */
    0.0, 100.0, NULL,
};

static EffectSettings usb_settings[] = {
    {"USB/RP Mix", USB_AUDIO_PLAYBACK_MIX, USB_POSITION, &values_rp_mix},
    {"USB Level", USB_AUDIO_LEVEL, USB_POSITION, &values_m12_to_24},
    {"Preset Level", PRESET_LEVEL, PRESET_POSITION, &values_0_to_99},
};

static EffectSettings gnx3k_wah_settings[] = {
    {"Min", WAH_MIN, WAH_POSITION_MIN_MAX, &values_0_to_99},
    {"Max", WAH_MAX, WAH_POSITION_MIN_MAX, &values_0_to_99},
};

static EffectSettings wah_settings[] = {
    {"Min", WAH_MIN, WAH_POSITION_MIN_MAX, &values_0_to_99},
    {"Max", WAH_MAX, WAH_POSITION_MIN_MAX, &values_0_to_99},
    {"Level", WAH_LEVEL, WAH_POSITION, &values_0_to_12},
};

static EffectSettings gnx3k_whammy_settings[] = {
    {"Shift Amount", WHAMMY_AMOUNT, GNX3K_WHAM_POSITION, &values_whammy_amount},
    {"Pedal", WHAMMY_PEDAL, GNX3K_WHAM_POSITION, &values_0_to_99},
    {"Mix", WHAMMY_MIX, GNX3K_WHAM_POSITION, &values_0_to_99},
};

static EffectSettings gnx3k_ips_settings[] = {
    {"Shift Amount", IPS_SHIFT_AMOUNT, GNX3K_WHAM_POSITION, &values_ips_shift},
    {"Scale", IPS_SCALE, GNX3K_WHAM_POSITION, &values_ips_scale},
    {"Key", IPS_KEY, GNX3K_WHAM_POSITION, &values_ips_key},
    {"Level", IPS_LEVEL, GNX3K_WHAM_POSITION, &values_0_to_99},
};

static EffectSettings gnx3k_detune_settings[] = {
    {"Shift Amount", GNX3K_DETUNE_AMOUNT, GNX3K_WHAM_POSITION, &values_m24_to_24_step2},
    {"Level", DETUNE_LEVEL, GNX3K_WHAM_POSITION, &values_0_to_99},
};

static EffectSettings gnx3k_pitch_settings[] = {
    {"Shift Amount", GNX3K_PITCH_AMOUNT, GNX3K_WHAM_POSITION, &values_m24_to_24},
    {"Level", GNX3K_PITCH_LEVEL, GNX3K_WHAM_POSITION, &values_0_to_99},
};

static EffectSettings gnx3k_talk_settings[] = {
    {"Mic Level", GNX3K_TALK_LEVEL, GNX3K_WHAM_POSITION, &values_0_to_99},
};

static EffectSettings gnx3k_amp_channel_settings[] = {
    {"Amp Channel", GNX_CHANNEL_AMP, GNX_CHANNEL_POSITION, &values_1_2_warp},
    {"Warp", GNX_WARP, GNX_CHANNEL_POSITION, &values_warp_0_to_98},
    {"Amp Warp", GNX_AMP_WARP, GNX_CHANNEL_POSITION, &values_warp_0_to_98},
    {"Cabinet Warp", GNX_CABINET_WARP, GNX_CHANNEL_POSITION, &values_warp_0_to_98},
};

static EffectSettings gnx3k_preset_settings[] = {
    {"Preset Level", PRESET_LEVEL, PRESET_POSITION, &values_0_to_99},
};

static EffectSettings gnx_comp_settings[] = {
    {"Attack", GNX3K_COMP_ATTACK, COMP_POSITION, &values_fast_medium_slow},
    {"Ratio", GNX3K_COMP_RATIO, COMP_POSITION, &values_comp_ratio},
    {"Threshold", GNX3K_COMP_THRESHOLD, COMP_POSITION, &values_0_to_99},
    {"Gain", GNX3K_COMP_GAIN, COMP_POSITION, &values_0_to_20},
};

static EffectSettings comp_digi_settings[] = {
    {"Sustain", COMP_SUSTAIN, COMP_POSITION, &values_0_to_99},
    {"Tone", COMP_TONE, COMP_POSITION, &values_0_to_99},
    {"Attack", COMP_ATTACK, COMP_POSITION, &values_0_to_99},
    {"Level", COMP_LEVEL, COMP_POSITION, &values_0_to_99},
};

static EffectSettings comp_cs_settings[] = {
    {"Sustain", COMP_SUSTAIN, COMP_POSITION, &values_0_to_99},
    {"Attack", COMP_ATTACK, COMP_POSITION, &values_0_to_99},
    {"Level", COMP_LEVEL, COMP_POSITION, &values_0_to_99},
};

static EffectSettings comp_dyna_settings[] = {
    {"Sensitivity", COMP_SENSITIVITY, COMP_POSITION, &values_0_to_99},
    {"Output", COMP_OUTPUT, COMP_POSITION, &values_0_to_99},
};

static EffectSettings dist_screamer_settings[] = {
    {"Drive", DIST_SCREAMER_DRIVE, DIST_POSITION, &values_0_to_99},
    {"Tone", DIST_SCREAMER_TONE, DIST_POSITION, &values_0_to_99},
    {"Level", DIST_SCREAMER_LVL, DIST_POSITION, &values_0_to_99},
};

static EffectSettings dist_808_settings[] = {
    {"Overdrive", DIST_808_OVERDRIVE, DIST_POSITION, &values_0_to_99},
    {"Tone", DIST_808_TONE, DIST_POSITION, &values_0_to_99},
    {"Level", DIST_808_LVL, DIST_POSITION, &values_0_to_99},
};

static EffectSettings dist_sparkdrive_settings[] = {
    {"Gain", DIST_SPARKDRIVE_GAIN, DIST_POSITION, &values_0_to_99},
    {"Tone", DIST_SPARKDRIVE_TONE, DIST_POSITION, &values_0_to_99},
    {"Clean", DIST_SPARKDRIVE_CLEAN, DIST_POSITION, &values_0_to_99},
    {"Volume", DIST_SPARKDRIVE_VOLUME, DIST_POSITION, &values_0_to_99},
};

static EffectSettings dist_guyod_settings[] = {
    {"Drive", DIST_GUYOD_DRIVE, DIST_POSITION, &values_0_to_99},
    {"Level", DIST_GUYOD_LVL, DIST_POSITION, &values_0_to_99},
};

static EffectSettings dist_dod250_settings[] = {
    {"Gain", DIST_DOD250_GAIN, DIST_POSITION, &values_0_to_99},
    {"Level", DIST_DOD250_LVL, DIST_POSITION, &values_0_to_99},
};

static EffectSettings dist_redline_settings[] = {
    {"Gain", DIST_REDLINE_GAIN, DIST_POSITION, &values_0_to_99},
    {"Low", DIST_REDLINE_LOW, DIST_POSITION, &values_0_to_99},
    {"High", DIST_REDLINE_HIGH, DIST_POSITION, &values_0_to_99},
    {"Level", DIST_REDLINE_LEVEL, DIST_POSITION, &values_0_to_99},
};

static EffectSettings dist_rodent_settings[] = {
    {"Dist", DIST_RODENT_DIST, DIST_POSITION, &values_0_to_99},
    {"Filter", DIST_RODENT_FILTER, DIST_POSITION, &values_0_to_99},
    {"Level", DIST_RODENT_LVL, DIST_POSITION, &values_0_to_99},
};

static EffectSettings dist_mx_settings[] = {
    {"Dist", DIST_MX_DIST, DIST_POSITION, &values_0_to_99},
    {"Output", DIST_MX_OUTPUT, DIST_POSITION, &values_0_to_99},
};

static EffectSettings dist_ds_settings[] = {
    {"Gain", DIST_DS_GAIN, DIST_POSITION, &values_0_to_99},
    {"Tone", DIST_DS_TONE, DIST_POSITION, &values_0_to_99},
    {"Level", DIST_DS_LVL, DIST_POSITION, &values_0_to_99},
};

static EffectSettings dist_grunge_settings[] = {
    {"Grunge", DIST_GRUNGE_GRUNGE, DIST_POSITION, &values_0_to_99},
    {"Face", DIST_GRUNGE_FACE, DIST_POSITION, &values_0_to_99},
    {"Loud", DIST_GRUNGE_LOUD, DIST_POSITION, &values_0_to_99},
    {"Butt", DIST_GRUNGE_BUTT, DIST_POSITION, &values_0_to_99},
};

static EffectSettings dist_zone_settings[] = {
    {"Gain", DIST_ZONE_GAIN, DIST_POSITION, &values_0_to_99},
    {"Low", DIST_ZONE_LOW, DIST_POSITION, &values_0_to_99},
    {"Mid level", DIST_ZONE_MID_LVL, DIST_POSITION, &values_0_to_99},
    {"Mid freq", DIST_ZONE_MID_FREQ, DIST_POSITION, &values_0_to_99},
    {"High", DIST_ZONE_HIGH, DIST_POSITION, &values_0_to_99},
    {"Level", DIST_ZONE_LEVEL, DIST_POSITION, &values_0_to_99},
};

static EffectSettings dist_death_settings[] = {
    {"Low", DIST_DEATH_LOW, DIST_POSITION, &values_0_to_99},
    {"Mid", DIST_DEATH_MID, DIST_POSITION, &values_0_to_99},
    {"High", DIST_DEATH_HIGH, DIST_POSITION, &values_0_to_99},
    {"Level", DIST_DEATH_LVL, DIST_POSITION, &values_0_to_99},
};

static EffectSettings dist_gonk_settings[] = {
    {"Gonk", DIST_GONK_GONK, DIST_POSITION, &values_0_to_99},
    {"Smear", DIST_GONK_SMEAR, DIST_POSITION, &values_0_to_99},
    {"Suck", DIST_GONK_SUCK, DIST_POSITION, &values_0_to_99},
    {"Heave", DIST_GONK_HEAVE, DIST_POSITION, &values_0_to_99},
};

static EffectSettings dist_8tavia_settings[] = {
    {"Drive", DIST_8TAVIA_DRIVE, DIST_POSITION, &values_0_to_99},
    {"Volume", DIST_8TAVIA_VOLUME, DIST_POSITION, &values_0_to_99},
};

static EffectSettings dist_fuzzlator_settings[] = {
    {"Fuzz", DIST_FUZZLATOR_FUZZ, DIST_POSITION, &values_0_to_99},
    {"Tone", DIST_FUZZLATOR_TONE, DIST_POSITION, &values_0_to_99},
    {"LooseTight", DIST_FUZZLATOR_LOOSETIGHT, DIST_POSITION, &values_loose_tight},
    {"Volume", DIST_FUZZLATOR_VOLUME, DIST_POSITION, &values_0_to_99},
};

static EffectSettings dist_classic_fuzz_settings[] = {
    {"Fuzz", DIST_CLASSIC_FUZZ_FUZZ, DIST_POSITION, &values_0_to_99},
    {"Tone", DIST_CLASSIC_FUZZ_TONE, DIST_POSITION, &values_0_to_99},
    {"Volume", DIST_CLASSIC_FUZZ_VOLUME, DIST_POSITION, &values_0_to_99},
};

static EffectSettings dist_fuzzy_settings[] = {
    {"Fuzz", DIST_FUZZY_FUZZ, DIST_POSITION, &values_0_to_99},
    {"Volume", DIST_FUZZY_VOLUME, DIST_POSITION, &values_0_to_99},
};

static EffectSettings dist_mp_settings[] = {
    {"Sustain", DIST_MP_SUSTAIN, DIST_POSITION, &values_0_to_99},
    {"Tone", DIST_MP_TONE, DIST_POSITION, &values_0_to_99},
    {"Volume", DIST_MP_VOLUME, DIST_POSITION, &values_0_to_99},
};

static EffectSettings rp250_amp_settings[] = {
    {"Gain", AMP_GAIN, AMP_POSITION, &values_0_to_99},
    {"Level", AMP_LEVEL, AMP_POSITION, &values_0_to_99},
};

static EffectSettings rp250_amp_settings2[] = {
    {"Level", AMP_LEVEL, AMP_POSITION, &values_0_to_99},
};

static EffectSettings rp355_amp_settings_A[] = {
    {"Gain", AMP_GAIN, AMP_POSITION, &values_0_to_99},
    {"Level", AMP_LEVEL, AMP_POSITION, &values_0_to_99},
};

static EffectSettings rp355_amp_settings_B[] = {
    {"Gain", AMP_GAIN, CH2_AMP_POSITION, &values_0_to_99},
    {"Level", AMP_LEVEL, CH2_AMP_POSITION, &values_0_to_99},
};

static EffectSettings rp355_amp_settings2_A[] = {
    {"Level", AMP_LEVEL, AMP_POSITION, &values_0_to_99},
};

static EffectSettings rp355_amp_settings2_B[] = {
    {"Level", AMP_LEVEL, CH2_AMP_POSITION, &values_0_to_99},
};

static EffectSettings rp500_amp_settings[] = {
    {"Gain", AMP_GAIN, AMP_POSITION, &values_0_to_99},
    {"Level", AMP_LEVEL, AMP_POSITION, &values_0_to_99},
    {"Bass", AMP_BASS, AMP_POSITION, &values_1_to_10_step_0p1},
    {"Mid", AMP_MID, AMP_POSITION, &values_1_to_10_step_0p1},
    {"Treble", AMP_TREBLE, AMP_POSITION, &values_1_to_10_step_0p1},
};

static EffectSettings rp500_amp_settings2[] = {
    {"Level", AMP_LEVEL, AMP_POSITION, &values_0_to_99},
    {"Bass", AMP_BASS, AMP_POSITION, &values_1_to_10_step_0p1},
    {"Mid", AMP_MID, AMP_POSITION, &values_1_to_10_step_0p1},
    {"Treble", AMP_TREBLE, AMP_POSITION, &values_1_to_10_step_0p1},
};

static EffectSettings rp500_amp_settings3[] = {
    {"Level", AMP_LEVEL, AMP_POSITION, &values_0_to_99},
};

static EffectSettings gnx3k_ch1_cab_tuning_settings[] = {
    {"Tuning", AMP_CAB_TUNING, AMP_CAB_POSITION, &values_m12_to_12_semitones},
};

static EffectSettings gnx3k_ch2_cab_tuning_settings[] = {
    {"Tuning", AMP_CAB_TUNING, CH2_AMP_CAB_POSITION, &values_m12_to_12_semitones},
};

static EffectSettings gnx3k_ch1_amp_eq_settings[] = {
    {"Gain", AMP_GAIN, AMP_POSITION, &values_0_to_99},
    {"Bass Freq", AMP_BASS_FREQ, AMP_POSITION, &values_eq_bass_hz},
    {"Bass Level", AMP_BASS_LEVEL, AMP_POSITION, &values_eq_db},
    {"Mid Freq", AMP_MID_FREQ, AMP_POSITION, &values_eq_mid_hz},
    {"Mid Level", AMP_MID_LEVEL, AMP_POSITION, &values_eq_db},
    {"Treb Freq", AMP_TREBLE_FREQ, AMP_POSITION, &values_eq_treb_hz},
    {"Treb Level", AMP_TREBLE_LEVEL, AMP_POSITION, &values_eq_db},
    {"Presence", AMP_PRESENCE, AMP_POSITION, &values_eq_db},
    {"Level", AMP_LEVEL, AMP_POSITION, &values_0_to_99},
};

static EffectSettings gnx3k_ch2_amp_eq_settings[] = {
    {"Gain", AMP_GAIN, CH2_AMP_POSITION, &values_0_to_99},
    {"Bass Freq", AMP_BASS_FREQ, CH2_AMP_POSITION, &values_eq_bass_hz},
    {"Bass Level", AMP_BASS_LEVEL, CH2_AMP_POSITION, &values_eq_db},
    {"Mid Freq", AMP_MID_FREQ, CH2_AMP_POSITION, &values_eq_mid_hz},
    {"Mid Level", AMP_MID_LEVEL, CH2_AMP_POSITION, &values_eq_db},
    {"Treb Freq", AMP_TREBLE_FREQ, CH2_AMP_POSITION, &values_eq_treb_hz},
    {"Treb Level", AMP_TREBLE_LEVEL, CH2_AMP_POSITION, &values_eq_db},
    {"Presence", AMP_PRESENCE, CH2_AMP_POSITION, &values_eq_db},
    {"Level", AMP_LEVEL, CH2_AMP_POSITION, &values_0_to_99},
};

static EffectSettings rp250_eq_settings[] = {
    {"Bass", EQ_BASS, EQ_POSITION, &values_eq_db},
    {"Mid", EQ_MID, EQ_POSITION, &values_eq_db},
    {"Treble", EQ_TREBLE, EQ_POSITION, &values_eq_db},
    {"Mid Hz", EQ_MID_HZ, EQ_POSITION, &values_eq_mid_hz},
    {"Treb Hz", EQ_TREBLE_HZ, EQ_POSITION, &values_eq_treb_hz},
};

static EffectSettings rp355_eq_settings_A[] = {
    {"Bass", EQ_BASS, EQ_POSITION, &values_eq_db},
    {"Mid", EQ_MID, EQ_POSITION, &values_eq_db},
    {"Treble", EQ_TREBLE, EQ_POSITION, &values_eq_db},
    {"Mid Hz", EQ_MID_HZ, EQ_POSITION, &values_eq_mid_hz},
    {"Treb Hz", EQ_TREBLE_HZ, EQ_POSITION, &values_eq_treb_hz},
    {"Presence", EQ_PRESENCE, EQ_POSITION, &values_eq_db},
};

static EffectSettings rp355_eq_settings_B[] = {
    {"Bass", EQ_BASS, EQ_POSITION_B, &values_eq_db},
    {"Mid", EQ_MID, EQ_POSITION_B, &values_eq_db},
    {"Treble", EQ_TREBLE, EQ_POSITION_B, &values_eq_db},
    {"Mid Hz", EQ_MID_HZ, EQ_POSITION_B, &values_eq_mid_hz},
    {"Treb Hz", EQ_TREBLE_HZ, EQ_POSITION_B, &values_eq_treb_hz},
    {"Presence", EQ_PRESENCE, EQ_POSITION_B, &values_eq_db},
};

static EffectSettings rp500_eq_settings[] = {
    {"Low Level", EQ_LOW_LEVEL, EQ_POSITION, &values_eq_db},
    {"Mid Level", EQ_MID_LEVEL, EQ_POSITION, &values_eq_db},
    {"High Level", EQ_HIGH_LEVEL, EQ_POSITION, &values_eq_db},
    {"Low Freq", EQ_LOW_FREQ, EQ_POSITION, &values_eq_low_freq},
    {"Mid Freq", EQ_MID_FREQ, EQ_POSITION, &values_eq_mid_freq},
    {"High Freq", EQ_HIGH_FREQ, EQ_POSITION, &values_eq_high_freq},
    {"Low Bandwidth", EQ_LOW_BANDWIDTH, EQ_POSITION, &values_eq_bandwidth},
    {"Mid Bandwidth", EQ_MID_BANDWIDTH, EQ_POSITION, &values_eq_bandwidth},
    {"High Bandwidth", EQ_HIGH_BANDWIDTH, EQ_POSITION, &values_eq_bandwidth},
};

static EffectSettings noisegate_silencer_settings[] = {
    {"Threshold", NOISEGATE_GATE_TRESHOLD, NOISEGATE_POSITION, &values_0_to_99},
    {"Attack", GNX3K_GATE_ATTACK, NOISEGATE_POSITION, &values_0_to_9},
};

static EffectSettings noisegate_pluck_settings[] = {
    {"Threshold", NOISEGATE_GATE_TRESHOLD, NOISEGATE_POSITION, &values_0_to_99},
    {"Attack", GNX3K_GATE_ATTACK, NOISEGATE_POSITION, &values_0_to_9},
    {"Sens", NOISEGATE_SWELL_SENS, NOISEGATE_POSITION, &values_0_to_99},
};

static EffectSettings noisegate_gate_settings[] = {
    {"Treshold", NOISEGATE_GATE_TRESHOLD, NOISEGATE_POSITION, &values_0_to_99},
    {"Attack", NOISEGATE_ATTACK, NOISEGATE_POSITION, &values_0_to_99},
    {"Release", NOISEGATE_RELEASE, NOISEGATE_POSITION, &values_0_to_99},
    {"Attn", NOISEGATE_ATTN, NOISEGATE_POSITION, &values_0_to_99},
};

static EffectSettings noisegate_swell_settings[] = {
    {"Sens", NOISEGATE_SWELL_SENS, NOISEGATE_POSITION, &values_0_to_99},
    {"Attack", NOISEGATE_ATTACK, NOISEGATE_POSITION, &values_0_to_99},
    {"Release", NOISEGATE_RELEASE, NOISEGATE_POSITION, &values_0_to_99},
    {"Attn", NOISEGATE_ATTN, NOISEGATE_POSITION, &values_0_to_99},
};

static EffectSettings gnx3k_chorus_settings[] = {
    {"Speed", CHORUS_SPEED, CHORUSFX_POSITION, &values_0_to_99},
    {"Depth", CHORUS_DEPTH, CHORUSFX_POSITION, &values_0_to_99},
    {"Predelay", CHORUS_PREDELAY, CHORUSFX_POSITION, &values_predelay_time},
    {"Wave", CHORUS_WAVE, CHORUSFX_POSITION, &values_waveform},
    {"Balance", CHORUS_BALANCE, CHORUSFX_POSITION, &values_balance},
    {"Level", CHORUS_LEVEL, CHORUSFX_POSITION, &values_0_to_99},
};

static EffectSettings gnx3k_flanger_settings[] = {
    {"Speed", FLANGER_SPEED, CHORUSFX_POSITION, &values_0_to_99},
    {"Depth", FLANGER_DEPTH, CHORUSFX_POSITION, &values_0_to_99},
    {"Regen", FLANGER_REGEN, CHORUSFX_POSITION, &values_0_to_99},
    {"Waveform", FLANGER_WAVE, CHORUSFX_POSITION, &values_waveform},
    {"Balance", FLANGER_BALANCE, CHORUSFX_POSITION, &values_balance},
    {"Mix", FLANGER_LEVEL, CHORUSFX_POSITION, &values_0_to_99},
};

static EffectSettings gnx3k_phaser_settings[] = {
    {"Speed", PHASER_SPEED, CHORUSFX_POSITION, &values_0_to_99},
    {"Depth", PHASER_DEPTH, CHORUSFX_POSITION, &values_0_to_99},
    {"Regen", PHASER_REGEN, CHORUSFX_POSITION, &values_0_to_99},
    {"Waveform", PHASER_WAVE, CHORUSFX_POSITION, &values_waveform},
    {"Balance", PHASER_BALANCE, CHORUSFX_POSITION, &values_balance},
    {"Level", PHASER_LEVEL, CHORUSFX_POSITION, &values_0_to_99},
};

static EffectSettings gnx3k_trigflanger_settings[] = {
    {"Speed", TRIG_FLANGER_SPEED, CHORUSFX_POSITION, &values_0_to_99},
    {"Sens", TRIG_FLANGER_SENS, CHORUSFX_POSITION, &values_0_to_99},
    {"Lfo Start", TRIG_FLANGER_LFO_START, CHORUSFX_POSITION, &values_0_to_99},
    {"Mix", TRIG_FLANGER_MIX, CHORUSFX_POSITION, &values_0_to_99},
};

static EffectSettings gnx3k_trigphaser_settings[] = {
    {"Speed", TRIG_PHASER_SPEED, CHORUSFX_POSITION, &values_0_to_99},
    {"Sens", TRIG_PHASER_SENS, CHORUSFX_POSITION, &values_0_to_99},
    {"Lfo Start", TRIG_PHASER_LFO_START, CHORUSFX_POSITION, &values_0_to_99},
    {"Mix", TRIG_FLANGER_MIX, CHORUSFX_POSITION, &values_0_to_99},
};

static EffectSettings gnx3k_unovibe_settings[] = {
    {"Chorus/Vibrato", UNOVIBE_CHORUS_VIBRATO, CHORUSFX_POSITION, &values_chorus_vibrato},
    {"Intensity", UNOVIBE_INTENSITY, CHORUSFX_POSITION, &values_0_to_99},
    {"Pedal Speed", UNOVIBE_PEDAL_SPEED, CHORUSFX_POSITION, &values_0_to_99},
};

static EffectSettings gnx3k_vibrato_settings[] = {
    {"Speed", VIBRATO_SPEED, CHORUSFX_POSITION, &values_0_to_99},
    {"Depth", VIBRATO_DEPTH, CHORUSFX_POSITION, &values_0_to_99},
    {"Waveform", VIBRATO_WAVEFORM, CHORUSFX_POSITION, &values_waveform},
};

static EffectSettings gnx3k_rotary_settings[] = {
    {"Speed", ROTARY_SPEED, CHORUSFX_POSITION, &values_0_to_99},
    {"Depth", ROTARY_INTENSITY, CHORUSFX_POSITION, &values_0_to_99},
    {"Doppler", ROTARY_DOPPLER, CHORUSFX_POSITION, &values_0_to_99},
    {"X-Over", ROTARY_CROSSOVER, CHORUSFX_POSITION, &values_200hz_to_1_5khz},
    {"Balance", ROTARY_BALANCE, CHORUSFX_POSITION, &values_balance},
    {"Mix", ROTARY_MIX, CHORUSFX_POSITION, &values_0_to_99},
};

static EffectSettings gnx3k_autoya_settings[] = {
    {"Speed", AUTOYA_SPEED, CHORUSFX_POSITION, &values_0_to_99},
    {"Depth", AUTOYA_DEPTH, CHORUSFX_POSITION, &values_0_to_99},
    {"Range", GNX3K_AUTOYA_RANGE, CHORUSFX_POSITION, &values_0_to_49},
    {"Balance", AUTOYA_BALANCE, CHORUSFX_POSITION, &values_balance},
    {"Mix", AUTOYA_MIX, CHORUSFX_POSITION, &values_0_to_99},
};

static EffectSettings gnx3k_yaya_settings[] = {
    {"Pedal", YAYA_PEDAL, CHORUSFX_POSITION, &values_0_to_99},
    {"Depth", YAYA_DEPTH, CHORUSFX_POSITION, &values_0_to_99},
    {"Range", GNX3K_YAYA_RANGE, CHORUSFX_POSITION, &values_0_to_49},
    {"Balance", YAYA_BALANCE, CHORUSFX_POSITION, &values_balance},
    {"Mix", YAYA_MIX, CHORUSFX_POSITION, &values_0_to_99},
};

static EffectSettings gnx3k_synthtalk_settings[] = {
    {"Attack", SYNTH_TALK_ATTACK, CHORUSFX_POSITION, &values_0_to_99},
    {"Release", GNX3K_SYNTH_TALK_RELEASE, CHORUSFX_POSITION, &values_synth_talk_release},
    {"Sens", SYNTH_TALK_SENS, CHORUSFX_POSITION, &values_0_to_99},
    {"Vox", SYNTH_TALK_VOX, CHORUSFX_POSITION, &values_0_to_99},
    {"Balance", SYNTH_TALK_BALANCE, CHORUSFX_POSITION, &values_balance},
};

static EffectSettings gnx3k_envelope_settings[] = {
    {"Sensitivity", ENVELOPE_SENSITIVITY, CHORUSFX_POSITION, &values_0_to_99},
    {"Range", ENVELOPE_RANGE, CHORUSFX_POSITION, &values_0_to_99},
    {"Balance", ENVELOPE_BALANCE, CHORUSFX_POSITION, &values_balance},
    {"Mix", ENVELOPE_MIX, CHORUSFX_POSITION, &values_0_to_99},
};

static EffectSettings gnx3k_mod_detune_settings[] = {
    {"Shift Amount", DETUNE_AMOUNT, CHORUSFX_POSITION, &values_m24_to_24},
    {"Balance", DETUNE_BALANCE, CHORUSFX_POSITION, &values_balance},
    {"Level", DETUNE_LEVEL, CHORUSFX_POSITION, &values_0_to_99},
};

static EffectSettings gnx3k_mod_pitch_settings[] = {
    {"Shift Amount", PITCH_AMOUNT, CHORUSFX_POSITION, &values_m12_to_24},
    {"Balance", PITCH_BALANCE, CHORUSFX_POSITION, &values_balance},
    {"Level", GNX3K_PITCH_LEVEL, CHORUSFX_POSITION, &values_0_to_99},
};

static EffectSettings chorusfx_ce_settings[] = {
    {"Speed", CHORUS_SPEED, CHORUSFX_POSITION, &values_0_to_99},
    {"Depth", CHORUS_DEPTH, CHORUSFX_POSITION, &values_0_to_99},
};

static EffectSettings chorusfx_tc_settings[] = {
    {"Speed", CHORUS_SPEED, CHORUSFX_POSITION, &values_0_to_99},
    {"Width", CHORUS_WIDTH, CHORUSFX_POSITION, &values_0_to_99},
    {"Intensity", CHORUS_INTENSITY, CHORUSFX_POSITION, &values_0_to_99},
};

static EffectSettings chorusfx_dual_settings[] = {
    {"Speed", CHORUS_SPEED, CHORUSFX_POSITION, &values_0_to_99},
    {"Depth", CHORUS_DEPTH, CHORUSFX_POSITION, &values_0_to_99},
    {"Level", CHORUS_LEVEL, CHORUSFX_POSITION, &values_0_to_99},
    {"Wave", CHORUS_WAVE, CHORUSFX_POSITION, &values_waveform},
};

static EffectSettings chorusfx_glisten_settings[] = {
    {"Speed", CHORUS_SPEED, CHORUSFX_POSITION, &values_0_to_99},
    {"Depth", CHORUS_DEPTH, CHORUSFX_POSITION, &values_0_to_99},
    {"Level", CHORUS_LEVEL, CHORUSFX_POSITION, &values_0_to_99},
};

static EffectSettings chorusfx_multi_settings[] = {
    {"Speed", CHORUS_SPEED, CHORUSFX_POSITION, &values_0_to_99},
    {"Depth", CHORUS_DEPTH, CHORUSFX_POSITION, &values_0_to_99},
    {"Level", CHORUS_LEVEL, CHORUSFX_POSITION, &values_0_to_99},
    {"Wave", CHORUS_WAVE, CHORUSFX_POSITION, &values_waveform},
};

static EffectSettings chorusfx_flanger_settings[] = {
    {"Speed", FLANGER_SPEED, CHORUSFX_POSITION, &values_0_to_99},
    {"Depth", FLANGER_DEPTH, CHORUSFX_POSITION, &values_0_to_99},
    {"Regen", FLANGER_REGEN, CHORUSFX_POSITION, &values_0_to_99},
    {"Level", FLANGER_LEVEL, CHORUSFX_POSITION, &values_0_to_99},
    {"Wave", FLANGER_WAVE, CHORUSFX_POSITION, &values_waveform},
};

static EffectSettings chorusfx_trig_flanger_settings[] = {
    {"Speed", TRIG_FLANGER_SPEED, CHORUSFX_POSITION, &values_0_to_99},
    {"Sens", TRIG_FLANGER_SENS, CHORUSFX_POSITION, &values_0_to_99},
    {"Lfo Start", TRIG_FLANGER_LFO_START, CHORUSFX_POSITION, &values_0_to_99},
    {"Level", TRIG_FLANGER_LEVEL, CHORUSFX_POSITION, &values_0_to_99},
};

static EffectSettings chorusfx_mxr_flanger_settings[] = {
    {"Speed", FLANGER_SPEED, CHORUSFX_POSITION, &values_0_to_99},
    {"Width", MXR_FLANGER_WIDTH, CHORUSFX_POSITION, &values_0_to_99},
    {"Regen", FLANGER_REGEN, CHORUSFX_POSITION, &values_0_to_99},
    {"Manual", MXR_FLANGER_MANUAL, CHORUSFX_POSITION, &values_0_to_99},
};

static EffectSettings chorusfx_eh_flanger_settings[] = {
    {"Rate", EH_FLANGER_RATE, CHORUSFX_POSITION, &values_0_to_99},
    {"Range", EH_FLANGER_RANGE, CHORUSFX_POSITION, &values_0_to_99},
    {"Color", EH_FLANGER_COLOR, CHORUSFX_POSITION, &values_0_to_99},
};

static EffectSettings chorusfx_ad_flanger_settings[] = {
    {"Speed", FLANGER_SPEED, CHORUSFX_POSITION, &values_0_to_99},
    {"Enhance", AD_FLANGER_ENHANCE, CHORUSFX_POSITION, &values_0_to_99},
    {"Range", EH_FLANGER_RANGE, CHORUSFX_POSITION, &values_0_to_99},
    {"Harmonics", AD_FLANGER_HARMONICS, CHORUSFX_POSITION, &values_odd_even},
    {"Manual", MXR_FLANGER_MANUAL, CHORUSFX_POSITION, &values_0_to_99},
};

static EffectSettings chorusfx_phaser_settings[] = {
    {"Speed", PHASER_SPEED, CHORUSFX_POSITION, &values_0_to_99},
    {"Depth", PHASER_DEPTH, CHORUSFX_POSITION, &values_0_to_99},
    {"Regen", PHASER_REGEN, CHORUSFX_POSITION, &values_0_to_99},
    {"Level", PHASER_LEVEL, CHORUSFX_POSITION, &values_0_to_99},
    {"Wave", PHASER_WAVE, CHORUSFX_POSITION, &values_waveform},
};

static EffectSettings chorusfx_trig_phaser_settings[] = {
    {"Speed", TRIG_PHASER_SPEED, CHORUSFX_POSITION, &values_0_to_99},
    {"Sens", TRIG_PHASER_SENS, CHORUSFX_POSITION, &values_0_to_99},
    {"Lfo Start", TRIG_PHASER_LFO_START, CHORUSFX_POSITION, &values_0_to_99},
    {"Level", TRIG_PHASER_LEVEL, CHORUSFX_POSITION, &values_0_to_99},
};

static EffectSettings chorusfx_mx_phaser_settings[] = {
    {"Speed", PHASER_SPEED, CHORUSFX_POSITION, &values_0_to_99},
    {"Intensity", MX_PHASER_INTENSITY, CHORUSFX_POSITION, &values_1_to_4},
};

static EffectSettings chorusfx_eh_phaser_settings[] = {
    {"Rate", EH_PHASER_RATE, CHORUSFX_POSITION, &values_0_to_99},
    {"Color", EH_PHASER_COLOR, CHORUSFX_POSITION, &values_on_off},
};

static EffectSettings chorusfx_vibrato_settings[] = {
    {"Speed", VIBRATO_SPEED, CHORUSFX_POSITION, &values_0_to_99},
    {"Depth", VIBRATO_DEPTH, CHORUSFX_POSITION, &values_0_to_99},
};

static EffectSettings chorusfx_rotary_settings[] = {
    {"Speed", ROTARY_SPEED, CHORUSFX_POSITION, &values_0_to_99},
    {"Intensity", ROTARY_INTENSITY, CHORUSFX_POSITION, &values_0_to_99},
    {"Doppler", ROTARY_DOPPLER, CHORUSFX_POSITION, &values_0_to_99},
    {"Crossover", ROTARY_CROSSOVER, CHORUSFX_POSITION, &values_0_to_99},
};

static EffectSettings chorusfx_vibropan_settings[] = {
    {"Speed", VIBROPAN_SPEED, CHORUSFX_POSITION, &values_0_to_99},
    {"Depth", VIBROPAN_DEPTH, CHORUSFX_POSITION, &values_0_to_99},
    {"Vibra", VIBROPAN_VIBRA, CHORUSFX_POSITION, &values_0_to_99},
    {"Wave", VIBROPAN_WAVE, CHORUSFX_POSITION, &values_waveform},
};

static EffectSettings chorusfx_unovibe_settings[] = {
    {"Pedal Speed", UNOVIBE_PEDAL_SPEED, CHORUSFX_POSITION, &values_0_to_99},
    {"Intensity", UNOVIBE_INTENSITY, CHORUSFX_POSITION, &values_0_to_99},
    {"Chorus/Vibrato", UNOVIBE_CHORUS_VIBRATO, CHORUSFX_POSITION, &values_chorus_vibrato},
    {"Volume", UNOVIBE_VOLUME, CHORUSFX_POSITION, &values_0_to_99},
};

static EffectSettings chorusfx_tremolo_settings[] = {
    {"Speed", TREMOLO_SPEED, CHORUSFX_POSITION, &values_0_to_99},
    {"Depth", TREMOLO_DEPTH, CHORUSFX_POSITION, &values_0_to_99},
    {"Wave", TREMOLO_WAVE, CHORUSFX_POSITION, &values_waveform},
};

static EffectSettings chorusfx_trem_settings[] = {
    {"Speed", TREMOLO_SPEED, CHORUSFX_POSITION, &values_0_to_99},
    {"Depth", TREMOLO_DEPTH, CHORUSFX_POSITION, &values_0_to_99},
};

static EffectSettings chorusfx_panner_settings[] = {
    {"Speed", PANNER_SPEED, CHORUSFX_POSITION, &values_0_to_99},
    {"Depth", PANNER_DEPTH, CHORUSFX_POSITION, &values_0_to_99},
    {"Wave", PANNER_WAVE, CHORUSFX_POSITION, &values_waveform},
};

static EffectSettings chorusfx_envelope_settings[] = {
    {"Sensitivity", ENVELOPE_SENSITIVITY, CHORUSFX_POSITION, &values_0_to_99},
    {"Range", ENVELOPE_RANGE, CHORUSFX_POSITION, &values_0_to_99},
};

static EffectSettings chorusfx_fx25_settings[] = {
    {"Sensitivity", ENVELOPE_SENSITIVITY, CHORUSFX_POSITION, &values_0_to_99},
    {"Range", ENVELOPE_RANGE, CHORUSFX_POSITION, &values_0_to_99},
    {"Blend", ENVELOPE_BLEND, CHORUSFX_POSITION, &values_0_to_99},
};

static EffectSettings chorusfx_autoya_settings[] = {
    {"Speed", AUTOYA_SPEED, CHORUSFX_POSITION, &values_0_to_99},
    {"Intensity", AUTOYA_INTENSITY, CHORUSFX_POSITION, &values_0_to_99},
    {"Range", AUTOYA_RANGE, CHORUSFX_POSITION, &values_0_to_49},
};

static EffectSettings chorusfx_yaya_settings[] = {
    {"Pedal", YAYA_PEDAL, CHORUSFX_POSITION, &values_0_to_99},
    {"Intensity", YAYA_INTENSITY, CHORUSFX_POSITION, &values_0_to_99},
    {"Range", YAYA_RANGE, CHORUSFX_POSITION, &values_0_to_49},
};

static EffectSettings chorusfx_synth_talk_settings[] = {
    {"Attack", SYNTH_TALK_ATTACK, CHORUSFX_POSITION, &values_0_to_99},
    {"Release", SYNTH_TALK_RELEASE, CHORUSFX_POSITION, &values_0_to_99},
    {"Sens", SYNTH_TALK_SENS, CHORUSFX_POSITION, &values_0_to_99},
    {"Vox", SYNTH_TALK_VOX, CHORUSFX_POSITION, &values_0_to_99},
    {"Balance", SYNTH_TALK_BALANCE, CHORUSFX_POSITION, &values_balance},
};

static EffectSettings chorusfx_step_filter_settings[] = {
    {"Speed", STEP_FILTER_SPEED, CHORUSFX_POSITION, &values_0_to_99},
    {"Intensity", STEP_FILTER_INTENSITY, CHORUSFX_POSITION, &values_0_to_99},
};

static EffectSettings chorusfx_sample_hold_settings[] = {
    {"Speed", SAMPLE_HOLD_SPEED, CHORUSFX_POSITION, &values_0_to_99},
    {"Intensity", SAMPLE_HOLD_INTENSITY, CHORUSFX_POSITION, &values_0_to_99},
};

static EffectSettings chorusfx_whammy_settings[] = {
    {"Pedal", WHAMMY_PEDAL, CHORUSFX_POSITION, &values_0_to_99},
    {"Mix", WHAMMY_MIX, CHORUSFX_POSITION, &values_0_to_99},
    {"Amount", WHAMMY_AMOUNT, CHORUSFX_POSITION, &values_whammy_amount},
};

static EffectSettings chorusfx_pitch_shift_settings[] = {
    {"Amount", PITCH_AMOUNT, CHORUSFX_POSITION, &values_m24_to_24},
    {"Mix", PITCH_MIX, CHORUSFX_POSITION, &values_0_to_99},
};

static EffectSettings chorusfx_detune_settings[] = {
    {"Amount", DETUNE_AMOUNT, CHORUSFX_POSITION, &values_m24_to_24},
    {"Level", DETUNE_LEVEL, CHORUSFX_POSITION, &values_0_to_99},
};

static EffectSettings chorusfx_ips_settings[] = {
    {"Shift", IPS_SHIFT_AMOUNT, CHORUSFX_POSITION, &values_ips_shift},
    {"Key", IPS_KEY, CHORUSFX_POSITION, &values_ips_key},
    {"Scale", IPS_SCALE, CHORUSFX_POSITION, &values_ips_scale},
    {"Level", IPS_LEVEL, CHORUSFX_POSITION, &values_0_to_99},
};

static EffectSettings chorusfx_octaver_settings[] = {
    {"Octave 1", OCTAVER_OCTAVE1, CHORUSFX_POSITION, &values_0_to_99},
    {"Octave 2", OCTAVER_OCTAVE2, CHORUSFX_POSITION, &values_0_to_99},
    {"Dry Level", OCTAVER_DRY_LEVEL, CHORUSFX_POSITION, &values_0_to_99},
};

static EffectSettings gnx3k_delay_settings[] = {
    {"Time", GNX3K_DELAY_TIME, DELAY_POSITION, &values_delay_time_0_2000},
    {"Feedback", GNX3K_DELAY_FEEDBACK, DELAY_POSITION, &values_delay_repeats},
    {"Thresh", GNX3K_DELAY_DUCK_THRESH, DELAY_POSITION, &values_delay_thresh},
    {"Atten", GNX3K_DELAY_DUCK_ATTEN, DELAY_POSITION, &values_0_to_99},
    {"Balance", GNX3K_DELAY_BALANCE, DELAY_POSITION, &values_balance},
    {"Level", DELAY_LEVEL, DELAY_POSITION, &values_0_to_99},
};

static EffectSettings gnx3k_delay_spread_settings[] = {
    {"Time", GNX3K_DELAY_TIME, DELAY_POSITION, &values_delay_time_0_2000},
    {"Feedback", GNX3K_DELAY_FEEDBACK, DELAY_POSITION, &values_delay_repeats},
    {"Thresh", GNX3K_DELAY_DUCK_THRESH, DELAY_POSITION, &values_delay_thresh},
    {"Atten", GNX3K_DELAY_DUCK_ATTEN, DELAY_POSITION, &values_0_to_99},
    {"Spread", GNX3K_DELAY_SPREAD, DELAY_POSITION, &values_delay_spread_0_49},
    {"Level", DELAY_LEVEL, DELAY_POSITION, &values_0_to_99},
};

static EffectSettings delay_analog_settings[] = {
    {"Time", DELAY_TIME, DELAY_POSITION, &values_delay_time},
    {"Level", DELAY_LEVEL, DELAY_POSITION, &values_0_to_99},
    {"Repeats", DELAY_REPEATS, DELAY_POSITION, &values_delay_repeats},
};

static EffectSettings delay_dm_settings[] = {
    {"Repeat Rate", DELAY_REPEAT_RATE_DM, DELAY_POSITION, &values_0_to_99},
    {"Echo", DELAY_ECHO, DELAY_POSITION, &values_0_to_99},
    {"Intensity", DELAY_INTENSITY, DELAY_POSITION, &values_0_to_99},
};

static EffectSettings delay_digital_settings[] = {
    {"Time", DELAY_TIME, DELAY_POSITION, &values_delay_time},
    {"Level", DELAY_LEVEL, DELAY_POSITION, &values_0_to_99},
    {"Repeats", DELAY_REPEATS, DELAY_POSITION, &values_delay_repeats},
    {"Ducker thresh", DELAY_DUCK_THRESH, DELAY_POSITION, &values_0_to_99},
    {"Ducker level", DELAY_DUCK_LEVEL, DELAY_POSITION, &values_0_to_99},
};

static EffectSettings delay_modulated_settings[] = {
    {"Time", DELAY_TIME, DELAY_POSITION, &values_delay_time},
    {"Level", DELAY_LEVEL, DELAY_POSITION, &values_0_to_99},
    {"Repeats", DELAY_REPEATS, DELAY_POSITION, &values_delay_repeats},
    {"Depth", DELAY_DEPTH, DELAY_POSITION, &values_0_to_99},
};

static EffectSettings delay_pong_settings[] = {
    {"Time", DELAY_TIME, DELAY_POSITION, &values_delay_time},
    {"Level", DELAY_LEVEL, DELAY_POSITION, &values_0_to_99},
    {"Repeats", DELAY_REPEATS, DELAY_POSITION, &values_delay_repeats},
    {"Ducker thresh", DELAY_DUCK_THRESH, DELAY_POSITION, &values_0_to_99},
    {"Ducker level", DELAY_DUCK_LEVEL, DELAY_POSITION, &values_0_to_99},
};

static EffectSettings delay_tape_settings[] = {
    {"Time", DELAY_TIME, DELAY_POSITION, &values_delay_time},
    {"Level", DELAY_LEVEL, DELAY_POSITION, &values_0_to_99},
    {"Repeats", DELAY_REPEATS, DELAY_POSITION, &values_delay_repeats},
    {"Wow", DELAY_TAPE_WOW, DELAY_POSITION, &values_0_to_99},
    {"Flutter", DELAY_TAPE_FLUTTER, DELAY_POSITION, &values_0_to_99},
};

static EffectSettings delay_echoplex_settings[] = {
    {"Time", DELAY_ECHOPLEX_TIME, DELAY_POSITION, &values_0_to_99},
    {"Repeats", DELAY_REPEATS_0_99, DELAY_POSITION, &values_0_to_99},
    {"Volume", DELAY_VOLUME, DELAY_POSITION, &values_0_to_99},
};

static EffectSettings rp500_delay_digital_settings[] = {
    {"Tap Time", DELAY_TAP_TIME, DELAY_POSITION, &values_delay_time_0_5000},
    {"Repeats", DELAY_REPEATS, DELAY_POSITION, &values_delay_repeats},
    {"Duck Thresh", DELAY_DUCK_THRESH, DELAY_POSITION, &values_0_to_99},
    {"Duck Level", DELAY_DUCK_LEVEL, DELAY_POSITION, &values_0_to_99},
    {"Level", DELAY_LEVEL, DELAY_POSITION, &values_0_to_99},
};

static EffectSettings rp500_delay_analog_settings[] = {
    {"Tap Time", DELAY_TAP_TIME, DELAY_POSITION, &values_delay_time_0_5000},
    {"Repeats", DELAY_REPEATS, DELAY_POSITION, &values_delay_repeats},
    {"Level", DELAY_LEVEL, DELAY_POSITION, &values_0_to_99},
};

static EffectSettings rp500_delay_dm_settings[] = {
    {"Repeat Rate", DELAY_REPEAT_RATE, DELAY_POSITION, &values_delay_repeat_rate_0_286},
    {"Echo", DELAY_ECHO, DELAY_POSITION, &values_0_to_99},
    {"Intensity", DELAY_INTENSITY, DELAY_POSITION, &values_0_to_99},
};

static EffectSettings rp500_delay_echoplex_settings[] = {
    {"Time", DELAY_TIME_0_760, DELAY_POSITION, &values_delay_time_0_760},
    {"Volume", DELAY_VOLUME, DELAY_POSITION, &values_0_to_99},
    {"Repeats", DELAY_REPEATS_0_99, DELAY_POSITION, &values_0_to_99},
};

static EffectSettings rp500_delay_modulated_settings[] = {
    {"Tap Time", DELAY_TAP_TIME_0_4990, DELAY_POSITION, &values_delay_time_0_4990},
    {"Repeats", DELAY_REPEATS, DELAY_POSITION, &values_delay_repeats},
    {"Level", DELAY_LEVEL, DELAY_POSITION, &values_0_to_99},
    {"Depth", DELAY_DEPTH, DELAY_POSITION, &values_0_to_99},
};

static EffectSettings rp500_delay_pong_settings[] = {
    {"Tap Time", DELAY_TAP_TIME, DELAY_POSITION, &values_delay_time_0_5000},
    {"Repeats", DELAY_REPEATS, DELAY_POSITION, &values_delay_repeats},
    {"Duck Thresh", DELAY_DUCK_THRESH, DELAY_POSITION, &values_0_to_99},
    {"Duck Level", DELAY_DUCK_LEVEL, DELAY_POSITION, &values_0_to_99},
    {"Level", DELAY_LEVEL, DELAY_POSITION, &values_0_to_99},
};

static EffectSettings rp500_delay_reverse_settings[] = {
    {"Time", DELAY_TIME_0_4650, DELAY_POSITION, &values_delay_time_0_4650},
    {"Repeats", DELAY_REPEATS, DELAY_POSITION, &values_delay_repeats},
    {"Mix", DELAY_MIX, DELAY_POSITION, &values_0_to_99},
};

static EffectSettings rp500_delay_tape_settings[] = {
    {"Tap Time", DELAY_TAP_TIME_0_4990, DELAY_POSITION, &values_delay_time_0_4990},
    {"Repeats", DELAY_REPEATS, DELAY_POSITION, &values_delay_repeats},
    {"Level", DELAY_LEVEL, DELAY_POSITION, &values_0_to_99},
    {"Tape Wow", DELAY_TAPE_WOW, DELAY_POSITION, &values_0_to_99},
    {"Tape Flutter", DELAY_TAPE_FLUTTER, DELAY_POSITION, &values_0_to_99},
};

static EffectSettings reverb_twin_settings[] = {
    {"Reverb", REVERB_LEVEL, REVERB_POSITION, &values_0_to_99},
};

static EffectSettings gnx3k_reverb_settings[] = {
    {"Predelay", REVERB_PREDELAY, REVERB_POSITION, &values_0_to_15},
    {"Decay", REVERB_DECAY, REVERB_POSITION, &values_0_to_99},
    {"Damping", REVERB_DAMPING, REVERB_POSITION, &values_0_to_99},
    {"Balance", REVERB_BALANCE, REVERB_POSITION, &values_balance},
    {"Level", REVERB_LEVEL, REVERB_POSITION, &values_0_to_99},
};

static EffectSettings reverb_lex_settings[] = {
    {"Predelay", REVERB_PREDELAY, REVERB_POSITION, &values_0_to_15},
    {"Decay", REVERB_DECAY, REVERB_POSITION, &values_0_to_99},
    {"Liveliness", REVERB_LIVELINESS, REVERB_POSITION, &values_0_to_99},
    {"Level", REVERB_LEVEL, REVERB_POSITION, &values_0_to_99},
};

static EffectGroup gnx3k_wah_group[] = {
    {GNX3K_WAH_TYPE_CRY, "Cry", gnx3k_wah_settings, G_N_ELEMENTS(gnx3k_wah_settings)},
    {GNX3K_WAH_TYPE_BOUTIQUE, "Boutique", gnx3k_wah_settings, G_N_ELEMENTS(gnx3k_wah_settings)},
    {GNX3K_WAH_TYPE_FULLRANGE, "Full-Range", gnx3k_wah_settings, G_N_ELEMENTS(gnx3k_wah_settings)},
};

static EffectGroup wah_group[] = {
    {WAH_TYPE_CRY, "Cry wah", wah_settings, G_N_ELEMENTS(wah_settings)},
    {WAH_TYPE_FULLRANGE, "Fullrange wah", wah_settings, G_N_ELEMENTS(wah_settings)},
    {WAH_TYPE_CLYDE, "Clyde wah", wah_settings, G_N_ELEMENTS(wah_settings)},
};

static EffectGroup gnx3k_whammy_group[] = {
    {GNX3K_WHAM_TYPE_WHAMMY, "Whammy", gnx3k_whammy_settings, G_N_ELEMENTS(gnx3k_whammy_settings)},
    {GNX3K_WHAM_TYPE_IPS, "IPS", gnx3k_ips_settings, G_N_ELEMENTS(gnx3k_ips_settings)},
    {GNX3K_WHAM_TYPE_DETUNE, "Detune", gnx3k_detune_settings, G_N_ELEMENTS(gnx3k_detune_settings)},
    {GNX3K_WHAM_TYPE_PITCH, "Pitch Shift", gnx3k_pitch_settings, G_N_ELEMENTS(gnx3k_pitch_settings)},
    {GNX3K_WHAM_TYPE_TALK1, "Talker 1", gnx3k_talk_settings, G_N_ELEMENTS(gnx3k_talk_settings)},
    {GNX3K_WHAM_TYPE_TALK2, "Talker 2", gnx3k_talk_settings, G_N_ELEMENTS(gnx3k_talk_settings)},
    {GNX3K_WHAM_TYPE_TALK3, "Talker 3", gnx3k_talk_settings, G_N_ELEMENTS(gnx3k_talk_settings)},
    {GNX3K_WHAM_TYPE_TALK4, "Talker 4", gnx3k_talk_settings, G_N_ELEMENTS(gnx3k_talk_settings)},
    {GNX3K_WHAM_TYPE_TALK5, "Talker 5", gnx3k_talk_settings, G_N_ELEMENTS(gnx3k_talk_settings)},
};

static EffectGroup gnx3k_amp_channel_group[] = {
    {-1, NULL, gnx3k_amp_channel_settings, G_N_ELEMENTS(gnx3k_amp_channel_settings)},
};

static EffectGroup usb_group[] = {
    {-1, NULL, usb_settings, G_N_ELEMENTS(usb_settings)},
};

static EffectGroup gnx3k_preset_group[] = {
    {-1, NULL, gnx3k_preset_settings, G_N_ELEMENTS(gnx3k_preset_settings)},
};

static EffectGroup gnx_comp_group[] = {
    {-1, NULL, gnx_comp_settings, G_N_ELEMENTS(gnx_comp_settings)},
};

static EffectGroup rp250_comp_group[] = {
    {COMP_TYPE_DIGI, "Digital compressor", comp_digi_settings, G_N_ELEMENTS(comp_digi_settings)},
    {COMP_TYPE_CS, "CS compressor", comp_cs_settings, G_N_ELEMENTS(comp_cs_settings)},
};

static EffectGroup rp355_comp_group[] = {
    {COMP_TYPE_DIGI, "Digital compressor", comp_digi_settings, G_N_ELEMENTS(comp_digi_settings)},
    {COMP_TYPE_CS, "CS compressor", comp_cs_settings, G_N_ELEMENTS(comp_cs_settings)},
    {COMP_TYPE_DYNA, "Dyna comp", comp_dyna_settings, G_N_ELEMENTS(comp_dyna_settings)},
};

static EffectGroup rp500_comp_group[] = {
    {COMP_TYPE_DIGI, "Digital compressor", comp_digi_settings, G_N_ELEMENTS(comp_digi_settings)},
    {COMP_TYPE_CS, "CS compressor", comp_cs_settings, G_N_ELEMENTS(comp_cs_settings)},
    {COMP_TYPE_DYNA, "Dyna comp", comp_dyna_settings, G_N_ELEMENTS(comp_dyna_settings)},
};

static EffectGroup rp250_dist_group[] = {
    {DIST_TYPE_SCREAMER, "Screamer", dist_screamer_settings, G_N_ELEMENTS(dist_screamer_settings)},
    {DIST_TYPE_808, "808", dist_808_settings, G_N_ELEMENTS(dist_808_settings)},
    {DIST_TYPE_GUYOD, "GuyOD", dist_guyod_settings, G_N_ELEMENTS(dist_guyod_settings)},
    {DIST_TYPE_DOD250, "DOD250", dist_dod250_settings, G_N_ELEMENTS(dist_dod250_settings)},
    {DIST_TYPE_RODENT, "Rodent", dist_rodent_settings, G_N_ELEMENTS(dist_rodent_settings)},
    {DIST_TYPE_MX, "MX", dist_mx_settings, G_N_ELEMENTS(dist_mx_settings)},
    {DIST_TYPE_DS, "DS", dist_ds_settings, G_N_ELEMENTS(dist_ds_settings)},
    {DIST_TYPE_GRUNGE, "Grunge", dist_grunge_settings, G_N_ELEMENTS(dist_grunge_settings)},
    {DIST_TYPE_ZONE, "Zone", dist_zone_settings, G_N_ELEMENTS(dist_zone_settings)},
    {DIST_TYPE_DEATH, "Death", dist_death_settings, G_N_ELEMENTS(dist_death_settings)},
    {DIST_TYPE_GONK, "Gonk", dist_gonk_settings, G_N_ELEMENTS(dist_gonk_settings)},
    {DIST_TYPE_FUZZY, "Fuzzy", dist_fuzzy_settings, G_N_ELEMENTS(dist_fuzzy_settings)},
    {DIST_TYPE_MP, "Big MP", dist_mp_settings, G_N_ELEMENTS(dist_mp_settings)},
};

static EffectGroup rp355_dist_group[] = {
    {DIST_TYPE_SCREAMER, "Screamer", dist_screamer_settings, G_N_ELEMENTS(dist_screamer_settings)},
    {DIST_TYPE_808, "808", dist_808_settings, G_N_ELEMENTS(dist_808_settings)},
    {DIST_TYPE_SPARKDRIVE, "Sparkdrive", dist_sparkdrive_settings, G_N_ELEMENTS(dist_sparkdrive_settings)},
    {DIST_TYPE_GUYOD, "OD Overdrive", dist_guyod_settings, G_N_ELEMENTS(dist_guyod_settings)},
    {DIST_TYPE_DOD250, "DOD250", dist_dod250_settings, G_N_ELEMENTS(dist_dod250_settings)},
    {DIST_TYPE_REDLINE, "Redline", dist_redline_settings, G_N_ELEMENTS(dist_redline_settings)},
    {DIST_TYPE_RODENT, "Rodent", dist_rodent_settings, G_N_ELEMENTS(dist_rodent_settings)},
    {DIST_TYPE_MX, "MX", dist_mx_settings, G_N_ELEMENTS(dist_mx_settings)},
    {DIST_TYPE_DS, "DS", dist_ds_settings, G_N_ELEMENTS(dist_ds_settings)},
    {DIST_TYPE_GRUNGE, "Grunge", dist_grunge_settings, G_N_ELEMENTS(dist_grunge_settings)},
    {DIST_TYPE_ZONE, "Zone", dist_zone_settings, G_N_ELEMENTS(dist_zone_settings)},
    {DIST_TYPE_DEATH, "Death", dist_death_settings, G_N_ELEMENTS(dist_death_settings)},
    {DIST_TYPE_GONK, "Gonk", dist_gonk_settings, G_N_ELEMENTS(dist_gonk_settings)},
    {DIST_TYPE_8TAVIA, "8tavia", dist_8tavia_settings, G_N_ELEMENTS(dist_8tavia_settings)},
    {DIST_TYPE_FUZZLATOR, "Fuzzlator", dist_fuzzlator_settings, G_N_ELEMENTS(dist_fuzzlator_settings)},
    {DIST_TYPE_CLASSIC_FUZZ, "Classic Fuzz", dist_classic_fuzz_settings, G_N_ELEMENTS(dist_classic_fuzz_settings)},
    {DIST_TYPE_FUZZY, "Fuzzy", dist_fuzzy_settings, G_N_ELEMENTS(dist_fuzzy_settings)},
    {DIST_TYPE_MP, "Big MP", dist_mp_settings, G_N_ELEMENTS(dist_mp_settings)},
};

static EffectGroup rp500_dist_group[] = {
    {DIST_TYPE_SCREAMER, "Screamer", dist_screamer_settings, G_N_ELEMENTS(dist_screamer_settings)},
    {DIST_TYPE_808, "808", dist_808_settings, G_N_ELEMENTS(dist_808_settings)},
    {DIST_TYPE_SPARKDRIVE, "Sparkdrive", dist_sparkdrive_settings, G_N_ELEMENTS(dist_sparkdrive_settings)},
    {DIST_TYPE_GUYOD, "OD Overdrive", dist_guyod_settings, G_N_ELEMENTS(dist_guyod_settings)},
    {DIST_TYPE_DOD250, "DOD250", dist_dod250_settings, G_N_ELEMENTS(dist_dod250_settings)},
    {DIST_TYPE_REDLINE, "Redline", dist_redline_settings, G_N_ELEMENTS(dist_redline_settings)},
    {DIST_TYPE_RODENT, "Rodent", dist_rodent_settings, G_N_ELEMENTS(dist_rodent_settings)},
    {DIST_TYPE_MX, "MX", dist_mx_settings, G_N_ELEMENTS(dist_mx_settings)},
    {DIST_TYPE_DS, "DS", dist_ds_settings, G_N_ELEMENTS(dist_ds_settings)},
    {DIST_TYPE_GRUNGE, "Grunge", dist_grunge_settings, G_N_ELEMENTS(dist_grunge_settings)},
    {DIST_TYPE_ZONE, "Zone", dist_zone_settings, G_N_ELEMENTS(dist_zone_settings)},
    {DIST_TYPE_DEATH, "Death", dist_death_settings, G_N_ELEMENTS(dist_death_settings)},
    {DIST_TYPE_GONK, "Gonk", dist_gonk_settings, G_N_ELEMENTS(dist_gonk_settings)},
    {DIST_TYPE_8TAVIA, "8tavia", dist_8tavia_settings, G_N_ELEMENTS(dist_8tavia_settings)},
    {DIST_TYPE_FUZZLATOR, "Fuzzlator", dist_fuzzlator_settings, G_N_ELEMENTS(dist_fuzzlator_settings)},
    {DIST_TYPE_CLASSIC_FUZZ, "Classic Fuzz", dist_classic_fuzz_settings, G_N_ELEMENTS(dist_classic_fuzz_settings)},
    {DIST_TYPE_FUZZY, "Fuzzy", dist_fuzzy_settings, G_N_ELEMENTS(dist_fuzzy_settings)},
    {DIST_TYPE_MP, "Big MP", dist_mp_settings, G_N_ELEMENTS(dist_mp_settings)},
};

static EffectGroup gnx4_dist_group[] = {
    {DIST_TYPE_SCREAMER, "Screamer", dist_screamer_settings, G_N_ELEMENTS(dist_screamer_settings)},
    {DIST_TYPE_RODENT, "Rodent", dist_rodent_settings, G_N_ELEMENTS(dist_rodent_settings)},
    {DIST_TYPE_DS, "DS Dist", dist_ds_settings, G_N_ELEMENTS(dist_ds_settings)},
    {DIST_TYPE_DOD250, "DOD250", dist_dod250_settings, G_N_ELEMENTS(dist_dod250_settings)},
    {DIST_TYPE_MP, "Big MP", dist_mp_settings, G_N_ELEMENTS(dist_mp_settings)},
    {DIST_TYPE_GUYOD, "Guy OD", dist_guyod_settings, G_N_ELEMENTS(dist_guyod_settings)},
    {DIST_TYPE_SPARKDRIVE, "Sparkdrive", dist_sparkdrive_settings, G_N_ELEMENTS(dist_sparkdrive_settings)},
    {DIST_TYPE_GRUNGE, "Grunge", dist_grunge_settings, G_N_ELEMENTS(dist_grunge_settings)},
    {DIST_TYPE_FUZZY, "Fuzzy", dist_fuzzy_settings, G_N_ELEMENTS(dist_fuzzy_settings)},
    {DIST_TYPE_ZONE, "Zone", dist_zone_settings, G_N_ELEMENTS(dist_zone_settings)},
};

static EffectGroup gnx3k_dist_group[] = {
    {DIST_TYPE_SCREAMER, "Screamer", dist_screamer_settings, G_N_ELEMENTS(dist_screamer_settings)},
    {DIST_TYPE_RODENT, "Rodent", dist_rodent_settings, G_N_ELEMENTS(dist_rodent_settings)},
    {DIST_TYPE_DS, "DS", dist_ds_settings, G_N_ELEMENTS(dist_ds_settings)},
    {DIST_TYPE_DOD250, "DOD250", dist_dod250_settings, G_N_ELEMENTS(dist_dod250_settings)},
    {DIST_TYPE_MP, "Big MP", dist_mp_settings, G_N_ELEMENTS(dist_mp_settings)},
    {DIST_TYPE_GUYOD, "Guy OD", dist_guyod_settings, G_N_ELEMENTS(dist_guyod_settings)},
    {DIST_TYPE_SPARKDRIVE, "Sparkdrive", dist_sparkdrive_settings, G_N_ELEMENTS(dist_sparkdrive_settings)},
    {DIST_TYPE_GRUNGE, "Grunge", dist_grunge_settings, G_N_ELEMENTS(dist_grunge_settings)},
    {DIST_TYPE_FUZZY, "Fuzzy", dist_fuzzy_settings, G_N_ELEMENTS(dist_fuzzy_settings)},
    {DIST_TYPE_ZONE, "Zone", dist_zone_settings, G_N_ELEMENTS(dist_zone_settings)},
    {DIST_TYPE_8TAVIA, "8tavia", dist_8tavia_settings, G_N_ELEMENTS(dist_8tavia_settings)},
};

static EffectGroup gnx3k_noisegate_group[] = {
    {NOISEGATE_GATE, "Silencer", noisegate_silencer_settings, G_N_ELEMENTS(noisegate_silencer_settings)},
    {NOISEGATE_SWELL, "Pluck", noisegate_pluck_settings, G_N_ELEMENTS(noisegate_pluck_settings)},
};

static EffectGroup noisegate_group[] = {
    {NOISEGATE_GATE, "Gate", noisegate_gate_settings, G_N_ELEMENTS(noisegate_gate_settings)},
    {NOISEGATE_SWELL, "Swell", noisegate_swell_settings, G_N_ELEMENTS(noisegate_swell_settings)},
};

static EffectGroup gnx4_chorusfx_group[] = {
    {GNX3K_MOD_TYPE_CHORUS, "Chorus", gnx3k_chorus_settings, G_N_ELEMENTS(gnx3k_chorus_settings)},
    {GNX3K_MOD_TYPE_FLANGER, "Flanger", gnx3k_flanger_settings, G_N_ELEMENTS(gnx3k_flanger_settings)},
    {GNX3K_MOD_TYPE_PHASER, "Phaser", gnx3k_phaser_settings, G_N_ELEMENTS(gnx3k_phaser_settings)},
    {GNX3K_MOD_TYPE_TRIGFLANGER, "Triggered Flanger", gnx3k_trigflanger_settings, G_N_ELEMENTS(gnx3k_trigflanger_settings)},
    {GNX3K_MOD_TYPE_TRIGPHASER, "Triggered Phaser", gnx3k_trigphaser_settings, G_N_ELEMENTS(gnx3k_trigphaser_settings)},
    {GNX3K_MOD_TYPE_TREMOLO, "Tremolo", chorusfx_tremolo_settings, G_N_ELEMENTS(chorusfx_tremolo_settings)},
    {GNX3K_MOD_TYPE_PANNER, "Panner", chorusfx_panner_settings, G_N_ELEMENTS(chorusfx_panner_settings)},
    {GNX3K_MOD_TYPE_VIBRATO, "Vibrato", gnx3k_vibrato_settings, G_N_ELEMENTS(gnx3k_vibrato_settings)},
    {GNX3K_MOD_TYPE_ROTARY, "Rotary", gnx3k_rotary_settings, G_N_ELEMENTS(gnx3k_rotary_settings)},
    {GNX3K_MOD_TYPE_AUTOYA, "Auto Ya", gnx3k_autoya_settings, G_N_ELEMENTS(gnx3k_autoya_settings)},
    {GNX3K_MOD_TYPE_YAYA, "Ya Ya", gnx3k_yaya_settings, G_N_ELEMENTS(gnx3k_yaya_settings)},
    {GNX3K_MOD_TYPE_SYNTHTALK, "Synthtalk", gnx3k_synthtalk_settings, G_N_ELEMENTS(gnx3k_synthtalk_settings)},
    {GNX3K_MOD_TYPE_ENVELOPE, "Envelope", gnx3k_envelope_settings, G_N_ELEMENTS(gnx3k_envelope_settings)},
    {GNX3K_MOD_TYPE_DETUNE, "Detune", gnx3k_mod_detune_settings, G_N_ELEMENTS(gnx3k_mod_detune_settings)},
    {GNX3K_MOD_TYPE_PITCH, "Pitch", gnx3k_mod_pitch_settings, G_N_ELEMENTS(gnx3k_mod_pitch_settings)},
};

static EffectGroup gnx3k_chorusfx_group[] = {
    {GNX3K_MOD_TYPE_CHORUS, "Chorus", gnx3k_chorus_settings, G_N_ELEMENTS(gnx3k_chorus_settings)},
    {GNX3K_MOD_TYPE_FLANGER, "Flanger", gnx3k_flanger_settings, G_N_ELEMENTS(gnx3k_flanger_settings)},
    {GNX3K_MOD_TYPE_PHASER, "Phaser", gnx3k_phaser_settings, G_N_ELEMENTS(gnx3k_phaser_settings)},
    {GNX3K_MOD_TYPE_TRIGFLANGER, "Triggered Flanger", gnx3k_trigflanger_settings, G_N_ELEMENTS(gnx3k_trigflanger_settings)},
    {GNX3K_MOD_TYPE_TRIGPHASER, "Triggered Phaser", gnx3k_trigphaser_settings, G_N_ELEMENTS(gnx3k_trigphaser_settings)},
    {GNX3K_MOD_TYPE_UNOVIBE, "Unovibe", gnx3k_unovibe_settings, G_N_ELEMENTS(gnx3k_unovibe_settings)},
    {GNX3K_MOD_TYPE_TREMOLO, "Tremolo", chorusfx_tremolo_settings, G_N_ELEMENTS(chorusfx_tremolo_settings)},
    {GNX3K_MOD_TYPE_PANNER, "Panner", chorusfx_panner_settings, G_N_ELEMENTS(chorusfx_panner_settings)},
    {GNX3K_MOD_TYPE_VIBRATO, "Vibrato", gnx3k_vibrato_settings, G_N_ELEMENTS(gnx3k_vibrato_settings)},
    {GNX3K_MOD_TYPE_ROTARY, "Rotary", gnx3k_rotary_settings, G_N_ELEMENTS(gnx3k_rotary_settings)},
    {GNX3K_MOD_TYPE_AUTOYA, "Auto Ya", gnx3k_autoya_settings, G_N_ELEMENTS(gnx3k_autoya_settings)},
    {GNX3K_MOD_TYPE_YAYA, "Ya Ya", gnx3k_yaya_settings, G_N_ELEMENTS(gnx3k_yaya_settings)},
    {GNX3K_MOD_TYPE_SYNTHTALK, "Synthtalk", gnx3k_synthtalk_settings, G_N_ELEMENTS(gnx3k_synthtalk_settings)},
    {GNX3K_MOD_TYPE_ENVELOPE, "Envelope", gnx3k_envelope_settings, G_N_ELEMENTS(gnx3k_envelope_settings)},
    {GNX3K_MOD_TYPE_DETUNE, "Detune", gnx3k_mod_detune_settings, G_N_ELEMENTS(gnx3k_mod_detune_settings)},
    {GNX3K_MOD_TYPE_PITCH, "Pitch", gnx3k_mod_pitch_settings, G_N_ELEMENTS(gnx3k_mod_pitch_settings)},
};

static EffectGroup rp250_chorusfx_group[] = {
    {CHORUS_TYPE_CE, "CE Chorus", chorusfx_ce_settings, G_N_ELEMENTS(chorusfx_ce_settings)},
    {CHORUS_TYPE_DUAL, "Dual Chorus", chorusfx_dual_settings, G_N_ELEMENTS(chorusfx_dual_settings)},
    {CHORUS_TYPE_MULTI, "Multi Chorus", chorusfx_multi_settings, G_N_ELEMENTS(chorusfx_multi_settings)},
    {CHORUS_TYPE_FLANGER, "Flanger", chorusfx_flanger_settings, G_N_ELEMENTS(chorusfx_flanger_settings)},
    {CHORUS_TYPE_MXR_FLANGER, "MXR FLANGER", chorusfx_mxr_flanger_settings, G_N_ELEMENTS(chorusfx_mxr_flanger_settings)},
    {CHORUS_TYPE_PHASER, "Phaser", chorusfx_phaser_settings, G_N_ELEMENTS(chorusfx_phaser_settings)},
    {CHORUS_TYPE_VIBRATO, "Vibrato", chorusfx_vibrato_settings, G_N_ELEMENTS(chorusfx_vibrato_settings)},
    {CHORUS_TYPE_ROTARY, "Rotary", chorusfx_rotary_settings, G_N_ELEMENTS(chorusfx_rotary_settings)},
    {CHORUS_TYPE_VIBROPAN, "Vibropan", chorusfx_vibropan_settings, G_N_ELEMENTS(chorusfx_vibropan_settings)},
    {CHORUS_TYPE_TREMOLO, "Tremolo", chorusfx_tremolo_settings, G_N_ELEMENTS(chorusfx_tremolo_settings)},
    {CHORUS_TYPE_PANNER, "Panner", chorusfx_panner_settings, G_N_ELEMENTS(chorusfx_panner_settings)},
    {CHORUS_TYPE_ENVELOPE, "Envelope", chorusfx_envelope_settings, G_N_ELEMENTS(chorusfx_envelope_settings)},
    {CHORUS_TYPE_AUTOYA, "AutoYa", chorusfx_autoya_settings, G_N_ELEMENTS(chorusfx_autoya_settings)},
    {CHORUS_TYPE_YAYA, "YaYa", chorusfx_yaya_settings, G_N_ELEMENTS(chorusfx_yaya_settings)},
    {CHORUS_TYPE_STEP_FILTER, "Step Filter", chorusfx_step_filter_settings, G_N_ELEMENTS(chorusfx_step_filter_settings)},
    {CHORUS_TYPE_WHAMMY, "Whammy", chorusfx_whammy_settings, G_N_ELEMENTS(chorusfx_whammy_settings)},
    {CHORUS_TYPE_PITCH_SHIFT, "Pitch Shift", chorusfx_pitch_shift_settings, G_N_ELEMENTS(chorusfx_pitch_shift_settings)},
    {CHORUS_TYPE_DETUNE, "Detune", chorusfx_detune_settings, G_N_ELEMENTS(chorusfx_detune_settings)},
    {CHORUS_TYPE_IPS, "IPS", chorusfx_ips_settings, G_N_ELEMENTS(chorusfx_ips_settings)},
};

static EffectGroup rp355_chorusfx_group[] = {
    {CHORUS_TYPE_CE, "CE Chorus", chorusfx_ce_settings, G_N_ELEMENTS(chorusfx_ce_settings)},
    {CHORUS_TYPE_TC, "TC Chorus", chorusfx_tc_settings, G_N_ELEMENTS(chorusfx_tc_settings)},
    {CHORUS_TYPE_DUAL, "Dual Chorus", chorusfx_dual_settings, G_N_ELEMENTS(chorusfx_dual_settings)},
    {CHORUS_TYPE_MULTI, "Multi Chorus", chorusfx_multi_settings, G_N_ELEMENTS(chorusfx_multi_settings)},
    {CHORUS_TYPE_FLANGER, "Flanger", chorusfx_flanger_settings, G_N_ELEMENTS(chorusfx_flanger_settings)},
    {CHORUS_TYPE_TRIGGERED_FLANGER, "Triggered Flanger", chorusfx_trig_flanger_settings, G_N_ELEMENTS(chorusfx_trig_flanger_settings)},
    {CHORUS_TYPE_MXR_FLANGER, "MXR FLANGER", chorusfx_mxr_flanger_settings, G_N_ELEMENTS(chorusfx_mxr_flanger_settings)},
    {CHORUS_TYPE_EH_FLANGER, "EH Flanger", chorusfx_eh_flanger_settings, G_N_ELEMENTS(chorusfx_eh_flanger_settings)},
    {CHORUS_TYPE_PHASER, "Phaser", chorusfx_phaser_settings, G_N_ELEMENTS(chorusfx_phaser_settings)},
    {CHORUS_TYPE_TRIGGERED_PHASER, "Triggered Phaser", chorusfx_trig_phaser_settings, G_N_ELEMENTS(chorusfx_trig_phaser_settings)},
    {CHORUS_TYPE_MX_PHASER, "MX Phaser", chorusfx_mx_phaser_settings, G_N_ELEMENTS(chorusfx_mx_phaser_settings)},
    {CHORUS_TYPE_EH_PHASER, "EH Phaser", chorusfx_eh_phaser_settings, G_N_ELEMENTS(chorusfx_eh_phaser_settings)},
    {CHORUS_TYPE_VIBRATO, "Vibrato", chorusfx_vibrato_settings, G_N_ELEMENTS(chorusfx_vibrato_settings)},
    {CHORUS_TYPE_ROTARY, "Rotary", chorusfx_rotary_settings, G_N_ELEMENTS(chorusfx_rotary_settings)},
    {CHORUS_TYPE_VIBROPAN, "Vibropan", chorusfx_vibropan_settings, G_N_ELEMENTS(chorusfx_vibropan_settings)},
    {CHORUS_TYPE_UNOVIBE, "Unovibe", chorusfx_unovibe_settings, G_N_ELEMENTS(chorusfx_unovibe_settings)},
    {CHORUS_TYPE_TREMOLO, "Tremolo", chorusfx_tremolo_settings, G_N_ELEMENTS(chorusfx_tremolo_settings)},
    {CHORUS_TYPE_OPTO_TREMOLO, "Opto Tremolo", chorusfx_trem_settings, G_N_ELEMENTS(chorusfx_trem_settings)},
    {CHORUS_TYPE_BIAS_TREMOLO, "Bias Tremolo", chorusfx_trem_settings, G_N_ELEMENTS(chorusfx_trem_settings)},
    {CHORUS_TYPE_PANNER, "Panner", chorusfx_panner_settings, G_N_ELEMENTS(chorusfx_panner_settings)},
    {CHORUS_TYPE_ENVELOPE, "Envelope", chorusfx_envelope_settings, G_N_ELEMENTS(chorusfx_envelope_settings)},
    {CHORUS_TYPE_FX25, "FX25", chorusfx_fx25_settings, G_N_ELEMENTS(chorusfx_fx25_settings)},
    {CHORUS_TYPE_AUTOYA, "AutoYa", chorusfx_autoya_settings, G_N_ELEMENTS(chorusfx_autoya_settings)},
    {CHORUS_TYPE_YAYA, "YaYa", chorusfx_yaya_settings, G_N_ELEMENTS(chorusfx_yaya_settings)},
    {CHORUS_TYPE_SYNTH_TALK, "Synth Talk", chorusfx_synth_talk_settings, G_N_ELEMENTS(chorusfx_synth_talk_settings)},
    {CHORUS_TYPE_STEP_FILTER, "Step Filter", chorusfx_step_filter_settings, G_N_ELEMENTS(chorusfx_step_filter_settings)},
    {CHORUS_TYPE_WHAMMY, "Whammy", chorusfx_whammy_settings, G_N_ELEMENTS(chorusfx_whammy_settings)},
    {CHORUS_TYPE_PITCH_SHIFT, "Pitch Shift", chorusfx_pitch_shift_settings, G_N_ELEMENTS(chorusfx_pitch_shift_settings)},
    {CHORUS_TYPE_DETUNE, "Detune", chorusfx_detune_settings, G_N_ELEMENTS(chorusfx_detune_settings)},
    {CHORUS_TYPE_IPS, "Harmony Pitch", chorusfx_ips_settings, G_N_ELEMENTS(chorusfx_ips_settings)},
    {CHORUS_TYPE_OCTAVER, "Octaver", chorusfx_octaver_settings, G_N_ELEMENTS(chorusfx_octaver_settings)},
};

static EffectGroup rp500_chorusfx_group[] = {
    {CHORUS_TYPE_CE, "CE Chorus", chorusfx_ce_settings, G_N_ELEMENTS(chorusfx_ce_settings)},
    {CHORUS_TYPE_TC, "TC Chorus", chorusfx_tc_settings, G_N_ELEMENTS(chorusfx_tc_settings)},
    {CHORUS_TYPE_DUAL, "Dual Chorus", chorusfx_dual_settings, G_N_ELEMENTS(chorusfx_dual_settings)},
    {CHORUS_TYPE_GLISTEN, "Glisten Chorus", chorusfx_glisten_settings, G_N_ELEMENTS(chorusfx_glisten_settings)},
    {CHORUS_TYPE_MULTI, "Multi Chorus", chorusfx_multi_settings, G_N_ELEMENTS(chorusfx_multi_settings)},
    {CHORUS_TYPE_FLANGER, "Flanger", chorusfx_flanger_settings, G_N_ELEMENTS(chorusfx_flanger_settings)},
    {CHORUS_TYPE_TRIGGERED_FLANGER, "Triggered Flanger", chorusfx_trig_flanger_settings, G_N_ELEMENTS(chorusfx_trig_flanger_settings)},
    {CHORUS_TYPE_MXR_FLANGER, "MXR FLANGER", chorusfx_mxr_flanger_settings, G_N_ELEMENTS(chorusfx_mxr_flanger_settings)},
    {CHORUS_TYPE_EH_FLANGER, "EH Flanger", chorusfx_eh_flanger_settings, G_N_ELEMENTS(chorusfx_eh_flanger_settings)},
    {CHORUS_TYPE_AD_FLANGER, "AD Flanger", chorusfx_ad_flanger_settings, G_N_ELEMENTS(chorusfx_ad_flanger_settings)},
    {CHORUS_TYPE_PHASER, "Phaser", chorusfx_phaser_settings, G_N_ELEMENTS(chorusfx_phaser_settings)},
    {CHORUS_TYPE_TRIGGERED_PHASER, "Triggered Phaser", chorusfx_trig_phaser_settings, G_N_ELEMENTS(chorusfx_trig_phaser_settings)},
    {CHORUS_TYPE_MX_PHASER, "MX Phaser", chorusfx_mx_phaser_settings, G_N_ELEMENTS(chorusfx_mx_phaser_settings)},
    {CHORUS_TYPE_EH_PHASER, "EH Phaser", chorusfx_eh_phaser_settings, G_N_ELEMENTS(chorusfx_eh_phaser_settings)},
    {CHORUS_TYPE_VIBRATO, "Vibrato", chorusfx_vibrato_settings, G_N_ELEMENTS(chorusfx_vibrato_settings)},
    {CHORUS_TYPE_ROTARY, "Rotary", chorusfx_rotary_settings, G_N_ELEMENTS(chorusfx_rotary_settings)},
    {CHORUS_TYPE_VIBROPAN, "Vibropan", chorusfx_vibropan_settings, G_N_ELEMENTS(chorusfx_vibropan_settings)},
    {CHORUS_TYPE_UNOVIBE, "Unovibe", chorusfx_unovibe_settings, G_N_ELEMENTS(chorusfx_unovibe_settings)},
    {CHORUS_TYPE_TREMOLO, "Tremolo", chorusfx_tremolo_settings, G_N_ELEMENTS(chorusfx_tremolo_settings)},
    {CHORUS_TYPE_SCATTER_TREM, "ScatterTrem", chorusfx_trem_settings, G_N_ELEMENTS(chorusfx_trem_settings)},
    {CHORUS_TYPE_OPTO_TREMOLO, "Opto Tremolo", chorusfx_trem_settings, G_N_ELEMENTS(chorusfx_trem_settings)},
    {CHORUS_TYPE_BIAS_TREMOLO, "Bias Tremolo", chorusfx_trem_settings, G_N_ELEMENTS(chorusfx_trem_settings)},
    {CHORUS_TYPE_PANNER, "Panner", chorusfx_panner_settings, G_N_ELEMENTS(chorusfx_panner_settings)},
    {CHORUS_TYPE_ENVELOPE, "Envelope", chorusfx_envelope_settings, G_N_ELEMENTS(chorusfx_envelope_settings)},
    {CHORUS_TYPE_FX25, "FX25", chorusfx_fx25_settings, G_N_ELEMENTS(chorusfx_fx25_settings)},
    {CHORUS_TYPE_AUTOYA, "AutoYa", chorusfx_autoya_settings, G_N_ELEMENTS(chorusfx_autoya_settings)},
    {CHORUS_TYPE_YAYA, "YaYa", chorusfx_yaya_settings, G_N_ELEMENTS(chorusfx_yaya_settings)},
    {CHORUS_TYPE_SYNTH_TALK, "Synth Talk", chorusfx_synth_talk_settings, G_N_ELEMENTS(chorusfx_synth_talk_settings)},
    {CHORUS_TYPE_STEP_FILTER, "Step Filter", chorusfx_step_filter_settings, G_N_ELEMENTS(chorusfx_step_filter_settings)},
    {CHORUS_TYPE_SAMPLE_HOLD, "Sample/Hold", chorusfx_sample_hold_settings, G_N_ELEMENTS(chorusfx_sample_hold_settings)},
    {CHORUS_TYPE_WHAMMY, "Whammy", chorusfx_whammy_settings, G_N_ELEMENTS(chorusfx_whammy_settings)},
    {CHORUS_TYPE_PITCH_SHIFT, "Pitch", chorusfx_pitch_shift_settings, G_N_ELEMENTS(chorusfx_pitch_shift_settings)},
    {CHORUS_TYPE_DETUNE, "Detune", chorusfx_detune_settings, G_N_ELEMENTS(chorusfx_detune_settings)},
    {CHORUS_TYPE_IPS, "Harmony Pitch", chorusfx_ips_settings, G_N_ELEMENTS(chorusfx_ips_settings)},
    {CHORUS_TYPE_OCTAVER, "Octaver", chorusfx_octaver_settings, G_N_ELEMENTS(chorusfx_octaver_settings)},
};

static EffectGroup gnx3k_delay_group[] = {
    {DELAY_GNX3K_TYPE_MONO, "Mono", gnx3k_delay_settings, G_N_ELEMENTS(gnx3k_delay_settings)},
    {DELAY_GNX3K_TYPE_PINGPONG, "Ping-Pong", gnx3k_delay_settings, G_N_ELEMENTS(gnx3k_delay_settings)},
    {DELAY_GNX3K_TYPE_ANALOG, "Analog", gnx3k_delay_settings, G_N_ELEMENTS(gnx3k_delay_settings)},
    {DELAY_GNX3K_TYPE_ANAPONG, "Analog-Pong", gnx3k_delay_settings, G_N_ELEMENTS(gnx3k_delay_settings)},
    {DELAY_GNX3K_TYPE_SPREAD, "Spread", gnx3k_delay_spread_settings, G_N_ELEMENTS(gnx3k_delay_spread_settings)},
};

static EffectGroup rp250_delay_group[] = {
    {DELAY_TYPE_ANALOG, "Analog", delay_analog_settings, G_N_ELEMENTS(delay_analog_settings)},
    {DELAY_TYPE_DIGITAL, "Digital", delay_digital_settings, G_N_ELEMENTS(delay_digital_settings)},
    {DELAY_TYPE_MODULATED, "Modulated", delay_modulated_settings, G_N_ELEMENTS(delay_modulated_settings)},
    {DELAY_TYPE_PONG, "Pong", delay_pong_settings, G_N_ELEMENTS(delay_pong_settings)},
    {DELAY_TYPE_TAPE, "Tape", delay_tape_settings, G_N_ELEMENTS(delay_tape_settings)},
};

static EffectGroup rp355_delay_group[] = {
    {DELAY_TYPE_ANALOG, "Analog", delay_analog_settings, G_N_ELEMENTS(delay_analog_settings)},
    {DELAY_TYPE_DM, "DM Delay", delay_dm_settings, G_N_ELEMENTS(delay_dm_settings)},
    {DELAY_TYPE_DIGITAL, "Digital", delay_digital_settings, G_N_ELEMENTS(delay_digital_settings)},
    {DELAY_TYPE_MODULATED, "Modulated", delay_modulated_settings, G_N_ELEMENTS(delay_modulated_settings)},
    {DELAY_TYPE_PONG, "Pong", delay_pong_settings, G_N_ELEMENTS(delay_pong_settings)},
    {DELAY_TYPE_TAPE, "Tape", delay_tape_settings, G_N_ELEMENTS(delay_tape_settings)},
    {DELAY_TYPE_ECHOPLEX, "Echo Plex", delay_echoplex_settings, G_N_ELEMENTS(delay_echoplex_settings)},
};

static EffectGroup rp500_delay_group[] = {
    {DELAY_RP500_TYPE_DIGITAL, "Digital", rp500_delay_digital_settings, G_N_ELEMENTS(rp500_delay_digital_settings)},
    {DELAY_RP500_TYPE_ANALOG, "Analog", rp500_delay_analog_settings, G_N_ELEMENTS(rp500_delay_analog_settings)},
    {DELAY_RP500_TYPE_DM, "DM Delay", rp500_delay_dm_settings, G_N_ELEMENTS(rp500_delay_dm_settings)},
    {DELAY_RP500_TYPE_ECHOPLEX, "Echo Plex", rp500_delay_echoplex_settings, G_N_ELEMENTS(rp500_delay_echoplex_settings)},
    {DELAY_RP500_TYPE_MODULATED, "Modulated", rp500_delay_modulated_settings, G_N_ELEMENTS(rp500_delay_modulated_settings)},
    {DELAY_RP500_TYPE_PONG, "Pong", rp500_delay_pong_settings, G_N_ELEMENTS(rp500_delay_pong_settings)},
    {DELAY_RP500_TYPE_REVERSE, "Reverse", rp500_delay_reverse_settings, G_N_ELEMENTS(rp500_delay_reverse_settings)},
    {DELAY_RP500_TYPE_TAPE, "Tape", rp500_delay_tape_settings, G_N_ELEMENTS(rp500_delay_tape_settings)},
};

static EffectGroup gnx3k_reverb_group[] = {
    {GNX3K_REVERB_TYPE_STUDIO, "Studio", gnx3k_reverb_settings, G_N_ELEMENTS(gnx3k_reverb_settings)},
    {GNX3K_REVERB_TYPE_ROOM, "Room", gnx3k_reverb_settings, G_N_ELEMENTS(gnx3k_reverb_settings)},
    {GNX3K_REVERB_TYPE_CLUB, "Club", gnx3k_reverb_settings, G_N_ELEMENTS(gnx3k_reverb_settings)},
    {GNX3K_REVERB_TYPE_PLATE, "Plate", gnx3k_reverb_settings, G_N_ELEMENTS(gnx3k_reverb_settings)},
    {GNX3K_REVERB_TYPE_HALL, "Hall", gnx3k_reverb_settings, G_N_ELEMENTS(gnx3k_reverb_settings)},
    {GNX3K_REVERB_TYPE_AMPHITHEATER, "Amphitheater", gnx3k_reverb_settings, G_N_ELEMENTS(gnx3k_reverb_settings)},
    {GNX3K_REVERB_TYPE_CHURCH, "Church", gnx3k_reverb_settings, G_N_ELEMENTS(gnx3k_reverb_settings)},
    {GNX3K_REVERB_TYPE_GARAGE, "Garage", gnx3k_reverb_settings, G_N_ELEMENTS(gnx3k_reverb_settings)},
    {GNX3K_REVERB_TYPE_ARENA, "Arena", gnx3k_reverb_settings, G_N_ELEMENTS(gnx3k_reverb_settings)},
    {GNX3K_REVERB_TYPE_SPRING, "Spring", gnx3k_reverb_settings, G_N_ELEMENTS(gnx3k_reverb_settings)},
};

static EffectGroup reverb_group[] = {
    {REVERB_TYPE_TWIN, "Twin", reverb_twin_settings, G_N_ELEMENTS(reverb_twin_settings)},
    {REVERB_TYPE_LEX_AMBIENCE, "Lexicon ambience", reverb_lex_settings, G_N_ELEMENTS(reverb_lex_settings)},
    {REVERB_TYPE_LEX_STUDIO, "Lexicon studio", reverb_lex_settings, G_N_ELEMENTS(reverb_lex_settings)},
    {REVERB_TYPE_LEX_ROOM, "Lexicon room", reverb_lex_settings, G_N_ELEMENTS(reverb_lex_settings)},
    {REVERB_TYPE_LEX_HALL, "Lexicon hall", reverb_lex_settings, G_N_ELEMENTS(reverb_lex_settings)},
    {REVERB_TYPE_EMT240_PLATE, "EMT240 Plate", reverb_lex_settings, G_N_ELEMENTS(reverb_lex_settings)},
};

static EffectGroup rp250_amp_group[] = {
    {AMP_TYPE_TWEED_CHAMP, "Tweed Champ", rp250_amp_settings, G_N_ELEMENTS(rp250_amp_settings)},
    {AMP_TYPE_TWEED_DELUXE, "Tweed Deluxe", rp250_amp_settings, G_N_ELEMENTS(rp250_amp_settings)},
    {AMP_TYPE_TWEED_BASSMAN, "Tweed Bassman", rp250_amp_settings, G_N_ELEMENTS(rp250_amp_settings)},
    {AMP_TYPE_BLACKFACE_TWIN, "Blackface Twin", rp250_amp_settings, G_N_ELEMENTS(rp250_amp_settings)},
    {AMP_TYPE_BLACKFACE_DELUXE, "Blackface Deluxe", rp250_amp_settings, G_N_ELEMENTS(rp250_amp_settings)},
    {AMP_TYPE_SUPER_LEAD_PLEXI, "Super Lead Plexi", rp250_amp_settings, G_N_ELEMENTS(rp250_amp_settings)},
    {AMP_TYPE_MASTER_VOLUME, "Master Volume", rp250_amp_settings, G_N_ELEMENTS(rp250_amp_settings)},
    {AMP_TYPE_JCM800, "JCM800", rp250_amp_settings, G_N_ELEMENTS(rp250_amp_settings)},
    {AMP_TYPE_JCM900, "JCM900", rp250_amp_settings, G_N_ELEMENTS(rp250_amp_settings)},
    {AMP_TYPE_AC15, "AC15", rp250_amp_settings, G_N_ELEMENTS(rp250_amp_settings)},
    {AMP_TYPE_AC30TB, "AC30TB", rp250_amp_settings, G_N_ELEMENTS(rp250_amp_settings)},
    {AMP_TYPE_HIWATT_100, "Hiwatt 100", rp250_amp_settings, G_N_ELEMENTS(rp250_amp_settings)},
    {AMP_TYPE_BOOGIE_MARK_II, "Boogie Mark II", rp250_amp_settings, G_N_ELEMENTS(rp250_amp_settings)},
    {AMP_TYPE_DUAL_RECTIFIER, "Dual Rectifier", rp250_amp_settings, G_N_ELEMENTS(rp250_amp_settings)},
    {AMP_TYPE_MATCHLESS_HC30, "Matchless HC30", rp250_amp_settings, G_N_ELEMENTS(rp250_amp_settings)},
    {AMP_TYPE_SOLO, "Solo", rp250_amp_settings, G_N_ELEMENTS(rp250_amp_settings)},
    {AMP_TYPE_METAL, "Metal", rp250_amp_settings, G_N_ELEMENTS(rp250_amp_settings)},
    {AMP_TYPE_BRIGHT, "Bright", rp250_amp_settings, G_N_ELEMENTS(rp250_amp_settings)},
    {AMP_TYPE_CLEAN, "Clean", rp250_amp_settings, G_N_ELEMENTS(rp250_amp_settings)},
    {AMP_TYPE_HIGH_GAIN, "High Gain", rp250_amp_settings, G_N_ELEMENTS(rp250_amp_settings)},
    {AMP_TYPE_ACOUSTIC, "Acoustic", rp250_amp_settings2, G_N_ELEMENTS(rp250_amp_settings2)},
    {AMP_TYPE_DIRECT, "Direct", rp250_amp_settings2, G_N_ELEMENTS(rp250_amp_settings2)},
};

static EffectGroup rp355_amp_select_group[] = {
    {AMP_CHANNEL_A, "A", NULL, -1},
    {AMP_CHANNEL_B, "B", NULL, -1},
};

static EffectGroup rp355_amp_group_A[] = {
    {AMP_TYPE_TWEED_CHAMP, "Tweed Champ", rp355_amp_settings_A, G_N_ELEMENTS(rp355_amp_settings_A)},
    {AMP_TYPE_TWEED_DELUXE, "Tweed Deluxe", rp355_amp_settings_A, G_N_ELEMENTS(rp355_amp_settings_A)},
    {AMP_TYPE_TWEED_BASSMAN, "Tweed Bassman", rp355_amp_settings_A, G_N_ELEMENTS(rp355_amp_settings_A)},
    {AMP_TYPE_BLACKFACE_TWIN, "Blackface Twin", rp355_amp_settings_A, G_N_ELEMENTS(rp355_amp_settings_A)},
    {AMP_TYPE_BLACKFACE_DELUXE, "Blackface Deluxe", rp355_amp_settings_A, G_N_ELEMENTS(rp355_amp_settings_A)},
    {AMP_TYPE_PLEXI_JTM_45, "Plexi JTM-45", rp355_amp_settings_A, G_N_ELEMENTS(rp355_amp_settings_A)},
    {AMP_TYPE_SUPER_LEAD_PLEXI, "Plexi Lead", rp355_amp_settings_A, G_N_ELEMENTS(rp355_amp_settings_A)},
    {AMP_TYPE_PLEXI_JUMP_PANEL, "Plexi Jump Panel", rp355_amp_settings_A, G_N_ELEMENTS(rp355_amp_settings_A)},
    {AMP_TYPE_MASTER_VOLUME, "Master Volume", rp355_amp_settings_A, G_N_ELEMENTS(rp355_amp_settings_A)},
    {AMP_TYPE_JCM800, "JCM800", rp355_amp_settings_A, G_N_ELEMENTS(rp355_amp_settings_A)},
    {AMP_TYPE_JCM900, "JCM900", rp355_amp_settings_A, G_N_ELEMENTS(rp355_amp_settings_A)},
    {AMP_TYPE_AC15, "AC-15", rp355_amp_settings_A, G_N_ELEMENTS(rp355_amp_settings_A)},
    {AMP_TYPE_AC30TB, "AC-30 TB", rp355_amp_settings_A, G_N_ELEMENTS(rp355_amp_settings_A)},
    {AMP_TYPE_HIWATT_100, "Hiwatt 100", rp355_amp_settings_A, G_N_ELEMENTS(rp355_amp_settings_A)},
    {AMP_TYPE_BOOGIE_MARK_II, "Boogie Mark IIC", rp355_amp_settings_A, G_N_ELEMENTS(rp355_amp_settings_A)},
    {AMP_TYPE_DUAL_RECTIFIER, "Dual Rectifier", rp355_amp_settings_A, G_N_ELEMENTS(rp355_amp_settings_A)},
    {AMP_TYPE_MATCHLESS_HC30, "Matchless HC30", rp355_amp_settings_A, G_N_ELEMENTS(rp355_amp_settings_A)},
    {AMP_TYPE_SOLDANO_100, "Soldano 100", rp355_amp_settings_A, G_N_ELEMENTS(rp355_amp_settings_A)},
    {AMP_TYPE_SOLO, "Solo", rp355_amp_settings_A, G_N_ELEMENTS(rp355_amp_settings_A)},
    {AMP_TYPE_METAL, "Metal", rp355_amp_settings_A, G_N_ELEMENTS(rp355_amp_settings_A)},
    {AMP_TYPE_BRIGHT, "Bright Clean", rp355_amp_settings_A, G_N_ELEMENTS(rp355_amp_settings_A)},
    {AMP_TYPE_CHUNK, "Chunk", rp355_amp_settings_A, G_N_ELEMENTS(rp355_amp_settings_A)},
    {AMP_TYPE_CLEAN, "Clean Tube", rp355_amp_settings_A, G_N_ELEMENTS(rp355_amp_settings_A)},
    {AMP_TYPE_HIGH_GAIN, "High Gain", rp355_amp_settings_A, G_N_ELEMENTS(rp355_amp_settings_A)},
    {AMP_TYPE_GSP2101_CLEAN_TUBE, "GSP2101 Clean Tube", rp355_amp_settings_A, G_N_ELEMENTS(rp355_amp_settings_A)},
    {AMP_TYPE_GSP2101_SAT_TUBE, "GSP2101 Sat Tube", rp355_amp_settings_A, G_N_ELEMENTS(rp355_amp_settings_A)},
    {AMP_TYPE_MONSTER, "Monster", rp355_amp_settings_A, G_N_ELEMENTS(rp355_amp_settings_A)},
    {AMP_TYPE_TWEEDFACE, "Tweedface", rp355_amp_settings_A, G_N_ELEMENTS(rp355_amp_settings_A)},
    {AMP_TYPE_BLACKBASS, "Blackbass", rp355_amp_settings_A, G_N_ELEMENTS(rp355_amp_settings_A)},
    {AMP_TYPE_STONER_ROCK, "Stoner Rock", rp355_amp_settings_A, G_N_ELEMENTS(rp355_amp_settings_A)},
    {AMP_TYPE_DARK_METAL, "Dark Metal", rp355_amp_settings_A, G_N_ELEMENTS(rp355_amp_settings_A)},
    {AMP_TYPE_BROWN_SOUND, "Brown Sound", rp355_amp_settings_A, G_N_ELEMENTS(rp355_amp_settings_A)},
    {AMP_TYPE_ACOUSTIC, "Dread Acoustic", rp355_amp_settings2_A, G_N_ELEMENTS(rp355_amp_settings2_A)},
    {AMP_TYPE_JUMBO_ACOUSTIC, "Jumbo Acoustic", rp355_amp_settings2_A, G_N_ELEMENTS(rp355_amp_settings2_A)},
    {AMP_TYPE_DIRECT, "Direct", rp355_amp_settings2_A, G_N_ELEMENTS(rp355_amp_settings2_A)},
};

static EffectGroup rp355_amp_group_B[] = {
    {AMP_TYPE_TWEED_CHAMP, "Tweed Champ", rp355_amp_settings_B, G_N_ELEMENTS(rp355_amp_settings_B)},
    {AMP_TYPE_TWEED_DELUXE, "Tweed Deluxe", rp355_amp_settings_B, G_N_ELEMENTS(rp355_amp_settings_B)},
    {AMP_TYPE_TWEED_BASSMAN, "Tweed Bassman", rp355_amp_settings_B, G_N_ELEMENTS(rp355_amp_settings_B)},
    {AMP_TYPE_BLACKFACE_TWIN, "Blackface Twin", rp355_amp_settings_B, G_N_ELEMENTS(rp355_amp_settings_B)},
    {AMP_TYPE_BLACKFACE_DELUXE, "Blackface Deluxe", rp355_amp_settings_B, G_N_ELEMENTS(rp355_amp_settings_B)},
    {AMP_TYPE_PLEXI_JTM_45, "Plexi JTM-45", rp355_amp_settings_B, G_N_ELEMENTS(rp355_amp_settings_B)},
    {AMP_TYPE_SUPER_LEAD_PLEXI, "Plexi Lead", rp355_amp_settings_B, G_N_ELEMENTS(rp355_amp_settings_B)},
    {AMP_TYPE_PLEXI_JUMP_PANEL, "Plexi Jump Panel", rp355_amp_settings_B, G_N_ELEMENTS(rp355_amp_settings_B)},
    {AMP_TYPE_MASTER_VOLUME, "Master Volume", rp355_amp_settings_B, G_N_ELEMENTS(rp355_amp_settings_B)},
    {AMP_TYPE_JCM800, "JCM800", rp355_amp_settings_B, G_N_ELEMENTS(rp355_amp_settings_B)},
    {AMP_TYPE_JCM900, "JCM900", rp355_amp_settings_B, G_N_ELEMENTS(rp355_amp_settings_B)},
    {AMP_TYPE_AC15, "AC-15", rp355_amp_settings_B, G_N_ELEMENTS(rp355_amp_settings_B)},
    {AMP_TYPE_AC30TB, "AC-30 TB", rp355_amp_settings_B, G_N_ELEMENTS(rp355_amp_settings_B)},
    {AMP_TYPE_HIWATT_100, "Hiwatt 100", rp355_amp_settings_B, G_N_ELEMENTS(rp355_amp_settings_B)},
    {AMP_TYPE_BOOGIE_MARK_II, "Boogie Mark IIC", rp355_amp_settings_B, G_N_ELEMENTS(rp355_amp_settings_B)},
    {AMP_TYPE_DUAL_RECTIFIER, "Dual Rectifier", rp355_amp_settings_B, G_N_ELEMENTS(rp355_amp_settings_B)},
    {AMP_TYPE_MATCHLESS_HC30, "Matchless HC30", rp355_amp_settings_B, G_N_ELEMENTS(rp355_amp_settings_B)},
    {AMP_TYPE_SOLDANO_100, "Soldano 100", rp355_amp_settings_B, G_N_ELEMENTS(rp355_amp_settings_B)},
    {AMP_TYPE_SOLO, "Solo", rp355_amp_settings_B, G_N_ELEMENTS(rp355_amp_settings_B)},
    {AMP_TYPE_METAL, "Metal", rp355_amp_settings_B, G_N_ELEMENTS(rp355_amp_settings_B)},
    {AMP_TYPE_BRIGHT, "Bright Clean", rp355_amp_settings_B, G_N_ELEMENTS(rp355_amp_settings_B)},
    {AMP_TYPE_CHUNK, "Chunk", rp355_amp_settings_B, G_N_ELEMENTS(rp355_amp_settings_B)},
    {AMP_TYPE_CLEAN, "Clean Tube", rp355_amp_settings_B, G_N_ELEMENTS(rp355_amp_settings_B)},
    {AMP_TYPE_HIGH_GAIN, "High Gain", rp355_amp_settings_B, G_N_ELEMENTS(rp355_amp_settings_B)},
    {AMP_TYPE_GSP2101_CLEAN_TUBE, "GSP2101 Clean Tube", rp355_amp_settings_B, G_N_ELEMENTS(rp355_amp_settings_B)},
    {AMP_TYPE_GSP2101_SAT_TUBE, "GSP2101 Sat Tube", rp355_amp_settings_B, G_N_ELEMENTS(rp355_amp_settings_B)},
    {AMP_TYPE_MONSTER, "Monster", rp355_amp_settings_B, G_N_ELEMENTS(rp355_amp_settings_B)},
    {AMP_TYPE_TWEEDFACE, "Tweedface", rp355_amp_settings_B, G_N_ELEMENTS(rp355_amp_settings_B)},
    {AMP_TYPE_BLACKBASS, "Blackbass", rp355_amp_settings_B, G_N_ELEMENTS(rp355_amp_settings_B)},
    {AMP_TYPE_STONER_ROCK, "Stoner Rock", rp355_amp_settings_B, G_N_ELEMENTS(rp355_amp_settings_B)},
    {AMP_TYPE_DARK_METAL, "Dark Metal", rp355_amp_settings_B, G_N_ELEMENTS(rp355_amp_settings_B)},
    {AMP_TYPE_BROWN_SOUND, "Brown Sound", rp355_amp_settings_B, G_N_ELEMENTS(rp355_amp_settings_B)},
    {AMP_TYPE_ACOUSTIC, "Dread Acoustic", rp355_amp_settings2_B, G_N_ELEMENTS(rp355_amp_settings2_B)},
    {AMP_TYPE_JUMBO_ACOUSTIC, "Jumbo Acoustic", rp355_amp_settings2_B, G_N_ELEMENTS(rp355_amp_settings2_B)},
    {AMP_TYPE_DIRECT, "Direct", rp355_amp_settings2_B, G_N_ELEMENTS(rp355_amp_settings2_B)},
};

static EffectGroup rp500_amp_group[] = {
    {AMP_TYPE_TWEED_CHAMP, "Tweed Champ", rp500_amp_settings, G_N_ELEMENTS(rp500_amp_settings)},
    {AMP_TYPE_TWEED_DELUXE, "Tweed Deluxe", rp500_amp_settings, G_N_ELEMENTS(rp500_amp_settings)},
    {AMP_TYPE_TWEED_BASSMAN, "Tweed Bassman", rp500_amp_settings, G_N_ELEMENTS(rp500_amp_settings)},
    {AMP_TYPE_BROWN_BASSMAN, "Brown Bassman", rp500_amp_settings, G_N_ELEMENTS(rp500_amp_settings)},
    {AMP_TYPE_BLACKFACE_TWIN, "Blackface Twin", rp500_amp_settings, G_N_ELEMENTS(rp500_amp_settings)},
    {AMP_TYPE_BLACKFACE_DELUXE, "Blackface Deluxe", rp500_amp_settings, G_N_ELEMENTS(rp500_amp_settings)},
    {AMP_TYPE_PLEXI_JTM_45, "Plexi JTM-45", rp500_amp_settings, G_N_ELEMENTS(rp500_amp_settings)},
    {AMP_TYPE_SUPER_LEAD_PLEXI, "Plexi Lead", rp500_amp_settings, G_N_ELEMENTS(rp500_amp_settings)},
    {AMP_TYPE_PLEXI_JUMP_PANEL, "Plexi Jump Panel", rp500_amp_settings, G_N_ELEMENTS(rp500_amp_settings)},
    {AMP_TYPE_MASTER_VOLUME, "Master Volume", rp500_amp_settings, G_N_ELEMENTS(rp500_amp_settings)},
    {AMP_TYPE_JCM800, "JCM800", rp500_amp_settings, G_N_ELEMENTS(rp500_amp_settings)},
    {AMP_TYPE_JCM900, "JCM900", rp500_amp_settings, G_N_ELEMENTS(rp500_amp_settings)},
    {AMP_TYPE_JCM2000, "JCM2000", rp500_amp_settings, G_N_ELEMENTS(rp500_amp_settings)},
    {AMP_TYPE_AC15, "AC-15", rp500_amp_settings, G_N_ELEMENTS(rp500_amp_settings)},
    {AMP_TYPE_AC30TB, "AC-30 TB", rp500_amp_settings, G_N_ELEMENTS(rp500_amp_settings)},
    {AMP_TYPE_HIWATT_100, "Hiwatt 100", rp500_amp_settings, G_N_ELEMENTS(rp500_amp_settings)},
    {AMP_TYPE_BOOGIE_MARK_II, "Boogie Mark IIC", rp500_amp_settings, G_N_ELEMENTS(rp500_amp_settings)},
    {AMP_TYPE_BOOGIE_MARK_IV, "Boogie Mark IV", rp500_amp_settings, G_N_ELEMENTS(rp500_amp_settings)},
    {AMP_TYPE_DUAL_RECTIFIER, "Dual Rectifier", rp500_amp_settings, G_N_ELEMENTS(rp500_amp_settings)},
    {AMP_TYPE_TRIPLE_RECTIFIER, "Triple Rectifier", rp500_amp_settings, G_N_ELEMENTS(rp500_amp_settings)},
    {AMP_TYPE_LEGACY_VL100, "Legacy VL-100", rp500_amp_settings, G_N_ELEMENTS(rp500_amp_settings)},
    {AMP_TYPE_MATCHLESS_HC30, "Matchless HC30", rp500_amp_settings, G_N_ELEMENTS(rp500_amp_settings)},
    {AMP_TYPE_SOLDANO_100, "Soldano 100", rp500_amp_settings, G_N_ELEMENTS(rp500_amp_settings)},
    {AMP_TYPE_SUPERGROUP, "Supergroup", rp500_amp_settings, G_N_ELEMENTS(rp500_amp_settings)},
    {AMP_TYPE_GA40, "GA-40", rp500_amp_settings, G_N_ELEMENTS(rp500_amp_settings)},
    {AMP_TYPE_OR120, "OR-120", rp500_amp_settings, G_N_ELEMENTS(rp500_amp_settings)},
    {AMP_TYPE_PV5150II, "PV 5150II", rp500_amp_settings, G_N_ELEMENTS(rp500_amp_settings)},
    {AMP_TYPE_RG100, "RG100", rp500_amp_settings, G_N_ELEMENTS(rp500_amp_settings)},
    {AMP_TYPE_JC120_JAZZ, "JC-120 Jazz", rp500_amp_settings, G_N_ELEMENTS(rp500_amp_settings)},
    {AMP_TYPE_SOLAR100, "Solar 100", rp500_amp_settings, G_N_ELEMENTS(rp500_amp_settings)},
    {AMP_TYPE_SOLO, "Solo", rp500_amp_settings, G_N_ELEMENTS(rp500_amp_settings)},
    {AMP_TYPE_METAL, "Metal", rp500_amp_settings, G_N_ELEMENTS(rp500_amp_settings)},
    {AMP_TYPE_BRIGHT, "Bright Clean", rp500_amp_settings, G_N_ELEMENTS(rp500_amp_settings)},
    {AMP_TYPE_CHUNK, "Chunk", rp500_amp_settings, G_N_ELEMENTS(rp500_amp_settings)},
    {AMP_TYPE_CLEAN, "Clean Tube", rp500_amp_settings, G_N_ELEMENTS(rp500_amp_settings)},
    {AMP_TYPE_HIGH_GAIN, "High Gain", rp500_amp_settings, G_N_ELEMENTS(rp500_amp_settings)},
    {AMP_TYPE_BLUES, "Blues", rp500_amp_settings, G_N_ELEMENTS(rp500_amp_settings)},
    {AMP_TYPE_FUZZ, "Fuzz", rp500_amp_settings, G_N_ELEMENTS(rp500_amp_settings)},
    {AMP_TYPE_SPANK, "Spank", rp500_amp_settings, G_N_ELEMENTS(rp500_amp_settings)},
    {AMP_TYPE_GSP2101_CLEAN_TUBE, "GSP2101 Clean Tube", rp500_amp_settings, G_N_ELEMENTS(rp500_amp_settings)},
    {AMP_TYPE_GSP2101_SAT_TUBE, "GSP2101 Sat Tube", rp500_amp_settings, G_N_ELEMENTS(rp500_amp_settings)},
    {AMP_TYPE_CRUNCH, "Crunch", rp500_amp_settings, G_N_ELEMENTS(rp500_amp_settings)},
    {AMP_TYPE_MONSTER, "Monster", rp500_amp_settings, G_N_ELEMENTS(rp500_amp_settings)},
    {AMP_TYPE_TWEEDFACE, "Tweedface", rp500_amp_settings, G_N_ELEMENTS(rp500_amp_settings)},
    {AMP_TYPE_BLACKBASS, "Blackbass", rp500_amp_settings, G_N_ELEMENTS(rp500_amp_settings)},
    {AMP_TYPE_STONER_ROCK, "Stoner Rock", rp500_amp_settings, G_N_ELEMENTS(rp500_amp_settings)},
    {AMP_TYPE_DARK_METAL, "Dark Metal", rp500_amp_settings, G_N_ELEMENTS(rp500_amp_settings)},
    {AMP_TYPE_TRANSISTOR, "Transistor", rp500_amp_settings, G_N_ELEMENTS(rp500_amp_settings)},
    {AMP_TYPE_BROWN_SOUND, "Brown Sound", rp500_amp_settings, G_N_ELEMENTS(rp500_amp_settings)},
    {AMP_TYPE_MOSH, "Mosh", rp500_amp_settings, G_N_ELEMENTS(rp500_amp_settings)},
    {AMP_TYPE_ACOUSTIC, "Dread Acoustic", rp500_amp_settings2, G_N_ELEMENTS(rp500_amp_settings2)},
    {AMP_TYPE_JUMBO_ACOUSTIC, "Jumbo Acoustic", rp500_amp_settings2, G_N_ELEMENTS(rp500_amp_settings2)},
    {AMP_TYPE_DIRECT, "Direct", rp500_amp_settings3, G_N_ELEMENTS(rp500_amp_settings3)},
};

static EffectGroup gnx3k_amp_group[] = {
    {AMP_TYPE_DIRECT, "Direct", NULL, -1},
    {AMP_TYPE_TWEED_CHAMP, "Tweed Champ", NULL, -1},
    {AMP_TYPE_TWEED_DELUXE, "Tweed Deluxe", NULL, -1},
    {AMP_TYPE_TWEED_BASSMAN, "Tweed Bassman", NULL, -1},
    {AMP_TYPE_BROWN_BASSMAN, "Brown Bassman", NULL, -1},
    {AMP_TYPE_BLACKFACE_TWIN, "Blackface Twin", NULL, -1},
    {AMP_TYPE_BLACKFACE_DELUXE, "Blackface Deluxe", NULL, -1},
    {AMP_TYPE_PLEXI_JTM_45, "Plexi JTM-45", NULL, -1},
    {AMP_TYPE_SUPER_LEAD_PLEXI, "Plexi Lead", NULL, -1},
    {AMP_TYPE_PLEXI_JUMP_PANEL, "Plexi Jump Panel", NULL, -1},
    {AMP_TYPE_MASTER_VOLUME, "Master Volume", NULL, -1},
    {AMP_TYPE_JCM800, "JCM800", NULL, -1},
    {AMP_TYPE_JCM900, "JCM900", NULL, -1},
    {AMP_TYPE_JCM2000, "JCM2000", NULL, -1},
    {AMP_TYPE_BOOGIE_MARK_II, "Boogie Mark II", NULL, -1},
    {AMP_TYPE_DUAL_RECTIFIER, "Dual Rectifier", NULL, -1},
    {AMP_TYPE_AC15, "AC-15", NULL, -1},
    {AMP_TYPE_AC30TB, "AC-30 TB", NULL, -1},
    {AMP_TYPE_HIWATT_100, "Hiwatt 100", NULL, -1},
    {AMP_TYPE_SOLO, "Solo", NULL, -1},
    {AMP_TYPE_MATCHLESS_HC30, "Matchless HC30", NULL, -1},
    {AMP_TYPE_LEGACY_VL100, "Legacy VL-100", NULL, -1},
    {AMP_TYPE_GSP2101_CLEAN_TUBE, "GSP2101 Clean Tube", NULL, -1},
    {AMP_TYPE_GSP2101_SAT_TUBE, "GSP2101 Sat Tube", NULL, -1},
    {AMP_TYPE_SPANK, "Spank", NULL, -1},
    {AMP_TYPE_METAL, "Metal", NULL, -1},
    {AMP_TYPE_BRIGHT, "Bright Clean", NULL, -1},
    {AMP_TYPE_CHUNK, "Chunk", NULL, -1},
    {AMP_TYPE_CLEAN, "Clean Tube", NULL, -1},
    {AMP_TYPE_CRUNCH, "Crunch", NULL, -1},
    {AMP_TYPE_HIGH_GAIN, "High Gain", NULL, -1},
    {AMP_TYPE_BLUES, "Blues", NULL, -1},
    {AMP_TYPE_FUZZ, "Fuzz", NULL, -1},
    {AMP_TYPE_JUMBO_ACOUSTIC, "Jumbo Acoustic", NULL, -1},
    {AMP_TYPE_ACOUSTIC, "Dread Acoustic", NULL, -1},
    {AMP_TYPE_SVT_ROCK_BASS, "SVT Rock Bass", NULL, -1},
    {AMP_TYPE_SVT_CLASSIC_BASS, "SVT Classic Bass", NULL, -1},
    {AMP_TYPE_PEG_B15_BASS, "Peg B-15 Bass", NULL, -1},
    {AMP_TYPE_BASIC_BASS , "Basic Bass", NULL, -1},
    {AMP_TYPE_SHOWMAN_BASS , "Showman Bass", NULL, -1},
    {AMP_TYPE_ASH_MODERN_BASS , "Ash Modern Bass", NULL, -1},
    {AMP_TYPE_BRIT_BASS , "Brit Bass", NULL, -1},
    {AMP_TYPE_SOLAR_BASS , "Solar 200s Bass", NULL, -1},
    {AMP_TYPE_BOOGIE_BASS , "Boogie Bass", NULL, -1},
    {AMP_TYPE_HART_BASS , "Hart Bass", NULL, -1},
};

static EffectGroup rp250_eq_group[] = {
    {EQ_TYPE_BRIGHT, "Bright", rp250_eq_settings, G_N_ELEMENTS(rp250_eq_settings)},
    {EQ_TYPE_MIDBOOST, "Mid Boost", rp250_eq_settings, G_N_ELEMENTS(rp250_eq_settings)},
    {EQ_TYPE_SCOOP, "Scoop", rp250_eq_settings, G_N_ELEMENTS(rp250_eq_settings)},
    {EQ_TYPE_WARM, "Warm", rp250_eq_settings, G_N_ELEMENTS(rp250_eq_settings)},
};

static EffectGroup rp355_eq_group_A[] = {
    {EQ_TYPE_BRIGHT, "Bright", rp355_eq_settings_A, G_N_ELEMENTS(rp355_eq_settings_A)},
    {EQ_TYPE_MIDBOOST, "Mid Boost", rp355_eq_settings_A, G_N_ELEMENTS(rp355_eq_settings_A)},
    {EQ_TYPE_SCOOP, "Scoop", rp355_eq_settings_A, G_N_ELEMENTS(rp355_eq_settings_A)},
    {EQ_TYPE_WARM, "Warm", rp355_eq_settings_A, G_N_ELEMENTS(rp355_eq_settings_A)},
};

static EffectGroup rp355_eq_group_B[] = {
    {EQ_TYPE_BRIGHT, "Bright", rp355_eq_settings_B, G_N_ELEMENTS(rp355_eq_settings_B)},
    {EQ_TYPE_MIDBOOST, "Mid Boost", rp355_eq_settings_B, G_N_ELEMENTS(rp355_eq_settings_B)},
    {EQ_TYPE_SCOOP, "Scoop", rp355_eq_settings_B, G_N_ELEMENTS(rp355_eq_settings_B)},
    {EQ_TYPE_WARM, "Warm", rp355_eq_settings_B, G_N_ELEMENTS(rp355_eq_settings_B)},
};

static EffectGroup rp500_eq_group[] = {
    {-1, NULL, rp500_eq_settings, G_N_ELEMENTS(rp500_eq_settings)},
};

static EffectGroup rp250_amp_cab_group[] = {
    {AMP_CAB_DIRECT, "Direct", NULL, -1},
    {AMP_CAB_CHAMP, "Champ 1x8", NULL, -1},
    {AMP_CAB_DELUXE, "Deluxe 1x12", NULL, -1},
    {AMP_CAB_TWIN, "Twin 2x12", NULL, -1},
    {AMP_CAB_BRITISH2_12, "British 2x12", NULL, -1},
    {AMP_CAB_BASSMAN, "Bassman 4x10", NULL, -1},
    {AMP_CAB_BRITISH4_12, "British 4x12", NULL, -1},
    {AMP_CAB_BRITISH_GREEN, "British Green 4x12", NULL, -1},
    {AMP_CAB_VINTAGE, "Vintage V30 4x12", NULL, -1},
    {AMP_CAB_DIGI_SOLO, "DigiTech Solo 4x12", NULL, -1},
    {AMP_CAB_DIGI_BRIGHT, "DigiTech Bright 2x12", NULL, -1},
    {AMP_CAB_DIGI_METAL, "DigiTech Metal 4x12", NULL, -1},
    {AMP_CAB_DIGI_ROCK, "DigiTech Rock 4x12", NULL, -1},
    {AMP_CAB_DIGI_ALT, "DigiTech Alt 4x12", NULL, -1},
    {AMP_CAB_DIGI_VNTG, "DigiTech Vntg 4x12", NULL, -1},
};

static EffectGroup rp355_amp_cab_group[] = {
    {AMP_CAB_DIRECT, "Direct", NULL, -1},
    {AMP_CAB_CHAMP, "Champ 1x8", NULL, -1},
    {AMP_CAB_DELUXE, "Deluxe 1x12", NULL, -1},
    {AMP_CAB_DELUXE_REVERB, "Deluxe Reverb 1x12", NULL, -1},
    {AMP_CAB_BLONDE2_12, "Blonde 2x12", NULL, -1},
    {AMP_CAB_TWIN, "Twin 2x12", NULL, -1},
    {AMP_CAB_BRITISH2_12, "British 2x12", NULL, -1},
    {AMP_CAB_BASSMAN, "Bassman 4x10", NULL, -1},
    {AMP_CAB_BRITISH4_12, "British 4x12", NULL, -1},
    {AMP_CAB_BRITISH_GREEN, "British Green 4x12", NULL, -1},
    {AMP_CAB_FANE4_12, "Fane 4x12", NULL, -1},
    {AMP_CAB_BOUTIQUE4_12, "Boutique 4x12", NULL, -1},
    {AMP_CAB_VINTAGE, "Vintage V30 4x12", NULL, -1},
    {AMP_CAB_DIGI_SOLO, "DigiTech Solo 4x12", NULL, -1},
    {AMP_CAB_DIGI_BRIGHT, "DigiTech Bright 2x12", NULL, -1},
    {AMP_CAB_DIGI_METAL, "DigiTech Metal 4x12", NULL, -1},
    {AMP_CAB_DIGI_ROCK, "DigiTech Rock 4x12", NULL, -1},
    {AMP_CAB_DIGI_ALT, "DigiTech Alt 4x12", NULL, -1},
    {AMP_CAB_DIGI_VNTG, "DigiTech Vntg 4x12", NULL, -1},
};

static EffectGroup rp500_amp_cab_group[] = {
    {AMP_CAB_DIRECT, "Direct", NULL, -1},
    {AMP_CAB_CHAMP, "Champ", NULL, -1},
    {AMP_CAB_DELUXE, "Deluxe", NULL, -1},
    {AMP_CAB_DELUXE_REVERB, "Deluxe Reverb 1x12", NULL, -1},
    {AMP_CAB_BRITISH1_12, "British 1x12", NULL, -1},
    {AMP_CAB_GA1_12, "GA 1x12", NULL, -1},
    {AMP_CAB_BLONDE2_12, "Blonde 2x12", NULL, -1},
    {AMP_CAB_TWIN, "Twin 2x12", NULL, -1},
    {AMP_CAB_BRITISH2_12, "British 2x12", NULL, -1},
    {AMP_CAB_JAZZ2_12, "Jazz 2x12", NULL, -1},
    {AMP_CAB_BASSMAN, "Bassman 4x10", NULL, -1},
    {AMP_CAB_BRITISH4_12, "British 4x12", NULL, -1},
    {AMP_CAB_BRITISH_GREEN, "Greenback 4x12", NULL, -1},
    {AMP_CAB_FANE4_12, "Fane 4x12", NULL, -1},
    {AMP_CAB_BOUTIQUE4_12, "Boutique 4x12", NULL, -1},
    {AMP_CAB_VINTAGE, "Vintage 4x12", NULL, -1},
    {AMP_CAB_RECTO4_12, "Recto 4x12", NULL, -1},
    {AMP_CAB_DIGI_SOLO, "DigiTech Solo 4x12", NULL, -1},
    {AMP_CAB_DIGI_BRIGHT, "DigiTech Bright 2x12", NULL, -1},
    {AMP_CAB_DIGI_METAL, "DigiTech Metal 4x12", NULL, -1},
    {AMP_CAB_DIGI_ROCK, "DigiTech Rock 4x12", NULL, -1},
    {AMP_CAB_DIGI_ALT, "DigiTech Alt 4x12", NULL, -1},
    {AMP_CAB_DIGI_VNTG, "DigiTech Vintage 4x12", NULL, -1},
    {AMP_CAB_DIGI_CHUNK, "DigiTech Chunk 4x12", NULL, -1},
    {AMP_CAB_DIGI_SPANK2_12, "DigiTech Spank 2x12", NULL, -1},
    {AMP_CAB_DIGI_SPKR_COMP, "DigiTech Spkr Comp", NULL, -1},
};

static EffectGroup gnx3k_amp_cab_group[] = {
    {GNX_AMP_CAB_DIRECT, "Direct", NULL, -1},
    {GNX_AMP_CAB_TWEED1_8, "Tweed 1x8", NULL, -1},
    {GNX_AMP_CAB_TWEED1_12, "Tweed 1x12", NULL, -1},
    {GNX_AMP_CAB_BLACKFACE1_12, "Blackface 1x12", NULL, -1},
    {GNX_AMP_CAB_BRITISH1_12, "British 1x12", NULL, -1},
    {GNX_AMP_CAB_BLACKFACE2_12, "Blackface 2x12", NULL, -1},
    {GNX_AMP_CAB_BLONDE2_12, "Blonde 2x12", NULL, -1},
    {GNX_AMP_CAB_BRITISH2_12, "British 2x12", NULL, -1},
    {GNX_AMP_CAB_TWEED4_10, "Tweed 4x10", NULL, -1},
    {GNX_AMP_CAB_BRITISH_70_4_12, "British 70 4x12", NULL, -1},
    {GNX_AMP_CAB_BRITISH_GREEN4_12, "British Green 4x12", NULL, -1},
    {GNX_AMP_CAB_STRAIGHT_V30_4_12, "Straight V30 4x12", NULL, -1},
    {GNX_AMP_CAB_SLANT_V30_4_12, "Slant V30 4x12", NULL, -1},
    {GNX_AMP_CAB_FANE4_12, "Fane 4x12", NULL, -1},
    {GNX_AMP_CAB_2101_SPKR_COMP, "2101 Spkr Comp", NULL, -1},
    {GNX_AMP_CAB_DIGI_SPANK, "Digitech Spank 4x12", NULL, -1},
    {GNX_AMP_CAB_DIGI_SOLO, "Digitech Solo 4x12", NULL, -1},
    {GNX_AMP_CAB_DIGI_METAL, "Digitech Metal 4x12", NULL, -1},
    {GNX_AMP_CAB_DIGI_BRIGHT, "Digitech Bright 4x12", NULL, -1},
    {GNX_AMP_CAB_DIGI_CHUNK, "Digitech Chunk 4x12", NULL, -1},
    {GNX_AMP_CAB_JUMBO_ACOUSTIC, "Jumbo Acoustic", NULL, -1},
    {GNX_AMP_CAB_DREAD_ACOUSTIC, "Dread Acoustic", NULL, -1},
    {GNX_AMP_CAB_HART_BASS1_15, "Hart Bass 1x15", NULL, -1},
    {GNX_AMP_CAB_BASIC_BASS1_15, "Basic Bass 1x15", NULL, -1},
    {GNX_AMP_CAB_PORTA_BASS1_15, "Porta Bass 1x15", NULL, -1},
    {GNX_AMP_CAB_REFLEX1_18, "Reflex 1x18", NULL, -1},
    {GNX_AMP_CAB_SOLAR_BASS2_15, "Solar Bass 2x15", NULL, -1},
    {GNX_AMP_CAB_DE_BASS4_10, "DE Bass 4x10", NULL, -1},
    {GNX_AMP_CAB_ASH_BASS4_10, "Ash Bass 4x10", NULL, -1},
    {GNX_AMP_CAB_GOLIATH_BASS4_10, "Goliath Bass 4x10", NULL, -1},
    {GNX_AMP_CAB_HART_BASS4_10, "Hart Bass 4x10", NULL, -1},
    {GNX_AMP_CAB_SVT_BASS8_10, "SVT Bass 8x10", NULL, -1},
};

static EffectGroup gnx3k_ch1_cab_tuning_group[] = {
    {-1, NULL, gnx3k_ch1_cab_tuning_settings, G_N_ELEMENTS(gnx3k_ch1_cab_tuning_settings)},
};

static EffectGroup gnx3k_ch2_cab_tuning_group[] = {
    {-1, NULL, gnx3k_ch2_cab_tuning_settings, G_N_ELEMENTS(gnx3k_ch2_cab_tuning_settings)},
};

static EffectGroup gnx3k_ch1_amp_eq_group[] = {
    {-1, NULL, gnx3k_ch1_amp_eq_settings, G_N_ELEMENTS(gnx3k_ch1_amp_eq_settings)},
};

static EffectGroup gnx3k_ch2_amp_eq_group[] = {
    {-1, NULL, gnx3k_ch2_amp_eq_settings, G_N_ELEMENTS(gnx3k_ch2_amp_eq_settings)},
};

/* LIBRARY_TONE, LIBRARY_POSITION */
static EffectGroup tone_lib_group[] = {
    {TONE_LIB_OVERDRIVE, "Overdrive", NULL, -1},
    {TONE_LIB_ROCK1, "Rock 1", NULL, -1},
    {TONE_LIB_ROCK2, "Rock 2", NULL, -1},
    {TONE_LIB_BLUES1, "Blues 1", NULL, -1},
    {TONE_LIB_BLUES2, "Blues 2", NULL, -1},
    {TONE_LIB_METAL1, "Metal 1", NULL, -1},
    {TONE_LIB_METAL2, "Metal 2", NULL, -1},
    {TONE_LIB_COUNTRY1, "Country 1", NULL, -1},
    {TONE_LIB_COUNTRY2, "Country 2", NULL, -1},
    {TONE_LIB_WARM_DRIVE, "Warm Drive", NULL, -1},
    {TONE_LIB_CRUNCH, "Crunch", NULL, -1},
    {TONE_LIB_TEXAS_TONE, "Texas Tone", NULL, -1},
    {TONE_LIB_ROCKABILLY, "Rockabilly", NULL, -1},
    {TONE_LIB_SOLO1, "Solo 1", NULL, -1},
    {TONE_LIB_SOLO2, "Solo 2", NULL, -1},
    {TONE_LIB_ROCKWAH, "Rock Wah", NULL, -1},
    {TONE_LIB_CHUNKY, "Chunky", NULL, -1},
    {TONE_LIB_SMOOTH, "Smooth", NULL, -1},
    {TONE_LIB_HEAVY, "Heavy", NULL, -1},
    {TONE_LIB_CLEAN1, "Clean 1", NULL, -1},
    {TONE_LIB_CLEAN2, "Clean 2", NULL, -1},
    {TONE_LIB_BRITISH1, "British 1", NULL, -1},
    {TONE_LIB_BRITISH2, "British 2", NULL, -1},
    {TONE_LIB_AMERICAN1, "American 1", NULL, -1},
    {TONE_LIB_AMERICAN2, "American 2", NULL, -1},
    {TONE_LIB_TUBE_DRIVE, "Tube Drive", NULL, -1},
    {TONE_LIB_DISTORTION, "Distortion", NULL, -1},
    {TONE_LIB_SCOOPED, "Scooped", NULL, -1},
    {TONE_LIB_PUNCHY, "Punchy", NULL, -1},
    {TONE_LIB_BRIGHT_CLEAN, "Bright Clean", NULL, -1},
};

/* LIBRARY_EFFECTS, LIBRARY_POSITION */
static EffectGroup effects_lib_group[] = {
    {EFFECTS_LIB_CHORUS, "Chorus", NULL, -1},
    {EFFECTS_LIB_PHASER, "Phaser", NULL, -1},
    {EFFECTS_LIB_FLANGER, "Flanger", NULL, -1},
    {EFFECTS_LIB_PITCH, "Pitch", NULL, -1},
    {EFFECTS_LIB_TREMOLO, "Tremolo", NULL, -1},
    {EFFECTS_LIB_ROTARY, "Rotary", NULL, -1},
    {EFFECTS_LIB_ENVELOPE, "Envelope Filter", NULL, -1},
    {EFFECTS_LIB_DIGITAL, "Digital Delay", NULL, -1},
    {EFFECTS_LIB_ANALOG, "Analog Delay", NULL, -1},
    {EFFECTS_LIB_PONG, "Pong Delay", NULL, -1},
    {EFFECTS_LIB_MOD, "Mod Delay", NULL, -1},
    {EFFECTS_LIB_TAPE, "Tape Delay", NULL, -1},
    {EFFECTS_LIB_HALL, "Hall Reverb", NULL, -1},
    {EFFECTS_LIB_PLATE, "Plate Reverb", NULL, -1},
    {EFFECTS_LIB_SPRING, "Spring Reverb", NULL, -1},
    {EFFECTS_LIB_CHORUS_DIGITAL, "Chorus + Digital Delay", NULL, -1},
    {EFFECTS_LIB_CHORUS_DELAY_REVERB, "Chorus + Delay + Reverb", NULL, -1},
    {EFFECTS_LIB_FLANGER_ANALOG, "Flanger + Analog Delay", NULL, -1},
    {EFFECTS_LIB_PHASER_TAPE, "Phaser + Tape Delay", NULL, -1},
    {EFFECTS_LIB_PHASER_MOD, "Phaser + Mod Delay", NULL, -1},
    {EFFECTS_LIB_PHASER_ROOM, "Phaser + Room Reverb", NULL, -1},
    {EFFECTS_LIB_DIGITAL_HALL, "Digital Delay + Hall Reverb", NULL, -1},
    {EFFECTS_LIB_ANALOG_SPRING, "Analog Delay + Spring Reverb", NULL, -1},
    {EFFECTS_LIB_CHORUS_HALL, "Chorus + Hall Reverb", NULL, -1},
    {EFFECTS_LIB_PONG_HALL, "Pong Delay + Hall Reverb", NULL, -1},
    {EFFECTS_LIB_TAPE_SPRING, "Tape Delay + Spring Reverb", NULL, -1},
    {EFFECTS_LIB_TREMOLO_TAPE, "Tremolo + Tape Delay", NULL, -1},
    {EFFECTS_LIB_PITCH_DIGITAL, "Pitch + Digital Delay", NULL, -1},
    {EFFECTS_LIB_MOD_PLATE, "Mod Delay + Plate Reverb", NULL, -1},
    {EFFECTS_LIB_ROTARY_TAPE, "Rotary + Tape Delay", NULL, -1},
};

static EffectGroup pickup_group[] = {
    {PICKUP_TYPE_HB_SC, "HB>SC", NULL, -1},
    {PICKUP_TYPE_SC_HB, "SC>HB", NULL, -1},
};

static EffectGroup pre_post_group[] = {
    {CHORUSFX_PRE, "PRE AMP", NULL, -1},
    {CHORUSFX_POST, "POST AMP", NULL, -1},
};

static Effect gnx3k_wah_effect[] = {
    {NULL, WAH_ON_OFF, WAH_TYPE, WAH_POSITION, gnx3k_wah_group, G_N_ELEMENTS(gnx3k_wah_group)},
};

static Effect wah_effect[] = {
    {NULL, WAH_ON_OFF, WAH_TYPE, WAH_POSITION, wah_group, G_N_ELEMENTS(wah_group)},
};

static Effect gnx3k_whammy_effect[] = {
    {NULL, GNX3K_WHAM_ENABLE, GNX3K_WHAM_TYPE, GNX3K_WHAM_POSITION, gnx3k_whammy_group, G_N_ELEMENTS(gnx3k_whammy_group)},
};

static Effect gnx3k_preset_effect[] = {
    {NULL, -1, -1, -1, gnx3k_preset_group, G_N_ELEMENTS(gnx3k_preset_group)},
};

static Effect rp250_comp_effect[] = {
    {NULL, COMP_ON_OFF, COMP_TYPE, COMP_POSITION, rp250_comp_group, G_N_ELEMENTS(rp250_comp_group)},
};

static Effect rp355_comp_effect[] = {
    {NULL, COMP_ON_OFF, COMP_TYPE, COMP_POSITION, rp355_comp_group, G_N_ELEMENTS(rp355_comp_group)},
};

static Effect gnx_comp_effect[] = {
    {"Compressor", COMP_ON_OFF, -1, COMP_POSITION, gnx_comp_group, G_N_ELEMENTS(gnx_comp_group)},
};

static Effect rp500_comp_effect[] = {
    {NULL, COMP_ON_OFF, COMP_TYPE, COMP_POSITION, rp500_comp_group, G_N_ELEMENTS(rp500_comp_group)},
};

static Effect rp250_dist_effect[] = {
    {NULL, DIST_ON_OFF, DIST_TYPE, DIST_POSITION, rp250_dist_group, G_N_ELEMENTS(rp250_dist_group)},
};

static Effect rp355_dist_effect[] = {
    {NULL, DIST_ON_OFF, DIST_TYPE, DIST_POSITION, rp355_dist_group, G_N_ELEMENTS(rp355_dist_group)},
};

static Effect rp500_dist_effect[] = {
    {NULL, DIST_ON_OFF, DIST_TYPE, DIST_POSITION, rp500_dist_group, G_N_ELEMENTS(rp500_dist_group)},
};

static Effect gnx4_dist_effect[] = {
    {NULL, DIST_ON_OFF, DIST_TYPE, DIST_POSITION, gnx4_dist_group, G_N_ELEMENTS(gnx4_dist_group)},
};

static Effect gnx3k_dist_effect[] = {
    {NULL, DIST_ON_OFF, DIST_TYPE, DIST_POSITION, gnx3k_dist_group, G_N_ELEMENTS(gnx3k_dist_group)},
};

static Effect gnx3k_noisegate_effect[] = {
    {NULL, NOISEGATE_ON_OFF, NOISEGATE_TYPE, NOISEGATE_POSITION, gnx3k_noisegate_group, G_N_ELEMENTS(gnx3k_noisegate_group)},
};

static Effect noisegate_effect[] = {
    {NULL, NOISEGATE_ON_OFF, NOISEGATE_TYPE, NOISEGATE_POSITION, noisegate_group, G_N_ELEMENTS(noisegate_group)},
};

static Effect gnx4_chorusfx_effect[] = {
    {NULL, CHORUSFX_ON_OFF, CHORUSFX_TYPE, CHORUSFX_POSITION, gnx4_chorusfx_group, G_N_ELEMENTS(gnx4_chorusfx_group)},
};

static Effect gnx3k_chorusfx_effect[] = {
    {NULL, CHORUSFX_ON_OFF, CHORUSFX_TYPE, CHORUSFX_POSITION, gnx3k_chorusfx_group, G_N_ELEMENTS(gnx3k_chorusfx_group)},
};

static Effect rp250_chorusfx_effect[] = {
    {NULL, CHORUSFX_ON_OFF, CHORUSFX_TYPE, CHORUSFX_POSITION, rp250_chorusfx_group, G_N_ELEMENTS(rp250_chorusfx_group)},
};

static Effect rp355_chorusfx_effect[] = {
    {"Position",-1, CHORUSFX_PRE_POST,CHORUSFX_POSITION, pre_post_group,G_N_ELEMENTS(pre_post_group)},
    {NULL, CHORUSFX_ON_OFF,CHORUSFX_TYPE, CHORUSFX_POSITION, rp355_chorusfx_group, G_N_ELEMENTS(rp355_chorusfx_group)},
};

static Effect rp500_chorusfx_effect[] = {
    {NULL, CHORUSFX_ON_OFF, CHORUSFX_TYPE, CHORUSFX_POSITION, rp500_chorusfx_group, G_N_ELEMENTS(rp500_chorusfx_group)},
};

static Effect gnx3k_delay_effect[] = {
    {NULL, DELAY_ON_OFF, DELAY_TYPE, DELAY_POSITION, gnx3k_delay_group, G_N_ELEMENTS(gnx3k_delay_group)},
};

static Effect rp250_delay_effect[] = {
    {NULL, DELAY_ON_OFF, DELAY_TYPE, DELAY_POSITION, rp250_delay_group, G_N_ELEMENTS(rp250_delay_group)},
};

static Effect rp355_delay_effect[] = {
    {NULL, DELAY_ON_OFF, DELAY_TYPE, DELAY_POSITION, rp355_delay_group, G_N_ELEMENTS(rp355_delay_group)},
};

static Effect rp500_delay_effect[] = {
    {NULL, DELAY_ON_OFF, DELAY_TYPE, DELAY_POSITION, rp500_delay_group, G_N_ELEMENTS(rp500_delay_group)},
};

static Effect gnx3k_reverb_effect[] = {
    {NULL, REVERB_ON_OFF, REVERB_TYPE, REVERB_POSITION, gnx3k_reverb_group, G_N_ELEMENTS(gnx3k_reverb_group)},
};

static Effect reverb_effect[] = {
    {NULL, REVERB_ON_OFF, REVERB_TYPE, REVERB_POSITION, reverb_group, G_N_ELEMENTS(reverb_group)},
};

static Effect rp250_amp_effect[] = {
    {NULL, AMP_ON_OFF, AMP_TYPE, AMP_POSITION, rp250_amp_group, G_N_ELEMENTS(rp250_amp_group)},
    {"Cabinet", -1, AMP_CAB_TYPE, AMP_CAB_POSITION, rp250_amp_cab_group, G_N_ELEMENTS(rp250_amp_cab_group)},
};

static Effect rp355_select_amp_effect[] = {
    {"Select Amp", -1, AMP_SELECT, AMP_SELECT_POSITION, rp355_amp_select_group, G_N_ELEMENTS(rp355_amp_select_group)},
};

static Effect rp355_amp_effect_A[] = {
    {NULL, AMP_ON_OFF, AMP_TYPE, AMP_POSITION, rp355_amp_group_A, G_N_ELEMENTS(rp355_amp_group_A)},
    {"Cabinet", -1, AMP_CAB_TYPE, AMP_CAB_POSITION, rp355_amp_cab_group, G_N_ELEMENTS(rp355_amp_cab_group)},
};

static Effect rp355_amp_effect_B[] = {
    {NULL, AMP_ON_OFF, AMP_TYPE, CH2_AMP_POSITION, rp355_amp_group_B, G_N_ELEMENTS(rp355_amp_group_B)},
    {"Cabinet", -1, AMP_CAB_TYPE, CH2_AMP_CAB_POSITION, rp355_amp_cab_group, G_N_ELEMENTS(rp355_amp_cab_group)},
};

static Effect rp500_amp_effect[] = {
    {NULL, AMP_ON_OFF, AMP_TYPE, AMP_POSITION, rp500_amp_group, G_N_ELEMENTS(rp500_amp_group)},
    {"Cabinet", -1, AMP_CAB_TYPE, AMP_CAB_POSITION, rp500_amp_cab_group, G_N_ELEMENTS(rp500_amp_cab_group)},
};

static Effect gnx3k_channel_1_effect[] = {
    {"EQ Enable", AMP_EQ_ON_OFF, AMP_TYPE, AMP_POSITION, gnx3k_amp_group, G_N_ELEMENTS(gnx3k_amp_group)},
    {NULL, -1, AMP_CAB_TYPE, AMP_CAB_POSITION, gnx3k_amp_cab_group, G_N_ELEMENTS(gnx3k_amp_cab_group)},
    {NULL, -1, -1, -1, gnx3k_ch1_cab_tuning_group, G_N_ELEMENTS(gnx3k_ch1_cab_tuning_group)},
    {NULL, -1, -1, -1, gnx3k_ch1_amp_eq_group, G_N_ELEMENTS(gnx3k_ch1_amp_eq_group)},
};

static Effect gnx3k_channel_2_effect[] = {
    {"EQ Enable", AMP_EQ_ON_OFF, AMP_TYPE, CH2_AMP_POSITION, gnx3k_amp_group, G_N_ELEMENTS(gnx3k_amp_group)},
    {NULL, -1, AMP_CAB_TYPE, CH2_AMP_CAB_POSITION, gnx3k_amp_cab_group, G_N_ELEMENTS(gnx3k_amp_cab_group)},
    {NULL, -1, -1, -1, gnx3k_ch2_cab_tuning_group, G_N_ELEMENTS(gnx3k_ch2_cab_tuning_group)},
    {NULL, -1, -1, -1, gnx3k_ch2_amp_eq_group, G_N_ELEMENTS(gnx3k_ch2_amp_eq_group)},
};

static Effect rp250_eq_effect[] = {
    {NULL, EQ_ON_OFF, EQ_TYPE, EQ_POSITION, rp250_eq_group, G_N_ELEMENTS(rp250_eq_group)},
};

static Effect rp355_eq_effect_A[] = {
    {NULL, EQ_ON_OFF, EQ_TYPE, EQ_POSITION, rp355_eq_group_A, G_N_ELEMENTS(rp355_eq_group_A)},
};

static Effect rp355_eq_effect_B[] = {
    {NULL, EQ_ON_OFF, EQ_TYPE, EQ_POSITION_B, rp355_eq_group_B, G_N_ELEMENTS(rp355_eq_group_B)},
};

static Effect rp500_eq_effect[] = {
    {"Enable Equalizer", EQ_ON_OFF, -1, EQ_POSITION, rp500_eq_group, G_N_ELEMENTS(rp500_eq_group)},
};

static Effect pickup_usb_effect[] = {
    {NULL, PICKUP_ON_OFF, PICKUP_TYPE, PICKUP_POSITION, pickup_group, G_N_ELEMENTS(pickup_group)},
    {NULL, -1, -1, -1, usb_group, G_N_ELEMENTS(usb_group)},
};

static Effect pickup_effect[] = {
    {NULL, PICKUP_ON_OFF, PICKUP_TYPE, PICKUP_POSITION, pickup_group, G_N_ELEMENTS(pickup_group)},
};

static Effect gnx3k_amp_channel_effect[] = {
    {NULL, -1, -1, -1, gnx3k_amp_channel_group, G_N_ELEMENTS(gnx3k_amp_channel_group)},
};

static EffectList rp250_effects[] = {
    {"Wah", wah_effect, G_N_ELEMENTS(wah_effect)},
    {"Amplifier", rp250_amp_effect, G_N_ELEMENTS(rp250_amp_effect)},
    {"Equalizer", rp250_eq_effect, G_N_ELEMENTS(rp250_eq_effect)},
    {"Compressor", rp250_comp_effect, G_N_ELEMENTS(rp250_comp_effect)},
    {"Distortion", rp250_dist_effect, G_N_ELEMENTS(rp250_dist_effect)},
    {"Noisegate", noisegate_effect, G_N_ELEMENTS(noisegate_effect)},
    {"Chorus/FX", rp250_chorusfx_effect, G_N_ELEMENTS(rp250_chorusfx_effect)},
    {"Delay", rp250_delay_effect, G_N_ELEMENTS(rp250_delay_effect)},
    {"Reverb", reverb_effect, G_N_ELEMENTS(reverb_effect)},
    {"Pickup/USB", pickup_usb_effect, G_N_ELEMENTS(pickup_usb_effect)},
};

static EffectList rp355_effects[] = {
    {"Pickup/USB", pickup_usb_effect, G_N_ELEMENTS(pickup_usb_effect)},
    {"Wah", wah_effect, G_N_ELEMENTS(wah_effect)},
    {"Compressor", rp355_comp_effect, G_N_ELEMENTS(rp355_comp_effect)},
    {"Distortion", rp355_dist_effect, G_N_ELEMENTS(rp355_dist_effect)},
    {"Select Amp", rp355_select_amp_effect, G_N_ELEMENTS(rp355_select_amp_effect)},
    {"Amplifier A", rp355_amp_effect_A, G_N_ELEMENTS(rp355_amp_effect_A)},
    {"Equalizer A", rp355_eq_effect_A, G_N_ELEMENTS(rp355_eq_effect_A)},
    {"Amplifier B", rp355_amp_effect_B, G_N_ELEMENTS(rp355_amp_effect_B)},
    {"Equalizer B", rp355_eq_effect_B, G_N_ELEMENTS(rp355_eq_effect_B)},
    {"Noisegate", noisegate_effect, G_N_ELEMENTS(noisegate_effect)},
    {"Chorus/FX", rp355_chorusfx_effect, G_N_ELEMENTS(rp355_chorusfx_effect)},
    {"Delay", rp355_delay_effect, G_N_ELEMENTS(rp355_delay_effect)},
    {"Reverb", reverb_effect, G_N_ELEMENTS(reverb_effect)},
};

static EffectList rp500_effects[] = {
    {"Wah", wah_effect, G_N_ELEMENTS(wah_effect)},
    {"Amplifier", rp500_amp_effect, G_N_ELEMENTS(rp500_amp_effect)},
    {"Equalizer", rp500_eq_effect, G_N_ELEMENTS(rp500_eq_effect)},
    {"Compressor", rp500_comp_effect, G_N_ELEMENTS(rp500_comp_effect)},
    {"Distortion", rp500_dist_effect, G_N_ELEMENTS(rp500_dist_effect)},
    {"Noisegate", noisegate_effect, G_N_ELEMENTS(noisegate_effect)},
    {"Chorus/FX", rp500_chorusfx_effect, G_N_ELEMENTS(rp500_chorusfx_effect)},
    {"Delay", rp500_delay_effect, G_N_ELEMENTS(rp500_delay_effect)},
    {"Reverb", reverb_effect, G_N_ELEMENTS(reverb_effect)},
};

static EffectList gnx4_effects[] = {
    {"Pickup", pickup_effect, G_N_ELEMENTS(pickup_effect)},
    {"Wah", gnx3k_wah_effect, G_N_ELEMENTS(gnx3k_wah_effect)},
    {"Compressor", gnx_comp_effect, G_N_ELEMENTS(gnx_comp_effect)},
    {"Whammy/IPS", gnx3k_whammy_effect, G_N_ELEMENTS(gnx3k_whammy_effect)},
    {"Stompbox", gnx4_dist_effect, G_N_ELEMENTS(gnx4_dist_effect)},
    {"Noisegate", gnx3k_noisegate_effect, G_N_ELEMENTS(gnx3k_noisegate_effect)},
    {"Chorus/Mod", gnx4_chorusfx_effect, G_N_ELEMENTS(gnx4_chorusfx_effect)},
    {"Delay", gnx3k_delay_effect, G_N_ELEMENTS(gnx3k_delay_effect)},
    {"Reverb", gnx3k_reverb_effect, G_N_ELEMENTS(gnx3k_reverb_effect)},
};

static EffectList gnx3000_effects[] = {
    {"Pickup", pickup_effect, G_N_ELEMENTS(pickup_effect)},
    {"Wah", gnx3k_wah_effect, G_N_ELEMENTS(gnx3k_wah_effect)},
    {"Whammy/IPS", gnx3k_whammy_effect, G_N_ELEMENTS(gnx3k_whammy_effect)},
    {"Preset Level", gnx3k_preset_effect, G_N_ELEMENTS(gnx3k_preset_effect)},
    {"Compressor", gnx_comp_effect, G_N_ELEMENTS(gnx_comp_effect)},
    {"Stompbox", gnx3k_dist_effect, G_N_ELEMENTS(gnx3k_dist_effect)},
    {"Noisegate", gnx3k_noisegate_effect, G_N_ELEMENTS(gnx3k_noisegate_effect)},
    {"Chorus/Mod", gnx3k_chorusfx_effect, G_N_ELEMENTS(gnx3k_chorusfx_effect)},
    {"Delay", gnx3k_delay_effect, G_N_ELEMENTS(gnx3k_delay_effect)},
    {"Reverb", gnx3k_reverb_effect, G_N_ELEMENTS(gnx3k_reverb_effect)},
};

static EffectList gnx3000_genetx[] = {
    {"Channel 1", gnx3k_channel_1_effect, G_N_ELEMENTS(gnx3k_channel_1_effect)},
    {"Channel 2", gnx3k_channel_2_effect, G_N_ELEMENTS(gnx3k_channel_2_effect)},
    {"Amp Channel", gnx3k_amp_channel_effect, G_N_ELEMENTS(gnx3k_amp_channel_effect)},
};

static Banks rp_banks[] = {
    {"User Presets", PRESETS_USER},
    {"System Presets", PRESETS_SYSTEM},
};

static Banks gnx4_banks[] = {
    {"Factory", PRESETS_SYSTEM},
    {"User Presets", PRESETS_USER},
    {"Flash Card", PRESETS_MEDIA_CARD},
};

static Banks gnx3k_banks[] = {
    {"User Presets", PRESETS_USER},
    {"Factory 1", PRESETS_SYSTEM},
    {"Factory 2", PRESETS_FACTORY2},
};

static EffectPage rp250_pages[] = {
    {"Effects", rp250_effects, G_N_ELEMENTS(rp250_effects), 2},
};

static EffectPage rp355_pages[] = {
    {"Effects", rp355_effects, G_N_ELEMENTS(rp355_effects), 2},
};

static EffectPage rp500_pages[] = {
    {"Effects", rp500_effects, G_N_ELEMENTS(rp500_effects), 2},
};

static EffectPage gnx4_pages[] = {
    {"Effects", gnx4_effects, G_N_ELEMENTS(gnx4_effects), 2},
};

static EffectPage gnx3000_pages[] = {
    {"Effects", gnx3000_effects, G_N_ELEMENTS(gnx3000_effects), 2},
    {"Amp/GeNetX", gnx3000_genetx, G_N_ELEMENTS(gnx3000_genetx), 1},
};

static Device rp250 = {
    .name = "DigiTech RP250",
    .family_id = 0x5E,
    .product_id = 0x02,
    .pages = rp250_pages,
    .n_pages = G_N_ELEMENTS(rp250_pages),
    .banks = rp_banks,
    .n_banks = G_N_ELEMENTS(rp_banks),
};

static Device rp355 = {
    .name = "DigiTech RP355",
    .family_id = 0x5E,
    .product_id = 0x09,
    .pages = rp355_pages,
    .n_pages = G_N_ELEMENTS(rp355_pages),
    .banks = rp_banks,
    .n_banks = G_N_ELEMENTS(rp_banks),
};

static Device rp500 = {
    .name = "DigiTech RP500",
    .family_id = 0x5E,
    .product_id = 0x05,
    .pages = rp500_pages,
    .n_pages = G_N_ELEMENTS(rp500_pages),
    .banks = rp_banks,
    .n_banks = G_N_ELEMENTS(rp_banks),
};

static Device gnx4 = {
    .name = "DigiTech GNX4",
    .family_id = 0x5C,
    .product_id = 0x03,
    .pages = gnx4_pages,
    .n_pages = G_N_ELEMENTS(gnx4_pages),
    .banks = gnx4_banks,
    .n_banks = G_N_ELEMENTS(gnx4_banks),
};

static Device gnx3000 = {
    .name = "DigiTech GNX3000",
    .family_id = 0x5C,
    .product_id = 0x04,
    .pages = gnx3000_pages,
    .n_pages = G_N_ELEMENTS(gnx3000_pages),
    .banks = gnx3k_banks,
    .n_banks = G_N_ELEMENTS(gnx3k_banks),
};

Device* supported_devices[] = {
    &rp250,
    &rp355,
    &rp500,
    &gnx4,
    &gnx3000,
};

int n_supported_devices = G_N_ELEMENTS(supported_devices);

#endif /* DOXYGEN_SHOULD_SKIP_THIS */

typedef struct {
    gchar *label;
    guint id;
    guint position;
    EffectValues *values;
} Modifier;

#ifndef DOXYGEN_SHOULD_SKIP_THIS

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
    {"Chorus Speed", CHORUS_SPEED, CHORUSFX_POSITION, &values_0_to_99},
    {"Chorus Depth", CHORUS_DEPTH, CHORUSFX_POSITION, &values_0_to_99},
    {"Chorus Level", CHORUS_LEVEL, CHORUSFX_POSITION, &values_0_to_99},
    {"Chorus Waveform", CHORUS_WAVE, CHORUSFX_POSITION, &values_waveform},
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
    {"Delay Repeats", DELAY_REPEATS, DELAY_POSITION, &values_delay_repeats},
    {"Delay Level", DELAY_LEVEL, DELAY_POSITION, &values_0_to_99},
    {"Delay Duck Thresh", DELAY_DUCK_THRESH, DELAY_POSITION, &values_0_to_99},
    {"Delay Duck Level", DELAY_DUCK_LEVEL, DELAY_POSITION, &values_0_to_99},
    {"Delay Mod Depth", DELAY_DEPTH, DELAY_POSITION, &values_0_to_99},
    {"Delay Tape Wow", DELAY_TAPE_WOW, DELAY_POSITION, &values_0_to_99},
    {"Delay Tape Flut", DELAY_TAPE_FLUTTER, DELAY_POSITION, &values_0_to_99},
    {"Reverb Enable", REVERB_ON_OFF, REVERB_POSITION, &values_on_off},
    {"Reverb Decay", REVERB_DECAY, REVERB_POSITION, &values_0_to_99},
    {"Reverb Liveliness", REVERB_LIVELINESS, REVERB_POSITION, &values_0_to_99},
    {"Reverb Level", REVERB_LEVEL, REVERB_POSITION, &values_0_to_99},
    {"Reverb Predelay", REVERB_PREDELAY, REVERB_POSITION, &values_0_to_15},
    {"Volume Pre FX", 2626, 13, &values_0_to_99},
    {"Volume Post FX", 2626, 17, &values_0_to_99},
};

int n_modifiers = G_N_ELEMENTS(modifiers);

#endif /* DOXYGEN_SHOULD_SKIP_THIS */

/**
 *  \param id modifier ID
 *  \param position modifier position
 *
 *  Gets modifier info.
 *
 *  \return Modifier which must not be freed, or NULL if no matching Modifier has been found.
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
 *  \param values possible setting values
 *
 *  Creates EffectSettings containing expression pedal min and max settings.
 *
 *  \return EffectSettings which must be freed using effect_settings_free.
 **/
static EffectSettings *get_modifier_settings(EffectValues *values)
{
    if (values == NULL)
        return NULL;

    /** \todo reuse exsisting settings if values is the same */
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
 *  \param settings settings to be freed
 *
 *  Frees all memory used by EffectSettings.
 **/
static void effect_settings_free(EffectSettings *settings)
{
    g_slice_free1(2 * sizeof(EffectSettings), settings);
}

/**
 *  Retrieves modifier linkable gruop from device.
 *
 *  \return ModifierGroup which must be freed using modifier_group_free.
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

    modifier_group->type = EXP_TYPE;
    modifier_group->position = EXP_POSITION;

    g_message("Group %d count %d", group_id, count);
    EffectGroup *group = g_slice_alloc(count * sizeof(EffectGroup));

    for (i=0; i<count; i++) {
        guint id = (str[12 + (i*3)] << 8) | str[13 + (i*3)];
        guint position = str[14 + (i*3)];

        group[i].type = (position << 16) | id;

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
 *  \param modifier_group group to be freed
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

/**
 *  \param device_id Device ID
 *  \param family_id Family ID
 *  \param product_id Product ID
 *  \param device Variable to hold device information
 *
 *  Gets appropiate effect list basing on device, family and product IDs.
 *
 *  \return TRUE if list and n_list were set, FALSE otherwise.
 **/
gboolean get_device_info(unsigned char device_id, unsigned char family_id,
                         unsigned char product_id,
                         Device **device)
{
    int x;
    for (x = 0; x < G_N_ELEMENTS(supported_devices); x++) {
        if (supported_devices[x]->product_id == product_id && supported_devices[x]->family_id == family_id) {
            *device = supported_devices[x];
            return TRUE;
        }
    }
    return FALSE;
}

