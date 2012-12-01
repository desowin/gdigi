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

#ifndef GDIGI_EFFECTS_H
#define GDIGI_EFFECTS_H

#include <glib.h>

typedef enum {
  VALUE_TYPE_PLAIN  = 0,      /**< value displayed directly */
  VALUE_TYPE_SUFFIX = 1 << 0, /**< use suffix for displaying */
  VALUE_TYPE_OFFSET = 1 << 1, /**< use value offset */
  VALUE_TYPE_STEP   = 1 << 2, /**< use value step different than 1 */
  VALUE_TYPE_LABEL  = 1 << 3, /**< use value labels (overrides any other option) */
  VALUE_TYPE_EXTRA  = 1 << 4, /**< use extra values */
  VALUE_TYPE_DECIMAL= 1 << 5, /**< display decimal places */
  VALUE_TYPE_POSID  = 1 << 6, /**< Packed Position/ID: (pos << 16) | id */
  VALUE_TYPE_NONE   = 1 << 7, /**< no value displayed */
} ValueType;

typedef struct _EffectValues {
    gdouble min;    /**< Minumum value */
    gdouble max;    /**< Maximum value */
    gint type;      /**< value type bitmask (ValueType) */

    GStrv labels;   /**< value labels */
    gint offset;    /**< value offset */
    gdouble step;   /**< value step */
    gchar *suffix;  /**< value suffix */

    struct _EffectValues *extra;
                    /**< additional value range,
                         use it when there're different range types */
    gint decimal;   /**< amount of decimal places to display */
} EffectValues;

typedef struct {
    gchar *label;         /**< Parameter name */
    guint id;             /**< ID (to set parameter) */
    guint position;       /**< position */
    EffectValues *values; /**< valid parameter values */
} EffectSettings;

typedef struct {
    guint type;                /**< value (type) */
    gchar *label;              /**< Effect name */
    EffectSettings *settings;  /**< possible parameters */
    gint settings_amt;         /**< possible parameters length */
} EffectGroup;

typedef struct {
    gchar *label;       /**< Group label */
    guint id;           /**< ID to set effect on/off, or if it isn't on/off group then -1 */
    guint type;         /**< ID to set effect type */
    guint position;     /**< position */
    EffectGroup *group; /**< possible effect types */
    gint group_amt;     /**< possible effect types length */
} Effect;

typedef struct {
    gchar *label;   /**< base effect name */
    Effect *effect; /**< list of supported effects */
    gint amt;       /**< list of supported effects length */
} EffectList;

typedef struct {
    guint type;         /**< ID to set effect type */
    guint position;
    EffectGroup *group;
    gint group_amt;
} ModifierGroup;

typedef struct {
    gchar *name;
    EffectList *effects;
    gint n_effects;
    gint n_rows;
} EffectPage;

typedef struct {
    gchar *name;
    PresetBank bank;
} Banks;

enum product_id {
   RP150    = 1,
   RP250    = 2,
   GNX4     = 3,
   GNX3000  = 4,
   RP500    = 5,
   RP1000   = 6,
   RP155    = 7,
   RP255    = 8,
   RP355    = 9,
};
   
typedef struct {
    gchar *name;
    unsigned char family_id;
    unsigned char product_id;
    EffectPage *pages;
    gint n_pages;
    Banks *banks;
    gint n_banks;
} Device;

gchar *get_position(guint position);
ModifierGroup *modifier_linkable_list();
ModifierGroup *ModifierLinkableList;
void modifier_group_free(ModifierGroup *modifier_group);
void get_values_info(EffectValues *values,
                     gdouble *min, gdouble *max, gboolean *custom);
gboolean get_device_info(unsigned char device_id, unsigned char family_id,
                         unsigned char product_id,
                         Device **device);


#endif /* GDIGI_EFFECTS_H */
