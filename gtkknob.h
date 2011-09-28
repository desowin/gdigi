/*****************************************************************************
 *
 * Most of this code comes from gAlan 0.2.0, copyright (C) 1999
 * Tony Garnock-Jones, with modifications by Sean Bolton,
 * copyright (C) 2004, and minor modifications by William Weston,
 * copyright (C) 2007, Tomasz Moń, copyright (C) 2009-2011
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *****************************************************************************/
#ifndef __GTK_KNOB_H__
#define __GTK_KNOB_H__

#include <gtk/gtk.h>

#ifdef __cplusplus
extern "C" {
#endif

#define GTK_KNOB(obj)		G_TYPE_CHECK_INSTANCE_CAST(obj, gtk_knob_get_type(), GtkKnob)
#define GTK_KNOB_CLASS(klass)	G_TYPE_CHECK_CLASS_CAST(klass, gtk_knob_get_type(), GtkKnobClass)
#define GTK_IS_KNOB(obj)	G_TYPE_CHECK_INSTANCE_TYPE(obj, gtk_knob_get_type())

typedef struct _GtkKnob		GtkKnob;
typedef struct _GtkKnobClass	GtkKnobClass;
typedef struct _GtkKnobAnim	GtkKnobAnim;

typedef enum
{
    GTK_KNOB_UPDATE_CONTINUOUS,
    GTK_KNOB_UPDATE_DISCONTINUOUS,
    GTK_KNOB_UPDATE_DELAYED
} GtkKnobUpdateType;

	/* better to make this an object and let widgets ref/deref it perhaps */
    struct _GtkKnobAnim {
	cairo_surface_t *image;
	gint width;  /* derived from image width */
	gint height;  /* derived from image height. */
	gint frame_width;  /* derived from pixbuf (width / height) or provided override for rectangular frames */
    };
    
    struct _GtkKnob {
	GtkWidget widget;

        GdkWindow *event_window;
	/* update policy (GTK_UPDATE_[CONTINUOUS/DELAYED/DISCONTINUOUS]) */
	guint policy : 2;

	/* State of widget (to do with user interaction) */
	guint8 state;
	gint saved_x, saved_y;

	/* ID of update timer, or 0 if none */
	guint32 timer;

	/* knob animation */
	GtkKnobAnim *anim;
	gint width, height;
	
	/* Old values from adjustment stored so we know when something changes */
	gdouble old_value;
	gdouble old_lower;
	gdouble old_upper;

	/* The adjustment object that stores the data for this knob */
	GtkAdjustment *adjustment;
    };

    struct _GtkKnobClass {
	GtkWidgetClass parent_class;
    };


    extern GtkWidget *gtk_knob_new(GtkAdjustment *adjustment, GtkKnobAnim *anim);
    extern GType gtk_knob_get_type(void);
    extern GtkAdjustment *gtk_knob_get_adjustment(GtkKnob *knob);
    extern void gtk_knob_set_update_policy(GtkKnob *knob, GtkKnobUpdateType  policy);
    extern void gtk_knob_set_adjustment(GtkKnob *knob, GtkAdjustment *adjustment);

    GtkKnobAnim *gtk_knob_animation_new_from_inline();

    void gtk_knob_set_animation (GtkKnob *knob, GtkKnobAnim *anim);
    void gtk_knob_animation_free(GtkKnobAnim *anim);

#ifdef __cplusplus
}
#endif

#endif
