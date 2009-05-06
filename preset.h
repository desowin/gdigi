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

#ifndef GDIGI_PRESET_H
#define GDIGI_PRESET_H

#include <glib.h>

typedef struct {
    gchar *name;
    GList *params;
} Preset;

Preset *create_preset_from_xml_file(gchar *filename, GError **error);
Preset *create_preset_from_data(GList *list);
void preset_free(Preset *preset);

#endif /* GDIGI_PRESET_H */
