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

#include <gtk/gtk.h>
#include "gdigi.h"

void value_changed_cb(GtkSpinButton *spinbutton, void (*callback)(struct usb_dev_handle*, int))
{
    int val = gtk_spin_button_get_value_as_int(spinbutton);
    callback(handle, val);
}

void toggled_cb(GtkToggleButton *button, void (*callback)(struct usb_dev_handle*, gboolean))
{
    gboolean val = gtk_toggle_button_get_active(button);
    callback(handle, val);
}

typedef struct {
    char *label;
    void (*callback)(struct usb_dev_handle*, int);
    gdouble min;
    gdouble max;
} SettingsWidget;

static SettingsWidget wah_widgets[] = {
    {"Wah min", set_wah_min, 0.0, 99.0},
    {"Wah max", set_wah_max, 0.0, 99.0},
    {"Wah level", set_wah_level, 0.0, 12.0},
};

static SettingsWidget comp_widgets[] = {
    {"Compressor sustain", set_comp_sustain, 0.0, 99.0},
    {"Compressor tone (digi only!)", set_comp_tone, 0.0, 99.0},
    {"Compressor attack", set_comp_attack, 0.0, 99.0},
    {"Compressor level", set_comp_level, 0.0, 99.0},
};

GtkWidget *create_table(SettingsWidget *widgets, gint amt)
{
    GtkWidget *table, *label, *widget;
    GtkObject *adj;
    int x;

    table = gtk_table_new(2, amt, FALSE);

    for (x = 0; x<amt; x++) {
        label = gtk_label_new(widgets[x].label);
        adj = gtk_adjustment_new(0.0, widgets[x].min, widgets[x].max, 1.0, 1.0, 1.0);
        widget = gtk_spin_button_new(GTK_ADJUSTMENT(adj), 1.0, 0);
        g_signal_connect(G_OBJECT(widget), "value-changed", G_CALLBACK(value_changed_cb), widgets[x].callback);

        gtk_table_attach(GTK_TABLE(table), label, 0, 1, x, x+1, GTK_SHRINK, GTK_SHRINK, 2, 2);
        gtk_table_attach(GTK_TABLE(table), widget, 1, 2, x, x+1, GTK_SHRINK, GTK_SHRINK, 2, 2);
    }

    return table;
}

GtkWidget *create_on_off_button(const gchar *label, gboolean state, void (*callback)(struct usb_dev_handle*, int))
{
    GtkWidget *button = gtk_toggle_button_new_with_label(label);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button), state);
    g_signal_connect(G_OBJECT(button), "toggled", G_CALLBACK(toggled_cb), callback);
    return button;
}

void create_window()
{
    GtkWidget *window;
    GtkWidget *vbox;
    GtkWidget *wah_vbox;
    GtkWidget *comp_vbox;
    GtkWidget *table;
    GtkWidget *widget;

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

    vbox = gtk_vbox_new(FALSE, 0);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    wah_vbox = gtk_vbox_new(FALSE, 0);
    gtk_container_add(GTK_CONTAINER(vbox), wah_vbox);

    widget = create_on_off_button("Wah", FALSE, set_wah_on_off);
    gtk_container_add(GTK_CONTAINER(wah_vbox), widget);

    table = create_table(wah_widgets, G_N_ELEMENTS(wah_widgets));
    gtk_container_add(GTK_CONTAINER(wah_vbox), table);

    comp_vbox = gtk_vbox_new(FALSE, 0);
    gtk_container_add(GTK_CONTAINER(vbox), comp_vbox);

    widget = create_on_off_button("Compressor", FALSE, set_comp_on_off);
    gtk_container_add(GTK_CONTAINER(comp_vbox), widget);

    table = create_table(comp_widgets, G_N_ELEMENTS(comp_widgets));
    gtk_container_add(GTK_CONTAINER(comp_vbox), table);

    gtk_widget_show_all(window);

    g_signal_connect(G_OBJECT(window), "delete_event", G_CALLBACK(gtk_main_quit), NULL);
}
