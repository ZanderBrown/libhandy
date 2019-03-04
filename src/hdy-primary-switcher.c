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
 * An applications main switcher. Intended to be used in a #GtkHeaderBar with
 * gtk_header_bar_set_custom_title()
 * 
 * C Usage
 * |[<!-- language="C" -->
 * GtkWidget *hb, *stack, *switcher;
 * 
 * hb = gtk_header_bar_new ();
 * stack = gtk_stack_new ();
 * switcher = hdy_primary_switcher_new ();
 * gtk_header_bar_set_custom_title (GTK_HEADER_BAR (hb), switcher);
 * hdy_switcher_set_stack (HDY_SWITCHER (switcher), GTK_STACK (stack));
 * ]|
 * 
 * Vala Usage
 * |[<!-- language="Vala" -->
 * var hb = new Gtk.HeaderBar ();
 * var stack = new Gtk.Stack ();
 * var switcher = new Hdy.PrimarySwitcher ();
 * hb.custom_title = switcher;
 * switcher.stack = stack;
 * ]|
 * 
 * Python Usage
 * |[<!-- language="Python" -->
 * hb = Gtk.HeaderBar ()
 * stack = Gtk.Stack ()
 * switcher = Handy.PrimarySwitcher ()
 * hb.props.custom_title = switcher
 * switcher.props.stack = stack
 * ]|
 * 
 * Design Information: [GitLab Issue](https://source.puri.sm/Librem5/libhandy/issues/64)
 * 
 * Since: 0.0.9
 */


typedef struct {
  GtkWidget            *switcher;
  GtkWidget            *label;
  HdySecondarySwitcher *secondary;
  gboolean              secondary_active;
  gulong                size_handler;
  GBinding             *secondary_active_binding;
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
  priv->size_handler = 0;

  /* The switcher and label are different widths but should
   * act as if they have the same height */
  gtk_stack_set_hhomogeneous (GTK_STACK (self), FALSE);
  gtk_stack_set_vhomogeneous (GTK_STACK (self), TRUE);
  gtk_widget_set_halign (GTK_WIDGET (self), GTK_ALIGN_CENTER);
  
  /* The 'real' switcher */
  priv->switcher = hdy_switcher_bar_new ();
  gtk_widget_show (priv->switcher);
  gtk_stack_add_named (GTK_STACK (self), priv->switcher, "switcher");

  /* Label used for mobile view */
  priv->label = gtk_label_new (NULL);
  context = gtk_widget_get_style_context (priv->label);
  gtk_style_context_add_class (context, "title");
  gtk_widget_set_halign (priv->label, GTK_ALIGN_CENTER);
  gtk_widget_set_valign (priv->label, GTK_ALIGN_CENTER);
  gtk_label_set_ellipsize (GTK_LABEL (priv->label), PANGO_ELLIPSIZE_END);
  gtk_widget_show (priv->label);
  gtk_stack_add_named (GTK_STACK (self), priv->label, "label");

  /* We use the switcher by default */
  gtk_stack_set_visible_child (GTK_STACK (self), priv->switcher);

  gtk_widget_set_focus_on_click (GTK_WIDGET (self), FALSE);
}

/* The secondary switcher is lost, presumably it was destroyed */
static void
lost_secondary_cb (gpointer data,
                   GObject *where_the_object_was)
{
  HdyPrimarySwitcherPrivate *priv;

  g_return_if_fail (HDY_IS_PRIMARY_SWITCHER (data));
  
  priv = hdy_primary_switcher_get_instance_private (HDY_PRIMARY_SWITCHER (data));

  priv->secondary = NULL;
  g_clear_object (&priv->secondary_active_binding);
  hdy_primary_switcher_set_secondary_active (HDY_PRIMARY_SWITCHER (data), FALSE);
}

static void
hdy_primary_switcher_dispose (GObject *object)
{
  HdyPrimarySwitcher *self = HDY_PRIMARY_SWITCHER (object);

  /* Release the secondary switcher */
  hdy_primary_switcher_set_secondary (self, NULL);

  G_OBJECT_CLASS (hdy_primary_switcher_parent_class)->dispose (object);
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
      /* Pass though to the switcher */
      g_object_get_property (G_OBJECT (priv->switcher), "icon-size", value);
      break;

    case PROP_STACK:
      /* Pass though to the switcher */
      g_object_get_property (G_OBJECT (priv->switcher), "stack", value);
      break;

    case PROP_SECONDARY:
      g_value_set_object (value, hdy_primary_switcher_get_secondary (self));
      break;

    case PROP_SECONDARY_ACTIVE:
      g_value_set_boolean (value, hdy_primary_switcher_get_secondary_active (self));
      break;

    case PROP_TITLE:
      g_value_set_string (value, hdy_primary_switcher_get_title (self));
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
      hdy_primary_switcher_set_secondary_active (self, g_value_get_boolean (value));
      break;

    case PROP_TITLE:
      hdy_primary_switcher_set_title (self, g_value_get_string (value));
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

  object_class->dispose = hdy_primary_switcher_dispose;
  object_class->get_property = hdy_primary_switcher_get_property;
  object_class->set_property = hdy_primary_switcher_set_property;

  /**
   * HdyPrimarySwitcher:title:
   *
   * Title shown in mobile view
   * 
   * See: hdy_primary_switcher_set_title()
   *
   * Since: 0.0.9
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
   * The #HdySecondarySwitcher to use
   * 
   * See: hdy_primary_switcher_set_secondary()
   *
   * Since: 0.0.9
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
   * Indicates the secondary switcher is active
   * 
   * You shouldn't set this directly but you can observe it with #GObject::notify
   *
   * Since: 0.0.9
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

/**
 * hdy_primary_switcher_set_title:
 * @self: a #HdyPrimarySwitcher
 * @title: the new title
 *
 * Set the title used when #HdyPrimarySwitcher:secondary is in use
 * 
 * Generally this should be in sync with the title of your #GtkWindow/#GtkHeaderBar
 *
 * Since: 0.0.9
 */
void
hdy_primary_switcher_set_title (HdyPrimarySwitcher *self,
                                const gchar        *title)
{
  HdyPrimarySwitcherPrivate *priv;

  g_return_if_fail (HDY_IS_PRIMARY_SWITCHER (self));
  
  priv = hdy_primary_switcher_get_instance_private (self);

  /* Set the contents of the label */
  g_object_set (priv->label,
                "label", title,
                NULL);

  g_object_notify (G_OBJECT (self), "title");
}

/**
 * hdy_primary_switcher_get_title:
 * @self: a #HdyPrimarySwitcher
 *
 * Get the title used when #HdyPrimarySwitcher:secondary is in use
 *
 * See: hdy_primary_switcher_set_title()
 * 
 * Returns: (nullable) (transfer none): the title, or %NULL if none has been set
 *
 * Since: 0.0.9
 */
gchar *
hdy_primary_switcher_get_title (HdyPrimarySwitcher *self)
{
  HdyPrimarySwitcherPrivate *priv;
  gchar *title;

  g_return_val_if_fail (HDY_IS_PRIMARY_SWITCHER (self), NULL);
  
  priv = hdy_primary_switcher_get_instance_private (self);

  /* Get the contents of the label */
  g_object_get (priv->label,
                "label", &title,
                NULL);

  return title;
}

/* size of secondary switcher (potentially) changed */
static void
secondary_allocate_cb (GtkWidget          *secondary,
                       GdkRectangle       *allocation,
                       HdyPrimarySwitcher *self)
{
  g_return_if_fail (HDY_IS_PRIMARY_SWITCHER (self));
  
  /* If the size is less/equal to 400 switch to secondary */
  g_object_set (self,
                "secondary-active", allocation->width <= 400,
                NULL);
}

/**
 * hdy_primary_switcher_set_secondary:
 * @self: a #HdyPrimarySwitcher
 * @secondary: the #HdySecondarySwitcher for mobile view
 *
 * Set the #HdySecondarySwitcher this switcher will defer to in mobile view
 * 
 * For best results the switcher should have a #HdyPrimarySwitcher:title set
 * 
 * Since: 0.0.9
 */
void
hdy_primary_switcher_set_secondary (HdyPrimarySwitcher   *self,
                                    HdySecondarySwitcher *secondary)
{
  HdyPrimarySwitcherPrivate *priv;

  g_return_if_fail (HDY_IS_PRIMARY_SWITCHER (self));
  g_return_if_fail (HDY_IS_SECONDARY_SWITCHER (secondary) || secondary == NULL);
  
  priv = hdy_primary_switcher_get_instance_private (self);

  /* Ignore attemts to set the current switcher */
  if (priv->secondary == secondary)
    return;

  /* Disconnect the old one */
  if (priv->secondary) {
    g_signal_handler_disconnect (G_OBJECT (priv->secondary), priv->size_handler);
    g_object_weak_unref (G_OBJECT (priv->secondary), lost_secondary_cb, self);
    g_clear_object (&priv->secondary_active_binding);
    priv->secondary = NULL;
  }

  /* Connect the new one */
  if (secondary) {
    priv->secondary = secondary;
    g_object_weak_ref (G_OBJECT (priv->secondary), lost_secondary_cb, self);

    /* HACK: Make mobile size switch based on the size of secondary not primary */
    priv->size_handler = g_signal_connect (priv->secondary, "size-allocate",
                                           G_CALLBACK (secondary_allocate_cb),
                                           self);

    /* Bind 'active' of the secondary switcher with 'secondary-active' of this one */
    priv->secondary_active_binding = g_object_bind_property (self,
                                                             "secondary-active",
                                                             priv->secondary,
                                                             "active",
                                                             G_BINDING_SYNC_CREATE);
  }

  g_object_notify (G_OBJECT (self), "secondary");
}

/**
 * hdy_primary_switcher_get_secondary:
 * @self: a #HdyPrimarySwitcher
 *
 * Get the #HdySecondarySwitcher used for mobile views
 *
 * See: hdy_primary_switcher_set_secondary()
 * 
 * Returns: (nullable) (transfer none): the #HdySecondarySwitcher, or %NULL if none has been set
 *
 * Since: 0.0.9
 */
HdySecondarySwitcher *
hdy_primary_switcher_get_secondary (HdyPrimarySwitcher *self)
{
  HdyPrimarySwitcherPrivate *priv;

  g_return_val_if_fail (HDY_IS_PRIMARY_SWITCHER (self), NULL);
  
  priv = hdy_primary_switcher_get_instance_private (self);

  return priv->secondary;
}

/**
 * hdy_primary_switcher_set_secondary_active:
 * @self: a #HdyPrimarySwitcher
 * @active: state of the secondary switcher
 *
 * This is included for completeness, you almost certainly shouldn't be
 * using this function
 * 
 * See: hdy_primary_switcher_get_secondary_active()
 * 
 * Since: 0.0.9
 */
void
hdy_primary_switcher_set_secondary_active (HdyPrimarySwitcher *self,
                                           gboolean            active)
{
  HdyPrimarySwitcherPrivate *priv;

  g_return_if_fail (HDY_IS_PRIMARY_SWITCHER (self));
  
  priv = hdy_primary_switcher_get_instance_private (self);

  /* Ignore change if it's actually the current state */
  if (priv->secondary_active == active)
    return;

  priv->secondary_active = active;
  if (active) {
    gtk_stack_set_visible_child (GTK_STACK (self), priv->label);
  } else {
    gtk_stack_set_visible_child (GTK_STACK (self), priv->switcher);
  }

  g_object_notify (G_OBJECT (self), "secondary-active");
}

/**
 * hdy_primary_switcher_get_secondary_active:
 * @self: a #HdyPrimarySwitcher
 *
 * Get the #HdySecondartSwitcher:active state
 *
 * Returns: %TRUE if the secondary is active
 *
 * Since: 0.0.9
 */
gboolean
hdy_primary_switcher_get_secondary_active (HdyPrimarySwitcher *self)
{
  HdyPrimarySwitcherPrivate *priv;

  g_return_val_if_fail (HDY_IS_PRIMARY_SWITCHER (self), FALSE);
  
  priv = hdy_primary_switcher_get_instance_private (self);

  return priv->secondary_active;
}

/**
 * hdy_primary_switcher_new:
 *
 * Create a #HdyPrimarySwitcher
 * 
 * Should be used with a #HdySecondarySwitcher (#HdyPrimarySwitcher:secondary) and
 * gernerally with #HdyPrimarySwitcher:title bound to #GtkWindow
 * #GtkWindow:title (g_object_bind_property())
 *
 * C Usage
 * |[<!-- language="C" -->
 * GtkWidget *switcher = hdy_primary_switcher_new ();
 * ]|
 * 
 * Vala Usage
 * |[<!-- language="Vala" -->
 * var switcher = new Hdy.PrimarySwitcher ();
 * ]|
 * 
 * Python Usage
 * |[<!-- language="Python" -->
 * switcher = Handy.PrimarySwitcher ()
 * ]|
 *
 * Since: 0.0.9
 */
GtkWidget *
hdy_primary_switcher_new (void)
{
  return g_object_new (HDY_TYPE_PRIMARY_SWITCHER, NULL);
}
