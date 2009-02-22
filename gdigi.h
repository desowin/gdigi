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
#include <glib.h>

enum {
  WAH_TYPE_CRY = 0,
  WAH_TYPE_FULLRANGE,
  WAH_TYPE_CLYDE
};

enum {
  COMP_TYPE_DIGI = 0,
  COMP_TYPE_CS
};

enum {
  PICKUP_TYPE_HB_SC = 0,
  PICKUP_TYPE_SC_HB
};

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

#define DIST_SCREAMER_DRIVE 0x02
#define DIST_SCREAMER_TONE  0x03
#define DIST_SCREAMER_LVL   0x04
#define DIST_808_OVERDRIVE  0x29
#define DIST_808_TONE       0x2A
#define DIST_808_LVL        0x2B
#define DIST_GUYOD_DRIVE    0x10
#define DIST_GUYOD_LVL      0x11
#define DIST_DOD250_GAIN    0x0B
#define DIST_DOD250_LVL     0x0C
#define DIST_RODENT_DIST    0x05
#define DIST_RODENT_FILTER  0x06
#define DIST_RODENT_LVL     0x07
#define DIST_MX_DIST        0x24
#define DIST_MX_OUTPUT      0x25
#define DIST_DS_GAIN        0x08
#define DIST_DS_TONE        0x09
#define DIST_DS_LVL         0x0A
#define DIST_GRUNGE_GRUNGE  0x16
#define DIST_GRUNGE_FACE    0x18
#define DIST_GRUNGE_LOUD    0x19
#define DIST_GRUNGE_BUTT    0x17
#define DIST_ZONE_GAIN      0x1C
#define DIST_ZONE_LOW       0x1F
#define DIST_ZONE_MID_LVL   0x1E
#define DIST_ZONE_MID_FREQ  0x1D
#define DIST_ZONE_HIGH      0x20
#define DIST_ZONE_LEVEL     0x21
#define DIST_DEATH_LOW      0x2D
#define DIST_DEATH_MID      0x2C
#define DIST_DEATH_HIGH     0x2F
#define DIST_DEATH_LVL      0x2E
#define DIST_GONK_GONK      0x30
#define DIST_GONK_SMEAR     0x27
#define DIST_GONK_SUCK      0x26
#define DIST_GONK_HEAVE     0x28
#define DIST_FUZZY_FUZZ     0x1A
#define DIST_FUZZY_VOLUME   0x1B
#define DIST_MP_SUSTAIN     0x0D
#define DIST_MP_TONE        0x0E
#define DIST_MP_VOLUME      0x0F

enum {
  EQ_TYPE_BRIGHT = 0,
  EQ_TYPE_MIDBOOST,
  EQ_TYPE_SCOOP,
  EQ_TYPE_WARM
};

enum {
  NOISEGATE_GATE = 0,
  NOISEGATE_SWELL
};

/* available only in Gate mode */
#define NOISEGATE_GATE_TRESHOLD 0x46
/* available only in Swell mode */
#define NOISEGATE_SWELL_SENS    0x47
/* available in both Gate and Swell modes */
#define NOISEGATE_ATTACK        0x48
#define NOISEGATE_RELEASE       0x49
#define NOISEGATE_ATTN          0x4A

enum {
  CHORUS_TYPE_CE = 0,
  CHORUS_TYPE_DUAL,
  CHORUS_TYPE_MULTI,
  CHORUS_TYPE_FLANGER,
  CHORUS_TYPE_MXR_FLANGER,
  CHORUS_TYPE_PHASER,
  CHORUS_TYPE_VIBRATO,
  CHORUS_TYPE_ROTARY,
  CHORUS_TYPE_VIBROPAN,
  CHORUS_TYPE_TREMOLO,
  CHORUS_TYPE_PANNER,
  CHORUS_TYPE_ENVELOPE,
  CHORUS_TYPE_AUTOYA,
  CHORUS_TYPE_YAYA,
  CHORUS_TYPE_STEP_FILTER,
  CHORUS_TYPE_WHAMMY,
  CHORUS_TYPE_PITCH_SHIFT,
  CHORUS_TYPE_DETUNE,
  CHORUS_TYPE_IPS
};

#define CHORUSFX_POSITION 14

#define CE_CHORUS_SPEED 837
#define CE_CHORUS_DEPTH 838

#define DUAL_CHORUS_SPEED 837
#define DUAL_CHORUS_DEPTH 838
#define DUAL_CHORUS_LEVEL 836
#define DUAL_CHORUS_WAVE 840

#define MULTI_CHORUS_SPEED 837
#define MULTI_CHORUS_DEPTH 838
#define MULTI_CHORUS_WAVE  840
#define MULTI_CHORUS_LEVEL 836

#define FLANGER_SPEED 902
#define FLANGER_DEPTH 903
#define FLANGER_REGEN 904
#define FLANGER_LEVEL 901
#define FLANGER_WAVE 905

#define MXR_FLANGER_SPEED 902
#define MXR_FLANGER_WIDTH 914
#define MXR_FLANGER_REGEN 904
#define MXR_FLANGER_MANUAL 917

#define PHASER_SPEED 962
#define PHASER_DEPTH 963
#define PHASER_REGEN 966
#define PHASER_LEVEL 965
#define PHASER_WAVE 967

#define VIBRATO_SPEED 1284
#define VIBRATO_DEPTH 1285

#define ROTARY_SPEED 1346
#define ROTARY_INTENSITY 1348
#define ROTARY_DOPPLER 1350
#define ROTARY_CROSSOVER 1351

#define VIBROPAN_SPEED 1314
#define VIBROPAN_DEPTH 1315
#define VIBROPAN_VIBRA 1316
#define VIBROPAN_WAVE 1317

#define TREMOLO_SPEED 1156
#define TREMOLO_DEPTH 1155
#define TREMOLO_WAVE 1157

#define PANNER_SPEED 1220
#define PANNER_DEPTH 1219
#define PANNER_WAVE 1221

#define ENVELOPE_SENSITIVITY 1606
#define ENVELOPE_RANGE 1605

#define AUTOYA_SPEED 1478
#define AUTOYA_INTENSITY 1482
#define AUTOYA_RANGE 1483

#define YAYA_PEDAL 1410
#define YAYA_INTENSITY 1417
#define YAYA_RANGE 1418

#define STEP_FILTER_SPEED 3010
#define STEP_FILTER_INTENSITY 3011

#define WHAMMY_AMOUNT 1797
#define WHAMMY_PEDAL 1795
#define WHAMMY_MIX 1796

#define PITCH_AMOUNT 1730
#define PITCH_MIX 1745

#define DETUNE_AMOUNT 1668
#define DETUNE_LEVEL 1667

#define IPS_SHIFT_AMOUNT 2754
#define IPS_KEY 2756
#define IPS_SCALE 2755
#define IPS_LEVEL 2757

/* DUAL_CHORUS_WAVE, MULTI_CHORUS_WAVE, FLANGER_WAVE, PHASER_WAVE,
   VIBROPAN_WAVE, TREMOLO_WAVE, PANNER_WAVE valid values */
#define WAVE_TRI    0x00
#define WAVE_SINE   0x01
#define WAVE_SQUARE 0x02

/* WHAMMY_AMOUNT valid values */
#define WHAMMY_OCT_UP 0x00
#define WHAMMY_2OCT_UP 0x01
#define WHAMMY_2ND_DN 0x02
#define WHAMMY_RV_2ND 0x03
#define WHAMMY_4TH_DN 0x04
#define WHAMMY_OCT_DN 0x05
#define WHAMMY_2OCT_DN 0x06
#define WHAMMY_DIV_BMB 0x07
#define WHAMMY_M3_MA 0x08
#define WHAMMY_2ND_MA3 0x09
#define WHAMMY_3RD_4TH 0x0A
#define WHAMMY_4TH_5TH 0x0B
#define WHAMMY_5TH_OCT 0x0C
#define WHAMMY_HOCT_UP 0x0D
#define WHAMMY_HOCT_DN 0x0E
#define WHAMMY_OCT_UD 0x0F

/* IPS_SHIFT_AMOUNT valid values */
#define IPS_OCT_D 0x00
#define IPS_7TH_DN 0x01
#define IPS_6TH_DN 0x02
#define IPS_5TH_DN 0x03
#define IPS_4TH_DN 0x04
#define IPS_3RD_DN 0x05
#define IPS_2ND_DN 0x06
#define IPS_2ND_UP 0x07
#define IPS_3RD_UP 0x08
#define IPS_4TH_UP 0x09
#define IPS_5TH_UP 0x0A
#define IPS_6TH_UP 0x0B
#define IPS_7TH_UP 0x0C
#define IPS_OCT_U 0x0D

/* IPS_KEY valid values */
#define IPS_E 0x00
#define IPS_F 0x01
#define IPS_GB 0x02
#define IPS_G 0x03
#define IPS_AB 0x04
#define IPS_A 0x05
#define IPS_BB 0x06
#define IPS_B 0x07
#define IPS_C 0x08
#define IPS_DD 0x09
#define IPS_D 0x0A
#define IPS_EB 0x0B

/* IPS_SCALE valid values */
#define IPS_MAJOR 0x00
#define IPS_MINOR 0x01
#define IPS_DORIA 0x02
#define IPS_MIXLYD 0x03
#define IPS_LYDIAN 0x04
#define IPS_HMINO 0x05

enum {
  DELAY_TYPE_ANALOG = 0,
  DELAY_TYPE_DIGITAL,
  DELAY_TYPE_MODULATED,
  DELAY_TYPE_PONG,
  DELAY_TYPE_TAPE
};

#define ANALOG_LEVEL 0x44
#define ANALOG_REPEATS 0x47

#define DIGITAL_LEVEL 0x44
#define DIGITAL_REPEATS 0x47
#define DIGITAL_DUCKER_THRESH 0x61
#define DIGITAL_DUCKER_LEVEL 0x62

#define MODULATED_LEVEL 0x44
#define MODULATED_REPEATS 0x47
#define MODULATED_DEPTH 0x51

#define PONG_LEVEL 0x44
#define PONG_REPEATS 0x47
#define PONG_DUCKER_THRESH 0x61
#define PONG_DUCKER_LEVEL 0x62

#define TAPE_LEVEL 0x44
#define TAPE_REPEATS 0x47
#define TAPE_WOW 0x63
#define TAPE_FLUTTER 0x64

enum {
  REVERB_TYPE_TWIN = 0,
  REVERB_TYPE_LEX_AMBIENCE,
  REVERB_TYPE_LEX_STUDIO,
  REVERB_TYPE_LEX_ROOM,
  REVERB_TYPE_LEX_HALL,
  REVERB_TYPE_EMT240_PLATE
};

#define TWIN_REVERB 0x05

#define LEX_AMBIENCE_PREDELAY 0x02
#define LEX_AMBIENCE_DECAY 0x07
#define LEX_AMBIENCE_LIVELINESS 0x0D
#define LEX_AMBIENCE_LEVEL 0x05

#define LEX_STUDIO_PREDELAY 0x02
#define LEX_STUDIO_DECAY 0x07
#define LEX_STUDIO_LIVELINESS 0x0D
#define LEX_STUDIO_LEVEL 0x05

#define LEX_ROOM_PREDELAY 0x02
#define LEX_ROOM_DECAY 0x07
#define LEX_ROOM_LIVELINESS 0x0D
#define LEX_ROOM_LEVEL 0x05

#define LEX_HALL_PREDELAY 0x02
#define LEX_HALL_DECAY 0x07
#define LEX_HALL_LIVELINESS 0x0D
#define LEX_HALL_LEVEL 0x05

#define EMT240_PLATE_PREDELAY 0x02
#define EMT240_PLATE_DECAY 0x07
#define EMT240_PLATE_LIVELINESS 0x0D
#define EMT240_PLATE_LEVEL 0x05

void set_wah_min(int level);
void set_wah_max(int level);
void set_wah_level(int level);
void set_wah_type(int type);
void set_wah_on_off(gboolean val);
void set_comp_sustain(int level);
void set_comp_tone(int level);
void set_comp_attack(int level);
void set_comp_level(int level);
void set_comp_type(int type);
void set_comp_on_off(gboolean val);
void switch_user_preset(int x);
void switch_system_preset(int x);
void set_pickup_type(int type);
void set_pickup_on_off(gboolean val);
void set_dist_type(int type);
void set_dist_option(guint32 option, int value);
void set_dist_on_off(gboolean val);
void set_preset_level(int level);
void set_eq_type(int type);
void set_eq_gain(int x);
void set_eq_level(int x);
void set_eq_bass(int x);
void set_eq_mid(int x);
void set_eq_mid_hz(int x);
void set_eq_treble(int x);
void set_eq_treb_hz(int x);
void set_eq_on_off(gboolean val);
void set_noisegate_type(int type);
void set_gate_option(guint32 option, int x);
void set_noisegate_on_off(gboolean val);
void set_chorusfx_option(guint32 option, int x);
void set_chorusfx_type(int type);
void set_chorusfx_on_off(gboolean val);
void set_delay_time(int x);
void set_delay_type(int type);
void set_delay_option(guint32 option, int x);
void set_delay_on_off(gboolean val);
void set_reverb_option(guint32 option, int x);
void set_reverb_type(int type);
void set_reverb_on_off(gboolean val);
