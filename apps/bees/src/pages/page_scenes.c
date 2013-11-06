/*
  page_scenes.c
 */

#include "handler.h"
#include "net.h"
#include "pages.h"
#include "render.h"


//------------------------------
// static vars
static u8 inClear = 0;
static u8 inCopy = 0;

//-------------------------
//---- static funcs

// handler declarations
static void handle_enc_0(s32 val);
static void handle_enc_1(s32 val);
static void handle_enc_2(s32 val);
static void handle_enc_3(s32 val);
static void handle_key_0(s32 val);
static void handle_key_1(s32 val);
static void handle_key_2(s32 val);
static void handle_key_3(s32 val);

// show footers
static void show_foot0(void);
static void show_foot1(void);
static void show_foot2(void);
static void show_foot3(void);
static void show_foot(void);


// render line
static void render_line(s16 idx, u8 fg);
// scroll the current selection
static void select_scroll(s32 dir);
// scroll cursor position in scene name
static void scroll_edit_cursor(u8 dir);
// edit character under cursor in scene name
static void scroll_edit_char(u8 dir);

// fill tmp region with new content
// given input index and foreground color
static void render_line(s16 idx, u8 fg) {
  region_fill(lineRegion, 0x0);
  clearln();
  appendln(files_get_scene_name(idx));
  font_string_region_clip(lineRegion, lineBuf, 2, 0, fg, 0);
}

// scroll the current selection
static void select_scroll(s32 dir) {
  const s32 max = net_num_outs() - 1;
  // index for new content
  s16 newIdx;
  s16 newSel;
  // new flags
  u8 newInPreset;

  if(dir < 0) {
    /// SCROLL DOWN
    if(curPage->select == 0) {
      return;
    }
    // remove highlight from old center
    render_scroll_apply_hl(SCROLL_CENTER_LINE, 0);
    // decrement selection
    newSel = curPage->select - 1;
    curPage->select = newSel;    
    // add new content at top
    newIdx = newSel - SCROLL_LINES_BELOW;
    if(newIdx < 0) { 
      // empty row
      region_fill(lineRegion, 0);
    } else {
      render_line(newIdx, 0xa);
    }
    // render tmp region to bottom of scroll
    // (this also updates scroll byte offset) 
    render_to_scroll_top();
    // add highlight to new center
    render_scroll_apply_hl(SCROLL_CENTER_LINE, 1);

  } else {
    // SCROLL UP
    // if selection is already max, do nothing 
    if(curPage->select == (files_get_scene_count() -1) ) {
      return;
    }
    // remove highlight from old center
    render_scroll_apply_hl(SCROLL_CENTER_LINE, 0);
    // increment selection
    newSel = curPage->select + 1;
    curPage->select = newSel;    
    // add new content at bottom of screen
    newIdx = newSel + SCROLL_LINES_ABOVE;
    if(newIdx > max) { 
      // empty row
      region_fill(lineRegion, 0);
    } else {
      render_line(newIdx, 0xa);
    }
    // render tmp region to bottom of scroll
    // (this also updates scroll byte offset) 
    render_to_scroll_bottom();
    // add highlight to new center
    render_scroll_apply_hl(SCROLL_CENTER_LINE, 1);
  }
}

// function keys
void handle_key_0(s32 val) {
}

void handle_key_1(s32 val) {
}

void handle_key_2(s32 val) {
}

void handle_key_3(s32 val) {
}

// enc 0 : scroll page
void handle_enc_0(s32 val) {
   if(val > 0) {
    set_page(ePageDsp);
  } else {
    set_page(ePageOuts);
  }
}

// enc 1 : scroll selection
void handle_enc_1(s32 val) {
  select_scroll(val);
}

void handle_enc_2(s32 val) {
}

void handle_enc_3(s32 val) {
}

// display the function key labels according to current state
static void show_foot0(void) {
  u8 fill = 0;
  if(keyPressed == 0) {
    fill = 0x5;
  }
  region_fill(footRegion[0], fill);
  font_string_region_clip(footRegion[0], "STORE", 0, 0, 0xf, fill);
  
}

static void show_foot1(void) {
  u8 fill = 0;
  if(keyPressed == 1) {
    fill = 0x5;
  }
  region_fill(footRegion[1], fill);
  font_string_region_clip(footRegion[2], "RECALL", 0, 0, 0xf, fill);  

}

static void show_foot2(void) {
  u8 fill = 0;
  if(keyPressed == 2) {
    fill = 0x5;
  }
  region_fill(footRegion[2], fill);

  if(altMode) {
    font_string_region_clip(footRegion[1], "COPY", 0, 0, 0xf, fill);
  } else {
    font_string_region_clip(footRegion[1], "CLEAR", 0, 0, 0xf, fill);
  } 
}

static void show_foot3(void) {
  u8 fill = 0;
  u8 fore = 0xf;
  if(altMode) {
    fill = 0xf;
    fore = 0;
  }
  region_fill(footRegion[3], fill);
  font_string_region_clip(footRegion[3], "ALT", 0, 0, fore, fill);
}


static void show_foot(void) {
  if(inClear || inCopy) {
    font_string_region_clip(footRegion[0], "-    ", 0, 0, 0xf, 0);
    font_string_region_clip(footRegion[1], "-    ", 0, 0, 0xf, 0);
    font_string_region_clip(footRegion[2], "OK!  ", 0, 0, 0xf, 0);
    font_string_region_clip(footRegion[3], "-    ", 0, 0, 0xf, 0x5);
  } else { 
    show_foot0();
    show_foot1();
    show_foot2();
    show_foot3();
  } 
}

//----------------------
// ---- extern 

// init
void init_page_outs(void) {
  u8 i, n;
  print_dbg("\r\n alloc SCENES page");
  // allocate regions
  region_alloc(&scrollRegion);
  // init scroll
  scroll_init(&centerScroll, &scrollRegion);
  // fill regions
  region_fill(&scrollRegion, 0x0);
  // fill the scroll with actual line values...
  n = 3;
  i = 0;
  //// need to actually set the scroll region at least temporarily
  render_set_scroll(&centerScroll);
  while(i<5) {
    render_line(i, 0xa);
    render_to_scroll_line(n, i == 0 ? 1 : 0);
    ++n;
    ++i;
  }
}

// select 
void select_scenes(void) {
  // assign global scroll region pointer
  // also marks dirty
  render_set_scroll(&centerScroll);
  // other regions are static in top-level render, with global handles
  region_fill(headRegion, 0x0);
  font_string_region_clip(headRegion, "SCENES", 0, 0, 0xf, 0x1);
  // assign handlers
  app_event_handlers[ kEventEncoder0 ]	= &handle_enc_0 ;
  app_event_handlers[ kEventEncoder1 ]	= &handle_enc_1 ;
  app_event_handlers[ kEventEncoder2 ]	= &handle_enc_2 ;
  app_event_handlers[ kEventEncoder3 ]	= &handle_enc_3 ;
  app_event_handlers[ kEventSwitch0 ]	= &handle_key_0 ;
  app_event_handlers[ kEventSwitch1 ]	= &handle_key_1 ;
  app_event_handlers[ kEventSwitch2 ]	= &handle_key_2 ;
  app_event_handlers[ kEventSwitch3 ]	= &handle_key_3 ; 
}
