/* GTK - The GIMP Toolkit
 * Copyright (C) 1995-1997 Peter Mattis, Spencer Kimball and Josh MacDonald
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

/*
 * Modified by the GTK+ Team and others 1997-2000.  See the AUTHORS
 * file for a list of people on the GTK+ Team.  See the ChangeLog
 * files for a list of changes.  These files are distributed with
 * GTK+ at ftp://ftp.gtk.org/pub/gtk/.
 */
#include "config.h"
#include "private.h"
#include <string.h>
#include <glib.h>
#include <gtk/gtk.h>
#include <glib/gi18n-lib.h>
#include "mate-colorsel.h"
#include "mate-colorseldialog.h"

enum {
  PROP_0,
  PROP_COLOR_SELECTION,
  PROP_HELP_BUTTON
};

/***************************/
/* MateColorSelectionDialog */
/***************************/

static void mate_color_selection_dialog_buildable_interface_init     (GtkBuildableIface *iface);
static GObject * mate_color_selection_dialog_buildable_get_internal_child (GtkBuildable *buildable,
									  GtkBuilder   *builder,
									  const gchar  *childname);

G_DEFINE_TYPE_WITH_CODE (MateColorSelectionDialog, mate_color_selection_dialog,
           GTK_TYPE_DIALOG,
           G_IMPLEMENT_INTERFACE (GTK_TYPE_BUILDABLE,
                      mate_color_selection_dialog_buildable_interface_init))

static GtkBuildableIface *parent_buildable_iface;

static void
mate_color_selection_dialog_get_property (GObject         *object,
					 guint            prop_id,
					 GValue          *value,
					 GParamSpec      *pspec)
{
  MateColorSelectionDialog *colorsel;

  colorsel = MATE_COLOR_SELECTION_DIALOG (object);

  switch (prop_id)
    {
    case PROP_COLOR_SELECTION:
      g_value_set_object (value, colorsel->colorsel);
      break;
    case PROP_HELP_BUTTON:
      g_value_set_object (value, colorsel->help_button);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static void
mate_color_selection_dialog_class_init (MateColorSelectionDialogClass *klass)
{
  GObjectClass   *gobject_class = G_OBJECT_CLASS (klass);
  gobject_class->get_property = mate_color_selection_dialog_get_property;

  g_object_class_install_property (gobject_class,
				   PROP_COLOR_SELECTION,
				   g_param_spec_object ("color-selection",
						     _("Color Selection"),
						     _("The color selection embedded in the dialog."),
						     GTK_TYPE_WIDGET,
						     G_PARAM_READABLE));
  g_object_class_install_property (gobject_class,
				   PROP_HELP_BUTTON,
				   g_param_spec_object ("help-button",
						     _("Help Button"),
						     _("The help button of the dialog."),
						     GTK_TYPE_WIDGET,
						     G_PARAM_READABLE));
}

static GtkWidget*
mate_add_dialog_button (GtkDialog      *dialog,
                        const char     *label,
                        const char     *icon,
                        GtkResponseType response)
{
    GtkWidget *widget;
    GtkWidget *image;

    widget = gtk_button_new_with_mnemonic (label);
    image = gtk_image_new_from_icon_name (icon, GTK_ICON_SIZE_BUTTON);
    gtk_button_set_image (GTK_BUTTON (widget), image);
    gtk_dialog_add_action_widget (dialog, widget, response);

    return widget;
}

static void
mate_color_selection_dialog_init (MateColorSelectionDialog *colorseldiag)
{
  GtkDialog *dialog = GTK_DIALOG (colorseldiag);

  _mate_desktop_init_i18n ();

  gtk_container_set_border_width (GTK_CONTAINER (dialog), 4);
  gtk_box_set_spacing (GTK_BOX (gtk_dialog_get_content_area (dialog)), 6);

  colorseldiag->colorsel = mate_color_selection_new ();
  gtk_container_set_border_width (GTK_CONTAINER (colorseldiag->colorsel), 2);
  mate_color_selection_set_has_palette (MATE_COLOR_SELECTION(colorseldiag->colorsel), FALSE);
  mate_color_selection_set_has_opacity_control (MATE_COLOR_SELECTION(colorseldiag->colorsel), FALSE);
  gtk_container_add (GTK_CONTAINER (gtk_dialog_get_content_area (GTK_DIALOG (colorseldiag))), colorseldiag->colorsel);
  gtk_widget_show (colorseldiag->colorsel);

  colorseldiag->help_button = mate_add_dialog_button (GTK_DIALOG (colorseldiag),
                                                      _("_About"),
                                                      "mate-desktop",
                                                      GTK_RESPONSE_HELP);

  gtk_widget_hide (colorseldiag->help_button);

  gtk_window_set_title (GTK_WINDOW (colorseldiag),
                        _("Color Selection"));
}

GtkWidget*
mate_color_selection_dialog_new (const gchar *title)
{
  MateColorSelectionDialog *colorseldiag;

  colorseldiag = g_object_new (MATE_TYPE_COLOR_SELECTION_DIALOG, NULL);

  if (title)
    gtk_window_set_title (GTK_WINDOW (colorseldiag), title);

  gtk_window_set_resizable (GTK_WINDOW (colorseldiag), FALSE);

  return GTK_WIDGET (colorseldiag);
}

/**
 * mate_color_selection_dialog_get_color_selection:
 * @colorsel: a #MateColorSelectionDialog
 *
 * Retrieves the #MateColorSelection widget embedded in the dialog.
 *
 * Returns: (transfer none): the embedded #MateColorSelection
 *
 * Since: 1.9.1
 **/
GtkWidget*
mate_color_selection_dialog_get_color_selection (MateColorSelectionDialog *colorsel)
{
  g_return_val_if_fail (MATE_IS_COLOR_SELECTION_DIALOG (colorsel), NULL);

  return colorsel->colorsel;
}

static void
mate_color_selection_dialog_buildable_interface_init (GtkBuildableIface *iface)
{
  parent_buildable_iface = g_type_interface_peek_parent (iface);
  iface->get_internal_child = mate_color_selection_dialog_buildable_get_internal_child;
}

static GObject *
mate_color_selection_dialog_buildable_get_internal_child (GtkBuildable *buildable,
							 GtkBuilder   *builder,
							 const gchar  *childname)
{
    if (strcmp(childname, "help_button") == 0)
	return G_OBJECT (MATE_COLOR_SELECTION_DIALOG(buildable)->help_button);
    else if (strcmp(childname, "color_selection") == 0)
	return G_OBJECT (MATE_COLOR_SELECTION_DIALOG(buildable)->colorsel);

    return parent_buildable_iface->get_internal_child (buildable, builder, childname);
}
