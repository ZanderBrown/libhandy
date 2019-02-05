/*
 * Copyright © 2019 Zander Brown <zbrown@gnome.org>
 * 
 * SPDX-License-Identifier: LGPL-2.1+
 */

#include <glib/gi18n.h>

#include "hdy-switcher.h"
#include "hdy-secondary-switcher.h"

/**
 * SECTION:hdy-secondary-switcher
 * @short_description: An adaptive switcher
 * @title: HdySecondarySwitcher
 * 
 * An alternative #GtkStackSwitcher
 * 
 * C Usage
 * |[<!-- language="C" -->
 * GtkWidget *stack, *switcher;
 * 
 * stack = gtk_stack_new ();
 * switcher = hdy_secondary_switcher_new ();
 * hdy_switcher_set_stack (HDY_SWITCHER (switcher), GTK_STACK (stack));
 * ]|
 * 
 * Vala Usage
 * |[<!-- language="Vala" -->
 * var stack = new Gtk.Stack ();
 * var switcher = new Hdy.SecondarySwitcher ();
 * switcher.stack = stack;
 * ]|
 * 
 * Python Usage
 * |[<!-- language="Python" -->
 * stack = Gtk.Stack ()
 * switcher = Handy.SecondarySwitcher ()
 * switcher.props.stack = stack
 * ]|
 * 
 * Design Information: [GitLab Issue](https://source.puri.sm/Librem5/libhandy/issues/64)
 * 
 * Since: 0.0.8
 */


typedef struct {
  GtkWidget *switcher;
  GtkWidget *wrap;
} HdySecondarySwitcherPrivate;

enum {
  PROP_0,
  PROP_ICON_SIZE,
  PROP_STACK,
  PROP_ACTIVE,
};

G_DEFINE_TYPE_WITH_CODE (HdySecondarySwitcher, hdy_secondary_switcher, GTK_TYPE_BIN,
                         G_ADD_PRIVATE (HdySecondarySwitcher)
                         G_IMPLEMENT_INTERFACE (HDY_TYPE_SWITCHER, NULL))

static void
hdy_secondary_switcher_init (HdySecondarySwitcher *self)
{
  HdySecondarySwitcherPrivate *priv;
  GtkStyleContext *context;

  priv = hdy_secondary_switcher_get_instance_private (self);

  context = gtk_widget_get_style_context (GTK_WIDGET (self));
  gtk_style_context_add_class (context, "hdy-secondary-switcher");

  priv->switcher = hdy_switcher_bar_new ();
  gtk_widget_show (priv->switcher);

  priv->wrap = gtk_action_bar_new ();
  gtk_widget_show (priv->wrap);
  gtk_action_bar_set_center_widget (GTK_ACTION_BAR (priv->wrap), priv->switcher);

  gtk_container_add (GTK_CONTAINER (self), priv->wrap);
}

static void
hdy_secondary_switcher_get_property (GObject      *object,
                                  guint         prop_id,
                                  GValue       *value,
                                  GParamSpec   *pspec)
{
  HdySecondarySwitcher *self = HDY_SECONDARY_SWITCHER (object);
  HdySecondarySwitcherPrivate *priv = hdy_secondary_switcher_get_instance_private (self);

  switch (prop_id) {
    case PROP_ICON_SIZE:
      g_object_get_property (G_OBJECT (priv->switcher), "icon-size", value);
      break;

    case PROP_STACK:
      g_object_get_property (G_OBJECT (priv->switcher), "stack", value);
      break;

    case PROP_ACTIVE:
      g_object_get_property (G_OBJECT (priv->wrap), "visible", value);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static void
hdy_secondary_switcher_set_property (GObject      *object,
                                   guint         prop_id,
                                   const GValue *value,
                                   GParamSpec   *pspec)
{
  HdySecondarySwitcher *self = HDY_SECONDARY_SWITCHER (object);
  HdySecondarySwitcherPrivate *priv = hdy_secondary_switcher_get_instance_private (self);

  switch (prop_id) {
    case PROP_ICON_SIZE:
      g_object_set_property (G_OBJECT (priv->switcher), "icon-size", value);
      break;

    case PROP_STACK:
      g_object_set_property (G_OBJECT (priv->switcher), "stack", value);
      break;

    case PROP_ACTIVE:
      g_object_set_property (G_OBJECT (priv->wrap), "visible", value);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static void
hdy_secondary_switcher_class_init (HdySecondarySwitcherClass *klass)
{
  GObjectClass   *object_class = G_OBJECT_CLASS (klass);

  object_class->get_property = hdy_secondary_switcher_get_property;
  object_class->set_property = hdy_secondary_switcher_set_property;

  g_object_class_override_property (object_class,
                                    PROP_ICON_SIZE,
                                    "icon-size");

  g_object_class_override_property (object_class,
                                    PROP_STACK,
                                    "stack");

  /**
   * HdySecondarySwitcher:active:
   *
   *
   * Since: 0.0.8
   */
  g_object_class_install_property (object_class,
                                   PROP_ACTIVE,
                                   g_param_spec_boolean ("active",
                                                        _("Active"),
                                                        _("Switcher in use"),
                                                        FALSE,
                                                        G_PARAM_READWRITE |
                                                        G_PARAM_STATIC_STRINGS));
}

/**
 * hdy_secondary_switcher_new:
 *
 * Create a #HdySecondarySwitcher with #GtkWindow:transient-for set to parent
 *
 * C Usage
 * |[<!-- language="C" -->
 * GtkWidget *switcher = hdy_secondary_switcher_new ();
 * ]|
 * 
 * Vala Usage
 * |[<!-- language="Vala" -->
 * var switcher = new Hdy.SecondarySwitcher ();
 * ]|
 * 
 * Python Usage
 * |[<!-- language="Python" -->
 * switcher = Handy.SecondarySwitcher ()
 * ]|
 *
 * Since: 0.0.8
 */
GtkWidget *
hdy_secondary_switcher_new (void)
{
  return g_object_new (HDY_TYPE_SECONDARY_SWITCHER, NULL);
}
