/* BCD(Binary Coded Decimal) clock. 
 *
 * Copyright (C) 2009 Kristian Gunstone
 *
 * For playing with binary and gtk2.
 * Guess which one was a pain in the ass :P
 *
 * Docs used:
 * http://en.wikipedia.org/wiki/Binary_clock#Binary-coded_decimal_clocks
 * http://library.gnome.org/devel/gtk/unstable/GtkBox.html
 * http://www.gtk.org/tutorial1.2/gtk_tut-6.html [deprecated but useful]
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <stdio.h>
#include <time.h>
#include <gtk/gtk.h>

// XXX Should probably put these in a struct and pass to the cb via a gpointer.
GtkWidget *segment[20];
GtkWidget *time_label[3];

static unsigned char
dec_to_bin(unsigned char dec, const unsigned char bits) {
  
  unsigned char offset = bits;
  unsigned char bin = 0;

  while(dec > 0) {
    bin ^= (dec & 1) << offset;
    dec >>= 1;
    offset--;
  }

  return(bin);
}

gboolean 
update_clock(gpointer *data) {
  
  time_t t;
  struct tm *tm;
  unsigned char dt[6], bt[6];
  unsigned char i;

  t = time(NULL);
  tm = localtime(&t);

  // Split time
  dt[0] = tm->tm_hour % 10;
  dt[1] = (tm->tm_hour - dt[0]) / 10;

  dt[2] = tm->tm_min % 10;
  dt[3] = (tm->tm_min - dt[2]) / 10;

  dt[4] = tm->tm_sec % 10;
  dt[5] = (tm->tm_sec - dt[4]) / 10;

  // Translate single-digit decimals to binary
  bt[0] = dec_to_bin(dt[1], 2);
  bt[1] = dec_to_bin(dt[0], 4); 
  bt[2] = dec_to_bin(dt[3], 3);
  bt[3] = dec_to_bin(dt[2], 4);
  bt[4] = dec_to_bin(dt[5], 3);
  bt[5] = dec_to_bin(dt[4], 4); 

  // Toggle the switches

  // Hour
  for(i = 0; i < 2; i++)
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(segment[0 + i]), 
				 bt[0] >> (2 - i) & 1 ? TRUE : FALSE);
  for(i = 0; i < 4; i++)
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(segment[2 + i]), 
				 bt[1] >> (4 - i) & 1 ? TRUE : FALSE);
  // Minute 
  for(i = 0; i < 3; i++)
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(segment[6 + i]), 
				 bt[2] >> (3 - i) & 1 ? TRUE : FALSE);
  for(i = 0; i < 4; i++)
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(segment[9 + i]), 
				 bt[3] >> (4 - i) & 1 ? TRUE : FALSE);
  // Second
  for(i = 0; i < 3; i++)
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(segment[13 + i]), 
				 bt[4] >> (3 - i) & 1 ? TRUE : FALSE);
  for(i = 0; i < 4; i++)
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(segment[16 + i]), 
				 bt[5] >> (4 - i) & 1 ? TRUE : FALSE);

  // Decimal display
  GString *h = g_string_new("  ");
  GString *m = g_string_new("  ");
  GString *s = g_string_new("  ");

  g_string_printf(h, "%02d", tm->tm_hour);
  g_string_printf(m, "%02d", tm->tm_min);
  g_string_printf(s, "%02d", tm->tm_sec);

  gtk_label_set_text(GTK_LABEL(time_label[0]), h->str);
  gtk_label_set_text(GTK_LABEL(time_label[1]), m->str); 
  gtk_label_set_text(GTK_LABEL(time_label[2]), s->str);

  g_string_free(h, TRUE);
  g_string_free(m, TRUE);
  g_string_free(s, TRUE);

  return(TRUE);
}

// Destroy event handler (called by delete_event)
static void 
destroy_event(GtkWidget *widget, gpointer data) {
  gtk_main_quit();
}

// Delete event handler
static gboolean 
delete_event(GtkWidget *widget, GdkEvent *event, gpointer data) {
  destroy_event(widget, data);
  return(FALSE);
}

int 
main(int argc, char *argv[]) {


  /**
   * Set up the gtk2 window. This is a LOT of code. Bare with me.
   */
  unsigned int i;
                                // Will contain:
  GtkWidget *window,            // [hbox_main]
	    *hbox_main,         // [vbox h, sep, vbox m, sep, vbox s]
	    *vbox_hour,         // [        hbox_hour,        hour_label]
	    *vbox_min,          // [         hbox_min,         min_label] 
	    *vbox_sec,          // [         hbox_sec,         sec_label]
	    *hbox_hour,         // [vbox_check_hour 0, vbox_check_hour 1]
	    *hbox_min,          // [ vbox_check_min 0,  vbox_check_min 1]
	    *hbox_sec,          // [ vbox_check_sec 0,  vbox_check_sec 1]
	    *vbox_check_hour[2],// 0=checkboxes, 1=checkboxes
	    *vbox_check_min[2], // 0=checkboxes, 1=checkboxes
	    *vbox_check_sec[2], // 0=checkboxes, 1=checkboxes
            *separator[2];
	                          

  gtk_init(&argc, &argv);

  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

  gtk_window_set_title(GTK_WINDOW(window), "BCD Clock");
  gtk_container_set_border_width(GTK_CONTAINER(window), 8);

  // Create hbox_main and add to window
  hbox_main = gtk_hbox_new(FALSE, 0); 
  gtk_container_add(GTK_CONTAINER(window), hbox_main);

  // Create vboxes
  vbox_hour = gtk_vbox_new(FALSE, 0);
  vbox_min  = gtk_vbox_new(FALSE, 0);
  vbox_sec  = gtk_vbox_new(FALSE, 0);

  // Create hboxes
  hbox_hour = gtk_hbox_new(FALSE, 0);
  hbox_min  = gtk_hbox_new(FALSE, 0);
  hbox_sec  = gtk_hbox_new(FALSE, 0);

  // Create checkbox vboxes
  for(i = 0; i < 2; i++) {
    vbox_check_hour[i] = gtk_vbox_new(FALSE, 0);
    vbox_check_min[i]  = gtk_vbox_new(FALSE, 0);
    vbox_check_sec[i]  = gtk_vbox_new(FALSE, 0);
    gtk_widget_show(vbox_check_hour[i]);
    gtk_widget_show(vbox_check_min[i]);
    gtk_widget_show(vbox_check_sec[i]);
  }

  // Create separators
  for(i = 0; i < 2; i++) {
    separator[i] = gtk_vseparator_new();
    gtk_widget_show(separator[i]);
  }
  // Create checkboxes
  for(i = 0; i < 20; i++) {
    segment[i] = gtk_check_button_new();
    gtk_widget_show(segment[i]);
  }

  // Create labels
  for(i = 0; i < 3; i++) {
    time_label[i] = gtk_label_new("  ");
    gtk_widget_show(time_label[i]);
  }

  // Pack checkboxes in checkbox vboxes
  for(i = 0; i < 2; i++)
    gtk_box_pack_end(GTK_BOX(vbox_check_hour[0]), segment[  i], FALSE, TRUE, 0); 
  for(i = 0; i < 4; i++)
    gtk_box_pack_end(GTK_BOX(vbox_check_hour[1]), segment[2+i], FALSE, TRUE, 0);

  for(i = 0; i < 3; i++)
    gtk_box_pack_end( GTK_BOX(vbox_check_min[0]), segment[6+i], FALSE, TRUE, 0);
  for(i = 0; i < 4; i++)
    gtk_box_pack_end( GTK_BOX(vbox_check_min[1]), segment[9+i], FALSE, TRUE, 0);

  for(i = 0; i < 3; i++)
    gtk_box_pack_end(GTK_BOX(vbox_check_sec[0]), segment[13+i], FALSE, TRUE, 0);
  for(i = 0; i < 4; i++)
    gtk_box_pack_end(GTK_BOX(vbox_check_sec[1]), segment[16+i], FALSE, TRUE, 0); 

  // Pack checkbox vboxes into h,m,s hboxes
  for(i = 0; i < 2; i++) {
    gtk_box_pack_start(GTK_BOX(hbox_hour), vbox_check_hour[i], FALSE, TRUE, 0); 
    gtk_box_pack_start(GTK_BOX(hbox_min), vbox_check_min[i], FALSE, TRUE, 0); 
    gtk_box_pack_start(GTK_BOX(hbox_sec), vbox_check_sec[i], FALSE, TRUE, 0); 
  }

  // Pack hboxes and labels into vboxes
  gtk_box_pack_start(GTK_BOX(vbox_hour),     hbox_hour, FALSE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(vbox_hour), time_label[0], FALSE, TRUE, 0);

  gtk_box_pack_start(GTK_BOX(vbox_min),      hbox_min, FALSE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(vbox_min), time_label[1], FALSE, TRUE, 0);

  gtk_box_pack_start(GTK_BOX(vbox_sec),      hbox_sec, FALSE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(vbox_sec), time_label[2], FALSE, TRUE, 0);

  // Pack vboxes and separators into main hbox
  gtk_box_pack_start(GTK_BOX(hbox_main),    vbox_hour, FALSE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(hbox_main), separator[0], FALSE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(hbox_main),     vbox_min, FALSE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(hbox_main), separator[1], FALSE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(hbox_main),     vbox_sec, FALSE, TRUE, 0);
  
  gtk_widget_show(hbox_hour);
  gtk_widget_show(hbox_min);
  gtk_widget_show(hbox_sec);

  gtk_widget_show(vbox_hour);
  gtk_widget_show(vbox_min);
  gtk_widget_show(vbox_sec);

  gtk_widget_show(hbox_main);

  // Install event handlers (for closing the program)
  g_signal_connect(G_OBJECT(window), 
                   "delete_event", 
                   G_CALLBACK(delete_event), 
                   NULL);
  g_signal_connect(G_OBJECT(window), 
                   "destroy_event", 
                   G_CALLBACK(destroy_event), 
                   NULL);
  
  gtk_widget_show(window);
  
  // Add our timeout callback
  g_timeout_add(500 /* Milliseconds */, (GSourceFunc) update_clock, NULL);

  // Go.
  gtk_main();

  return(0);
}
