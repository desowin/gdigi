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
#include "gui.h"
#include "effects.h"

extern EffectList effects[];
extern int n_effects;

void value_changed_option_cb(GtkSpinButton *spinbutton, EffectSettings *setting)
{
    g_return_if_fail(setting != NULL);

    int val = gtk_spin_button_get_value_as_int(spinbutton);
    set_option(setting->option, setting->position, val);
}

void toggled_cb(GtkToggleButton *button, Effect *effect)
{
    g_return_if_fail(effect != NULL);

    guint val = gtk_toggle_button_get_active(button);
    set_option(effect->option, effect->position, val);
}

GtkWidget *create_table(EffectSettings *settings, gint amt)
{
    GtkWidget *table, *label, *widget;
    GtkObject *adj;
    int x;

    table = gtk_table_new(2, amt, FALSE);

    for (x = 0; x<amt; x++) {
        label = gtk_label_new(settings[x].label);
        adj = gtk_adjustment_new(0.0, settings[x].min, settings[x].max, 1.0, 1.0, 1.0);
        widget = gtk_spin_button_new(GTK_ADJUSTMENT(adj), 1.0, 0);
        g_signal_connect(G_OBJECT(widget), "value-changed", G_CALLBACK(value_changed_option_cb), &settings[x]);

        gtk_table_attach(GTK_TABLE(table), label, 0, 1, x, x+1, GTK_SHRINK, GTK_SHRINK, 2, 2);
        gtk_table_attach(GTK_TABLE(table), widget, 1, 2, x, x+1, GTK_SHRINK, GTK_SHRINK, 2, 2);
    }

    return table;
}

GtkWidget *create_on_off_button(Effect *effect)
{
    GtkWidget *button = gtk_toggle_button_new_with_label(effect->label);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button), FALSE);
    g_signal_connect(G_OBJECT(button), "toggled", G_CALLBACK(toggled_cb), effect);
    return button;
}

typedef struct {
    gint id;               /* effect group ID (value) */
    gint option;           /* option ID */
    gint position;         /* position */
    GtkWidget *child;      /* child widget */
} EffectSettingsGroup;

void effect_settings_group_free(EffectSettingsGroup *group)
{
    /* destroy widget without parent */
    if (gtk_widget_get_parent(group->child) == NULL)
        gtk_widget_destroy(group->child);

    g_object_unref(group->child);
    g_free(group);
}

void combo_box_changed_cb(GtkComboBox *widget, gpointer data)
{
    GtkWidget *child;
    GtkWidget *vbox;
    EffectSettingsGroup *settings = NULL;
    gchar *name = NULL;
    gint x;
    g_object_get(G_OBJECT(widget), "active", &x, NULL);

    vbox = g_object_get_data(G_OBJECT(widget), "vbox");

    if (x != -1) {
        name = g_strdup_printf("SettingsGroup%d", x);
        settings = g_object_get_data(G_OBJECT(widget), name);
        g_free(name);

        if (settings != NULL)
            set_option(settings->option, settings->position, settings->id);

        child = g_object_get_data(G_OBJECT(widget), "active_child");
        if (child != NULL) {
            gtk_container_remove(GTK_CONTAINER(gtk_widget_get_parent(gtk_widget_get_parent(vbox))), child);
        }
        gtk_container_add(GTK_CONTAINER(gtk_widget_get_parent(gtk_widget_get_parent(vbox))), settings->child);
        gtk_widget_show_all(gtk_widget_get_parent(gtk_widget_get_parent(vbox)));
        g_object_set_data(G_OBJECT(widget), "active_child", settings->child);
    }
}

GtkWidget *create_widget_container(EffectGroup *group, gint amt)
{
    GtkWidget *vbox;
    GtkWidget *widget;
    GtkWidget *combo_box = NULL;
    EffectSettingsGroup *settings = NULL;
    gchar *name = NULL;
    gint x;
    gint cmbox_no = -1;

    vbox = gtk_vbox_new(FALSE, 0);

    for (x = 0; x<amt; x++) {
        if (group[x].label) {
            if (combo_box == NULL) {
                combo_box = gtk_combo_box_new_text();
                gtk_container_add(GTK_CONTAINER(vbox), combo_box);
                g_signal_connect(G_OBJECT(combo_box), "changed", G_CALLBACK(combo_box_changed_cb), group);
                g_object_set_data(G_OBJECT(combo_box), "vbox", vbox);
            }
            gtk_combo_box_append_text(GTK_COMBO_BOX(combo_box), group[x].label);
            cmbox_no++;

            widget = create_table(group[x].settings, group[x].settings_amt);
            g_object_ref_sink(widget);

            settings = g_malloc(sizeof(EffectSettingsGroup));
            settings->id = group[x].id;
            settings->option = group[x].option;
            settings->position = group[x].position;
            settings->child = widget;

            name = g_strdup_printf("SettingsGroup%d", cmbox_no);
            g_object_set_data_full(G_OBJECT(combo_box), name, settings, ((GDestroyNotify)effect_settings_group_free));
            g_free(name);
        } else {
            widget = create_table(group[x].settings, group[x].settings_amt);
            gtk_container_add(GTK_CONTAINER(vbox), widget);
        }
    }

    return vbox;
};

GtkWidget *create_vbox(Effect *widgets, gint amt)
{
    GtkWidget *vbox;
    GtkWidget *hbox;
    GtkWidget *widget;
    GtkWidget *table;
    int x;

    vbox = gtk_vbox_new(FALSE, 0);
    hbox = gtk_hbox_new(FALSE, 0);

    gtk_box_set_homogeneous(GTK_BOX(hbox), TRUE);

    for (x = 0; x<amt; x++) {
        widget = create_on_off_button(&widgets[x]);
        gtk_box_pack_start(GTK_BOX(hbox), widget, TRUE, TRUE, 2);

        table = create_widget_container(widgets[x].group, widgets[x].group_amt);
        gtk_box_pack_start(GTK_BOX(hbox), table, TRUE, TRUE, 2);
    }

    gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 2);
    return vbox;
}

void create_window()
{
    GtkWidget *window;
    GtkWidget *vbox;
    GtkWidget *hbox;
    GtkWidget *widget;
    gint x;

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

    vbox = gtk_vbox_new(FALSE, 0);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    for (x = 0; x<n_effects; x++) {
        if ((x % 2) == 0) {
            hbox = gtk_hbox_new(TRUE, 0);
            gtk_box_pack_start(GTK_BOX(vbox), hbox, TRUE, TRUE, 2);
        }
        widget = create_vbox(effects[x].effect, effects[x].amt);
        gtk_box_pack_start(GTK_BOX(hbox), widget, TRUE, TRUE, 2);
    }

    gtk_widget_show_all(window);

    g_signal_connect(G_OBJECT(window), "delete_event", G_CALLBACK(gtk_main_quit), NULL);
}
