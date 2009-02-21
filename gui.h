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

typedef struct {
    char *label;
    void (*callback)(int);
    void (*callback_with_option)(guint32, int);
    gdouble min;
    gdouble max;
    guint32 option;
} SettingsWidget;

typedef struct {
    gint id;
    gchar *label;
    void (*callback)(int);
    SettingsWidget *widgets;
    gint widgets_amt;
    GtkWidget *child; /* child widget - set inside create_widget_container */
} WidgetContainer;

typedef struct {
    char *label;
    gboolean value;
    void (*callback)(gboolean);
    WidgetContainer *widgets;
    gint widgets_amt;
} VBoxWidget;

typedef struct {
    VBoxWidget *widget;
    gint amt;
} VBoxes;

void create_window();
