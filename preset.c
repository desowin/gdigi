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
  PARSER_TYPE_PARAM_TEXT,
  PARSER_TYPE_GENETX_VERSION,
  PARSER_TYPE_GENETX_TYPE,
  PARSER_TYPE_GENETX_CHANNEL,
  PARSER_TYPE_GENETX_NAME,
  PARSER_TYPE_GENETX_DATA
};

enum {
  SECTION_NOT_SET = -1,
  SECTION_PARAMS,
  SECTION_GENETX
};

typedef struct {
    int depth;
    int id;
    int section;
    Preset *preset;
} AppData;

static void XMLCALL start(void *data, const char *el, const char **attr) {
    AppData *ad = (AppData *) data;

    ad->id = PARSER_TYPE_NOT_SET;
    if (g_strcmp0(el, "Name") == 0) {
        if (ad->depth == 1) {
            ad->id = PARSER_TYPE_PRESET_NAME;
        } else if (ad->depth == 3) {
            if (ad->section == SECTION_PARAMS) {
                ad->id = PARSER_TYPE_PARAM_NAME;
            } else if (ad->section == SECTION_GENETX) {
                ad->id = PARSER_TYPE_GENETX_NAME;
            }
        }
    }

    if (g_strcmp0(el, "Params") == 0) {
        ad->section = SECTION_PARAMS;
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
    } else if (g_strcmp0(el, "Genetx") == 0) {
        ad->section = SECTION_GENETX;
        if (ad->preset->genetxs != NULL)
            g_message("Genetx already exists!");
    } else if (g_strcmp0(el, "GenetxModel") == 0) {
        SettingGenetx *genetx = setting_genetx_new();
        ad->preset->genetxs = g_list_prepend(ad->preset->genetxs, genetx);
    } else if (g_strcmp0(el, "Version") == 0) {
        ad->id = PARSER_TYPE_GENETX_VERSION;
    } else if (g_strcmp0(el, "Type") == 0) {
        ad->id = PARSER_TYPE_GENETX_TYPE;
    } else if (g_strcmp0(el, "Channel") == 0) {
        ad->id = PARSER_TYPE_GENETX_CHANNEL;
    } else if (g_strcmp0(el, "Data") == 0) {
        ad->id = PARSER_TYPE_GENETX_DATA;
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

    if (ad->section == SECTION_PARAMS) {
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
    } else if (ad->section == SECTION_GENETX) {
        if (ad->preset->genetxs == NULL)
            return;

        SettingGenetx *genetx = (SettingGenetx *) ad->preset->genetxs->data;
        if (genetx == NULL)
            return;

        gchar *value = g_strndup(text, len);

        switch (ad->id) {
            case PARSER_TYPE_GENETX_VERSION:
                if (g_strcmp0(value, "Version1") == 0) {
                    genetx->version = GENETX_VERSION_1;
                } else if (g_strcmp0(value, "Version2") == 0) {
                    genetx->version = GENETX_VERSION_2;
                } else {
                    g_message("Unknown GeNetX version: %s", value);
                }
                break;
            case PARSER_TYPE_GENETX_TYPE:
                if (g_strcmp0(value, "Amp") == 0) {
                    genetx->type = GENETX_TYPE_AMP;
                } else if (g_strcmp0(value, "Cabinet") == 0) {
                    genetx->type = GENETX_TYPE_CABINET;
                } else {
                    g_message("Unknown GeNetX type: %s", value);
                }
                break;
            case PARSER_TYPE_GENETX_CHANNEL:
                if (g_strcmp0(value, "Channel1") == 0) {
                    genetx->channel = GENETX_CHANNEL1;
                } else if (g_strcmp0(value, "Channel2") == 0) {
                    genetx->channel = GENETX_CHANNEL2;
                } else {
                    g_message("Unknown GeNetX channel: %s", value);
                }
                break;
            case PARSER_TYPE_GENETX_NAME:
                /* reassign pointer */
                genetx->name = value;
                value = NULL;
                break;
            case PARSER_TYPE_GENETX_DATA:
                {
                    guchar *data = NULL;
                    gsize length = 0;

                    data = g_base64_decode(value, &length);
                    genetx->data = g_string_new_len((gchar *) data, length);

                    g_free(data);
                    break;
                }
        }

        g_free(value);
    }
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
    GError *err = NULL;
    gchar *contents;

    if (g_file_get_contents(filename, &contents, NULL, &err) == FALSE) {
        g_message("Failed to get %s contents: %s", filename, err->message);
        *error = g_error_copy(err);
        g_error_free(err);
        return NULL;
    }

    AppData *ad = g_slice_new(AppData);
    ad->depth = 0;
    ad->preset = g_slice_new(Preset);
    ad->preset->name = NULL;
    ad->preset->params = NULL;
    ad->preset->genetxs = NULL;
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
        return NULL;
    }

    Preset *preset = ad->preset;
    preset->params = g_list_reverse(preset->params);
    preset->genetxs = g_list_reverse(preset->genetxs);

    XML_ParserFree(p);
    g_slice_free(AppData, ad);

    g_free(contents);
    return preset;
}

gint params_cmp(gconstpointer a, gconstpointer b)
{
    const SettingParam *param_a = a;
    const SettingParam *param_b = b;

    if (param_a->position != param_b->position) {
        return (param_a->position > param_b->position) ? 1 : -1;
    }
    if (param_a->id != param_b->id) {
        return (param_a->id > param_b->id) ? 1 : -1;
    }

    return 0;
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
    gint total, n, x;
    gint bank, number, modified;
    gchar *name;

    g_return_val_if_fail(list != NULL, NULL);

    Preset *preset = g_slice_new(Preset);
    preset->name = NULL;
    preset->params = NULL;
    preset->genetxs = NULL;

    iter = list;
    for (iter = list; iter; iter = g_list_next(iter)) {
        data = (GString*) iter->data;
        switch (get_message_id(data)) {
            case RECEIVE_PRESET_START:
                bank = (unsigned char)data->str[8];
                number = (unsigned char)data->str[9];
                name = g_strdup(&data->str[10]);
                modified = (unsigned char)data->str[11+strlen(name)];

                if ((bank == PRESETS_EDIT_BUFFER) && (number == 0)) {
                    g_message("Received current edit buffer");
                } else {
                    g_message("Received preset %d from bank %d", number, bank);
                }

                g_message("Modified flag: %d Name: %s", modified, name);
                preset->name = name;
                break;
            case RECEIVE_PRESET_PARAMETERS:
                x = 0x09;
                n = 0;
                total = (unsigned char)data->str[x];
                x++;

                do {
                    SettingParam *param = setting_param_new_from_data(&data->str[x], &x);
                    n++;
                    preset->params = g_list_prepend(preset->params, param);
                    g_message("%3d ID %4d Position %2d Value %6.1d",
                               n, param->id, param->position, param->value);
                } while ((x < data->len) && n<total);
                g_message("TOTAL %d", total);
                preset->params = g_list_sort(preset->params, params_cmp);
                break;
            case RECEIVE_PRESET_END:
                break;
            default:
                g_message("Unhandled message in preset messages list");
        }
    }

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

    if (preset->genetxs != NULL) {
        GList *iter;
        for (iter = preset->genetxs; iter; iter = iter->next) {
            setting_genetx_free((SettingGenetx*)iter->data);
        }
        g_list_free(preset->genetxs);
    }

    g_free(preset->name);

    g_slice_free(Preset, preset);
}
