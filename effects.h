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

#include <glib/gtypes.h>

typedef struct {
    char *label;
    gdouble min;
    gdouble max;
    guint option;
    guint position;
} SettingsWidget;

typedef struct {
    guint id;
    gchar *label;
    guint option;
    guint position;
    SettingsWidget *widgets;
    gint widgets_amt;
} WidgetContainer;

typedef struct {
    char *label;
    gboolean value;
    guint option;
    guint position;
    WidgetContainer *widgets;
    gint widgets_amt;
} VBoxWidget;

typedef struct {
    VBoxWidget *widget;
    gint amt;
} VBoxes;
