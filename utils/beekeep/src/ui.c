// stdlib
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// gtk
#include <gtk/gtk.h>

/// bees
#include "net_protected.h"
#include "scene.h"

// beekeep
#include "ui.h"
#include "ui_files.h"
#include "ui_handlers.h"
#include "ui_lists.h"
#include "ui_op_menu.h"

//----------------------------------
//--- extern variables

// scrollboxes
ScrollBox boxOuts;
ScrollBox boxIns;
ScrollBox boxOps;
ScrollBox boxParams;
ScrollBox boxPresets;

// new-op label
GtkWidget* newOpLabel;
// connect/disconect input button
GtkWidget* connectInputBut;
// connect/disconnect param button
GtkWidget* connectParamBut;
// selections
int opSelect = -1;
int outSelect = -1;
int inSelect = -1;
int paramSelect = -1;
int presetSelect = -1;
op_id_t newOpSelect = -1;

//----------------------------
//--- static functions

// make a scrollable box
static void scroll_box_new( ScrollBox* scrollbox, 
			    GtkWidget* parent, // must be GtkGrid*
			    gint w,
			    gint h,
			    list_fill_fn fill )
{
  
  GtkWidget* scroll;
  GtkWidget* list;

  scroll = gtk_scrolled_window_new (NULL, NULL);
  gtk_scrolled_window_set_policy( GTK_SCROLLED_WINDOW(scroll),
				  GTK_POLICY_AUTOMATIC,
				  GTK_POLICY_AUTOMATIC);

  gtk_widget_set_hexpand(scroll, TRUE);
  gtk_widget_set_vexpand(scroll, TRUE);

  // sibling argument is NULL... should set them all in a row
  gtk_grid_attach_next_to(GTK_GRID(parent), 
			  scroll, 
			  NULL,
			  GTK_POS_RIGHT,
			  w, h);

  list = gtk_list_box_new ();

  (*fill)(GTK_LIST_BOX(list));

  gtk_container_add( GTK_CONTAINER(scroll), list );
  gtk_widget_show (list);

  //  scrollbox->scroll = GTK_SCROLLED_WINDOW(scroll);
  scrollbox->scroll = scroll;
  scrollbox->list = GTK_LIST_BOX(list);
}

// destroyer-iterator
static void destroy_iter(GtkWidget* wgt, gpointer data) {
  gtk_widget_destroy(wgt);
}

// remove all list elements
void scroll_box_clear( ScrollBox* scrollbox ) {
  gtk_list_box_select_row(scrollbox->list, NULL);
  gtk_container_foreach(GTK_CONTAINER(scrollbox->list), &(destroy_iter), NULL);
}

//--------------------
//--- callbacks

static void scene_name_entry_callback( GtkEntry *entry, gpointer data) {
  const char* str;
  str = gtk_entry_get_text(entry);
  printf("\r\n setting scene name from widget: %s", str);
  scene_set_name(str);
}

static void create_op_but_callback( GtkWidget* but, gpointer data) {
  ui_create_op();
}

static void delete_op_but_callback( GtkWidget* but, gpointer data) {
  ui_delete_op();
}

static void connect_in_but_callback( GtkWidget* but, gpointer data) {
  ui_connect_in();
}

static void connect_param_but_callback( GtkWidget* but, gpointer data) {
  ui_connect_param();
}

static void write_scn_but_callback( GtkWidget* but, gpointer data) {
  write_scn();
}

static void write_json_but_callback( GtkWidget* but, gpointer data) {
  write_json();
}

/* not really useful yet
   static void write_gv_but_callback( GtkWidget* but, gpointer data) {
   write_gv();
   }
*/


//------------------------
//---- init, build, connect
void ui_init(void) {

  GtkWidget *window;
  GtkWidget *grid;
  GtkWidget *labelOps; // need to store this one
  GtkWidget *label;
  GtkWidget *opMenu;
  GtkWidget *wgt, *xgt; // temp
  
  //---  window
  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (window), "bees editor");
  gtk_window_set_default_size(GTK_WINDOW(window), 1200, 800);
  //////////////////
  /// FIXME: need to set a delete handler and do some cleanup.
  /// as it stands, getting core dump from null pointers on window close.
  //  g_signal_connect (window, "delete-event", G_CALLBACK (delete_handler), NULL);
  ///////////////////////
  g_signal_connect (window, "destroy", G_CALLBACK (gtk_main_quit), NULL);

  // grid layout
  grid = gtk_grid_new();
  //// argg
  //  gtk_grid_set_column_spacing (GTK_GRID(grid), 35);
  //  gtk_grid_set_row_spacing (GTK_GRID(grid), 20);

  //// blrrrhg
  gtk_grid_set_column_homogeneous(GTK_GRID(grid), TRUE);
  gtk_container_add(GTK_CONTAINER(window), grid);


  //--- create scrolling list things 

  scroll_box_new( &boxOps, 	grid, 2, 24, &fill_ops );
  scroll_box_new( &boxOuts, 	grid, 4, 24, &fill_outs );
  scroll_box_new( &boxIns, 	grid, 4, 24, &fill_ins );
  scroll_box_new( &boxParams, 	grid, 5, 24, &fill_params ); 
  scroll_box_new( &boxPresets, 	grid, 4, 24, &fill_presets );

  //--- list labels
  labelOps = gtk_label_new("OPERATORS");
  gtk_grid_attach_next_to(GTK_GRID(grid), labelOps, 
			  boxOps.scroll, GTK_POS_TOP, 2, 1);

  label = gtk_label_new("OUTPUTS");
  gtk_grid_attach_next_to(GTK_GRID(grid), label, 
			  boxOuts.scroll, GTK_POS_TOP, 2, 1);

  label = gtk_label_new("INPUTS");
  gtk_grid_attach_next_to(GTK_GRID(grid), label, 
			  boxIns.scroll, GTK_POS_TOP, 2, 1);


  label = gtk_label_new("PARAMETERS");
  gtk_grid_attach_next_to(GTK_GRID(grid), label, 
			  boxParams.scroll, GTK_POS_TOP, 2, 1);

  label = gtk_label_new("PRESETS");
  gtk_grid_attach_next_to(GTK_GRID(grid), label, 
			  boxPresets.scroll, GTK_POS_TOP, 2, 1);


  //--- button / labels / entries in top row
  // scene name label
  wgt = gtk_label_new("SCENE:");
  gtk_grid_attach_next_to( GTK_GRID(grid), wgt, labelOps, GTK_POS_TOP, 1, 1);

  // scene name
  xgt = gtk_entry_new();
  gtk_entry_set_text( GTK_ENTRY(xgt), scene_get_name() );
  gtk_grid_attach_next_to( GTK_GRID(grid), xgt, wgt, GTK_POS_RIGHT, 2, 1);
  g_signal_connect( xgt, "activate", G_CALLBACK(scene_name_entry_callback), NULL);

  // export .scn button
  wgt = gtk_button_new_with_label("write .scn");
  gtk_grid_attach_next_to( GTK_GRID(grid), wgt, xgt, GTK_POS_RIGHT, 1, 1 );
  g_signal_connect( wgt, "clicked", G_CALLBACK(write_scn_but_callback), NULL);

  // export .json button
  xgt = gtk_button_new_with_label("write .json");
  gtk_grid_attach_next_to( GTK_GRID(grid), xgt, wgt, GTK_POS_RIGHT, 1, 1 );
  g_signal_connect( xgt, "clicked", G_CALLBACK(write_json_but_callback), NULL);

  // export .gv button
  /* not really useful yet
     wgt = gtk_button_new_with_label("write .gv");
     gtk_grid_attach( GTK_GRID(grid), wgt, 7, 0, 1, 1 );
     g_signal_connect( wgt, "clicked", G_CALLBACK(write_gv_but_callback), NULL);
  */

  // clear button

  // select module button (file dialog?)

  //--- buttons and labels below lists
  // new op label
  newOpLabel = gtk_label_new("    ");
  gtk_grid_attach_next_to( GTK_GRID(grid), newOpLabel, 
			   boxOps.scroll, GTK_POS_BOTTOM, 1, 1 );

  // new op menu
  opMenu = create_op_menu();
  wgt = gtk_menu_button_new();
  gtk_menu_button_set_popup( GTK_MENU_BUTTON(wgt), opMenu );
  gtk_grid_attach_next_to( GTK_GRID(grid), wgt, 
			   newOpLabel, GTK_POS_RIGHT, 1, 1 );

  // create op button
  xgt = gtk_button_new_with_label("CREATE");
  g_signal_connect(xgt, "clicked", G_CALLBACK(create_op_but_callback), NULL);
  gtk_grid_attach_next_to( GTK_GRID(grid), xgt, 
			   wgt, GTK_POS_RIGHT, 1, 1 );
  
  // delete op button
  wgt = gtk_button_new_with_label("DELETE");
  g_signal_connect(wgt, "clicked", G_CALLBACK(delete_op_but_callback), NULL);
  gtk_grid_attach_next_to( GTK_GRID(grid), wgt, 
		   xgt, GTK_POS_RIGHT, 1, 1 );

  // toggle-connect-to-input button
  connectInputBut = gtk_toggle_button_new_with_label("CONNECT");
  g_signal_connect(connectInputBut, "clicked", 
		   G_CALLBACK(connect_in_but_callback), NULL);
  gtk_grid_attach_next_to( GTK_GRID(grid), connectInputBut, 
			   boxIns.scroll, GTK_POS_BOTTOM, 1, 1 );
  
  // toggle-connect-to-param button
  connectParamBut = gtk_toggle_button_new_with_label("CONNECT");
  g_signal_connect(connectParamBut, "clicked", 
		   G_CALLBACK(connect_param_but_callback), NULL);
  gtk_grid_attach_next_to( GTK_GRID(grid), connectParamBut, 
			   boxParams.scroll, GTK_POS_BOTTOM, 1, 1 );

  // store-output-in-preset button

  // store-input-in-preset button

  /// show everything
  gtk_widget_show_all(window);
}


void refresh_connect_input_but(void) {
  // still can't get this to work... 
  // setting the "active" property triggers the callback, no matter what...
/* #if 0 */
/*   gboolean c; */
/*   c = (net_get_target(outSelect) == inSelect); */
/*   printf("\r\n refresh input connection button; value: %d", (int)c); */
/*   printf("\t selections: out: %d; in: %d", outSelect, inSelect); */
/*   g_object_set(connectInputBut,"active", c, NULL); */
/*   gtk_widget_show(connectInputBut); */
/* #else */

  /// okay, the workaround is blocking/unblocking the signal.
  gboolean c;
  if(outSelect == -1) { 
    c = FALSE;
  } else {
    c = (net_get_target(outSelect) == inSelect && inSelect != -1);
  }
  g_signal_handlers_block_by_func( connectInputBut, 
				   G_CALLBACK(connect_in_but_callback), 
				   NULL);
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(connectInputBut), c);
  g_signal_handlers_unblock_by_func( connectInputBut, 
				     G_CALLBACK(connect_in_but_callback), 
				     NULL);
  //#endif
}


void refresh_connect_param_but(void) {
/* #if 0 */
/*   gboolean c = (net_get_target(outSelect) == (paramSelect + net->numIns)); */
/*   printf("\r\n refresh param connection button; value: %d", (int)c); */
/*   g_object_set(connectParamBut,"active", c, NULL); */
/*   gtk_widget_show(connectParamBut); */
/* #else */
  /// okay, the workaround is blocking/unblocking the signal.
  gboolean c;
  int t;
  if(outSelect == -1) { 
    c = FALSE;
  } else {
    t = net_get_target(outSelect);
    c = (t == (paramSelect + net->numIns) && t != -1);
  }
  g_signal_handlers_block_by_func( connectParamBut, 
				   G_CALLBACK(connect_param_but_callback), 
				   NULL);
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(connectParamBut), c);
  g_signal_handlers_unblock_by_func( connectParamBut, 
				     G_CALLBACK(connect_param_but_callback), 
				     NULL);
  //#endif
}
