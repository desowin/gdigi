/*****************************************************************************
 *
 * Most of this code comes from gAlan 0.2.0, copyright (C) 1999
 * Tony Garnock-Jones, with modifications from Sean Bolton,
 * copyright (C) 2004, William Weston copyright (C) 2007,
 * Pete Shorthose copyright (C) 2007, and Tomasz Moń,
 * copyright (C) 2009-2011
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

#include <math.h>
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include <string.h>
#include "gtkknob.h"
#include "knob.h"

#ifndef M_PI
# define M_PI		3.14159265358979323846	/* pi */
#endif
#ifndef M_1_PI
# define M_1_PI		0.31830988618379067154	/* 1/pi */
#endif


#define SCROLL_DELAY_LENGTH	250
#define KNOB_SIZE		32	/* this should really be read from the knob image */

#define STATE_IDLE		0
#define STATE_PRESSED		1
#define STATE_DRAGGING		2

static void gtk_knob_class_init(GtkKnobClass *klass);
static void gtk_knob_init(GtkKnob *knob);
static void gtk_knob_destroy(GtkWidget *object);
static void gtk_knob_realize(GtkWidget *widget);
static void gtk_knob_unrealize(GtkWidget *widget);
static void gtk_knob_map(GtkWidget *widget);
static void gtk_knob_unmap(GtkWidget *widget);
static void gtk_knob_get_preferred_width(GtkWidget *widget, gint *minimum_width, gint *natural_width);
static void gtk_knob_get_preferred_height(GtkWidget *widget, gint *minimum_height, gint *natural_height);
static void gtk_knob_size_allocate(GtkWidget *widget, GtkAllocation *allocation);
static gboolean gtk_knob_draw(GtkWidget *widget, cairo_t *cr);
static gint gtk_knob_scroll(GtkWidget *widget, GdkEventScroll *event);
static gint gtk_knob_button_press(GtkWidget *widget, GdkEventButton *event);
static gint gtk_knob_button_release(GtkWidget *widget, GdkEventButton *event);
static gint gtk_knob_key_press(GtkWidget *widget, GdkEventKey *event);
static gint gtk_knob_motion_notify(GtkWidget *widget, GdkEventMotion *event);
static gint gtk_knob_timer(GtkKnob *knob);

static void gtk_knob_update_mouse_update(GtkKnob *knob);
static void gtk_knob_update_mouse(GtkKnob *knob, gint x, gint y, gboolean step);
static void gtk_knob_update(GtkKnob *knob);
static void gtk_knob_adjustment_changed(GtkAdjustment *adjustment, gpointer data);
static void gtk_knob_adjustment_value_changed(GtkAdjustment *adjustment, gpointer data);

/* Local data */

static GtkWidgetClass *parent_class = NULL;


/*****************************************************************************
 *
 * gtk_knob_get_type()
 *
 *****************************************************************************/
GType
gtk_knob_get_type(void) {
    static GType knob_type = 0;

    if (!knob_type) {
	static const GTypeInfo knob_info = {
	    sizeof (GtkKnobClass),
            NULL,
	    NULL,
	    (GClassInitFunc) gtk_knob_class_init,
	    NULL,
	    NULL,
	    sizeof (GtkKnob),
	    0,
	    (GInstanceInitFunc) gtk_knob_init,
	};
	knob_type = g_type_register_static (GTK_TYPE_WIDGET, "GtkKnob", &knob_info, 0);
    }

    return knob_type;
}


/*****************************************************************************
 *
 * gtk_knob_class_init()
 *
 *****************************************************************************/
static void
gtk_knob_class_init (GtkKnobClass *klass) {
    GtkWidgetClass *widget_class;

    widget_class = (GtkWidgetClass*) klass;

    parent_class = g_type_class_peek_parent(klass);

    widget_class->destroy = gtk_knob_destroy;

    widget_class->realize              = gtk_knob_realize;
    widget_class->unrealize            = gtk_knob_unrealize;
    widget_class->draw                 = gtk_knob_draw;
    widget_class->get_preferred_width  = gtk_knob_get_preferred_width;
    widget_class->get_preferred_height = gtk_knob_get_preferred_height;
    widget_class->size_allocate        = gtk_knob_size_allocate;
    widget_class->scroll_event         = gtk_knob_scroll;
    widget_class->button_press_event   = gtk_knob_button_press;
    widget_class->button_release_event = gtk_knob_button_release;
    widget_class->key_press_event      = gtk_knob_key_press;
    widget_class->motion_notify_event  = gtk_knob_motion_notify;
    widget_class->map                  = gtk_knob_map;
    widget_class->unmap                = gtk_knob_unmap;
}


/*****************************************************************************
 *
 * gtk_knob_init()
 *
 *****************************************************************************/
static void
gtk_knob_init (GtkKnob *knob) {
    knob->policy     = GTK_KNOB_UPDATE_CONTINUOUS;
    knob->state      = STATE_IDLE;
    knob->saved_x    = 0;
    knob->saved_y    = 0;
    knob->timer      = 0;
    knob->anim       = NULL;
    knob->old_value  = 0.0;
    knob->old_lower  = 0.0;
    knob->old_upper  = 0.0;
    knob->adjustment = NULL;

    gtk_widget_set_can_focus(GTK_WIDGET(knob), TRUE);
    gtk_widget_set_has_window(GTK_WIDGET(knob), FALSE);
    gtk_widget_set_hexpand(GTK_WIDGET(knob), FALSE);
    gtk_widget_set_vexpand(GTK_WIDGET(knob), FALSE);
    gtk_widget_add_events(GTK_WIDGET(knob), GDK_SCROLL_MASK);
}


/*****************************************************************************
 *
 * gtk_knob_new()
 *
 *****************************************************************************/
GtkWidget *
gtk_knob_new(GtkAdjustment *adjustment, GtkKnobAnim *anim) {
    GtkKnob *knob;

    g_return_val_if_fail (anim != NULL, NULL);

    knob = g_object_new (gtk_knob_get_type (), NULL);

    gtk_knob_set_animation (knob, anim);

    if (!adjustment) {
	adjustment = (GtkAdjustment*) gtk_adjustment_new (0.0, 0.0, 0.0,
							  0.0, 0.0, 0.0);
    }

    gtk_knob_set_adjustment (knob, adjustment);

    return GTK_WIDGET (knob);
}


/*****************************************************************************
 *
 * gtk_knob_destroy()
 *
 *****************************************************************************/
static void
gtk_knob_destroy(GtkWidget *object) {
    GtkKnob *knob;

    g_return_if_fail (object != NULL);
    g_return_if_fail (GTK_IS_KNOB (object));

    knob = GTK_KNOB (object);

    gtk_knob_set_adjustment (knob, NULL);
    /* FIXME: needs ref counting for automatic GtkKnobAnim cleanup
       if (knob->anim) {
	   gtk_knob_anim_unref (knob->anim);
	   knob->anim = NULL;
       }
    */

    if (GTK_WIDGET_CLASS(parent_class)->destroy) {
	(*GTK_WIDGET_CLASS (parent_class)->destroy) (object);
    }
}


/*****************************************************************************
 *
 * gtk_knob_get_adjustment()
 *
 *****************************************************************************/
GtkAdjustment*
gtk_knob_get_adjustment(GtkKnob *knob) {

    g_return_val_if_fail (knob != NULL, NULL);
    g_return_val_if_fail (GTK_IS_KNOB (knob), NULL);

    return knob->adjustment;
}


/*****************************************************************************
 *
 * gtk_knob_set_update_policy()
 *
 *****************************************************************************/
void
gtk_knob_set_update_policy(GtkKnob *knob, GtkKnobUpdateType policy) {

    g_return_if_fail (knob != NULL);
    g_return_if_fail (GTK_IS_KNOB (knob));

    knob->policy = policy;
}


/*****************************************************************************
 *
 * gtk_knob_set_adjustment()
 *
 *****************************************************************************/
void
gtk_knob_set_adjustment(GtkKnob *knob, GtkAdjustment *adjustment) {

    g_return_if_fail (knob != NULL);
    g_return_if_fail (GTK_IS_KNOB (knob));

    if (knob->adjustment) {
	g_signal_handlers_disconnect_matched(knob->adjustment,
					     G_SIGNAL_MATCH_DATA,
					     0, 0, NULL, NULL,
					     knob);
	g_object_unref (knob->adjustment);
    }

    knob->adjustment = adjustment;
    if (adjustment) {
        g_object_ref_sink (adjustment);

        g_signal_connect (adjustment, "changed",
    			G_CALLBACK(gtk_knob_adjustment_changed),
			knob);
	g_signal_connect (adjustment, "value_changed",
			G_CALLBACK(gtk_knob_adjustment_value_changed),
			knob);

	knob->old_value = gtk_adjustment_get_value(adjustment);
	knob->old_lower = gtk_adjustment_get_lower(adjustment);
	knob->old_upper = gtk_adjustment_get_upper(adjustment);

	gtk_knob_update (knob);
    }
}


/*****************************************************************************
 *
 * gtk_knob_realize()
 *
 *****************************************************************************/
static void
gtk_knob_realize(GtkWidget *widget) {
    GtkKnob *knob;
    GdkWindow *window;
    GdkWindowAttr attributes;
    GtkAllocation allocation;

    gint attributes_mask;

    g_return_if_fail (widget != NULL);
    g_return_if_fail (GTK_IS_KNOB (widget));

    gtk_widget_get_allocation (widget, &allocation);
    gtk_widget_set_realized(widget, TRUE);
    knob = GTK_KNOB (widget);

    attributes.window_type = GDK_WINDOW_CHILD;
    attributes.x           = allocation.x;
    attributes.y           = allocation.y;
    attributes.width       = allocation.width;
    attributes.height      = allocation.height;
    attributes.wclass      = GDK_INPUT_ONLY;;
    attributes.event_mask  =
	gtk_widget_get_events (widget) |
	GDK_BUTTON_PRESS_MASK |
	GDK_BUTTON_RELEASE_MASK | GDK_POINTER_MOTION_MASK |
	GDK_POINTER_MOTION_HINT_MASK;

    attributes_mask = GDK_WA_X | GDK_WA_Y;

    window = gtk_widget_get_parent_window(widget);
    gtk_widget_set_window(widget, window);
    g_object_ref(window);

    knob->event_window = gdk_window_new(window, &attributes, attributes_mask);
    gdk_window_set_user_data(knob->event_window, knob);
}

static void
gtk_knob_unrealize(GtkWidget *widget)
{
    GtkKnob *knob = GTK_KNOB(widget);

    if (knob->event_window)
    {
        gdk_window_set_user_data(knob->event_window, NULL);
        gdk_window_destroy(knob->event_window);
        knob->event_window = NULL;
    }

    GTK_WIDGET_CLASS(parent_class)->unrealize(widget);
}

static void
gtk_knob_map(GtkWidget *widget)
{
    GtkKnob *knob = GTK_KNOB(widget);

    GTK_WIDGET_CLASS(parent_class)->map(widget);

    if (knob->event_window)
        gdk_window_show(knob->event_window);
}

static void
gtk_knob_unmap(GtkWidget *widget)
{
    GtkKnob *knob = GTK_KNOB(widget);

    if (knob->event_window)
        gdk_window_hide(knob->event_window);

    GTK_WIDGET_CLASS(parent_class)->unmap(widget);
}

static void
gtk_knob_get_preferred_width(GtkWidget *widget,
                             gint *minimum_width,
                             gint *natural_width)
{
    g_return_if_fail (widget != NULL);
    g_return_if_fail (GTK_IS_KNOB (widget));

    *minimum_width = *natural_width = GTK_KNOB(widget)->width; 
}

static void
gtk_knob_get_preferred_height(GtkWidget *widget,
                              gint *minimum_height,
                              gint *natural_height)
{
    g_return_if_fail (widget != NULL);
    g_return_if_fail (GTK_IS_KNOB (widget));

    *minimum_height = *natural_height = GTK_KNOB(widget)->height; 
}


/*****************************************************************************
 *
 * gtk_knob_size_allocate()
 *
 *****************************************************************************/
static void
gtk_knob_size_allocate (GtkWidget *widget, GtkAllocation *allocation) {
    GtkKnob *knob;

    g_return_if_fail (widget != NULL);
    g_return_if_fail (GTK_IS_KNOB (widget));
    g_return_if_fail (allocation != NULL);

    gtk_widget_set_allocation(widget, allocation);
    knob = GTK_KNOB (widget);

    if (gtk_widget_get_realized (widget)) {
	gdk_window_move_resize (knob->event_window,
				allocation->x, allocation->y,
				allocation->width, allocation->height);
    }
}


/*****************************************************************************
 *
 * gtk_knob_draw()
 *
 *****************************************************************************/
static gboolean
gtk_knob_draw(GtkWidget *widget, cairo_t *cr) {
    GtkKnob *knob;
    gdouble dx, dy;
    gint frames;

    g_return_val_if_fail (widget != NULL, FALSE);
    g_return_val_if_fail (GTK_IS_KNOB (widget), FALSE);
    g_return_val_if_fail (cr != NULL, FALSE);
    g_return_val_if_fail (GTK_IS_ADJUSTMENT (GTK_KNOB (widget)->adjustment), FALSE);

    knob = GTK_KNOB (widget);

    frames = ((knob->anim->width / knob->anim->frame_width) - 1);
    dx = gtk_adjustment_get_value(knob->adjustment) - gtk_adjustment_get_lower(knob->adjustment);	/* value, from 0 */
    dy = gtk_adjustment_get_upper(knob->adjustment) - gtk_adjustment_get_lower(knob->adjustment);	/* range */

    dx = (int)(frames * dx / dy) * knob->width;  /* check this for height != width */

    cairo_surface_t *surface =
        cairo_surface_create_for_rectangle(knob->anim->image,
                                           dx, 0.0,
                                           (double)knob->width,
                                           (double)knob->height);
    cairo_set_source_surface(cr, surface, 0, 0);
    cairo_paint(cr);
    cairo_surface_destroy(surface);

    if (gtk_widget_has_focus(widget)) {
        GtkStyleContext *context;

        context = gtk_widget_get_style_context(widget);
        gtk_style_context_save(context);
        gtk_style_context_set_state(context, gtk_widget_get_state_flags (widget));
        cairo_save(cr);
        gtk_render_focus(context, cr,
                         0, 0,
                         gtk_widget_get_allocated_width(widget),
                         gtk_widget_get_allocated_height(widget));
        cairo_restore(cr);
        gtk_style_context_restore(context);
    }

    return FALSE;
}


/*****************************************************************************
 *
 * gtk_knob_scroll()
 *
 *****************************************************************************/
static gint
gtk_knob_scroll(GtkWidget *widget, GdkEventScroll *event) {
    GtkKnob *knob;

    g_return_val_if_fail (widget != NULL, FALSE);
    g_return_val_if_fail (GTK_IS_KNOB (widget), FALSE);
    g_return_val_if_fail (event != NULL, FALSE);
    g_return_val_if_fail (GTK_IS_ADJUSTMENT (GTK_KNOB (widget)->adjustment), FALSE);

    knob = GTK_KNOB (widget);

    switch (event->direction) {
    case GDK_SCROLL_UP:
	gtk_adjustment_set_value(knob->adjustment,
            gtk_adjustment_get_value(knob->adjustment) +
            gtk_adjustment_get_step_increment(knob->adjustment));
	g_signal_emit_by_name (knob->adjustment, "value_changed");
	break;
    case GDK_SCROLL_DOWN:
	gtk_adjustment_set_value(knob->adjustment,
            gtk_adjustment_get_value(knob->adjustment) -
            gtk_adjustment_get_step_increment(knob->adjustment));
	g_signal_emit_by_name (knob->adjustment, "value_changed");
	break;
    default:
        break;
    }

    return FALSE;
}


/*****************************************************************************
 *
 * gtk_knob_button_press()
 *
 *****************************************************************************/
static gint
gtk_knob_button_press(GtkWidget *widget, GdkEventButton *event) {
    GtkKnob *knob;

    g_return_val_if_fail (widget != NULL, FALSE);
    g_return_val_if_fail (GTK_IS_KNOB (widget), FALSE);
    g_return_val_if_fail (event != NULL, FALSE);
    g_return_val_if_fail (GTK_IS_ADJUSTMENT (GTK_KNOB (widget)->adjustment), FALSE);

    knob = GTK_KNOB (widget);

    switch (knob->state) {
    case STATE_IDLE:
	switch (event->button) {
	case 1:
	case 3:
	    if (!gtk_widget_has_focus(widget))
	        gtk_widget_grab_focus(widget);
	    knob->state   = STATE_PRESSED;
	    knob->saved_x = event->x;
	    knob->saved_y = event->y;
	    break;
	case 2:
	    gtk_adjustment_set_value(knob->adjustment,
                floor ((gtk_adjustment_get_lower(knob->adjustment) +
		gtk_adjustment_get_upper(knob->adjustment) + 1.0)
		* 0.5));
	    g_signal_emit_by_name (knob->adjustment, "value_changed");
	    break;
	}
	break;
    }

    return FALSE;
}


/*****************************************************************************
 *
 * gtk_knob_button_release()
 *
 *****************************************************************************/
static gint
gtk_knob_button_release(GtkWidget *widget, GdkEventButton *event) {
    GtkKnob *knob;

    g_return_val_if_fail (widget != NULL, FALSE);
    g_return_val_if_fail (GTK_IS_KNOB (widget), FALSE);
    g_return_val_if_fail (event != NULL, FALSE);
    g_return_val_if_fail (GTK_IS_ADJUSTMENT (GTK_KNOB (widget)->adjustment), FALSE);

    knob = GTK_KNOB (widget);

    switch (knob->state) {

    case STATE_PRESSED:
	knob->state = STATE_IDLE;
	break;

    case STATE_DRAGGING:
	knob->state = STATE_IDLE;

	switch (event->button) {
	case 1:
	case 3:
	    if (knob->policy != GTK_KNOB_UPDATE_CONTINUOUS
		&& knob->old_value != gtk_adjustment_get_value(knob->adjustment))
	    {
		g_signal_emit_by_name (knob->adjustment, "value_changed");
	    }
	    break;
	}

	break;
    }

    return FALSE;
}

static gint gtk_knob_key_press(GtkWidget *widget, GdkEventKey *event)
{
    GtkKnob *knob;

    g_return_val_if_fail (widget != NULL, FALSE);
    g_return_val_if_fail (GTK_IS_KNOB (widget), FALSE);
    g_return_val_if_fail (event != NULL, FALSE);
    g_return_val_if_fail (GTK_IS_ADJUSTMENT (GTK_KNOB (widget)->adjustment), FALSE);

    knob = GTK_KNOB (widget);

    switch (event->keyval) {

    case GDK_KEY_Up:
        if (gtk_widget_has_focus (widget))
        {
            gtk_adjustment_set_value (knob->adjustment,
                                      knob->old_value + gtk_adjustment_get_step_increment(knob->adjustment));
            return TRUE;
        }
        return FALSE;

    case GDK_KEY_Down:
        if (gtk_widget_has_focus (widget))
        {
            gtk_adjustment_set_value (knob->adjustment,
                                      knob->old_value - gtk_adjustment_get_step_increment(knob->adjustment));
            return TRUE;
        }
        return FALSE;
    default:
        break;
    }

    return GTK_WIDGET_CLASS (parent_class)->key_press_event (widget, event);
}

/*****************************************************************************
 *
 * gtk_knob_motion_notify()
 *
 *****************************************************************************/
static gint
gtk_knob_motion_notify(GtkWidget *widget, GdkEventMotion *event) {
    GtkKnob *knob;
    GdkModifierType mods;
    gint x, y;

    g_return_val_if_fail (widget != NULL, FALSE);
    g_return_val_if_fail (GTK_IS_KNOB (widget), FALSE);
    g_return_val_if_fail (event != NULL, FALSE);

    knob = GTK_KNOB (widget);

    x = event->x;
    y = event->y;

    if (event->is_hint || (event->window != gtk_widget_get_window(widget))) {
	gdk_window_get_pointer(gtk_widget_get_window(widget), &x, &y, &mods);
    }

    switch (knob->state) {

    case STATE_PRESSED:
	knob->state = STATE_DRAGGING;
	/* fall through */

    case STATE_DRAGGING:
	if (mods & GDK_BUTTON1_MASK) {
	    gtk_knob_update_mouse (knob, x, y, TRUE);
	    return TRUE;
	}
	else if (mods & GDK_BUTTON3_MASK) {
	    gtk_knob_update_mouse (knob, x, y, FALSE);
	    return TRUE;
	}
	break;
    }

    return FALSE;
}


/*****************************************************************************
 *
 * gtk_knob_timer()
 *
 *****************************************************************************/
static gint
gtk_knob_timer(GtkKnob *knob) {

    g_return_val_if_fail (knob != NULL, FALSE);
    g_return_val_if_fail (GTK_IS_KNOB (knob), FALSE);
    g_return_val_if_fail (GTK_IS_ADJUSTMENT (knob->adjustment), FALSE);

    if (knob->policy == GTK_KNOB_UPDATE_DELAYED) {
	g_signal_emit_by_name (knob->adjustment, "value_changed");
    }

    /* don't keep running this timer */
    return FALSE;
}


/*****************************************************************************
 *
 * gtk_knob_update_mouse_update()
 *
 *****************************************************************************/
static void
gtk_knob_update_mouse_update(GtkKnob *knob) {
    g_return_if_fail(GTK_IS_ADJUSTMENT (knob->adjustment));

    if (knob->policy == GTK_KNOB_UPDATE_CONTINUOUS) {
	g_signal_emit_by_name (knob->adjustment, "value_changed");
    }
    else {
	gtk_widget_queue_draw (GTK_WIDGET (knob));

	if (knob->policy == GTK_KNOB_UPDATE_DELAYED) {
	    if (knob->timer) {
		g_source_remove (knob->timer);
	    }
	    knob->timer = g_timeout_add (SCROLL_DELAY_LENGTH,
					 (GSourceFunc) gtk_knob_timer,
					 (gpointer) knob);
	}
    }
}


/*****************************************************************************
 *
 * gtk_knob_update_mouse()
 *
 *****************************************************************************/
static void
gtk_knob_update_mouse(GtkKnob *knob, gint x, gint y, gboolean step) {
    gdouble old_value, new_value, dv, dh;
    gdouble angle;

    g_return_if_fail (knob != NULL);
    g_return_if_fail (GTK_IS_KNOB (knob));
    g_return_if_fail (GTK_IS_ADJUSTMENT (knob->adjustment));

    old_value = gtk_adjustment_get_value(knob->adjustment);

    angle = atan2f (-y + (knob->height >> 1), x - (knob->width >> 1));

    /* inverted cartesian graphics coordinate system */
    dv = knob->saved_y - y;
    dh = x - knob->saved_x;
    knob->saved_x = x;
    knob->saved_y = y;

    if ((x >= 0) && (x <= knob->width)) {
        dh = 0;  /* dead zone */
    } else {
        angle = cosf (angle);
        dh *= angle * angle;
    }

    new_value = gtk_adjustment_get_value(knob->adjustment) +
                dv * (step ? gtk_adjustment_get_step_increment(knob->adjustment) : gtk_adjustment_get_page_increment(knob->adjustment)) +
                dh * (gtk_adjustment_get_upper(knob->adjustment) -
                gtk_adjustment_get_lower(knob->adjustment)) * 0.005; /* 0.005 == (1 / 200) */

    new_value = MAX (MIN (new_value, gtk_adjustment_get_upper(knob->adjustment)),
                     gtk_adjustment_get_lower(knob->adjustment));

    gtk_adjustment_set_value(knob->adjustment, new_value);

    if (gtk_adjustment_get_value(knob->adjustment) != old_value) {
        gtk_knob_update_mouse_update (knob);
    }
}


/*****************************************************************************
 *
 * gtk_knob_update()
 *
 *****************************************************************************/
static void
gtk_knob_update(GtkKnob *knob) {
    gdouble new_value;

    g_return_if_fail (knob != NULL);
    g_return_if_fail (GTK_IS_KNOB (knob));
    g_return_if_fail (GTK_IS_ADJUSTMENT (knob->adjustment));

    if (gtk_adjustment_get_step_increment(knob->adjustment) == 1) {
	new_value = floor (gtk_adjustment_get_value(knob->adjustment) + 0.5);
    }
    else {
	new_value = gtk_adjustment_get_value(knob->adjustment);
    }

    if (new_value < gtk_adjustment_get_lower(knob->adjustment)) {
	new_value = gtk_adjustment_get_lower(knob->adjustment);
    }

    if (new_value > gtk_adjustment_get_upper(knob->adjustment)) {
	new_value = gtk_adjustment_get_upper(knob->adjustment);
    }

    if (new_value != gtk_adjustment_get_value(knob->adjustment)) {
	gtk_adjustment_set_value(knob->adjustment, new_value);
	g_signal_emit_by_name (knob->adjustment, "value_changed");
    }

    gtk_widget_queue_draw (GTK_WIDGET (knob));
}


/*****************************************************************************
 *
 * gtk_knob_adjustment_changed()
 *
 *****************************************************************************/
static void
gtk_knob_adjustment_changed(GtkAdjustment *adjustment, gpointer data) {
    GtkKnob *knob;

    g_return_if_fail (adjustment != NULL);
    g_return_if_fail (data != NULL);

    knob = GTK_KNOB (data);

    if ((knob->old_value != gtk_adjustment_get_value(adjustment)) ||
	(knob->old_lower != gtk_adjustment_get_lower(adjustment)) ||
	(knob->old_upper != gtk_adjustment_get_upper(adjustment)))
    {
	gtk_knob_update (knob);

	knob->old_value = gtk_adjustment_get_value(adjustment);
	knob->old_lower = gtk_adjustment_get_lower(adjustment);
	knob->old_upper = gtk_adjustment_get_upper(adjustment);
    }
}


/*****************************************************************************
 *
 * gtk_knob_adjustment_value_changed()
 *
 *****************************************************************************/
static void
gtk_knob_adjustment_value_changed (GtkAdjustment *adjustment, gpointer data) {
    GtkKnob *knob;

    g_return_if_fail (adjustment != NULL);
    g_return_if_fail (data != NULL);

    knob = GTK_KNOB (data);

    if (knob->old_value != gtk_adjustment_get_value(adjustment)) {
	gtk_knob_update (knob);
	knob->old_value = gtk_adjustment_get_value(adjustment);
    }
}


/*****************************************************************************
 *
 * gtk_knob_set_animation()
 *
 *****************************************************************************/
void
gtk_knob_set_animation (GtkKnob *knob, GtkKnobAnim *anim) {
    g_return_if_fail (knob != NULL);
    g_return_if_fail (anim != NULL);
    g_return_if_fail (GTK_IS_KNOB (knob));

    knob->anim   = (GtkKnobAnim *)anim;
    knob->width  = anim->frame_width;
    knob->height = anim->height;

    if (gtk_widget_get_realized (GTK_WIDGET(knob))) {
    	gtk_widget_queue_resize (GTK_WIDGET (knob));
    }
}

/**
 *  Reads embedded knob image
 **/
static cairo_status_t
get_knob_image(void *closure, unsigned char *data, unsigned int length)
{
    int *offset = (int *)closure;

    if ((*offset + length) > sizeof (knob_png))
        return CAIRO_STATUS_READ_ERROR;

    memcpy (data, knob_png + *offset, length);
    *offset = *offset + length;

    return CAIRO_STATUS_SUCCESS;
}

/*****************************************************************************
 *
 * gtk_knob_animation_new_from_inline()
 *
 *****************************************************************************/
GtkKnobAnim *
gtk_knob_animation_new_from_inline() {
    GtkKnobAnim *anim = g_new0 (GtkKnobAnim, 1);
    int offset = 0;

    anim->image =
        cairo_image_surface_create_from_png_stream(get_knob_image, &offset);

    anim->width       = cairo_image_surface_get_width(anim->image);
    anim->height      = cairo_image_surface_get_height(anim->image);
    anim->frame_width = anim->height;

    return anim;
}

/*****************************************************************************
 *
 * gtk_knob_animation_free()
 *
 *****************************************************************************/
void
gtk_knob_animation_free(GtkKnobAnim *anim) {
    g_return_if_fail (anim != NULL);

    if (anim->image)
        cairo_surface_destroy(anim->image);

    g_free (anim);
}
