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
#include <string.h>
#include "gdigi.h"
#include "gui.h"
#include "effects.h"
#include "preset.h"
#include "gtkknob.h"
#include "knob.h"

typedef struct {
    GtkObject *widget;

    /* used for combo boxes, if widget isn't combo box, then both value and x are -1 */
    gint value;           /**< effect type value */
    gint x;               /**< combo box item number */
} WidgetTreeElem;

#ifndef DOXYGEN_SHOULD_SKIP_THIS
static GtkKnobAnim *knob_anim = NULL; /* animation used by knobs */
#endif /* DOXYGEN_SHOULD_SKIP_THIS */
static GTree *widget_tree = NULL;     /**< this tree contains lists containing WidgetTreeElem data elements */
static gboolean allow_send = FALSE;   /**< if FALSE GUI parameter changes won't be sent to device */

/**
 *  \param parent transient parent, or NULL for none
 *  \param message error description
 *
 *  Shows error message dialog.
 **/
void show_error_message(GtkWidget *parent, gchar *message)
{
    g_return_if_fail(message != NULL);

    GtkWidget *msg = gtk_message_dialog_new(GTK_WINDOW(parent),
                                            GTK_DIALOG_MODAL,
                                            GTK_MESSAGE_ERROR,
                                            GTK_BUTTONS_OK,
                                            message);

    gtk_dialog_run(GTK_DIALOG(msg));
    gtk_widget_destroy(msg);
}

/**
 *  \param value value to examine
 *  \param values EffectValues to check value against
 *
 *  Examines whether value fits inside values range for given EffectValues.
 *
 *  \return TRUE is value fits inside range, otherwise FALSE.
 **/
static gboolean check_value_range(gint value, EffectValues *values)
{
    if (((gint) values->min <= value) && (value <= (gint) values->max))
        return TRUE;
    else
        return FALSE;
}

/**
 *  \param spin a GtkSpinButton
 *  \param new_val return value for valid input
 *  \param values signal user data, EffectValues for this parameter
 *
 *  Custom GtkSpinButton "input" handler for EffectValues with non plain type.
 *
 *  \return TRUE if new_val was set, otherwise FALSE.
 **/
static gboolean custom_value_input_cb(GtkSpinButton *spin, gdouble *new_val, EffectValues *values)
{
    gchar *text = g_strdup(gtk_entry_get_text(GTK_ENTRY(spin)));
    gchar *err = NULL;
    gdouble value;

    for (;;) {
        if (values->type & VALUE_TYPE_LABEL) {
            /** search labels for value */
            gint n;
            for (n = 0; values->labels[n] != NULL; n++) {
                if (g_strcmp0(values->labels[n], text) == 0) {
                    /* Value found */
                    *new_val = values->min + (gdouble)n;
                    g_free(text);
                    return TRUE;
                }
            }

            /* Value not found */
            if (values->type & VALUE_TYPE_EXTRA) {
                values = values->extra;
                continue;
            } else {
                g_free(text);
                return FALSE;
            }
        }

        if (values->type & VALUE_TYPE_SUFFIX) {
            /* remove suffix from entry text */
            gchar *tmp;

            tmp = strstr(text, values->suffix);
            if (tmp != NULL) {
                gchar *temp = g_strndup(text, tmp - text);
                g_free(text);
                text = temp;
            }
        }

        g_strstrip(text);

        value = g_strtod(text, &err);
        if (*err) {
            if (values->type & VALUE_TYPE_EXTRA) {
                values = values->extra;
                continue;
            } else {
                g_free(text);
                return FALSE;
            }
        }

        if (values->type & VALUE_TYPE_STEP) {
            value /= values->step;
        }

        if (values->type & VALUE_TYPE_OFFSET) {
            value -= values->offset;
        }

        if (check_value_range((gint) value, values) == FALSE) {
            if (values->type & VALUE_TYPE_EXTRA) {
                values = values->extra;
                continue;
            } else {
                g_free(text);
                return FALSE;
            }
        }

        *new_val = value;

        g_free(text);

        return TRUE;
    }
}

/**
 *  \param spin a GtkSpinButton
 *  \param values signal user data, EffectValues for this parameter
 *
 *  Custom GtkSpinButton "output" handler for EffectValues with non plain type.
 *
 *  \return TRUE if text was set, otherwise FALSE.
 **/
static gboolean custom_value_output_cb(GtkSpinButton *spin, EffectValues *values)
{
    GtkAdjustment *adj;
    gchar *text;
    gdouble value;

    adj = gtk_spin_button_get_adjustment(spin);
    value = gtk_adjustment_get_value(adj);

    while (check_value_range(value, values) == FALSE) {
        if (values->type & VALUE_TYPE_EXTRA) {
            values = values->extra;
        } else {
            g_message("custom_value_output_cb called with out of bounds value");
            return FALSE;
        }
    }

    if (values->type & VALUE_TYPE_LABEL) {
        gtk_entry_set_text(GTK_ENTRY(spin), values->labels[(gint) value - (gint) values->min]);
        return TRUE;
    }

    if (values->type & VALUE_TYPE_OFFSET) {
        value += (gdouble) values->offset;
    }

    if (values->type & VALUE_TYPE_STEP) {
        value *= values->step;
    }

    if (values->type & VALUE_TYPE_DECIMAL) {
        text = g_strdup_printf("%.*f", values->decimal, value);
    } else {
        text = g_strdup_printf("%d", (gint) value);
    }

    if (values->type & VALUE_TYPE_SUFFIX) {
        gchar *tmp;
        tmp = g_strdup_printf("%s %s", text, values->suffix);
        g_free(text);
        text = tmp;
    }

    gtk_entry_set_text(GTK_ENTRY(spin), text);
    g_free(text);

    return TRUE;
}

/**
 *  \param adj the object which emitted the signal
 *  \param setting setting controlled by adj
 *
 *  Sets effect value.
 **/
void value_changed_option_cb(GtkAdjustment *adj, EffectSettings *setting)
{
    g_return_if_fail(setting != NULL);

    if (allow_send) {
        gdouble val;
        g_object_get(G_OBJECT(adj), "value", &val, NULL);
        set_option(setting->id, setting->position, (gint)val);
    }
}

/**
 *  \param button the object which emitted the signal
 *  \param effect effect controlled by button
 *
 *  Turns effect on/off basing on state of button.
 **/
void toggled_cb(GtkToggleButton *button, Effect *effect)
{
    g_return_if_fail(effect != NULL);

    if (allow_send) {
        guint val = gtk_toggle_button_get_active(button);
        set_option(effect->id, effect->position, val);
    }
}

/**
 *  \param widget GtkObject to add to widget tree
 *  \param id object controlled ID
 *  \param position object controlled position
 *  \param value effect value type (if widget is GtkComboBox, otherwise -1)
 *  \param x combo box item number (if widget is GtkComboBox, otherwise -1)
 *
 *  Adds widget to widget tree.
 **/
static WidgetTreeElem *widget_tree_add(GtkObject *widget, gint id, gint position, gint value, gint x)
{
    GList *list;
    WidgetTreeElem *el;
    gpointer key;

    el = g_slice_new(WidgetTreeElem);
    el->widget = widget;
    el->value = value;
    el->x = x;

    key = GINT_TO_POINTER((position << 16) | id);

    list = g_tree_lookup(widget_tree, key);

    if (list == NULL) {
        list = g_list_append(list, el);
        g_tree_insert(widget_tree, key, list);
    } else {
        list = g_list_append(list, el);

        /* replace the list pointer */
        g_tree_steal(widget_tree, key);
        g_tree_insert(widget_tree, key, list);
    }

    return el;
}

/**
 *  \param el widget tree element
 *  \param param parameter to set
 *
 *  Sets widget tree element value to param value.
 **/
static void apply_widget_setting(WidgetTreeElem *el, SettingParam *param)
{
    if (el->value == -1) {
        if (GTK_IS_TOGGLE_BUTTON(el->widget))
            gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(el->widget), (param->value == 0) ? FALSE : TRUE);
        else if (GTK_IS_ADJUSTMENT(el->widget))
            gtk_adjustment_set_value(GTK_ADJUSTMENT(el->widget), (gdouble)param->value);
    } else { /* combo box */
        if (el->value == param->value)
            gtk_combo_box_set_active(GTK_COMBO_BOX(el->widget), el->x);
    }
}

/**
 *  \param param SettingParam to apply to GUI
 *
 *  Applies SettingParam to GUI
 **/
void apply_setting_param_to_gui(SettingParam *param)
{
    gpointer key;
    g_return_if_fail(param != NULL);

    allow_send = FALSE;
    key = GINT_TO_POINTER((param->position << 16) | param->id);
    GList *list = g_tree_lookup(widget_tree, key);
    g_list_foreach(list, (GFunc)apply_widget_setting, param);
    allow_send = TRUE;
}

/**
 *  \param preset preset to sync
 *
 *  Synces GUI with preset.
 **/
static void apply_preset_to_gui(Preset *preset)
{
    g_return_if_fail(preset != NULL);
    g_return_if_fail(widget_tree != NULL);

    allow_send = FALSE;

    GList *iter = preset->params;
    while (iter) {
        gpointer key;

        SettingParam *param = iter->data;
        iter = iter->next;

        if (param != NULL) {
            key = GINT_TO_POINTER((param->position << 16) | param->id);
            GList *list = g_tree_lookup(widget_tree, key);
            g_list_foreach(list, (GFunc)apply_widget_setting, param);
        }
    }

    allow_send = TRUE;
}

/**
 *  Synces GUI with device current edit buffer.
 **/
static void apply_current_preset()
{
    GList *list = get_current_preset();
    Preset *preset = create_preset_from_data(list);
    message_list_free(list);
    apply_preset_to_gui(preset);
    preset_free(preset);
}

gboolean apply_current_preset_to_gui(gpointer data)
{
    apply_current_preset();
    return FALSE;
}

/**
 *  \param settings effect parameters
 *  \param amt amount of effect parameters
 *  \param widget_table hash table matching settings pointer with created table (may be NULL)
 *
 *  Creates knobs that allow user to set effect parameters.
 *
 *  \return GtkTable containing necessary widgets to set effect parameters.
 **/
GtkWidget *create_table(EffectSettings *settings, gint amt, GHashTable *widget_table)
{
    GtkWidget *table, *label, *widget, *knob;
    GtkObject *adj;
    int x;

    if (widget_table != NULL) {
        table = g_hash_table_lookup(widget_table, settings);
        if (table != NULL)
            return table;
    }

    table = gtk_table_new(3, amt, FALSE);

    for (x = 0; x<amt; x++) {
        gdouble min, max;
        gboolean custom;

        get_values_info(settings[x].values, &min, &max, &custom);

        label = gtk_label_new(settings[x].label);
        adj = gtk_adjustment_new(0.0, min, max,
                                 1.0,  /* step increment */
                                 MAX((max / 100), 5.0), /* page increment */
                                 0.0);
        knob = gtk_knob_new(GTK_ADJUSTMENT(adj), knob_anim);

        widget = gtk_spin_button_new(GTK_ADJUSTMENT(adj), 1.0, 0);
        gtk_spin_button_set_numeric(GTK_SPIN_BUTTON(widget), FALSE);
        gtk_spin_button_set_update_policy(GTK_SPIN_BUTTON(widget), GTK_UPDATE_IF_VALID);
        if (custom == TRUE) {
            g_signal_connect(G_OBJECT(widget), "input", G_CALLBACK(custom_value_input_cb), settings[x].values);
            g_signal_connect(G_OBJECT(widget), "output", G_CALLBACK(custom_value_output_cb), settings[x].values);
        }

        widget_tree_add(adj, settings[x].id, settings[x].position, -1, -1);
        gtk_table_attach(GTK_TABLE(table), label, 0, 1, x, x+1, GTK_SHRINK, GTK_SHRINK, 2, 2);
        gtk_table_attach(GTK_TABLE(table), knob, 1, 2, x, x+1, GTK_SHRINK, GTK_SHRINK, 2, 2);
        gtk_table_attach(GTK_TABLE(table), widget, 2, 3, x, x+1, GTK_SHRINK, GTK_SHRINK, 2, 2);

        g_signal_connect(G_OBJECT(adj), "value-changed", G_CALLBACK(value_changed_option_cb), &settings[x]);

        if (widget_table != NULL) {
            g_hash_table_insert(widget_table, settings, table);
        }
    }

    return table;
}

/**
 *  \param effect Effect that can be turned on/off
 *
 *  Creates toggle button that allow user to turn effect on/off.
 *
 *  \return GtkToggleButton
 **/
GtkWidget *create_on_off_button(Effect *effect)
{
    GtkWidget *button;
    if (effect->label == NULL)
        button = gtk_check_button_new();
    else
        button = gtk_check_button_new_with_label(effect->label);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button), FALSE);
    g_signal_connect(G_OBJECT(button), "toggled", G_CALLBACK(toggled_cb), effect);
    widget_tree_add(GTK_OBJECT(button), effect->id, effect->position, -1, -1);
    return button;
}

typedef struct {
    gint type;             /**< effect group type (value) */
    gint id;               /**< option ID */
    gint position;         /**< position */
    GtkWidget *child;      /**< child widget */
} EffectSettingsGroup;

/**
 *  \param group group to be freed
 *
 *  Frees all memory used by group
 **/
void effect_settings_group_free(EffectSettingsGroup *group)
{
    if (group->child != NULL) {
        /* destroy widget without parent */
        if (gtk_widget_get_parent(group->child) == NULL)
            gtk_widget_destroy(group->child);

        g_object_unref(group->child);
    }

    g_slice_free(EffectSettingsGroup, group);
}

/**
 *  \param widget the object which emitted the signal
 *  \param data user data (unused, can be anything)
 *
 *  Switches effect type and shows widgets allowing to set selected effect type parameters.
 **/
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

        if (settings != NULL && allow_send)
            set_option(settings->id, settings->position, settings->type);

        child = g_object_get_data(G_OBJECT(widget), "active_child");
        if (child == settings->child) {
            return;
        }

        if (child != NULL) {
            gtk_container_remove(GTK_CONTAINER(gtk_widget_get_parent(gtk_widget_get_parent(vbox))), child);
        }

        if (settings->child != NULL) {
            gtk_container_add(GTK_CONTAINER(gtk_widget_get_parent(gtk_widget_get_parent(vbox))), settings->child);
            gtk_widget_show_all(gtk_widget_get_parent(gtk_widget_get_parent(vbox)));
        }
        g_object_set_data(G_OBJECT(widget), "active_child", settings->child);
    }
}

/**
 *  \param group Effect type groups
 *  \param amt amount of effect groups
 *  \param id ID to set effect type
 *  \param position position
 *
 *  Creates widget allowing user to choose effect type.
 *
 *  \return widget that allow user to set effect type.
 **/
GtkWidget *create_widget_container(EffectGroup *group, gint amt, gint id, gint position)
{
    GtkWidget *vbox;
    GtkWidget *widget;
    GtkWidget *combo_box = NULL;
    GHashTable *widget_table;
    EffectSettingsGroup *settings = NULL;
    gchar *name = NULL;
    gint x;
    gint cmbox_no = -1;

    vbox = gtk_vbox_new(FALSE, 0);

    widget_table = g_hash_table_new(g_direct_hash, g_direct_equal);

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

            if ((group[x].settings != NULL) && (group[x].settings_amt > 0)) {
                widget = create_table(group[x].settings, group[x].settings_amt, widget_table);
                g_object_ref_sink(widget);
            } else
                widget = NULL;

            settings = g_slice_new(EffectSettingsGroup);
            settings->id = id;
            settings->type = group[x].type;
            settings->position = position;
            settings->child = widget;

            widget_tree_add(GTK_OBJECT(combo_box), id, position, group[x].type, x);

            name = g_strdup_printf("SettingsGroup%d", cmbox_no);
            g_object_set_data_full(G_OBJECT(combo_box), name, settings, ((GDestroyNotify)effect_settings_group_free));
            g_free(name);
        } else {
            if ((group[x].settings != NULL) && (group[x].settings_amt > 0)) {
                widget = create_table(group[x].settings, group[x].settings_amt, widget_table);
                gtk_container_add(GTK_CONTAINER(vbox), widget);
            }
        }
    }

    g_hash_table_destroy(widget_table);

    return vbox;
}

/**
 *  \param widgets Effect descriptions
 *  \param amt amount of effect descriptions
 *  \param label frame label (can be NULL)
 *
 *  Creates frame (with optional label) containing widgets allowing user to set effect options.
 *
 *  \return widget that allow user to set effect options.
 **/
GtkWidget *create_vbox(Effect *widgets, gint amt, gchar *label)
{
    GtkWidget *vbox;
    GtkWidget *widget;
    GtkWidget *table;
    GtkWidget *container;
    GtkWidget *frame;
    int x;
    int y;

    frame = gtk_frame_new(label);

    vbox = gtk_vbox_new(FALSE, 0);

    table = gtk_table_new(2, amt, FALSE);
    gtk_table_set_col_spacings(GTK_TABLE(table), 2);

    for (x = 0; x<amt; x++) {
        if ((widgets[x].id != -1) && (widgets[x].position != -1)) {
            widget = create_on_off_button(&widgets[x]);
            gtk_table_attach_defaults(GTK_TABLE(table), widget, 0, 1, x, x+1);

            if (widgets[x].label)
                y = 1;
            else
                y = 0;

        } else if (widgets[x].label) {
            widget = gtk_label_new(widgets[x].label);
            gtk_table_attach_defaults(GTK_TABLE(table), widget, 0, 1, x, x+1);
            y = 0;
        } else {
            /* Default to 1 */
            if (x == 0)
                y = 1;
        }

        container = create_widget_container(widgets[x].group, widgets[x].group_amt, widgets[x].type, widgets[x].position);
        gtk_table_attach_defaults(GTK_TABLE(table), container, 1-y, 2-y, x+y, x+y+1);
    }
    gtk_box_pack_start(GTK_BOX(vbox), table, FALSE, FALSE, 2);

    gtk_container_add(GTK_CONTAINER(frame), vbox);
    return frame;
}

enum {
  PRESET_NAME_COLUMN = 0,
  PRESET_NUMBER_COLUMN,
  PRESET_BANK_COLUMN,
  NUM_COLUMNS
};

/**
 *  \param treeview the object which emitted the signal
 *  \param path the GtkTreePath for the activated row
 *  \param column the GtkTreeViewColumn in which the activation occurred
 *  \param model model holding preset names
 *
 *  Sets active device preset to preset selected by user.
 **/
void row_activate_cb(GtkTreeView *treeview, GtkTreePath *path, GtkTreeViewColumn *column, GtkTreeModel *model) {
    GtkTreeIter iter;
    gint id;
    gint bank;

    gtk_tree_model_get_iter(model, &iter, path);
    gtk_tree_model_get(model, &iter, PRESET_NUMBER_COLUMN, &id, PRESET_BANK_COLUMN, &bank, -1);

    if ((bank != -1) && (id != -1)) {
        switch_preset(bank, id);
        apply_current_preset();
    }
}

/**
 *  \param model model to fill
 *  \param bank preset bank
 *  \param name preset bank description visible to user
 *
 *  Appends to model preset names found in device preset bank.
 **/
static void fill_store_with_presets(GtkTreeStore *model, guint bank, gchar *name)
{
    GtkTreeIter iter;
    GtkTreeIter child_iter;
    int x;

    GStrv presets = query_preset_names(bank);
    g_return_if_fail(presets != NULL);

    gtk_tree_store_append(model, &iter, NULL);
    gtk_tree_store_set(model, &iter,
                       PRESET_NAME_COLUMN, name,
                       PRESET_NUMBER_COLUMN, -1,
                       PRESET_BANK_COLUMN, -1,
                       -1);

    for (x=0; x<g_strv_length(presets); x++) {
        gchar *tmp = g_strdup_printf("%d - %s", x+1, presets[x]);

        gtk_tree_store_append(model, &child_iter, &iter);
        gtk_tree_store_set(model, &child_iter,
                           PRESET_NAME_COLUMN, tmp,
                           PRESET_NUMBER_COLUMN, x,
                           PRESET_BANK_COLUMN, bank,
                           -1);

        g_free(tmp);
    }
    g_strfreev(presets);
}

/**
 *  \param model model to fill
 *
 *  Fills model with preset names found on device.
 **/
static void fill_store(GtkTreeStore *model)
{
    Device *device = g_object_get_data(G_OBJECT(model), "device");

    g_return_if_fail(device != NULL);

    gint i;
    for (i=0; i<device->n_banks; i++)
        fill_store_with_presets(model,
                                device->banks[i].bank,
                                device->banks[i].name);
}

/**
 *  \param device device information
 *
 *  Creates treeview showing list of presets available on device.
 *
 *  \return treeview containing all preset names found on device.
 **/
GtkWidget *create_preset_tree(Device *device)
{
    GtkWidget *treeview;
    GtkTreeStore *store;
    GtkCellRenderer *renderer;

    store = gtk_tree_store_new(NUM_COLUMNS, G_TYPE_STRING, G_TYPE_INT, G_TYPE_INT);
    g_object_set_data(G_OBJECT(store), "device", device);
    fill_store(store);

    treeview = gtk_tree_view_new_with_model(GTK_TREE_MODEL(store));
    g_object_unref(store);

    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(treeview),
                                                -1, "Preset name",
                                                renderer, "text",
                                                PRESET_NAME_COLUMN, NULL);

    g_object_set(G_OBJECT(treeview), "headers-visible", FALSE, NULL);
    g_signal_connect(G_OBJECT(treeview), "realize", G_CALLBACK(gtk_tree_view_expand_all), NULL);
    g_signal_connect(G_OBJECT(treeview), "row-activated", G_CALLBACK(row_activate_cb), GTK_TREE_MODEL(store));

    return treeview;
}

/**
 *  \param window application toplevel window
 *  \param default_name default preset name
 *
 *  Shows window allowing user to store current edit buffer.
 **/
static void show_store_preset_window(GtkWidget *window, gchar *default_name)
{
    GtkWidget *dialog, *cmbox, *entry, *table, *label, *vbox;
    GStrv names;
    int x;

    dialog = gtk_dialog_new_with_buttons("Store preset",
                                         GTK_WINDOW(window),
                                         GTK_DIALOG_DESTROY_WITH_PARENT,
                                         GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                                         GTK_STOCK_OK, GTK_RESPONSE_ACCEPT,
                                         NULL);

    vbox = gtk_dialog_get_content_area(GTK_DIALOG(dialog));

    table = gtk_table_new(2, 2, FALSE);
    gtk_container_add(GTK_CONTAINER(vbox), table);

    cmbox = gtk_combo_box_new_text();
    names = query_preset_names(PRESETS_USER);
    for (x=0; x<g_strv_length(names); x++) {
        gchar *title = g_strdup_printf("%d - %s", x+1, names[x]);
        gtk_combo_box_append_text(GTK_COMBO_BOX(cmbox), title);
        g_free(title);
    }
    g_strfreev(names);
    gtk_table_attach_defaults(GTK_TABLE(table), cmbox, 1, 2, 0, 1);

    entry = gtk_entry_new();
    if (default_name != NULL)
        gtk_entry_set_text(GTK_ENTRY(entry), default_name);
    gtk_table_attach_defaults(GTK_TABLE(table), entry, 1, 2, 1, 2);

    label = gtk_label_new("Preset slot:");
    gtk_table_attach_defaults(GTK_TABLE(table), label, 0, 1, 0, 1);

    label = gtk_label_new("Preset name:");
    gtk_table_attach_defaults(GTK_TABLE(table), label, 0, 1, 1, 2);

    gtk_widget_show_all(vbox);
    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        gint number = gtk_combo_box_get_active(GTK_COMBO_BOX(cmbox));
        if (number != -1) {
            store_preset_name(number, gtk_entry_get_text(GTK_ENTRY(entry)));
        }
    }

    gtk_widget_destroy(dialog);
}

/**
 *  \param action the object which emitted the signal
 *
 *  Shows store preset window.
 **/
static void action_store_cb(GtkAction *action)
{
    GtkWidget *window = g_object_get_data(G_OBJECT(action), "window");
    show_store_preset_window(window, NULL);
}

/**
 *  \param action the object which emitted the signal
 *
 *  Shows about dialog.
 **/
static void action_show_about_dialog_cb(GtkAction *action)
{
    static const gchar * const authors[] = {
        "Tomasz Moń <desowin@gmail.com>",
        NULL
    };
    static const gchar copyright[] = "Copyright \xc2\xa9 2009 Tomasz Moń";
    static const gchar website[] = "http://desowin.org/gdigi/";

    GtkWidget *window = g_object_get_data(G_OBJECT(action), "window");

    gtk_show_about_dialog(GTK_WINDOW(window),
                          "authors", authors,
                          "copyright", copyright,
                          "website", website,
                          NULL);
}

#ifndef DOXYGEN_SHOULD_SKIP_THIS

typedef struct {
    gchar *name;
    gchar *suffix;
} SupportedFileTypes;

static SupportedFileTypes file_types[] = {
    {"RP155Preset", "*.rp155p"},
    {"RP250Preset", "*.rp250p"},
    {"RP255Preset", "*.rp255p"},
    {"RP355Preset", "*.rp355p"},
    {"RP500Preset", "*.rp500p"},
    {"RP1000Preset", "*.rp1000p"},
    {"GNX4 Preset", "*.g4p"},
    {"GNX3kPreset", "*.g3kp"},
};

static guint n_file_types = G_N_ELEMENTS(file_types);

#endif /* DOXYGEN_SHOULD_SKIP_THIS */

/**
 *  \param action the object which emitted the signal
 *
 *  Shows file chooser dialog.
 *  If user opens valid preset file, the preset gets applied to edit buffer and store preset window is shown.
 **/
static void action_open_preset_cb(GtkAction *action)
{
    static GtkWidget *dialog = NULL;

    if (dialog != NULL)
        return;

    GtkWidget *window = g_object_get_data(G_OBJECT(action), "window");

    dialog = gtk_file_chooser_dialog_new("Open Preset", GTK_WINDOW(window),
                                         GTK_FILE_CHOOSER_ACTION_OPEN,
                                         GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                                         GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
                                         NULL);

    GtkFileFilter *filter;
    filter = gtk_file_filter_new();
    gtk_file_filter_set_name(filter, "All Supported Types");
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), filter);

    int x;
    for (x=0; x<n_file_types; x++) {
        GtkFileFilter *current_filter = gtk_file_filter_new();

        gtk_file_filter_set_name(current_filter, file_types[x].name);
        gtk_file_filter_add_pattern(current_filter, file_types[x].suffix);
        gtk_file_filter_add_pattern(filter, file_types[x].suffix);

        gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), current_filter);
    }

    gboolean loaded = FALSE;
    while (!loaded && gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        GError *error = NULL;
        gchar *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        Preset *preset = create_preset_from_xml_file(filename, &error);
        if (error) {
            show_error_message(window, error->message);
            g_error_free(error);
            error = NULL;
        } else if (preset != NULL) {
            apply_preset_to_gui(preset);

            gtk_widget_hide(dialog);

            GString *start = g_string_new(NULL);
            g_string_append_printf(start,
                                   "%c%c%s%c%c%c",
                                   PRESETS_EDIT_BUFFER, 0,
                                   preset->name, 0 /* NULL terminated string */,
                                   0 /* modified */,
                                   /* messages to follow */
                                   preset->genetxs ? 10 : 2);

            send_message(RECEIVE_PRESET_START, start->str, start->len);
            send_preset_parameters(preset->params);
            if (preset->genetxs != NULL) {
                gint i;

                /* GNX4 sends messages in following order:
                 *   Section Bank  Index
                 *      0x00 0x04 0x0000
                 *      0x00 0x04 0x0001
                 *      0x01 0x04 0x0000
                 *      0x01 0x04 0x0001
                 *      0x00 0x04 0x0002
                 *      0x00 0x04 0x0003
                 *      0x01 0x04 0x0002
                 *      0x01 0x04 0x0003
                 */

                /* GNX3000 sends messages in following order:
                 *   Section Bank  Index
                 *      0x07 0x04 0x0000
                 *      0x07 0x04 0x0001
                 *      0x08 0x04 0x0000
                 *      0x08 0x04 0x0001
                 *      0x07 0x04 0x0002
                 *      0x07 0x04 0x0003
                 *      0x08 0x04 0x0002
                 *      0x08 0x04 0x0003
                 */
                for (i = 0; i < 2; i++) {
                    GList *iter = preset->genetxs;

                    while (iter) {
                        SectionID section;
                        guint bank, index;

                        SettingGenetx *genetx = (SettingGenetx *) iter->data;
                        iter = iter->next;

                        section = get_genetx_section_id(genetx->version,
                                                        genetx->type);
                        bank = 0x04;

                        if (i == 0) {
                            index = genetx->channel;
                        } else {
                            if (genetx->channel == GENETX_CHANNEL1) {
                                index = GENETX_CHANNEL1_CUSTOM;
                            } else if (genetx->channel == GENETX_CHANNEL2) {
                                index = GENETX_CHANNEL2_CUSTOM;
                            }
                        }

                        send_object(section, bank, index,
                                    genetx->name, genetx->data);
                    }
                }
            }
            send_message(RECEIVE_PRESET_END, NULL, 0);

            show_store_preset_window(window, preset->name);

            g_string_free(start, TRUE);
            preset_free(preset);
            loaded = TRUE;
        }
        g_free(filename);
    }

    gtk_widget_destroy(dialog);
    dialog = NULL;
}

/**
 *  \param list widget tree list to be freed
 *
 *  Frees all memory used by widget tree list.
 */
static void widget_tree_elem_free(GList *list)
{
    GList *iter;
    for (iter = list; iter; iter = iter->next) {
        g_slice_free(WidgetTreeElem, iter->data);
    }
    g_list_free(list);
}

/**
 *  \param action the object which emitted the signal
 *
 *  Destroys action object "window" data, then stops gtk main loop.
 **/
static void action_quit_cb(GtkAction *action)
{
    GtkWidget *window = g_object_get_data(G_OBJECT(action), "window");

    gtk_widget_destroy(window);
    gtk_main_quit();
}

#ifndef DOXYGEN_SHOULD_SKIP_THIS

static GtkActionEntry entries[] = {
    {"File", NULL, "_File"},
    {"Preset", NULL, "_Preset"},
    {"Help", NULL, "_Help"},
    {"Open", GTK_STOCK_OPEN, "_Open", "<control>O", "Open preset file", G_CALLBACK(action_open_preset_cb)},
    {"Quit", GTK_STOCK_QUIT, "_Quit", "<control>Q", "Quit", G_CALLBACK(action_quit_cb)},
    {"Store", NULL, "_Store", "<control>S", "Store", G_CALLBACK(action_store_cb)},
    {"About", GTK_STOCK_ABOUT, "_About", "<control>A", "About", G_CALLBACK(action_show_about_dialog_cb)},
};
static guint n_entries = G_N_ELEMENTS(entries);

static const gchar *menu_info =
"<ui>"
" <menubar name='MenuBar'>"
"  <menu action='File'>"
"   <menuitem action='Open'/>"
"   <separator/>"
"   <menuitem action='Quit'/>"
"  </menu>"
"  <menu action='Preset'>"
"   <menuitem action='Store'/>"
"  </menu>"
"  <menu action='Help'>"
"   <menuitem action='About'/>"
"  </menu>"
" </menubar>"
"</ui>";

#endif /* DOXYGEN_SHOULD_SKIP_THIS */

/**
 *  \param ui GtkUIManager to lookup actions
 *  \param path path to action
 *  \param window toplevel window
 *
 *  Sets action object "window" data to toplevel window.
 **/
static void add_action_data(GtkUIManager *ui, const gchar *path, GtkWidget *window)
{
    GtkAction *action;

    action = gtk_ui_manager_get_action(ui, path);
    g_return_if_fail(action != NULL);

    g_object_set_data(G_OBJECT(action), "window", window);
}

/**
 *  \param window toplevel window
 *  \param vbox vbox to hold menubar
 *
 *  Creates menubar (adds accel group to toplevel window as well) and packs it into vbox.
 **/
static void add_menubar(GtkWidget *window, GtkWidget *vbox)
{
    GtkUIManager *ui;
    GtkActionGroup *actions;
    GError *error = NULL;

    actions = gtk_action_group_new("Actions");
    gtk_action_group_add_actions(actions, entries, n_entries, NULL);

    ui = gtk_ui_manager_new();
    gtk_ui_manager_insert_action_group(ui, actions, 0);
    g_object_unref(actions);
    gtk_window_add_accel_group(GTK_WINDOW(window), gtk_ui_manager_get_accel_group(ui));

    if (!gtk_ui_manager_add_ui_from_string(ui, menu_info, -1, &error)) {
        g_message("building menus failed: %s", error->message);
        g_error_free(error);
        error = NULL;
    }
    gtk_box_pack_start(GTK_BOX(vbox),
                       gtk_ui_manager_get_widget(ui, "/MenuBar"),
                       FALSE, FALSE, 0);

    add_action_data(ui, "/MenuBar/File/Quit", window);
    add_action_data(ui, "/MenuBar/File/Open", window);
    add_action_data(ui, "/MenuBar/Preset/Store", window);
    add_action_data(ui, "/MenuBar/Help/About", window);

    g_object_unref(ui);
}

static gint widget_tree_key_compare_func(gconstpointer a, gconstpointer b, gpointer data)
{
    gint position_a = GPOINTER_TO_INT(a) & 0xFF0000;
    gint position_b = GPOINTER_TO_INT(b) & 0xFF0000;

    if (position_a > position_b) {
        return 1;
    } else if (position_a == position_b) {
        gint val_a = GPOINTER_TO_INT(a) & 0xFFFF;
        gint val_b = GPOINTER_TO_INT(b) & 0xFFFF;
        return val_a - val_b;
    } else
        return -1;
}

/**
 *  Creates main window.
 **/
void gui_create(Device *device)
{
    GtkWidget *window;
    GtkWidget *vbox;
    GtkWidget *hbox;
    GtkWidget *widget;
    GtkWidget *notebook;
    GtkWidget *sw;             /* scrolled window to carry preset treeview */
    gint x;
    gint i;

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "gdigi");

    vbox = gtk_vbox_new(FALSE, 0);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    add_menubar(window, vbox);

    hbox = gtk_hbox_new(FALSE, 0);
    gtk_container_add(GTK_CONTAINER(vbox), hbox);

    sw = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(sw), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
    gtk_box_pack_start(GTK_BOX(hbox), sw, FALSE, FALSE, 0);

    widget = create_preset_tree(device);
    gtk_container_add(GTK_CONTAINER(sw), widget);

    notebook = gtk_notebook_new();
    gtk_box_pack_start(GTK_BOX(hbox), notebook, TRUE, TRUE, 2);

    knob_anim = gtk_knob_animation_new_from_inline(knob_pixbuf);

    widget_tree = g_tree_new_full(widget_tree_key_compare_func,
                                  NULL, /* key compare data */
                                  NULL, /* key destroy func */
                                  (GDestroyNotify) widget_tree_elem_free);

    gtk_notebook_set_show_tabs(GTK_NOTEBOOK(notebook), device->n_pages > 1 ? TRUE : FALSE);

    for (i = 0; i<device->n_pages; i++) {
        GtkWidget *label = NULL;
        vbox = gtk_vbox_new(FALSE, 0);
        label = gtk_label_new(device->pages[i].name);

        gtk_notebook_append_page(GTK_NOTEBOOK(notebook), vbox, label);

        for (x = 0; x<device->pages[i].n_effects; x++) {
            if ((x % ((device->pages[i].n_effects+1)/device->pages[i].n_rows)) == 0) {
                hbox = gtk_hbox_new(FALSE, 0);
                gtk_box_pack_start(GTK_BOX(vbox), hbox, TRUE, TRUE, 2);
            }
            widget = create_vbox(device->pages[i].effects[x].effect, device->pages[i].effects[x].amt, device->pages[i].effects[x].label);
            gtk_box_pack_start(GTK_BOX(hbox), widget, TRUE, TRUE, 2);
        }
    }

    apply_current_preset();
    gtk_widget_show_all(window);

    g_signal_connect(G_OBJECT(window), "delete_event", G_CALLBACK(gtk_main_quit), NULL);
}

/**
 *  Frees memory allocated by gui_create which is not explicitly freed when main window is destroyed.
 **/
void gui_free()
{
    g_tree_destroy(widget_tree);
    widget_tree = NULL;

    gtk_knob_animation_free(knob_anim);
    knob_anim = NULL;
}

/**
 *  \param device Variable to hold device information
 *
 *  Displays dialogbox stating that device is unsupported.
 *
 *  \return TRUE if user selects "compability mode", otherwise FALSE.
 **/
gboolean unsupported_device_dialog(Device **device)
{
    extern Device* supported_devices[];
    extern int n_supported_devices;

    GtkWidget *dialog;
    GtkWidget *label;
    GtkWidget *combo_box;
    GtkWidget *vbox;
    int x;

    dialog = gtk_dialog_new_with_buttons("Unsupported device",
                                         NULL, GTK_DIALOG_MODAL,
                                         GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                                         GTK_STOCK_OK, GTK_RESPONSE_ACCEPT,
                                         NULL);

    vbox = gtk_dialog_get_content_area(GTK_DIALOG(dialog));

    label = gtk_label_new("Your device appears to be unsupported by gdigi.\n"
                          "As some of the settings may be common between different devices,\n"
                          "you can now select compability mode with one of the supported devices.\n"
                          "Please take a look at gdigi's HACKING file.");
    gtk_container_add(GTK_CONTAINER(vbox), label);

    combo_box = gtk_combo_box_new_text();
    for (x=0; x<n_supported_devices; x++) {
        gtk_combo_box_append_text(GTK_COMBO_BOX(combo_box), supported_devices[x]->name);
    }

    gtk_container_add(GTK_CONTAINER(vbox), combo_box);

    gtk_widget_show_all(vbox);
    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        gint number = gtk_combo_box_get_active(GTK_COMBO_BOX(combo_box));
        if (number != -1 && number <n_supported_devices) {
            g_message("Starting %s compability mode", supported_devices[number]->name);
            *device = supported_devices[number];
            gtk_widget_destroy(dialog);
            return TRUE;
        }
    }

    gtk_widget_destroy(dialog);
    return FALSE;
}
