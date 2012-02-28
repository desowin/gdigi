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
#include <libxml/encoding.h>
#include <libxml/xmlreader.h>
#include <libxml/xmlwriter.h>
#include <string.h>
#include "preset.h"
#include "gdigi.h"
#include "gui.h"
#include "gdigi_xml.h"

#ifndef DOXYGEN_SHOULD_SKIP_THIS
// FIXME: Bring the xml settings into this module?
extern XmlSettings xml_settings[];
extern guint n_xml_settings;
extern EffectValues values_on_off;
/**
 *  \param id modifier ID
 *  \param position modifier position
 *
 *  Gets modifier info.
 *
 *  \return Modifier which must not be freed, or NULL if no matching Modifier has been found.
*/
XmlSettings *get_xml_settings (guint id, guint position)
{
    gint x;

    for (x=0; x< n_xml_settings; x++) {
        if ((xml_settings[x].id == id) && (xml_settings[x].position == position)) {
            return xml_settings + x;
        }
    }

    return NULL;
}

gchar *
map_xml_value(XmlSettings *xml, gint value)
{
    switch (xml->values->type) {
    case VALUE_TYPE_LABEL:
        if ((xml->values == &values_on_off) && (value > 1)) {
            g_warning("Skipping modifier->label %s\n", xml->label);
            return NULL;
        }   
        if (value > xml->values->max || value < xml->values->min) {
            g_warning("%s value %d out of range %0.1f %0.1f",
                      xml->label, value, xml->values->min, xml->values->max);
        } 
        {
            XmlLabel *labels = xml->xml_labels;
            guint labels_amt = xml->xml_labels_amt;
            gint i;

            // Maybe this can be a computation: i = xml->values->min + val
            for (i = 0; i < labels_amt; i++) {
                if (labels[i].type  == value) {
                    return (labels[i].label);
                }
            }
        }
        break;

    default:
        break;
    }
    return NULL;
}

gboolean value_is_extra (EffectValues *val, SettingParam *param)
{
    if ((param->value < val->min) || (param->value > val->max)) {
        return TRUE;
    }
    return FALSE;
}

#define GDIGI_ENCODING "utf-8"
void
write_preset_to_xml(Preset *preset, gchar *filename)
{
    int rc;
    xmlTextWriterPtr writer;
    GList *iter_params = preset->params;
    guint last_id = 0;
    guint last_position = 0;

    printf("Creating a new xml doc\n");
    /* Create a new XmlWriter for uri, with no compression. */
    writer = xmlNewTextWriterFilename(filename, 0);
    if (writer == NULL) {
        printf("testXmlwriterFilename: Error creating the xml writer\n");
        return;
    }

    /* 
     * Start the document with the xml default for the version,
     * encoding and the default for the standalone declaration.
     */
    rc = xmlTextWriterStartDocument(writer, NULL, GDIGI_ENCODING, NULL);
    if (rc < 0) {
        printf
            ("testXmlwriterFilename: Error at xmlTextWriterStartDocument\n");
        return;
    }

    rc = xmlTextWriterSetIndent(writer, 1);
    rc = xmlTextWriterSetIndentString(writer, BAD_CAST "  ");
    /* Write the tag identifying type of prefix, schema version and ns. */
    rc = xmlTextWriterStartElement(writer, BAD_CAST get_preset_filename(product_id));

    rc = xmlTextWriterWriteAttribute(writer, BAD_CAST "SchemaVersion",
                                     BAD_CAST "1.2");

   
    rc = xmlTextWriterWriteAttribute(writer, BAD_CAST "xmlns",
                                     BAD_CAST "http://www.digitech.com/xml/preset");

    /* Write the Name tag. */
    rc = xmlTextWriterWriteElement(writer, BAD_CAST "Name", BAD_CAST preset->name);

    rc = xmlTextWriterStartElement(writer, BAD_CAST "Params");

    while (iter_params) {
        XmlSettings *xml;
        SettingParam *param = (SettingParam *) iter_params->data;
 
        if (param->id == last_id && param->position == last_position) {
            g_warning("Skipping duplicate parameter id %d position %d",
                       last_id, last_position);
            iter_params  = iter_params->next;
            continue;
        }

        rc = xmlTextWriterStartElement(writer, BAD_CAST "Param");

        rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "ID",
                                             "%d", param->id);
        rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "Position",
                                             "%d", param->position);
        rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "Value",
                                             "%d", param->value);

        last_id = param->id;
        last_position = param->position;
        xml  = get_xml_settings(param->id, param->position);
        if (!xml) {
            printf("Failed to get xml settings for id %d position %d\n",
                    param->id, param->position);
        } else {
            ValueType type;
            gchar *suffix = "";
            gdouble step = 1.0;
            gint offset = 0;
            gboolean        decimal = FALSE;
            EffectValues *values = NULL;

            rc = xmlTextWriterWriteElement(writer, BAD_CAST "Name",
                                                   BAD_CAST xml->label);
            values = xml->values;
            type = values->type;
            while ((type & VALUE_TYPE_EXTRA) && value_is_extra(values, param)) {
                values = values->extra;
                type = values->type;
            }
            type &= ~VALUE_TYPE_EXTRA;

            if (type & VALUE_TYPE_OFFSET) {
                offset = values->offset;
                type &= ~VALUE_TYPE_OFFSET;
            }

            if (type & VALUE_TYPE_STEP) {
                step = values->step;
                type &= ~VALUE_TYPE_STEP;
            }

            if (type & VALUE_TYPE_SUFFIX) {
                suffix = values->suffix;
                type &= ~VALUE_TYPE_SUFFIX;
            }

            if (type & VALUE_TYPE_DECIMAL) {
                decimal = TRUE;
                type &= ~VALUE_TYPE_DECIMAL;
            }

            switch (type) {
            case VALUE_TYPE_LABEL:
            {
                char *textp = map_xml_value(xml, param->value);
                if (!textp) {
                    g_warning("Unable to map %s value %d for id %d position %d",
                              xml->label, param->value, param->id,
                              param->position);
                    textp = "";
                }
                rc = xmlTextWriterWriteElement(writer, BAD_CAST "Text",
                                                       BAD_CAST textp);
                break;
            }

            case VALUE_TYPE_PLAIN:
            {
                if (decimal) {
                    double value = (param->value + offset) * step;
                    rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "Text",
                            "%0.2f%s", value, suffix);
                } else {    
                    gint value = (param->value + offset) * step;
                    rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "Text",
                            "%d%s", value, suffix);
                }
                break;

            }
            case VALUE_TYPE_NONE:
                rc = xmlTextWriterStartElement(writer, BAD_CAST "Text");
                rc = xmlTextWriterEndElement(writer);
                break;

            default:
                g_warning("Unhandled value type %d", type);
                break;
            }
        }
        
        rc = xmlTextWriterEndElement(writer);

        iter_params  = iter_params->next;
    }

    rc = xmlTextWriterEndDocument(writer);
    if (rc < 0) {
        printf("testXmlwriterFilename: Error at xmlTextWriterEndDocument\n");
        return;
    }

    xmlFreeTextWriter(writer);
}
#endif /* DOXYGEN_SHOULD_SKIP_THIS */
