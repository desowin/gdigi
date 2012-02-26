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

#ifndef GDIGI_GUI_H
#define GDIGI_GUI_H

#include <gtk/gtk.h>
#include <glib.h>
#include "effects.h"

gchar * get_preset_filename(int prod_id);
void show_error_message(GtkWidget *parent, gchar *message);
void apply_setting_param_to_gui(SettingParam *param);
gboolean apply_current_preset_to_gui(gpointer data);
void gui_create(Device *device);
void gui_free();
gboolean unsupported_device_dialog(Device **device);

#endif /* GDIGI_GUI_H */
