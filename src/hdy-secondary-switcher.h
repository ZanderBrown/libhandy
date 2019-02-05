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

G_BEGIN_DECLS

#define HDY_TYPE_SECONDARY_SWITCHER (hdy_secondary_switcher_get_type())

struct _HdySecondarySwitcherClass {
  GtkBinClass parent_class;
};

G_DECLARE_DERIVABLE_TYPE (HdySecondarySwitcher, hdy_secondary_switcher, HDY, SECONDARY_SWITCHER, GtkBin)

GtkWidget *hdy_secondary_switcher_new (void);

G_END_DECLS
