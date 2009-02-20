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

#define CE_CHORUS_SPEED 0x000345
#define CE_CHORUS_DEPTH 0x000346

#define DUAL_CHORUS_SPEED 0x000345
#define DUAL_CHORUS_DEPTH 0x000346
#define DUAL_CHORUS_LEVEL 0x000344
#define DUAL_CHORUS_WAVE 0x000348

#define MULTI_CHORUS_SPEED 0x000345
#define MULTI_CHORUS_DEPTH 0x000346
#define MULTI_CHORUS_WAVE  0x000348
#define MULTI_CHORUS_LEVEL 0x000344

#define FLANGER_SPEED 0x200306
#define FLANGER_DEPTH 0x200307
#define FLANGER_REGEN 0x200308
#define FLANGER_LEVEL 0x200305
#define FLANGER_WAVE 0x200309

#define MXR_FLANGER_SPEED 0x200306
#define MXR_FLANGER_WIDTH 0x200312
#define MXR_FLANGER_REGEN 0x200308
#define MXR_FLANGER_MANUAL 0x200315

#define PHASER_SPEED 0x200342
#define PHASER_DEPTH 0x200343
#define PHASER_REGEN 0x200346
#define PHASER_LEVEL 0x200345
#define PHASER_WAVE 0x200347

#define VIBRATO_SPEED 0x000504
#define VIBRATO_DEPTH 0x000505

#define ROTARY_SPEED 0x000542
#define ROTARY_INTENSITY 0x000544
#define ROTARY_DOPPLER 0x000546
#define ROTARY_CROSSOVER 0x000547

#define VIBROPAN_SPEED 0x000522
#define VIBROPAN_DEPTH 0x000523
#define VIBROPAN_VIBRA 0x000524
#define VIBROPAN_WAVE 0x000525

#define TREMOLO_SPEED 0x200404
#define TREMOLO_DEPTH 0x200403
#define TREMOLO_WAVE 0x200405

#define PANNER_SPEED 0x200444
#define PANNER_DEPTH 0x200443
#define PANNER_WAVE 0x200445

#define ENVELOPE_SENSITIVITY 0x000646
#define ENVELOPE_RANGE 0x000645

#define AUTOYA_SPEED 0x200546
#define AUTOYA_INTENSITY 0x20054A
#define AUTOYA_RANGE 0x20054B

#define YAYA_PEDAL 0x200502
#define YAYA_INTENSITY 0x200509
#define YAYA_RANGE 0x20050A

#define STEP_FILTER_SPEED 0x200B42
#define STEP_FILTER_INTENSITY 0x200B43

#define WHAMMY_AMOUNT 0x000705
#define WHAMMY_PEDAL 0x000703
#define WHAMMY_MIX 0x000704

#define PITCH_AMOUNT 0x200642
#define PITCH_MIX 0x200651

#define DETUNE_AMOUNT 0x200604
#define DETUNE_LEVEL 0x200603

#define IPS_SHIFT_AMOUNT 0x200A42
#define IPS_KEY 0x200A44
#define IPS_SCALE 0x200A43
#define IPS_LEVEL 0x200A45

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
