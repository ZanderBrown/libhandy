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

#define HDY_TYPE_SWITCHER_BUTTON (hdy_switcher_button_get_type())

struct _HdySwitcherButtonClass {
  GtkRadioButtonClass parent_class;
};

G_DECLARE_DERIVABLE_TYPE (HdySwitcherButton, hdy_switcher_button, HDY, SWITCHER_BUTTON, GtkRadioButton)

#define HDY_TYPE_SWITCHER (hdy_switcher_get_type())

struct _HdySwitcherClass {
  GtkBoxClass parent_class;
};

G_DECLARE_DERIVABLE_TYPE (HdySwitcher, hdy_switcher, HDY, SWITCHER, GtkBox)

GtkWidget *hdy_switcher_new           (void);
void       hdy_switcher_set_icon_size (HdySwitcher *self,
                                       GtkIconSize  icon_size);
void       hdy_switcher_set_stack     (HdySwitcher *self,
                                       GtkStack    *stack);
GtkStack  *hdy_switcher_get_stack     (HdySwitcher *self);

G_END_DECLS
