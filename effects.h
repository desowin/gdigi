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

#include <glib/gtypes.h>

typedef struct {
    char *label;    /* Parameter name */
    gdouble min;    /* Parameter minumum value */
    gdouble max;    /* Parameter maximum value */
    guint option;   /* ID (to set parameter) */
    guint position; /* position */
} EffectSettings;

typedef struct {
    guint id;                  /* value (type) */
    gchar *label;              /* Effect name */
    guint option;              /* ID (to set effect type) */
    guint position;            /* position */
    EffectSettings *settings;  /* possible parameters */
    gint settings_amt;         /* possible parameters length */
} EffectGroup;

typedef struct {
    char *label;        /* Base effect name */
    guint option;       /* ID (to set effect on/off) */
    guint position;     /* position */
    EffectGroup *group; /* possible effect types */
    gint group_amt;     /* possible effect types length */
} Effect;

typedef struct {
    Effect *effect; /* list of supported effects */
    gint amt;       /* list of supported effects length */
} EffectList;

#endif /* GDIGI_EFFECTS_H */
