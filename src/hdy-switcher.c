/*
 * Copyright © 2019 Zander Brown <zbrown@gnome.org>
 * 
 * SPDX-License-Identifier: LGPL-2.1+
 */

#include <glib/gi18n.h>

#include "hdy-switcher.h"

G_DEFINE_INTERFACE (HdySwitcher, hdy_switcher, G_TYPE_OBJECT)


/**
 * SECTION:hdy-switcher
 * @short_description: An adaptive switcher
 * @title: HdySwitcher
 * @See_also: #HdyPrimarySwitcher, #HdySwitcherBar
 * 
 * Something that can control a #GtkStack
 * 
 * Design Information: [GitLab Issue](https://source.puri.sm/Librem5/libhandy/issues/64)
 * 
 * Since: 0.0.8
 */

static void
hdy_switcher_default_init (HdySwitcherInterface *iface)
{
  /**
   * HdySwitcher:icon-size:
   *
   * Use the "icon-size" property to change the size of the images
   *
   * Since: 0.0.8
   */
  g_object_interface_install_property (iface,
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
   * The #GtkStack the #HdySwitcherBar controls
   *
   * Since: 0.0.8
   */
  g_object_interface_install_property (iface,
                                       g_param_spec_object ("stack",
                                                            _("Stack"),
                                                            _("Stack"),
                                                            GTK_TYPE_STACK,
                                                            G_PARAM_READWRITE |
                                                            G_PARAM_CONSTRUCT |
                                                            G_PARAM_STATIC_STRINGS));
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
 * hdy_switcher_set_icon_size (HDY_SWITCHER_BAR (switcher), GTK_ICON_SIZE_BUTTON);
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
  g_return_if_fail (HDY_IS_SWITCHER (self));

  g_object_set (self,
                "icon-size", &icon_size,
                NULL);
}

/**
 * hdy_switcher_get_icon_size:
 * @self: a #HdySwitcher
 *
 * Get the icon size of the images used in the #HdySwitcher
 * 
 * See: hdy_switcher_set_icon_size()
 *
 * Since: 0.0.8
 */
GtkIconSize
hdy_switcher_get_icon_size (HdySwitcher *self)
{
  GtkIconSize size;

  g_return_val_if_fail (HDY_IS_SWITCHER (self), GTK_ICON_SIZE_BUTTON);

  g_object_get (self,
                "icon-size", &size,
                NULL);

  return size;
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
 * hdy_switcher_set_stack (HDY_SWITCHER_BAR (switcher), GTK_STACK (stack));
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
  g_return_if_fail (HDY_IS_SWITCHER (self));

  g_object_set (self,
                "stack", &stack,
                NULL);
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
  GtkStack *stack;

  g_return_val_if_fail (HDY_IS_SWITCHER (self), NULL);
 
  g_object_get (self,
                "stack", &stack,
                NULL);

  return stack;
}
