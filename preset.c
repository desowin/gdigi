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

#include <gio/gio.h>
#include <expat.h>
#include <string.h>
#include "preset.h"
#include "gdigi.h"

#ifndef DOXYGEN_SHOULD_SKIP_THIS

enum {
  PARSER_TYPE_NOT_SET = -1,
  PARSER_TYPE_PRESET_NAME = 0,
  PARSER_TYPE_PARAM_ID,
  PARSER_TYPE_PARAM_POSITION,
  PARSER_TYPE_PARAM_VALUE,
  PARSER_TYPE_PARAM_NAME,
  PARSER_TYPE_PARAM_TEXT
};

typedef struct {
    int depth;
    int id;
    Preset *preset;
} AppData;

static void XMLCALL start(void *data, const char *el, const char **attr) {
    AppData *ad = (AppData *) data;

    ad->id = PARSER_TYPE_NOT_SET;
    if (g_strcmp0(el, "Name") == 0) {
        if (ad->depth == 1) {
            ad->id = PARSER_TYPE_PRESET_NAME;
        } else if (ad->depth == 3) {
            ad->id = PARSER_TYPE_PARAM_NAME;
        }
    }

    if (g_strcmp0(el, "Params") == 0) {
        if (ad->preset->params != NULL)
            g_message("Params aleady exists!");
    } else if (g_strcmp0(el, "Param") == 0) {
        SettingParam *param = setting_param_new();
        ad->preset->params = g_list_prepend(ad->preset->params, param);
    } else if (g_strcmp0(el, "ID") == 0) {
        ad->id = PARSER_TYPE_PARAM_ID;
    } else if (g_strcmp0(el, "Position") == 0) {
        ad->id = PARSER_TYPE_PARAM_POSITION;
    } else if (g_strcmp0(el, "Value") == 0) {
        ad->id = PARSER_TYPE_PARAM_VALUE;
    } else if (g_strcmp0(el, "Text") == 0) {
        ad->id = PARSER_TYPE_PARAM_TEXT;
    }

    ad->depth++;
}

static void XMLCALL end(void *data, const char *el) {
    AppData *ad = (AppData *) data;
    ad->depth--;
    ad->id = PARSER_TYPE_NOT_SET;
}

static void XMLCALL text_cb(void *data, const char* text, int len)
{
    AppData *ad = (AppData *) data;

    if ((ad == NULL) || (ad->preset == NULL))
        return;

    if (ad->id == PARSER_TYPE_PRESET_NAME) {
        if (ad->preset->name != NULL)
            g_free(ad->preset->name);
        ad->preset->name = g_strndup(text, len);
    }

    if (ad->preset->params == NULL)
        return;

    SettingParam *param = (SettingParam *) ad->preset->params->data;
    if (param == NULL)
        return;

    gchar *value = g_strndup(text, len);

    switch (ad->id) {
        case PARSER_TYPE_PARAM_ID:
            param->id = atoi(value);
            break;
        case PARSER_TYPE_PARAM_POSITION:
            param->position = atoi(value);
            break;
        case PARSER_TYPE_PARAM_VALUE:
            param->value = atoi(value);
            break;
    }

    g_free(value);
}

#endif /* DOXYGEN_SHOULD_SKIP_THIS */

/**
 *  \param filename valid path to file
 *  \param error return location for an error
 *
 *  Tries to open file pointed by path, then parses it.
 *
 *  \return Preset which must be freed using preset_free, or NULL on error.
 **/
Preset *create_preset_from_xml_file(gchar *filename, GError **error)
{
    GFile *file;
    GError *err = NULL;
    gchar *contents;

    file = g_file_new_for_path(filename);
    if (g_file_get_contents(filename, &contents, NULL, &err) == FALSE) {
        g_message("Failed to get %s contents: %s", filename, err->message);
        *error = g_error_copy(err);
        g_error_free(err);
        g_object_unref(file);
        return NULL;
    }

    AppData *ad = g_slice_new(AppData);
    ad->depth = 0;
    ad->preset = g_slice_new(Preset);
    ad->preset->name = NULL;
    ad->preset->params = NULL;
    ad->id = PARSER_TYPE_NOT_SET;

    XML_Parser p;
    p = XML_ParserCreate(NULL);
    XML_SetUserData(p, (void *) ad);
    XML_SetElementHandler(p, start, end);
    XML_SetCharacterDataHandler(p, text_cb);

    if (XML_Parse(p, contents, strlen(contents), XML_TRUE) != XML_STATUS_OK) {
        g_set_error(error, 0, 0, "Parse error at line %d:\n%s",
                    (int)XML_GetCurrentLineNumber(p),
                    XML_ErrorString(XML_GetErrorCode(p)));
        preset_free(ad->preset);
        g_slice_free(AppData, ad);
        g_free(contents);
        g_object_unref(file);
        return NULL;
    }

    Preset *preset = ad->preset;
    preset->params = g_list_reverse(preset->params);

    XML_ParserFree(p);
    g_slice_free(AppData, ad);

    g_free(contents);
    g_object_unref(file);
    return preset;
}

/**
 *  \param list list containing unpacked preset SysEx messages.
 *
 *  Parses message
 *
 *  \return Preset which must be freed using preset_free, or NULL on error.
 **/
Preset *create_preset_from_data(GList *list)
{
    GString *data;
    GList *iter;
    gint total;
    gint n;
    gint x;

    g_return_val_if_fail(list != NULL, NULL);

    iter = list;
    do {
        data = (GString*) iter->data;
        iter = g_list_next(iter);
    } while (get_message_id(data) != RECEIVE_PRESET_PARAMETERS);

    x = 0x09;
    n = 0;
    total = (unsigned char)data->str[x];
    x++;

    Preset *preset = g_slice_new(Preset);
    preset->name = NULL; /* TODO */
    preset->params = NULL;

    do {
        SettingParam *param = setting_param_new_from_data(&data->str[x], &x);
        n++;
        preset->params = g_list_prepend(preset->params, param);
        g_message("%d ID %d Position %d Value %d", n, param->id, param->position, param->value);
    } while ((x < data->len) && n<total);
    g_message("TOTAL %d", total);
    preset->params = g_list_reverse(preset->params);

    return preset;
}

/**
 *  \param preset preset to be freed
 *
 *  Frees all memory used by preset.
 **/
void preset_free(Preset *preset)
{
    g_return_if_fail(preset != NULL);

    if (preset->params != NULL) {
        GList *iter;
        for (iter = preset->params; iter; iter = iter->next) {
            setting_param_free((SettingParam*)iter->data);
        }
        g_list_free(preset->params);
    }

    if (preset->name != NULL)
        g_free(preset->name);

    g_slice_free(Preset, preset);
}
