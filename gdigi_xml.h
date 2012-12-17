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

#ifndef GDIGI_XML_H
#define GDIGI_XML_H

#include <glib.h>
#include "effects.h"
typedef struct {
    guint   type;
    gchar  *label;
} XmlLabel;

typedef struct {
    guint           id;
    guint           position;
    gchar          *label;
    EffectValues   *values;
    XmlLabel       *xml_labels;        /* 'type' id's have a label group. */
    guint           xml_labels_amt;
} XmlSettings;

XmlSettings *get_xml_settings(guint id, guint position);
gboolean value_is_extra(EffectValues *val, int value);
gchar * map_xml_value(XmlSettings *xml, EffectValues *values, gint value);

#endif /* GDIGI_XML_H */
