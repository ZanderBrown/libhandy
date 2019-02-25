/*
 * Copyright © 2019 Zander Brown <zbrown@gnome.org>
 * 
 * Based on HdySwitcher.c, Copyright (c) 2013 Red Hat, Inc.
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
 * An alternative #HdySwitcher
 * 
 * Design Information: [GitLab Issue](https://source.puri.sm/Librem5/libhandy/issues/64)
 * 
 * Since: 0.0.8
 */

#define TIMEOUT_EXPAND 500

typedef struct {
  GtkStack *stack;
  GHashTable *buttons;
  gint icon_size;
  gboolean in_child_changed;
  GtkWidget *switch_button;
  guint switch_timer;
} HdySwitcherPrivate;

enum {
  PROP_0,
  PROP_ICON_SIZE,
  PROP_STACK
};

G_DEFINE_TYPE_WITH_CODE (HdySwitcher, hdy_switcher, GTK_TYPE_BOX,
                         G_ADD_PRIVATE (HdySwitcher))

static void
hdy_switcher_init (HdySwitcher *switcher)
{
  GtkStyleContext *context;
  HdySwitcherPrivate *priv;

  gtk_widget_set_has_window (GTK_WIDGET (switcher), FALSE);

  priv = hdy_switcher_get_instance_private (switcher);

  priv->icon_size = GTK_ICON_SIZE_MENU;
  priv->stack = NULL;
  priv->buttons = g_hash_table_new (g_direct_hash, g_direct_equal);

  context = gtk_widget_get_style_context (GTK_WIDGET (switcher));
  gtk_style_context_add_class (context, "stack-switcher");
  gtk_style_context_add_class (context, GTK_STYLE_CLASS_LINKED);

  gtk_orientable_set_orientation (GTK_ORIENTABLE (switcher), GTK_ORIENTATION_HORIZONTAL);

  gtk_drag_dest_set (GTK_WIDGET (switcher), 0, NULL, 0, 0);
  gtk_drag_dest_set_track_motion (GTK_WIDGET (switcher), TRUE);
}

static void
on_button_clicked (GtkWidget        *widget,
                   HdySwitcher *self)
{
  GtkWidget *child;
  HdySwitcherPrivate *priv;

  priv = hdy_switcher_get_instance_private (self);

  if (!priv->in_child_changed)
    {
      child = g_object_get_data (G_OBJECT (widget), "stack-child");
      gtk_stack_set_visible_child (priv->stack, child);
    }
}

static void
rebuild_child (GtkWidget   *self,
               const gchar *icon_name,
               const gchar *title,
               gint         icon_size)
{
  GtkStyleContext *context;
  GtkWidget *button_child;

  button_child = gtk_bin_get_child (GTK_BIN (self));
  if (button_child != NULL)
    gtk_widget_destroy (button_child);

  button_child = NULL;
  context = gtk_widget_get_style_context (GTK_WIDGET (self));

  if (icon_name != NULL)
    {
      button_child = gtk_image_new_from_icon_name (icon_name, icon_size);
      if (title != NULL)
        gtk_widget_set_tooltip_text (GTK_WIDGET (self), title);

      gtk_style_context_remove_class (context, "text-button");
      gtk_style_context_add_class (context, "image-button");
    }
  else if (title != NULL)
    {
      button_child = gtk_label_new (title);

      gtk_widget_set_tooltip_text (GTK_WIDGET (self), NULL);

      gtk_style_context_remove_class (context, "image-button");
      gtk_style_context_add_class (context, "text-button");
    }

  if (button_child)
    {
      gtk_widget_set_halign (GTK_WIDGET (button_child), GTK_ALIGN_CENTER);
      gtk_widget_show_all (button_child);
      gtk_container_add (GTK_CONTAINER (self), button_child);
    }
}

static void
update_needs_attention (GtkWidget *widget, GtkWidget *button, gpointer data)
{
  GtkContainer *container;
  gboolean needs_attention;
  GtkStyleContext *context;

  container = GTK_CONTAINER (data);
  gtk_container_child_get (container, widget,
                           "needs-attention", &needs_attention,
                           NULL);

  context = gtk_widget_get_style_context (button);
  if (needs_attention)
    gtk_style_context_add_class (context, GTK_STYLE_CLASS_NEEDS_ATTENTION);
  else
    gtk_style_context_remove_class (context, GTK_STYLE_CLASS_NEEDS_ATTENTION);
}

static void
update_button (HdySwitcher *self,
               GtkWidget        *widget,
               GtkWidget        *button)
{
  gchar *title;
  gchar *icon_name;
  HdySwitcherPrivate *priv;

  priv = hdy_switcher_get_instance_private (self);

  gtk_container_child_get (GTK_CONTAINER (priv->stack), widget,
                           "title", &title,
                           "icon-name", &icon_name,
                           NULL);

  rebuild_child (button, icon_name, title, priv->icon_size);

  gtk_widget_set_visible (button, gtk_widget_get_visible (widget) && (title != NULL || icon_name != NULL));

  g_free (title);
  g_free (icon_name);

  update_needs_attention (widget, button, priv->stack);
}

static void
on_title_icon_visible_updated (GtkWidget        *widget,
                               GParamSpec       *pspec,
                               HdySwitcher *self)
{
  GtkWidget *button;
  HdySwitcherPrivate *priv;

  priv = hdy_switcher_get_instance_private (self);

  button = g_hash_table_lookup (priv->buttons, widget);
  update_button (self, widget, button);
}

static void
on_position_updated (GtkWidget        *widget,
                     GParamSpec       *pspec,
                     HdySwitcher *self)
{
  GtkWidget *button;
  gint position;
  HdySwitcherPrivate *priv;

  priv = hdy_switcher_get_instance_private (self);

  button = g_hash_table_lookup (priv->buttons, widget);

  gtk_container_child_get (GTK_CONTAINER (priv->stack), widget,
                           "position", &position,
                           NULL);

  gtk_box_reorder_child (GTK_BOX (self), button, position);
}

static void
on_needs_attention_updated (GtkWidget        *widget,
                            GParamSpec       *pspec,
                            HdySwitcher *self)
{
  GtkWidget *button;
  HdySwitcherPrivate *priv;

  priv = hdy_switcher_get_instance_private (self);

  button = g_hash_table_lookup (priv->buttons, widget);
  update_button (self, widget, button);
}

static void
remove_switch_timer (HdySwitcher *self)
{
  HdySwitcherPrivate *priv;

  priv = hdy_switcher_get_instance_private (self);

  if (priv->switch_timer)
    {
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
  while (g_hash_table_iter_next (&iter, NULL, &value))
    {
      gtk_widget_get_allocation (GTK_WIDGET (value), &allocation);
      if (x >= allocation.x && x <= allocation.x + allocation.width &&
          y >= allocation.y && y <= allocation.y + allocation.height)
        {
          button = GTK_WIDGET (value);
          retval = TRUE;
          break;
        }
    }

  if (button != priv->switch_button)
    remove_switch_timer (self);

  priv->switch_button = button;

  if (button && !priv->switch_timer)
    {
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

static void
add_child (GtkWidget        *widget,
           HdySwitcher *self)
{
  GtkWidget *button;
  GList *group;
  HdySwitcherPrivate *priv;

  priv = hdy_switcher_get_instance_private (self);

  button = gtk_radio_button_new (NULL);

  gtk_widget_set_focus_on_click (button, FALSE);
  gtk_toggle_button_set_mode (GTK_TOGGLE_BUTTON (button), FALSE);

  update_button (self, widget, button);

  group = gtk_container_get_children (GTK_CONTAINER (self));
  if (group != NULL)
    {
      gtk_radio_button_join_group (GTK_RADIO_BUTTON (button), GTK_RADIO_BUTTON (group->data));
      g_list_free (group);
    }

  gtk_container_add (GTK_CONTAINER (self), button);

  g_object_set_data (G_OBJECT (button), "stack-child", widget);
  g_signal_connect (button, "clicked", G_CALLBACK (on_button_clicked), self);
  g_signal_connect (widget, "notify::visible", G_CALLBACK (on_title_icon_visible_updated), self);
  g_signal_connect (widget, "child-notify::title", G_CALLBACK (on_title_icon_visible_updated), self);
  g_signal_connect (widget, "child-notify::icon-name", G_CALLBACK (on_title_icon_visible_updated), self);
  g_signal_connect (widget, "child-notify::position", G_CALLBACK (on_position_updated), self);
  g_signal_connect (widget, "child-notify::needs-attention", G_CALLBACK (on_needs_attention_updated), self);

  g_hash_table_insert (priv->buttons, widget, button);
}

static void
remove_child (GtkWidget        *widget,
              HdySwitcher *self)
{
  GtkWidget *button;
  HdySwitcherPrivate *priv;

  priv = hdy_switcher_get_instance_private (self);

  g_signal_handlers_disconnect_by_func (widget, on_title_icon_visible_updated, self);
  g_signal_handlers_disconnect_by_func (widget, on_position_updated, self);
  g_signal_handlers_disconnect_by_func (widget, on_needs_attention_updated, self);

  button = g_hash_table_lookup (priv->buttons, widget);
  gtk_container_remove (GTK_CONTAINER (self), button);
  g_hash_table_remove (priv->buttons, widget);
}

static void
populate_switcher (HdySwitcher *self)
{
  HdySwitcherPrivate *priv;
  GtkWidget *widget, *button;

  priv = hdy_switcher_get_instance_private (self);
  gtk_container_foreach (GTK_CONTAINER (priv->stack), (GtkCallback)add_child, self);

  widget = gtk_stack_get_visible_child (priv->stack);
  if (widget)
    {
      button = g_hash_table_lookup (priv->buttons, widget);
      priv->in_child_changed = TRUE;
      gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (button), TRUE);
      priv->in_child_changed = FALSE;
    }
}

static void
clear_switcher (HdySwitcher *self)
{
  HdySwitcherPrivate *priv;

  priv = hdy_switcher_get_instance_private (self);
  gtk_container_foreach (GTK_CONTAINER (priv->stack), (GtkCallback)remove_child, self);
}

static void
on_child_changed (GtkWidget        *widget,
                  GParamSpec       *pspec,
                  HdySwitcher *self)
{
  GtkWidget *child;
  GtkWidget *button;
  HdySwitcherPrivate *priv;

  priv = hdy_switcher_get_instance_private (self);

  child = gtk_stack_get_visible_child (GTK_STACK (widget));
  button = g_hash_table_lookup (priv->buttons, child);
  if (button != NULL)
    {
      priv->in_child_changed = TRUE;
      gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (button), TRUE);
      priv->in_child_changed = FALSE;
    }
}

static void
on_stack_child_added (GtkContainer     *container,
                      GtkWidget        *widget,
                      HdySwitcher *self)
{
  add_child (widget, self);
}

static void
on_stack_child_removed (GtkContainer     *container,
                        GtkWidget        *widget,
                        HdySwitcher *self)
{
  remove_child (widget, self);
}

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
 * @switcher: a #HdySwitcher
 * @stack: (allow-none): a #GtkStack
 *
 * Sets the stack to control.
 *
 * Since: 3.10
 */
void
hdy_switcher_set_stack (HdySwitcher *switcher,
                              GtkStack         *stack)
{
  HdySwitcherPrivate *priv;

  g_return_if_fail (HDY_IS_SWITCHER (switcher));
  g_return_if_fail (GTK_IS_STACK (stack) || stack == NULL);

  priv = hdy_switcher_get_instance_private (switcher);

  if (priv->stack == stack)
    return;

  if (priv->stack)
    {
      disconnect_stack_signals (switcher);
      clear_switcher (switcher);
      g_clear_object (&priv->stack);
    }
  if (stack)
    {
      priv->stack = g_object_ref (stack);
      populate_switcher (switcher);
      connect_stack_signals (switcher);
    }

  gtk_widget_queue_resize (GTK_WIDGET (switcher));

  g_object_notify (G_OBJECT (switcher), "stack");
}

/**
 * hdy_switcher_get_stack:
 * @switcher: a #HdySwitcher
 *
 * Retrieves the stack.
 * See hdy_switcher_set_stack().
 *
 * Returns: (nullable) (transfer none): the stack, or %NULL if
 *    none has been set explicitly.
 *
 * Since: 3.10
 */
GtkStack *
hdy_switcher_get_stack (HdySwitcher *switcher)
{
  HdySwitcherPrivate *priv;
  g_return_val_if_fail (HDY_IS_SWITCHER (switcher), NULL);

  priv = hdy_switcher_get_instance_private (switcher);
  return priv->stack;
}

void
hdy_switcher_set_icon_size (HdySwitcher *switcher,
                                  gint              icon_size)
{
  HdySwitcherPrivate *priv;

  g_return_if_fail (HDY_IS_SWITCHER (switcher));

  priv = hdy_switcher_get_instance_private (switcher);

  if (icon_size != priv->icon_size)
    {
      priv->icon_size = icon_size;

      if (priv->stack != NULL)
        {
          clear_switcher (switcher);
          populate_switcher (switcher);
        }

      g_object_notify (G_OBJECT (switcher), "icon-size");
    }
}

static void
hdy_switcher_get_property (GObject      *object,
                                 guint         prop_id,
                                 GValue       *value,
                                 GParamSpec   *pspec)
{
  HdySwitcher *switcher = HDY_SWITCHER (object);
  HdySwitcherPrivate *priv;

  priv = hdy_switcher_get_instance_private (switcher);
  switch (prop_id)
    {
    case PROP_ICON_SIZE:
      g_value_set_int (value, priv->icon_size);
      break;

    case PROP_STACK:
      g_value_set_object (value, priv->stack);
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
  HdySwitcher *switcher = HDY_SWITCHER (object);

  switch (prop_id)
    {
    case PROP_ICON_SIZE:
      hdy_switcher_set_icon_size (switcher, g_value_get_int (value));
      break;

    case PROP_STACK:
      hdy_switcher_set_stack (switcher, g_value_get_object (value));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static void
hdy_switcher_dispose (GObject *object)
{
  HdySwitcher *switcher = HDY_SWITCHER (object);

  remove_switch_timer (switcher);
  hdy_switcher_set_stack (switcher, NULL);

  G_OBJECT_CLASS (hdy_switcher_parent_class)->dispose (object);
}

static void
hdy_switcher_finalize (GObject *object)
{
  HdySwitcher *switcher = HDY_SWITCHER (object);
  HdySwitcherPrivate *priv;

  priv = hdy_switcher_get_instance_private (switcher);

  g_hash_table_destroy (priv->buttons);

  G_OBJECT_CLASS (hdy_switcher_parent_class)->finalize (object);
}

static void
hdy_switcher_class_init (HdySwitcherClass *class)
{
  GObjectClass *object_class = G_OBJECT_CLASS (class);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (class);

  object_class->get_property = hdy_switcher_get_property;
  object_class->set_property = hdy_switcher_set_property;
  object_class->dispose = hdy_switcher_dispose;
  object_class->finalize = hdy_switcher_finalize;

  widget_class->drag_motion = hdy_switcher_drag_motion;
  widget_class->drag_leave = hdy_switcher_drag_leave;

  /**
   * HdySwitcher:icon-size:
   *
   * Use the "icon-size" property to change the size of the image displayed
   * when a #HdySwitcher is displaying icons.
   *
   * Since: 3.20
   */
  g_object_class_install_property (object_class,
                                   PROP_ICON_SIZE,
                                   g_param_spec_int ("icon-size",
                                                     _("Icon Size"),
                                                     _("Symbolic size to use for named icon"),
                                                     0, G_MAXINT,
                                                     GTK_ICON_SIZE_MENU,
                                                     G_PARAM_EXPLICIT_NOTIFY |
                                                     G_PARAM_READWRITE |
                                                     G_PARAM_STATIC_STRINGS));

  g_object_class_install_property (object_class,
                                   PROP_STACK,
                                   g_param_spec_object ("stack",
                                                        _("Stack"),
                                                        _("Stack"),
                                                        GTK_TYPE_STACK,
                                                        G_PARAM_READWRITE |
                                                        G_PARAM_CONSTRUCT |
                                                        G_PARAM_STATIC_STRINGS));

  gtk_widget_class_set_css_name (widget_class, "hdyswitcher");
}

/**
 * hdy_switcher_new:
 *
 * Create a #HdySwitcher with #GtkWindow:transient-for set to parent
 *
 * C Usage
 * |[<!-- language="C" -->
 * GtkWidget *switcher = hdy_switcher_new ();
 * hdy_switcher_set_stack (HDY_SWITCHER (switcher), GTK_STACK (stack));
 * ]|
 * 
 * Vala Usage
 * |[<!-- language="Vala" -->
 * var switcher = new Hdy.Switcher ();
 * switcher.stack = stack;
 * ]|
 * 
 * Python Usage
 * |[<!-- language="Python" -->
 * switcher = Handy.Switcher ()
 * switcher.props.stack = stack
 * ]|
 *
 * Since: 0.0.8
 */
GtkWidget *
hdy_switcher_new (void)
{
  return g_object_new (HDY_TYPE_SWITCHER, NULL);
}
