/*
 * Copyright © 2019 Zander Brown <zbrown@gnome.org>
 *
 * SPDX-License-Identifier: LGPL-2.1+
 */

#include <glib/gi18n.h>

#include "hdy-switcher-button-private.h"

/**
 * SECTION:hdy-switcher-button-private
 * @short_description: Button used in #HdySwitcherBar
 * @title: HdySwitcherButton
 * @See_also: #HdySwitcherBar
 * @stability: Private
 * 
 * Only for internal use by #HdySwitcherBar
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

/**
 * hdy_switcher_button_get_size:
 * @self: a #HdySwitcherButton
 * @v_min_width: (inout): the minimum width when vertical
 * @h_min_width: (inout): the minimum width when horizontal
 * @h_nat_width: (inout): the natural width
 *
 * Since: 0.0.8
 */
void
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

/**
 * hdy_switcher_button_get_stack_child:
 * @self: a #HdySwitcherButton
 * 
 * Returns: (transfer none): the #GtkWidget page in the #GtkStack
 *
 * Since: 0.0.8
 */
GtkWidget *
hdy_switcher_button_get_stack_child (HdySwitcherButton *self)
{
  HdySwitcherButtonPrivate *priv;

  g_return_val_if_fail (HDY_IS_SWITCHER_BUTTON (self), NULL);

  priv = hdy_switcher_button_get_instance_private (self);

  return priv->stack_child;
}
