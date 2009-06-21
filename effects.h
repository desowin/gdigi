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

typedef struct {
    gdouble min;    /**< Minumum value */
    gdouble max;    /**< Maximum value */
    GStrv labels;   /**< value labels */
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

typedef struct {
    gchar *name;
    unsigned char family_id;
    unsigned char product_id;
    EffectPage *pages;
    gint n_pages;
    Banks *banks;
    gint n_banks;
} Device;

ModifierGroup *modifier_linkable_list();
void modifier_group_free(ModifierGroup *modifier_group);
gboolean get_device_info(unsigned char device_id, unsigned char family_id,
                         unsigned char product_id,
                         Device **device);

#endif /* GDIGI_EFFECTS_H */
