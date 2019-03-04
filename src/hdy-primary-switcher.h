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

#include "hdy-switcher-bar.h"
#include "hdy-secondary-switcher.h"

G_BEGIN_DECLS

#define HDY_TYPE_PRIMARY_SWITCHER (hdy_primary_switcher_get_type())

struct _HdyPrimarySwitcherClass {
  GtkStackClass parent_class;
};

G_DECLARE_DERIVABLE_TYPE (HdyPrimarySwitcher, hdy_primary_switcher, HDY, PRIMARY_SWITCHER, GtkStack)

GtkWidget            *hdy_primary_switcher_new                  (void);
void                  hdy_primary_switcher_set_title            (HdyPrimarySwitcher   *self,
                                                                 const gchar          *title);
gchar                *hdy_primary_switcher_get_title            (HdyPrimarySwitcher   *self);
void                  hdy_primary_switcher_set_secondary        (HdyPrimarySwitcher   *self,
                                                                 HdySecondarySwitcher *secondary);
HdySecondarySwitcher *hdy_primary_switcher_get_secondary        (HdyPrimarySwitcher   *self);
void                  hdy_primary_switcher_set_secondary_active (HdyPrimarySwitcher   *self,
                                                                 gboolean              active);
gboolean              hdy_primary_switcher_get_secondary_active (HdyPrimarySwitcher   *self);

G_END_DECLS
