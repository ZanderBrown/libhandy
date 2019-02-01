/*
 * Copyright © 2019 Zander Brown <zbrown@gnome.org>
 * 
 * Based on gtkstackswitcher.c, Copyright (c) 2013 Red Hat, Inc.
 * https://gitlab.gnome.org/GNOME/gtk/blob/a0129f556b1fd655215165739d0277d7f7a2c1a8/gtk/gtkstackswitcher.c
 *
 * SPDX-License-Identifier: LGPL-2.1+
 */

#include <glib/gi18n.h>

#include "hdy-switcher.h"

/**
 * SECTION:hdy-switcher
 * @short_description: An adaptive switcher
 * @title: HdySwitcher
 * 
 * An alternative #GtkStackSwitcher
 * 
 * C Usage
 * |[<!-- language="C" -->
 * GtkWidget *stack, *switcher;
 * 
 * stack = gtk_stack_new ();
 * switcher = hdy_switcher_new ();
 * hdy_switcher_set_stack (HDY_SWITCHER (switcher), GTK_STACK (stack));
 * ]|
 * 
 * Vala Usage
 * |[<!-- language="Vala" -->
 * var stack = new Gtk.Stack ();
 * var switcher = new Hdy.Switcher ();
 * switcher.stack = stack;
 * ]|
 * 
 * Python Usage
 * |[<!-- language="Python" -->
 * stack = Gtk.Stack ()
 * switcher = Handy.Switcher ()
 * switcher.props.stack = stack
 * ]|
 * 
 * Design Information: [GitLab Issue](https://source.puri.sm/Librem5/libhandy/issues/64)
 * 
 * Since: 0.0.8
 */


#define VERTICAL_SPACING 4
#define HORIZONTAL_SPACING 8

typedef struct {
  GtkWidget      *wrap;
  GtkIconSize     icon_size;
  GtkOrientation  orientation;

  GtkWidget      *v_wrap;
  GtkWidget      *v_image;
  GtkWidget      *v_label;

  GtkWidget      *h_wrap;
  GtkWidget      *h_image;
  GtkWidget      *h_label;

  GtkWidget      *stack_child;
} HdySwitcherButtonPrivate;

enum {
  BTN_PROP_0,
  BTN_PROP_ICON_SIZE,
  BTN_PROP_ICON_NAME,
  BTN_PROP_NEEDS_ATTENTION,
  BTN_PROP_LABEL,
  BTN_PROP_STACK_CHILD,
  BTN_PROP_ORIENTATION
};

G_DEFINE_TYPE_WITH_CODE (HdySwitcherButton, hdy_switcher_button, GTK_TYPE_RADIO_BUTTON,
                         G_ADD_PRIVATE (HdySwitcherButton)
                         G_IMPLEMENT_INTERFACE (GTK_TYPE_ORIENTABLE, NULL))

static void
hdy_switcher_button_init (HdySwitcherButton *self)
{
  HdySwitcherButtonPrivate *priv;
  GtkStyleContext *context;

  priv = hdy_switcher_button_get_instance_private (self);

  priv->wrap = g_object_new (GTK_TYPE_STACK,
                             "transition-type", GTK_STACK_TRANSITION_TYPE_CROSSFADE,
                             "homogeneous", FALSE,
                             "vhomogeneous", TRUE,
                             NULL);
  gtk_widget_show (priv->wrap);
  gtk_container_add (GTK_CONTAINER (self), priv->wrap);

  priv->h_wrap = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, HORIZONTAL_SPACING);
  gtk_widget_set_halign (priv->h_wrap, GTK_ALIGN_CENTER);
  gtk_widget_set_valign (priv->h_wrap, GTK_ALIGN_CENTER);
  context = gtk_widget_get_style_context (priv->h_wrap);
  gtk_style_context_add_class (context, "wide");
  gtk_widget_show (priv->h_wrap);
  gtk_stack_add_named (GTK_STACK (priv->wrap), priv->h_wrap, "h");

  priv->h_image = gtk_image_new ();
  g_object_bind_property (self, "icon-size", priv->h_image, "icon-size", G_BINDING_SYNC_CREATE);
  g_object_bind_property (self, "icon-name", priv->h_image, "icon-name", G_BINDING_SYNC_CREATE);
  gtk_widget_show (priv->h_image);
  gtk_container_add (GTK_CONTAINER (priv->h_wrap), priv->h_image);

  priv->h_label = gtk_label_new (NULL);
  gtk_widget_show (priv->h_label);
  gtk_container_add (GTK_CONTAINER (priv->h_wrap), priv->h_label);

  priv->v_wrap = gtk_box_new (GTK_ORIENTATION_VERTICAL, VERTICAL_SPACING);
  gtk_widget_set_halign (priv->v_wrap, GTK_ALIGN_CENTER);
  gtk_widget_set_valign (priv->v_wrap, GTK_ALIGN_CENTER);
  context = gtk_widget_get_style_context (priv->v_wrap);
  gtk_style_context_add_class (context, "narrow");
  gtk_widget_show (priv->v_wrap);
  gtk_stack_add_named (GTK_STACK (priv->wrap), priv->v_wrap, "v");

  priv->v_image = gtk_image_new ();
  g_object_bind_property (self, "icon-size", priv->v_image, "icon-size", G_BINDING_SYNC_CREATE);
  g_object_bind_property (self, "icon-name", priv->v_image, "icon-name", G_BINDING_SYNC_CREATE);
  gtk_widget_show (priv->v_image);
  gtk_container_add (GTK_CONTAINER (priv->v_wrap), priv->v_image);

  priv->v_label = gtk_label_new (NULL);
  gtk_widget_show (priv->v_label);
  gtk_container_add (GTK_CONTAINER (priv->v_wrap), priv->v_label);

  gtk_stack_set_visible_child (GTK_STACK (priv->wrap), priv->h_wrap);

  gtk_widget_set_focus_on_click (GTK_WIDGET (self), FALSE);
  gtk_toggle_button_set_mode (GTK_TOGGLE_BUTTON (self), FALSE);
}


static void
hdy_switcher_button_get_property (GObject      *object,
                                  guint         prop_id,
                                  GValue       *value,
                                  GParamSpec   *pspec)
{
  HdySwitcherButton *self = HDY_SWITCHER_BUTTON (object);
  HdySwitcherButtonPrivate *priv = hdy_switcher_button_get_instance_private (self);
  GtkStyleContext *context;

  switch (prop_id) {
    case BTN_PROP_ICON_SIZE:
      g_value_set_int (value, priv->icon_size);
      break;

    case BTN_PROP_ICON_NAME:
      g_object_get_property (G_OBJECT (priv->h_image), "icon-name", value);
      break;

    case BTN_PROP_NEEDS_ATTENTION:
      context = gtk_widget_get_style_context (GTK_WIDGET (self));
      g_value_set_boolean (value,
                           gtk_style_context_has_class (context,
                                                        GTK_STYLE_CLASS_NEEDS_ATTENTION));
      break;

    case BTN_PROP_LABEL:
      g_object_get_property (G_OBJECT (priv->h_label), "label", value);
      break;

    case BTN_PROP_STACK_CHILD:
      g_value_set_object (value, priv->stack_child);
      break;

    case BTN_PROP_ORIENTATION:
      g_value_set_enum (value, priv->orientation);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static void
hdy_switcher_button_set_property (GObject      *object,
                                  guint         prop_id,
                                  const GValue *value,
                                  GParamSpec   *pspec)
{
  HdySwitcherButton *self = HDY_SWITCHER_BUTTON (object);
  HdySwitcherButtonPrivate *priv = hdy_switcher_button_get_instance_private (self);
  GtkStyleContext *context;

  switch (prop_id) {
    case BTN_PROP_ICON_SIZE:
      priv->icon_size = g_value_get_int (value);
      break;

    case BTN_PROP_ICON_NAME:
      g_object_set_property (G_OBJECT (priv->h_image), "icon-name", value);
      g_object_set_property (G_OBJECT (priv->v_image), "icon-name", value);
      break;

    case BTN_PROP_NEEDS_ATTENTION:
      context = gtk_widget_get_style_context (GTK_WIDGET (self));
      if (g_value_get_boolean (value)) {
        gtk_style_context_add_class (context, GTK_STYLE_CLASS_NEEDS_ATTENTION);
      } else {
        gtk_style_context_remove_class (context, GTK_STYLE_CLASS_NEEDS_ATTENTION);
      }
      break;

    case BTN_PROP_LABEL:
      g_object_set_property (G_OBJECT (priv->h_label), "label", value);
      g_object_set_property (G_OBJECT (priv->v_label), "label", value);
      break;

    case BTN_PROP_STACK_CHILD:
      priv->stack_child = g_value_get_object (value);
      break;

    case BTN_PROP_ORIENTATION:
      priv->orientation = g_value_get_enum (value);
      if (priv->orientation == GTK_ORIENTATION_HORIZONTAL) {
        gtk_stack_set_visible_child (GTK_STACK (priv->wrap), priv->h_wrap);
      } else {
        gtk_stack_set_visible_child (GTK_STACK (priv->wrap), priv->v_wrap);
      }
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static void
hdy_switcher_button_get_size (HdySwitcherButton *self,
                              gint              *v_min_width,
                              gint              *h_min_width,
                              gint              *h_nat_width)
{
  HdySwitcherButtonPrivate *priv = hdy_switcher_button_get_instance_private (self);

  if (v_min_width) {
    gtk_widget_get_preferred_width (priv->v_wrap, v_min_width, NULL);
  }
  if (h_min_width || h_nat_width) {
    gtk_widget_get_preferred_width (priv->h_wrap, h_min_width, h_nat_width);
  }
}

static void
hdy_switcher_button_class_init (HdySwitcherButtonClass *klass)
{
  GObjectClass   *object_class = G_OBJECT_CLASS (klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  object_class->get_property = hdy_switcher_button_get_property;
  object_class->set_property = hdy_switcher_button_set_property;

  /**
   * HdySwitcherButton:icon-size:
   *
   * Use the "icon-size" property to change the size of the image in the #HdySwitcherButton
   *
   * Note: This is controlled via #GtkStack
   * 
   * Since: 0.0.8
   */
  g_object_class_install_property (object_class,
                                   BTN_PROP_ICON_SIZE,
                                   g_param_spec_int ("icon-size",
                                                     _("Icon Size"),
                                                     _("Symbolic size to use for named icon"),
                                                     0, G_MAXINT,
                                                     GTK_ICON_SIZE_BUTTON,
                                                     G_PARAM_EXPLICIT_NOTIFY |
                                                     G_PARAM_READWRITE |
                                                     G_PARAM_STATIC_STRINGS));

  /**
   * HdySwitcherButton:icon-name:
   *
   * Use the "icon-name" property to change the icon displayed in the #HdySwitcherButton
   * 
   * Note: This is controlled via #GtkStack
   *
   * Since: 0.0.8
   */
  g_object_class_install_property (object_class,
                                   BTN_PROP_ICON_NAME,
                                   g_param_spec_string ("icon-name",
                                                        _("Icon Name"),
                                                        _("Icon name for image"),
                                                        "text-x-generic-symbolic",
                                                        G_PARAM_EXPLICIT_NOTIFY |
                                                        G_PARAM_READWRITE |
                                                        G_PARAM_STATIC_STRINGS));

  /**
   * HdySwitcherButton:needs-attention:
   *
   * Show a hint on the #HdySwitcherButton that a page wants attention
   *
   * Note: This is controlled via #GtkStack
   * 
   * Since: 0.0.8
   */
  g_object_class_install_property (object_class,
                                   BTN_PROP_NEEDS_ATTENTION,
                                   g_param_spec_boolean ("needs-attention",
                                                         _("Needs Attention"),
                                                         _("Hint the child needs attention"),
                                                         FALSE,
                                                         G_PARAM_EXPLICIT_NOTIFY |
                                                         G_PARAM_READWRITE |
                                                         G_PARAM_STATIC_STRINGS));

  g_object_class_override_property (object_class,
                                    BTN_PROP_LABEL,
                                    "label");

  /**
   * HdySwitcherButton:stack-child:
   *
   * The page represented by this button
   *
   * Note: This is controlled via #HdySwitcher
   * 
   * Since: 0.0.8
   */
  g_object_class_install_property (object_class,
                                   BTN_PROP_STACK_CHILD,
                                   g_param_spec_object ("stack-child",
                                                        _("Stack Child"),
                                                        _("Page represented by button"),
                                                        GTK_TYPE_WIDGET,
                                                        G_PARAM_EXPLICIT_NOTIFY |
                                                        G_PARAM_READWRITE |
                                                        G_PARAM_CONSTRUCT_ONLY |
                                                        G_PARAM_STATIC_STRINGS));

  g_object_class_override_property (object_class,
                                    BTN_PROP_ORIENTATION,
                                    "orientation");

  gtk_widget_class_set_css_name (widget_class, "hdyswitcherbutton");
}

/* Get this buttons page */
static GtkWidget *
hdy_switcher_button_get_stack_child (HdySwitcherButton *self)
{
  HdySwitcherButtonPrivate *priv;

  g_return_val_if_fail (HDY_IS_SWITCHER_BUTTON (self), NULL);

  priv = hdy_switcher_button_get_instance_private (self);

  return priv->stack_child;
}

#define TIMEOUT_EXPAND 500

typedef struct {
  GtkStack *stack;
  GHashTable *buttons;
  gint icon_size;
  gboolean in_child_changed;
  GtkWidget *switch_button;
  guint switch_timer;
  GtkOrientation orientation;
} HdySwitcherPrivate;

enum {
  PROP_0,
  PROP_ICON_SIZE,
  PROP_STACK,
  PROP_ORIENTATION
};

G_DEFINE_TYPE_WITH_CODE (HdySwitcher, hdy_switcher, GTK_TYPE_BOX,
                         G_ADD_PRIVATE (HdySwitcher))

static void
hdy_switcher_init (HdySwitcher *self)
{
  HdySwitcherPrivate *priv;

  gtk_widget_set_has_window (GTK_WIDGET (self), FALSE);

  priv = hdy_switcher_get_instance_private (self);

  /* Setup our private data */
  priv->orientation = GTK_ORIENTATION_HORIZONTAL;
  priv->icon_size = GTK_ICON_SIZE_BUTTON;
  priv->stack = NULL;
  priv->buttons = g_hash_table_new (g_direct_hash, g_direct_equal);

  gtk_widget_set_valign (GTK_WIDGET (self), GTK_ALIGN_FILL);
  gtk_box_set_homogeneous (GTK_BOX (self), TRUE);

  gtk_drag_dest_set (GTK_WIDGET (self), 0, NULL, 0, 0);
  gtk_drag_dest_set_track_motion (GTK_WIDGET (self), TRUE);
}

/* Switch to a buttons page when it's clicked */
static void
on_button_clicked (GtkWidget   *widget,
                   HdySwitcher *self)
{
  GtkWidget *child;
  HdySwitcherPrivate *priv;

  priv = hdy_switcher_get_instance_private (self);

  if (priv->in_child_changed)
    return;

  child = hdy_switcher_button_get_stack_child (HDY_SWITCHER_BUTTON (widget));
  gtk_stack_set_visible_child (priv->stack, child);
}

/* Sync a button to it's page */
static void
update_button (HdySwitcher *self,
               GtkWidget   *widget,
               GtkWidget   *button)
{
  gchar *title;
  gchar *icon_name;
  gboolean needs_attention;
  HdySwitcherPrivate *priv;

  priv = hdy_switcher_get_instance_private (self);

  /* Get properties from the page and set them on the button */
  gtk_container_child_get (GTK_CONTAINER (priv->stack), widget,
                           "title", &title,
                           "icon-name", &icon_name,
                           "needs-attention", &needs_attention,
                           NULL);

  g_object_set (G_OBJECT (button),
                "icon-name", icon_name,
                "icon-size", priv->icon_size,
                "label", title,
                "needs-attention", needs_attention,
                NULL);

  /* Hide the button if the page is invisible or has neither a title or icon */
  gtk_widget_set_visible (button, gtk_widget_get_visible (widget) && (title != NULL || icon_name != NULL));

  g_free (title);
  g_free (icon_name);
}

/* The Title/Icon/Visibility of a page changed */
static void
on_title_icon_visible_updated (GtkWidget   *widget,
                               GParamSpec  *pspec,
                               HdySwitcher *self)
{
  GtkWidget *button;
  HdySwitcherPrivate *priv;

  priv = hdy_switcher_get_instance_private (self);

  /* Update the button */
  button = g_hash_table_lookup (priv->buttons, widget);
  update_button (self, widget, button);
}

/* The stack was reordered, reflect this in the buttons */
static void
on_position_updated (GtkWidget   *widget,
                     GParamSpec  *pspec,
                     HdySwitcher *self)
{
  GtkWidget *button;
  gint position;
  HdySwitcherPrivate *priv;

  priv = hdy_switcher_get_instance_private (self);

  button = g_hash_table_lookup (priv->buttons, widget);

  /* Get the new position */
  gtk_container_child_get (GTK_CONTAINER (priv->stack), widget,
                           "position", &position,
                           NULL);

  /* Move the button there */
  gtk_box_reorder_child (GTK_BOX (self), button, position);
}

/* A page has started/stoped requesting attention */
static void
on_needs_attention_updated (GtkWidget        *widget,
                            GParamSpec       *pspec,
                            HdySwitcher *self)
{
  GtkWidget *button;
  HdySwitcherPrivate *priv;

  priv = hdy_switcher_get_instance_private (self);

  /* Keep the button in sync */
  button = g_hash_table_lookup (priv->buttons, widget);
  update_button (self, widget, button);
}

static void
remove_switch_timer (HdySwitcher *self)
{
  HdySwitcherPrivate *priv;

  priv = hdy_switcher_get_instance_private (self);

  if (priv->switch_timer) {
    g_source_remove (priv->switch_timer);
    priv->switch_timer = 0;
  }
}

static gboolean
hdy_switcher_switch_timeout (gpointer data)
{
  HdySwitcher *self = data;
  HdySwitcherPrivate *priv;
  GtkWidget *button;

  priv = hdy_switcher_get_instance_private (self);

  priv->switch_timer = 0;

  button = priv->switch_button;
  priv->switch_button = NULL;

  if (button)
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (button), TRUE);

  return G_SOURCE_REMOVE;
}

static gboolean
hdy_switcher_drag_motion (GtkWidget      *widget,
                          GdkDragContext *context,
                          gint            x,
                          gint            y,
                          guint           time)
{
  HdySwitcher *self = HDY_SWITCHER (widget);
  HdySwitcherPrivate *priv;
  GtkAllocation allocation;
  GtkWidget *button;
  GHashTableIter iter;
  gpointer value;
  gboolean retval = FALSE;

  gtk_widget_get_allocation (widget, &allocation);

  priv = hdy_switcher_get_instance_private (self);

  x += allocation.x;
  y += allocation.y;

  button = NULL;
  g_hash_table_iter_init (&iter, priv->buttons);
  while (g_hash_table_iter_next (&iter, NULL, &value)) {
    gtk_widget_get_allocation (GTK_WIDGET (value), &allocation);
    if (x >= allocation.x && x <= allocation.x + allocation.width &&
        y >= allocation.y && y <= allocation.y + allocation.height) {
      button = GTK_WIDGET (value);
      retval = TRUE;
      break;
    }
  }

  if (button != priv->switch_button)
    remove_switch_timer (self);

  priv->switch_button = button;

  if (button && !priv->switch_timer) {
    priv->switch_timer = gdk_threads_add_timeout (TIMEOUT_EXPAND,
                                                  hdy_switcher_switch_timeout,
                                                  self);
    g_source_set_name_by_id (priv->switch_timer, "[gtk+] hdy_switcher_switch_timeout");
  }

  return retval;
}

static void
hdy_switcher_drag_leave (GtkWidget      *widget,
                         GdkDragContext *context,
                         guint           time)
{
  HdySwitcher *self = HDY_SWITCHER (widget);

  remove_switch_timer (self);
}

/* Add a button by stack page */
static void
add_child (GtkWidget   *widget,
           HdySwitcher *self)
{
  GtkWidget *button;
  GList *group;
  HdySwitcherPrivate *priv;

  priv = hdy_switcher_get_instance_private (self);

  /* Create the button and bind some simple properties */
  button = g_object_new (HDY_TYPE_SWITCHER_BUTTON,
                         "stack-child", widget,
                         NULL);
  g_object_bind_property (self, "orientation", button, "orientation", G_BINDING_SYNC_CREATE);
  g_object_bind_property (self, "icon-size", button, "icon-size", G_BINDING_SYNC_CREATE);

  update_button (self, widget, button);

  /* Add the button the the group */
  group = gtk_container_get_children (GTK_CONTAINER (self));
  if (group != NULL) {
    gtk_radio_button_join_group (GTK_RADIO_BUTTON (button), GTK_RADIO_BUTTON (group->data));
    g_list_free (group);
  }

  gtk_container_add (GTK_CONTAINER (self), button);

  g_signal_connect (button, "clicked", G_CALLBACK (on_button_clicked), self);
  /* Listen to changes on the page */
  g_signal_connect (widget, "notify::visible", G_CALLBACK (on_title_icon_visible_updated), self);
  g_signal_connect (widget, "child-notify::title", G_CALLBACK (on_title_icon_visible_updated), self);
  g_signal_connect (widget, "child-notify::icon-name", G_CALLBACK (on_title_icon_visible_updated), self);
  g_signal_connect (widget, "child-notify::position", G_CALLBACK (on_position_updated), self);
  g_signal_connect (widget, "child-notify::needs-attention", G_CALLBACK (on_needs_attention_updated), self);

  /* Store the relationship between the page and button */
  g_hash_table_insert (priv->buttons, widget, button);
}

/* Remove button by stack page */
static void
remove_child (GtkWidget   *widget,
              HdySwitcher *self)
{
  GtkWidget *button;
  HdySwitcherPrivate *priv;

  priv = hdy_switcher_get_instance_private (self);

  g_signal_handlers_disconnect_by_func (widget, on_title_icon_visible_updated, self);
  g_signal_handlers_disconnect_by_func (widget, on_position_updated, self);
  g_signal_handlers_disconnect_by_func (widget, on_needs_attention_updated, self);

  /* Find the pages button, remove it from the UI & table */
  button = g_hash_table_lookup (priv->buttons, widget);
  gtk_container_remove (GTK_CONTAINER (self), button);
  g_hash_table_remove (priv->buttons, widget);
}

/* Generate buttons for the stack */
static void
populate_switcher (HdySwitcher *self)
{
  HdySwitcherPrivate *priv;
  GtkWidget *widget, *button;

  priv = hdy_switcher_get_instance_private (self);

  /* Create the buttons */
  gtk_container_foreach (GTK_CONTAINER (priv->stack), (GtkCallback)add_child, self);

  widget = gtk_stack_get_visible_child (priv->stack);
  if (!widget)
    return;

  /* Find the button for the current page and mark it as active */
  button = g_hash_table_lookup (priv->buttons, widget);
  priv->in_child_changed = TRUE;
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (button), TRUE);
  priv->in_child_changed = FALSE;
}

/* Remove all buttons from the switcher */
static void
clear_switcher (HdySwitcher *self)
{
  HdySwitcherPrivate *priv;

  priv = hdy_switcher_get_instance_private (self);
  gtk_container_foreach (GTK_CONTAINER (priv->stack), (GtkCallback)remove_child, self);
}

/* The active stack page changed */
static void
on_child_changed (GtkWidget   *widget,
                  GParamSpec  *pspec,
                  HdySwitcher *self)
{
  GtkWidget *child;
  GtkWidget *button;
  HdySwitcherPrivate *priv;

  priv = hdy_switcher_get_instance_private (self);

  child = gtk_stack_get_visible_child (GTK_STACK (widget));

  /* Find the button for the page */
  button = g_hash_table_lookup (priv->buttons, child);

  /* Can't do anything without a button */
  if (button == NULL)
    return;

  priv->in_child_changed = TRUE;
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (button), TRUE);
  priv->in_child_changed = FALSE;
}

/* A page was added to the stack */
static void
on_stack_child_added (GtkContainer *container,
                      GtkWidget    *widget,
                      HdySwitcher  *self)
{
  add_child (widget, self);
}

/* A page was removed from the stack */
static void
on_stack_child_removed (GtkContainer *container,
                        GtkWidget    *widget,
                        HdySwitcher  *self)
{
  remove_child (widget, self);
}

/* Disconnect to various signals on the stack itself */
static void
disconnect_stack_signals (HdySwitcher *switcher)
{
  HdySwitcherPrivate *priv;

  priv = hdy_switcher_get_instance_private (switcher);
  g_signal_handlers_disconnect_by_func (priv->stack, on_stack_child_added, switcher);
  g_signal_handlers_disconnect_by_func (priv->stack, on_stack_child_removed, switcher);
  g_signal_handlers_disconnect_by_func (priv->stack, on_child_changed, switcher);
  g_signal_handlers_disconnect_by_func (priv->stack, disconnect_stack_signals, switcher);
}

/* Connect to various signals on the stack itself */
static void
connect_stack_signals (HdySwitcher *switcher)
{
  HdySwitcherPrivate *priv;

  priv = hdy_switcher_get_instance_private (switcher);
  g_signal_connect_after (priv->stack, "add",
                          G_CALLBACK (on_stack_child_added), switcher);
  g_signal_connect_after (priv->stack, "remove",
                          G_CALLBACK (on_stack_child_removed), switcher);
  g_signal_connect (priv->stack, "notify::visible-child",
                    G_CALLBACK (on_child_changed), switcher);
  g_signal_connect_swapped (priv->stack, "destroy",
                            G_CALLBACK (disconnect_stack_signals), switcher);
}

/**
 * hdy_switcher_set_stack:
 * @self: a #HdySwitcher
 * @stack: (allow-none): a #GtkStack
 *
 * Sets the #GtkStack to control.
 *
 * C Usage
 * |[<!-- language="C" -->
 * hdy_switcher_set_stack (HDY_SWITCHER (switcher), GTK_STACK (stack));
 * ]|
 * 
 * Vala Usage
 * |[<!-- language="Vala" -->
 * switcher.stack = stack;
 * ]|
 * 
 * Python Usage
 * |[<!-- language="Python" -->
 * switcher.props.stack = stack
 * ]|
 * 
 * Since: 0.0.8
 */
void
hdy_switcher_set_stack (HdySwitcher *self,
                        GtkStack    *stack)
{
  HdySwitcherPrivate *priv;

  g_return_if_fail (HDY_IS_SWITCHER (self));
  g_return_if_fail (GTK_IS_STACK (stack) || stack == NULL);

  priv = hdy_switcher_get_instance_private (self);

  /* Don't do anything, it's already the stack */
  if (priv->stack == stack)
    return;

  /* Disconnect from the current stack (if any) */
  if (priv->stack) {
    disconnect_stack_signals (self);
    clear_switcher (self);
    g_clear_object (&priv->stack);
  }

  /* Connect to the new stack (if any) */
  if (stack) {
    priv->stack = g_object_ref (stack);
    populate_switcher (self);
    connect_stack_signals (self);
  }

  gtk_widget_queue_resize (GTK_WIDGET (self));

  g_object_notify (G_OBJECT (self), "stack");
}

/**
 * hdy_switcher_get_stack:
 * @self: a #HdySwitcher
 *
 * Get the #GtkStack being controlled by the #HdySwitcher
 *
 * See: hdy_switcher_set_stack()
 * 
 * Returns: (nullable) (transfer none): the #GtkStack, or %NULL if none has been set
 *
 * Since: 0.0.8
 */
GtkStack *
hdy_switcher_get_stack (HdySwitcher *self)
{
  HdySwitcherPrivate *priv;
  g_return_val_if_fail (HDY_IS_SWITCHER (self), NULL);

  priv = hdy_switcher_get_instance_private (self);

  return priv->stack;
}

/**
 * hdy_switcher_set_icon_size:
 * @self: a #HdySwitcher
 * @icon_size: the new icon size
 *
 * Change the icon size of the images used in the #HdySwitcher
 * 
 * C Usage
 * |[<!-- language="C" -->
 * hdy_switcher_set_icon_size (HDY_SWITCHER (switcher), GTK_ICON_SIZE_BUTTON);
 * ]|
 * 
 * Vala Usage
 * |[<!-- language="Vala" -->
 * switcher.icon_size = Gtk.IconSize.BUTTON;
 * ]|
 * 
 * Python Usage
 * |[<!-- language="Python" -->
 * switcher.props.icon_size = Gtk.IconSize.BUTTON
 * ]|
 *
 * Since: 0.0.8
   */
void
hdy_switcher_set_icon_size (HdySwitcher *self,
                            GtkIconSize  icon_size)
{
  HdySwitcherPrivate *priv;

  g_return_if_fail (HDY_IS_SWITCHER (self));

  priv = hdy_switcher_get_instance_private (self);

  /* Surpress non-changes */
  if (icon_size == priv->icon_size)
    return;

  g_object_notify (G_OBJECT (self), "icon-size");
}

/* Handle GtkOrientable.set_orientation */
static void
hdy_switcher_set_orientation (HdySwitcher    *self,
                              GtkOrientation  orientation)
{
  HdySwitcherPrivate *priv;

  g_return_if_fail (HDY_IS_SWITCHER (self));

  priv = hdy_switcher_get_instance_private (self);

  /* Surpress non-changes to avoid needless style recalculations */
  if (orientation == priv->orientation)
    return;

  priv->orientation = orientation;

  g_object_notify (G_OBJECT (self), "orientation");
}

static void
hdy_switcher_get_property (GObject      *object,
                           guint         prop_id,
                           GValue       *value,
                           GParamSpec   *pspec)
{
  HdySwitcher *self = HDY_SWITCHER (object);
  HdySwitcherPrivate *priv;

  priv = hdy_switcher_get_instance_private (self);
  switch (prop_id)
    {
    case PROP_ICON_SIZE:
      g_value_set_int (value, priv->icon_size);
      break;

    case PROP_STACK:
      g_value_set_object (value, priv->stack);
      break;

    case PROP_ORIENTATION:
      g_value_set_enum (value, priv->orientation);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static void
hdy_switcher_set_property (GObject      *object,
                           guint         prop_id,
                           const GValue *value,
                           GParamSpec   *pspec)
{
  HdySwitcher *self = HDY_SWITCHER (object);

  switch (prop_id)
    {
    case PROP_ICON_SIZE:
      hdy_switcher_set_icon_size (self, g_value_get_int (value));
      break;

    case PROP_STACK:
      hdy_switcher_set_stack (self, g_value_get_object (value));
      break;

    case PROP_ORIENTATION:
      hdy_switcher_set_orientation (self, g_value_get_enum (value));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static void
hdy_switcher_dispose (GObject *object)
{
  HdySwitcher *self = HDY_SWITCHER (object);

  remove_switch_timer (self);
  hdy_switcher_set_stack (self, NULL);

  G_OBJECT_CLASS (hdy_switcher_parent_class)->dispose (object);
}

static void
hdy_switcher_finalize (GObject *object)
{
  HdySwitcher *self = HDY_SWITCHER (object);
  HdySwitcherPrivate *priv;

  priv = hdy_switcher_get_instance_private (self);

  g_hash_table_destroy (priv->buttons);

  G_OBJECT_CLASS (hdy_switcher_parent_class)->finalize (object);
}

static void
hdy_switcher_get_preferred_width (GtkWidget *widget,
                                  gint      *min,
                                  gint      *nat)
{
  HdySwitcher *self = HDY_SWITCHER (widget);
  gint widest_v = 0;
  gint widest_h = 0;
  gint count = 0;

  for (GList *l = gtk_container_get_children (GTK_CONTAINER (self)); l != NULL; l = g_list_next (l)) {
    gint v_min = 0;
    gint h_nat = 0;

    hdy_switcher_button_get_size (HDY_SWITCHER_BUTTON (l->data), &v_min, NULL, &h_nat);

    if (v_min > widest_v)
      widest_v = v_min;
    
    if (h_nat > widest_h)
      widest_h = h_nat;

    count++;
  }

  *min = widest_v * count;
  *nat = widest_h * count;
}

static gint
is_narrow (HdySwitcher *self,
           gint         width)
{
  gint widest_h = 0;
  gint count = 0;

  for (GList *l = gtk_container_get_children (GTK_CONTAINER (self)); l != NULL; l = g_list_next (l)) {
    gint h_min = 0;

    hdy_switcher_button_get_size (HDY_SWITCHER_BUTTON (l->data), NULL, &h_min, NULL);
    
    widest_h = MAX (widest_h, h_min);

    count++;
  }

  return (widest_h * count) > width;
}

static void
hdy_switcher_size_allocate (GtkWidget        *widget,
                                   GtkAllocation    *allocation)
{
  if (is_narrow (HDY_SWITCHER (widget), allocation->width)) {
    gtk_orientable_set_orientation (GTK_ORIENTABLE (widget), GTK_ORIENTATION_VERTICAL);
  } else {
    gtk_orientable_set_orientation (GTK_ORIENTABLE (widget), GTK_ORIENTATION_HORIZONTAL);
  }

  GTK_WIDGET_CLASS (hdy_switcher_parent_class)->size_allocate (widget, allocation);
}

static void
hdy_switcher_class_init (HdySwitcherClass *class)
{
  GObjectClass *object_class = G_OBJECT_CLASS (class);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (class);
  GtkCssProvider *provider;

  object_class->get_property = hdy_switcher_get_property;
  object_class->set_property = hdy_switcher_set_property;
  object_class->dispose = hdy_switcher_dispose;
  object_class->finalize = hdy_switcher_finalize;

  widget_class->size_allocate = hdy_switcher_size_allocate;
  widget_class->get_preferred_width = hdy_switcher_get_preferred_width;
  widget_class->drag_motion = hdy_switcher_drag_motion;
  widget_class->drag_leave = hdy_switcher_drag_leave;

  /**
   * HdySwitcher:icon-size:
   *
   * Use the "icon-size" property to change the size of the images
   *
   * Since: 0.0.8
   */
  g_object_class_install_property (object_class,
                                   PROP_ICON_SIZE,
                                   g_param_spec_int ("icon-size",
                                                     _("Icon Size"),
                                                     _("Symbolic size to use for named icon"),
                                                     0, G_MAXINT,
                                                     GTK_ICON_SIZE_BUTTON,
                                                     G_PARAM_EXPLICIT_NOTIFY |
                                                     G_PARAM_READWRITE |
                                                     G_PARAM_STATIC_STRINGS));

  /**
   * HdySwitcher:stack:
   *
   * The #GtkStack the #HdySwitcher controls
   *
   * Since: 0.0.8
   */
  g_object_class_install_property (object_class,
                                   PROP_STACK,
                                   g_param_spec_object ("stack",
                                                        _("Stack"),
                                                        _("Stack"),
                                                        GTK_TYPE_STACK,
                                                        G_PARAM_READWRITE |
                                                        G_PARAM_CONSTRUCT |
                                                        G_PARAM_STATIC_STRINGS));

  /* We switch the buttons orientation instead of the box containing them */
  g_object_class_override_property (object_class,
                                    PROP_ORIENTATION,
                                    "orientation");

  gtk_widget_class_set_css_name (widget_class, "hdyswitcher");

  /* Load the styles for the widget, this feels very hacky */
  provider = gtk_css_provider_new ();
  gtk_css_provider_load_from_resource (provider, "/sm/puri/handy/styles/hdy-switcher.css");
  gtk_style_context_add_provider_for_screen (gdk_screen_get_default (),
                                             GTK_STYLE_PROVIDER (provider),
                                             GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
  g_object_unref (provider);
}

/**
 * hdy_switcher_new:
 *
 * Create a #HdySwitcher with #GtkWindow:transient-for set to parent
 *
 * C Usage
 * |[<!-- language="C" -->
 * GtkWidget *switcher = hdy_switcher_new ();
 * ]|
 * 
 * Vala Usage
 * |[<!-- language="Vala" -->
 * var switcher = new Hdy.Switcher ();
 * ]|
 * 
 * Python Usage
 * |[<!-- language="Python" -->
 * switcher = Handy.Switcher ()
 * ]|
 *
 * Since: 0.0.8
 */
GtkWidget *
hdy_switcher_new (void)
{
  return g_object_new (HDY_TYPE_SWITCHER, NULL);
}
