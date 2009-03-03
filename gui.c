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
#include "preset.h"

extern EffectList effects[];
extern int n_effects;

static gboolean allow_send = FALSE;

void show_error_message(GtkWidget *parent, gchar *message)
{
    g_return_if_fail(message != NULL);

    GtkWidget *msg = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL,
                                            GTK_MESSAGE_ERROR,
                                            GTK_BUTTONS_OK,
                                            message);

    gtk_dialog_run(GTK_DIALOG(msg));
    gtk_widget_destroy(msg);
}

typedef struct {
    GtkWidget *widget;
    gint id;
    gint position;

    /* used for combo boxes, if widget isn't combo box, then both value and x are -1 */
    gint value;           /* effect type value */
    gint x;               /* combo box item number */
} WidgetListElem;

void value_changed_option_cb(GtkSpinButton *spinbutton, EffectSettings *setting)
{
    g_return_if_fail(setting != NULL);

    if (allow_send) {
        gint val = gtk_spin_button_get_value_as_int(spinbutton);
        set_option(setting->option, setting->position, val);
    }
}

void toggled_cb(GtkToggleButton *button, Effect *effect)
{
    g_return_if_fail(effect != NULL);

    if (allow_send) {
        guint val = gtk_toggle_button_get_active(button);
        set_option(effect->option, effect->position, val);
    }
}

static void widget_list_add(GList **list, GtkWidget *widget, gint id, gint position, gint value, gint x)
{
    WidgetListElem *el;

    el = g_malloc(sizeof(WidgetListElem));
    el->widget = widget;
    el->id = id;
    el->position = position;
    el->value = value;
    el->x = x;

    *list = g_list_prepend(*list, el);
}

static void apply_widget_setting(WidgetListElem *el, SettingParam *param)
{
    if ((el->id == param->id) && (el->position == param->position)) {
        if (el->value == -1) {
            if (GTK_IS_TOGGLE_BUTTON(el->widget))
                gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(el->widget), (param->value == 0) ? FALSE : TRUE);
            else if (GTK_IS_SPIN_BUTTON(el->widget))
                gtk_spin_button_set_value(GTK_SPIN_BUTTON(el->widget), param->value);
        } else { /* combo box */
            if (el->value == param->value)
                gtk_combo_box_set_active(GTK_COMBO_BOX(el->widget), el->x);
        }
    }
}

static void apply_preset_to_gui(GList *list, Preset *preset)
{
    allow_send = FALSE;

    GList *iter = preset->params;
    while (iter) {
        SettingParam *param = iter->data;
        iter = iter->next;

        if (param != NULL)
            g_list_foreach(list, (GFunc)apply_widget_setting, param);
    }

    allow_send = TRUE;
}

static void apply_current_preset(GList *list)
{
    GString *msg = get_current_preset();
    Preset *preset = create_preset_from_data(msg);
    g_string_free(msg, TRUE);
    apply_preset_to_gui(list, preset);
    preset_free(preset);
}

GtkWidget *create_table(GList **list, EffectSettings *settings, gint amt)
{
    GtkWidget *table, *label, *widget;
    GtkObject *adj;
    int x;

    table = gtk_table_new(2, amt, FALSE);

    for (x = 0; x<amt; x++) {
        label = gtk_label_new(settings[x].label);
        adj = gtk_adjustment_new(0.0, settings[x].min, settings[x].max, 1.0, 1.0, 0.0);
        widget = gtk_spin_button_new(GTK_ADJUSTMENT(adj), 1.0, 0);
        g_signal_connect(G_OBJECT(widget), "value-changed", G_CALLBACK(value_changed_option_cb), &settings[x]);
        widget_list_add(list, widget, settings[x].option, settings[x].position, -1, -1);
        gtk_table_attach(GTK_TABLE(table), label, 0, 1, x, x+1, GTK_SHRINK, GTK_SHRINK, 2, 2);
        gtk_table_attach(GTK_TABLE(table), widget, 1, 2, x, x+1, GTK_SHRINK, GTK_SHRINK, 2, 2);
    }

    return table;
}

GtkWidget *create_on_off_button(GList **list, Effect *effect)
{
    GtkWidget *button = gtk_toggle_button_new_with_label(effect->label);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button), FALSE);
    g_signal_connect(G_OBJECT(button), "toggled", G_CALLBACK(toggled_cb), effect);
    widget_list_add(list, button, effect->option, effect->position, -1, -1);
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

        if (settings != NULL && allow_send)
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

GtkWidget *create_widget_container(GList **list, EffectGroup *group, gint amt)
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

            widget = create_table(list, group[x].settings, group[x].settings_amt);
            g_object_ref_sink(widget);

            settings = g_malloc(sizeof(EffectSettingsGroup));
            settings->id = group[x].id;
            settings->option = group[x].option;
            settings->position = group[x].position;
            settings->child = widget;
            widget_list_add(list, combo_box, group[x].option, group[x].position, group[x].id, x);

            name = g_strdup_printf("SettingsGroup%d", cmbox_no);
            g_object_set_data_full(G_OBJECT(combo_box), name, settings, ((GDestroyNotify)effect_settings_group_free));
            g_free(name);
        } else {
            widget = create_table(list, group[x].settings, group[x].settings_amt);
            gtk_container_add(GTK_CONTAINER(vbox), widget);
        }
    }

    return vbox;
};

GtkWidget *create_vbox(GList **list, Effect *widgets, gint amt)
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
        widget = create_on_off_button(list, &widgets[x]);
        gtk_box_pack_start(GTK_BOX(hbox), widget, TRUE, TRUE, 2);

        table = create_widget_container(list, widgets[x].group, widgets[x].group_amt);
        gtk_box_pack_start(GTK_BOX(hbox), table, TRUE, TRUE, 2);
    }

    gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 2);
    return vbox;
}

enum {
  PRESET_NAME_COLUMN = 0,
  PRESET_NUMBER_COLUMN,
  PRESET_BANK_COLUMN,
  NUM_COLUMNS
};

void row_activate_cb(GtkTreeView *treeview, GtkTreePath *path, GtkTreeViewColumn *column, GtkTreeModel *model) {
    GtkTreeIter iter;
    gint id;
    gint bank;

    gtk_tree_model_get_iter(model, &iter, path);
    gtk_tree_model_get(model, &iter, PRESET_NUMBER_COLUMN, &id, PRESET_BANK_COLUMN, &bank, -1);

    if ((bank != -1) && (id != -1)) {
        GList **list = g_object_get_data(G_OBJECT(treeview), "widgets-list");
        switch_preset(bank, id);
        if (*list != NULL)
            apply_current_preset(*list);
    }
}

static void fill_store_with_presets(GtkTreeStore *model, guint bank, gchar *name)
{
    GtkTreeIter iter;
    GtkTreeIter child_iter;
    int x;

    GStrv presets = query_preset_names(bank);
    gtk_tree_store_append(model, &iter, NULL);
    gtk_tree_store_set(model, &iter,
                       PRESET_NAME_COLUMN, name,
                       PRESET_NUMBER_COLUMN, -1,
                       PRESET_BANK_COLUMN, -1,
                       -1);

    for (x=0; x<g_strv_length(presets); x++) {
        gtk_tree_store_append(model, &child_iter, &iter);
        gtk_tree_store_set(model, &child_iter,
                           PRESET_NAME_COLUMN, presets[x],
                           PRESET_NUMBER_COLUMN, x,
                           PRESET_BANK_COLUMN, bank,
                           -1);
    }
    g_strfreev(presets);
}

static void fill_store(GtkTreeStore *model)
{
    fill_store_with_presets(model, PRESETS_USER, "User Presets");
    fill_store_with_presets(model, PRESETS_SYSTEM, "System Presets");
}

GtkWidget *create_preset_tree(GList **list)
{
    GtkWidget *treeview;
    GtkTreeStore *store;
    GtkCellRenderer *renderer;

    store = gtk_tree_store_new(NUM_COLUMNS, G_TYPE_STRING, G_TYPE_INT, G_TYPE_INT);
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

    g_object_set_data(G_OBJECT(treeview), "widgets-list", list);

    return treeview;
}

static void show_store_preset_window(GtkWidget *window, gchar *default_name)
{
    GtkWidget *dialog, *cmbox, *entry, *table, *label;
    GStrv names;
    int x;

    dialog = gtk_dialog_new_with_buttons("Store preset",
                                         GTK_WINDOW(window),
                                         GTK_DIALOG_DESTROY_WITH_PARENT,
                                         GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                                         GTK_STOCK_OK, GTK_RESPONSE_ACCEPT,
                                         NULL);

    table = gtk_table_new(2, 2, FALSE);
    gtk_container_add(GTK_CONTAINER(GTK_DIALOG(dialog)->vbox), table);

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

    gtk_widget_show_all(GTK_DIALOG(dialog)->vbox);
    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        gint number = gtk_combo_box_get_active(GTK_COMBO_BOX(cmbox));
        if (number != -1) {
            store_preset_name(number, gtk_entry_get_text(GTK_ENTRY(entry)));
        }
    }

    gtk_widget_destroy(dialog);
}

static void action_store_cb(GtkAction *action)
{
    GtkWidget *window = g_object_get_data(G_OBJECT(action), "window");
    show_store_preset_window(window, NULL);
}

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

typedef struct {
    gchar *name;
    gchar *suffix;
} SupportedFileTypes;

static SupportedFileTypes file_types[] = {
    {"RP250Preset", "*.rp250p"},
};
static guint n_file_types = G_N_ELEMENTS(file_types);

static void action_open_preset_cb(GtkAction *action)
{
    static GtkWidget *dialog = NULL;

    if (dialog != NULL)
        return;

    GtkWidget *window = g_object_get_data(G_OBJECT(action), "window");
    GList **list = g_object_get_data(G_OBJECT(action), "widget-list");

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
        gchar *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        Preset *preset = create_preset_from_xml_file(filename);
        if (preset != NULL) {
            if (list != NULL)
                apply_preset_to_gui(*list, preset);

            gtk_widget_hide(dialog);

            GString *msg = g_string_sized_new(500);
            GList *iter = preset->params;
            gint len = g_list_length(iter);

            g_string_append_printf(msg, "%c%c",
                                   ((len & 0xFF00) >> 8),
                                   (len & 0xFF));

            while (iter) {
                SettingParam *param = iter->data;
                iter = iter->next;

                g_string_append_printf(msg, "%c%c%c",
                                       ((param->id & 0xFF00) >> 8),
                                       (param->id & 0xFF),
                                       param->position);

                append_value(msg, param->value);
            };

            GString *start = g_string_new(NULL);
            g_string_append_printf(start,
                                   "%c%c%s%c%c%c",
                                   PRESETS_EDIT_BUFFER, 0,
                                   preset->name, 0 /* NULL terminated string */,
                                   0 /* modified */, 2 /* messages to follow */);

            send_message(RECEIVE_PRESET_START, start->str, start->len);
            send_message(RECEIVE_PRESET_PARAMETERS, msg->str, msg->len);
            send_message(RECEIVE_PRESET_END, NULL, 0);

            show_store_preset_window(window, preset->name);

            g_string_free(start, TRUE);
            g_string_free(msg, TRUE);
            preset_free(preset);
            loaded = TRUE;
        }
        g_free(filename);
    }

    gtk_widget_destroy(dialog);
    dialog = NULL;
}

static void widget_list_free(GList *list)
{
    GList *iter;
    for (iter = list; iter; iter = iter->next) {
        g_free(iter->data);
    }
    g_list_free(list);
}

static void action_quit_cb(GtkAction *action)
{
    GtkWidget *window = g_object_get_data(G_OBJECT(action), "window");
    GList **list = g_object_get_data(G_OBJECT(action), "widget-list");

    widget_list_free(*list);
    *list = NULL;

    gtk_widget_destroy(window);
    gtk_main_quit();
}

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

static void add_action_data(GtkUIManager *ui, const gchar *path, GtkWidget *window, GList **list)
{
    GtkAction *action;

    action = gtk_ui_manager_get_action(ui, path);
    g_object_set_data(G_OBJECT(action), "window", window);
    g_object_set_data(G_OBJECT(action), "widget-list", list);
}

static void add_menubar(GList **list, GtkWidget *window, GtkWidget *vbox)
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

    add_action_data(ui, "/MenuBar/File/Quit", window, list);
    add_action_data(ui, "/MenuBar/File/Open", window, list);
    add_action_data(ui, "/MenuBar/Preset/Store", window, list);
    add_action_data(ui, "/MenuBar/Help/About", window, list);

    g_object_unref(ui);
}

void create_window()
{
    GtkWidget *window;
    GtkWidget *vbox;
    GtkWidget *hbox;
    GtkWidget *widget;
    GtkWidget *sw;             /* scrolled window to carry preset treeview */
    static GList *list = NULL; /* widget list (every data element is WidgetListElem) */
    gint x;

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "gdigi");

    vbox = gtk_vbox_new(FALSE, 0);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    add_menubar(&list, window, vbox);

    hbox = gtk_hbox_new(FALSE, 0);
    gtk_container_add(GTK_CONTAINER(vbox), hbox);

    sw = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(sw), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
    gtk_box_pack_start(GTK_BOX(hbox), sw, FALSE, FALSE, 0);

    widget = create_preset_tree(&list);
    gtk_container_add(GTK_CONTAINER(sw), widget);

    vbox = gtk_vbox_new(FALSE, 0);
    gtk_box_pack_start(GTK_BOX(hbox), vbox, TRUE, TRUE, 2);

    for (x = 0; x<n_effects; x++) {
        if ((x % 3) == 0) {
            hbox = gtk_hbox_new(TRUE, 0);
            gtk_box_pack_start(GTK_BOX(vbox), hbox, TRUE, TRUE, 2);
        }
        widget = create_vbox(&list, effects[x].effect, effects[x].amt);
        gtk_box_pack_start(GTK_BOX(hbox), widget, TRUE, TRUE, 2);
    }

    apply_current_preset(list);
    gtk_widget_show_all(window);

    g_signal_connect(G_OBJECT(window), "delete_event", G_CALLBACK(gtk_main_quit), NULL);
}
