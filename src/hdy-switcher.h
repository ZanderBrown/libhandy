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


#define HDY_TYPE_SWITCHER_BUTTON (hdy_switcher_button_get_type())

struct _HdySwitcherButtonClass {
  GtkRadioButtonClass parent_class;
};

G_DECLARE_DERIVABLE_TYPE (HdySwitcherButton, hdy_switcher_button, HDY, SWITCHER_BUTTON, GtkRadioButton)


#define HDY_TYPE_SWITCHER_BAR (hdy_switcher_bar_get_type())

struct _HdySwitcherBarClass {
  GtkBoxClass parent_class;
};

G_DECLARE_DERIVABLE_TYPE (HdySwitcherBar, hdy_switcher_bar, HDY, SWITCHER_BAR, GtkBox)

GtkWidget  *hdy_switcher_bar_new           (void);


#define HDY_TYPE_PRIMARY_SWITCHER (hdy_primary_switcher_get_type())

struct _HdyPrimarySwitcherClass {
  GtkStackClass parent_class;
};

G_DECLARE_DERIVABLE_TYPE (HdyPrimarySwitcher, hdy_primary_switcher, HDY, PRIMARY_SWITCHER, GtkStack)

GtkWidget      *hdy_primary_switcher_new           (void);
void            hdy_primary_switcher_set_secondary (HdyPrimarySwitcher *self,
                                                    HdySwitcherBar     *secondary);
HdySwitcherBar *hdy_primary_switcher_get_secondary (HdyPrimarySwitcher *self);


G_END_DECLS
