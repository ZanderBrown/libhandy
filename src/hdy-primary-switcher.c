/*
 * Copyright © 2019 Zander Brown <zbrown@gnome.org>
 * 
 * SPDX-License-Identifier: LGPL-2.1+
 */

#include <glib/gi18n.h>

#include "hdy-switcher.h"
#include "hdy-primary-switcher.h"

/**
 * SECTION:hdy-primary-switcher
 * @short_description: An adaptive switcher
 * @title: HdyPrimarySwitcher
 * 
 * An alternative #GtkStackSwitcher
 * 
 * C Usage
 * |[<!-- language="C" -->
 * GtkWidget *stack, *switcher;
 * 
 * stack = gtk_stack_new ();
 * switcher = hdy_primary_switcher_new ();
 * hdy_switcher_set_stack (HDY_SWITCHER (switcher), GTK_STACK (stack));
 * ]|
 * 
 * Vala Usage
 * |[<!-- language="Vala" -->
 * var stack = new Gtk.Stack ();
 * var switcher = new Hdy.PrimarySwitcher ();
 * switcher.stack = stack;
 * ]|
 * 
 * Python Usage
 * |[<!-- language="Python" -->
 * stack = Gtk.Stack ()
 * switcher = Handy.PrimarySwitcher ()
 * switcher.props.stack = stack
 * ]|
 * 
 * Design Information: [GitLab Issue](https://source.puri.sm/Librem5/libhandy/issues/64)
 * 
 * Since: 0.0.8
 */


typedef struct {
  GtkWidget            *switcher;
  GtkWidget            *label;
  HdySecondarySwitcher *secondary;
  gboolean              secondary_active;
} HdyPrimarySwitcherPrivate;

enum {
  PROP_0,
  PROP_ICON_SIZE,
  PROP_STACK,
  PROP_SECONDARY,
  PROP_SECONDARY_ACTIVE,
  PROP_TITLE,
};

G_DEFINE_TYPE_WITH_CODE (HdyPrimarySwitcher, hdy_primary_switcher, GTK_TYPE_STACK,
                         G_ADD_PRIVATE (HdyPrimarySwitcher)
                         G_IMPLEMENT_INTERFACE (HDY_TYPE_SWITCHER, NULL))

static void
hdy_primary_switcher_init (HdyPrimarySwitcher *self)
{
  HdyPrimarySwitcherPrivate *priv;
  GtkStyleContext *context;

  priv = hdy_primary_switcher_get_instance_private (self);

  priv->secondary_active = FALSE;

  gtk_stack_set_hhomogeneous (GTK_STACK (self), FALSE);
  gtk_stack_set_vhomogeneous (GTK_STACK (self), FALSE);
  gtk_widget_set_halign (GTK_WIDGET (self), GTK_ALIGN_CENTER);
  
  priv->switcher = hdy_switcher_bar_new ();
  gtk_widget_show (priv->switcher);
  gtk_stack_add_named (GTK_STACK (self), priv->switcher, "switcher");

  priv->label = gtk_label_new (NULL);
  context = gtk_widget_get_style_context (priv->label);
  gtk_style_context_add_class (context, "title");
  gtk_widget_set_halign (priv->label, GTK_ALIGN_CENTER);
  gtk_widget_set_valign (priv->label, GTK_ALIGN_CENTER);
  gtk_label_set_ellipsize (GTK_LABEL (priv->label), PANGO_ELLIPSIZE_END);
  g_object_bind_property (self, "title", priv->label, "label", G_BINDING_SYNC_CREATE);
  gtk_widget_show (priv->label);

  gtk_stack_add_named (GTK_STACK (self), priv->label, "label");

  gtk_stack_set_visible_child (GTK_STACK (self), priv->switcher);

  gtk_widget_set_focus_on_click (GTK_WIDGET (self), FALSE);
}

static void
hdy_primary_switcher_get_property (GObject      *object,
                                  guint         prop_id,
                                  GValue       *value,
                                  GParamSpec   *pspec)
{
  HdyPrimarySwitcher *self = HDY_PRIMARY_SWITCHER (object);
  HdyPrimarySwitcherPrivate *priv = hdy_primary_switcher_get_instance_private (self);

  switch (prop_id) {
    case PROP_ICON_SIZE:
      g_object_get_property (G_OBJECT (priv->switcher), "icon-size", value);
      break;

    case PROP_STACK:
      g_object_get_property (G_OBJECT (priv->switcher), "stack", value);
      break;

    case PROP_SECONDARY:
      g_value_set_object (value, hdy_primary_switcher_get_secondary (self));
      break;

    case PROP_SECONDARY_ACTIVE:
      g_value_set_boolean (value, priv->secondary_active);
      break;

    case PROP_TITLE:
      g_object_get_property (G_OBJECT (priv->label), "label", value);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static void
hdy_primary_switcher_set_property (GObject      *object,
                                   guint         prop_id,
                                   const GValue *value,
                                   GParamSpec   *pspec)
{
  HdyPrimarySwitcher *self = HDY_PRIMARY_SWITCHER (object);
  HdyPrimarySwitcherPrivate *priv = hdy_primary_switcher_get_instance_private (self);

  switch (prop_id) {
    case PROP_ICON_SIZE:
      g_object_set_property (G_OBJECT (priv->switcher), "icon-size", value);
      break;

    case PROP_STACK:
      g_object_set_property (G_OBJECT (priv->switcher), "stack", value);
      break;

    case PROP_SECONDARY:
      hdy_primary_switcher_set_secondary (self, g_value_get_object (value));
      break;

    case PROP_SECONDARY_ACTIVE:
      priv->secondary_active = g_value_get_boolean (value);
      if (priv->secondary_active) {
        gtk_stack_set_visible_child (GTK_STACK (self), priv->label);
      } else {
        gtk_stack_set_visible_child (GTK_STACK (self), priv->switcher);
      }
      break;

    case PROP_TITLE:
      g_object_set_property (G_OBJECT (priv->label), "label", value);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static void
hdy_primary_switcher_class_init (HdyPrimarySwitcherClass *klass)
{
  GObjectClass   *object_class = G_OBJECT_CLASS (klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  object_class->get_property = hdy_primary_switcher_get_property;
  object_class->set_property = hdy_primary_switcher_set_property;

  /**
   * HdyPrimarySwitcher:title:
   *
   *
   * Since: 0.0.8
   */
  g_object_class_install_property (object_class,
                                   PROP_TITLE,
                                   g_param_spec_string ("title",
                                                        _("Title"),
                                                        _("Title to show when collapsed"),
                                                        NULL,
                                                        G_PARAM_READWRITE |
                                                        G_PARAM_STATIC_STRINGS));

  /**
   * HdyPrimarySwitcher:secondary:
   *
   *
   * Since: 0.0.8
   */
  g_object_class_install_property (object_class,
                                   PROP_SECONDARY,
                                   g_param_spec_object ("secondary",
                                                        _("Secondary"),
                                                        _("Alternative switcher"),
                                                        HDY_TYPE_SECONDARY_SWITCHER,
                                                        G_PARAM_EXPLICIT_NOTIFY |
                                                        G_PARAM_READWRITE |
                                                        G_PARAM_STATIC_STRINGS));

  /**
   * HdyPrimarySwitcher:secondary-active:
   *
   *
   * Since: 0.0.8
   */
  g_object_class_install_property (object_class,
                                   PROP_SECONDARY_ACTIVE,
                                   g_param_spec_boolean ("secondary-active",
                                                        _("Secondary Active"),
                                                        _("Secondary switcher in use"),
                                                        FALSE,
                                                        G_PARAM_READWRITE |
                                                        G_PARAM_STATIC_STRINGS));

  g_object_class_override_property (object_class,
                                    PROP_ICON_SIZE,
                                    "icon-size");

  g_object_class_override_property (object_class,
                                    PROP_STACK,
                                    "stack");

  gtk_widget_class_set_css_name (widget_class, "hdyprimaryswitcher");
}

static void
secondary_allocate_cb (GtkWidget          *secondary,
                       GdkRectangle       *allocation,
                       HdyPrimarySwitcher *self)
{
  HdyPrimarySwitcherPrivate *priv;

  g_return_if_fail (HDY_IS_PRIMARY_SWITCHER (self));
  
  priv = hdy_primary_switcher_get_instance_private (self);

  g_object_set (self,
                "secondary-active", allocation->width <= 400,
                NULL);
}

void
hdy_primary_switcher_set_secondary (HdyPrimarySwitcher   *self,
                                    HdySecondarySwitcher *secondary)
{
  HdyPrimarySwitcherPrivate *priv;

  g_return_if_fail (HDY_IS_PRIMARY_SWITCHER (self));
  g_return_if_fail (HDY_IS_SECONDARY_SWITCHER (secondary) || secondary == NULL);
  
  priv = hdy_primary_switcher_get_instance_private (self);

  priv->secondary = secondary;

  if (priv->secondary) {
    g_signal_connect (priv->secondary, "size-allocate",
                      G_CALLBACK (secondary_allocate_cb), self);
    g_object_bind_property (self, "secondary-active", priv->secondary, "active", G_BINDING_SYNC_CREATE);
  }

  g_object_notify (G_OBJECT (self), "secondary");
}

/**
 * hdy_primary_switcher_get_secondary:
 * @self: a #HdyPrimarySwitcher
 *
 * Get the #GtkStack being controlled by the #HdySwitcher
 *
 * See: hdy_primary_switcher_set_secondary()
 * 
 * Returns: (nullable) (transfer none): the #HdySecondarySwitcher, or %NULL if none has been set
 *
 * Since: 0.0.8
 */
HdySecondarySwitcher *
hdy_primary_switcher_get_secondary (HdyPrimarySwitcher *self)
{
  HdyPrimarySwitcherPrivate *priv;

  g_return_val_if_fail (HDY_IS_PRIMARY_SWITCHER (self), NULL);
  
  priv = hdy_primary_switcher_get_instance_private (self);

  return priv->secondary;
}

GtkWidget *
hdy_primary_switcher_new (void)
{
  return g_object_new (HDY_TYPE_PRIMARY_SWITCHER, NULL);
}
