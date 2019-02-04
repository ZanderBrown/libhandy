/*
 * Copyright © 2019 Zander Brown <zbrown@gnome.org>
 *
 * SPDX-License-Identifier: LGPL-2.1+
 */

#pragma once

#if !defined(_HANDY_INSIDE) && !defined(HANDY_COMPILATION)
#error "Only <handy.h> can be included directly."
#endif

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define HDY_TYPE_SWITCHER hdy_switcher_get_type ()
G_DECLARE_INTERFACE (HdySwitcher, hdy_switcher, HDY, SWITCHER, GObject)

struct _HdySwitcherInterface
{
  GTypeInterface parent_iface;
};

void        hdy_switcher_set_icon_size (HdySwitcher *self,
                                        GtkIconSize  icon_size);
GtkIconSize hdy_switcher_get_icon_size (HdySwitcher *self);
void        hdy_switcher_set_stack     (HdySwitcher *self,
                                        GtkStack    *stack);
GtkStack   *hdy_switcher_get_stack     (HdySwitcher *self);

G_END_DECLS
