#include <pebble.h>
//#define ALPHANUM
#include "main.h"

//#define SDK_3_0 //Enable SDK 3.0 Compatibility

#define KEY_FOREGROUND 1
#define KEY_BACKGROUND 2
#define KEY_THICKNESS 3
#define KEY_USE_SECONDS 4
#define KEY_STORAGE_VERSION 5
#define KEY_DIGIT_MARGIN 6
#define KEY_CORNER_RADIUS 7
#define KEY_DIGIT_SPACING 8

#define APP_STORAGE_VERSION 2

#define STORAGE_VERSION_V1 1
#define STORAGE_VERSION_V2 2

#define DEFAULT_THICKNESS 15
#define DEFAULT_DIGIT_MARGIN 8
#define DEFAULT_DIGIT_SPACING 15
#define DEFAULT_CORNER_RADIUS 4
#define DEFAULT_FG_COLOR 0xFFFFFF
#define DEFAULT_BG_COLOR 0x000000

Window *window;
Layer *clock_layer;

// config
int clock_stroke = 1;
int margin = DEFAULT_DIGIT_MARGIN;
int inner_margin = DEFAULT_DIGIT_SPACING;
int corner_radius = DEFAULT_CORNER_RADIUS;
GColor clockColor;
GColor backgroundColor;

// constants
int device_width = PBL_DISPLAY_WIDTH;
int device_height = PBL_DISPLAY_HEIGHT;
int letter_height = 49;
int min = 0;
int hour = 0;
int sec = 0;

// #define SEGMENT_DEBUG 1
//#define DEBUG 1

int get_second_letter(int num){
  return alphabet[num % 10];
}

int get_first_letter(int num){
  return alphabet[num / 10];
}

int letter_from_char(char c) {
  #ifdef ALPHANUM
  if (c >= 0x41 && c <= 0x41 + 25) {
    return abcs[c - 0x41];
  }

  if (c >= 0x61 && c <= 0x61 + 25) {
    return abcs[c - 0x61];
  }
  #endif

  if (c >= 0x30 && c <= 0x30 + 9) {
    return alphabet[c - 0x30];
  }

  if (c == 45) {
    return DIGIT_DASH;
  }

  return DIGIT_DASH;
}

static void draw_letter(int letter, int stroke_width, GRect rect, GContext * ctx){
  graphics_context_set_stroke_color(ctx, clockColor);
  graphics_context_set_fill_color(ctx, clockColor);

  int x = rect.origin.x;
  int y = rect.origin.y;
  int w = rect.size.w;
  int h = rect.size.h;


  int effective_corner_rad = corner_radius;

  // don't let the corner radius be larger than the stroke width
  if (corner_radius > stroke_width) {
    effective_corner_rad = stroke_width;
  }

  // don't let the corner radius be larger than 8 (pebble max)
  if (effective_corner_rad > 8) {
    effective_corner_rad = 8;
  }

  // don't let the corner radius be negative
  if (effective_corner_rad < 0) {
    effective_corner_rad = 0;
  }

  #ifdef DEBUG
    APP_LOG(APP_LOG_LEVEL_INFO, "using draw settings sw=%d, cr=%d, ecr=%d, w=%d, h=%d", stroke_width, corner_radius, effective_corner_rad, w, h);
  #endif
  
  #ifdef ALPHANUM
    graphics_context_set_antialiased(ctx, false);
    
    int diag_offset = 0.7071067811865476 * stroke_width;
    if (diag_offset < 1) { diag_offset = 1; }

    if (letter & SPECIAL_TLDESC_DIAG) {
      GPathInfo tldesc_shape = {
        .num_points = 6,
        .points = (GPoint []) {
          {x, y-diag_offset},
          {x, y}, 
          {x+diag_offset, y},
          {x + w/2, y + h/2 - diag_offset},
          {x + w/2, y + h/2},
          {x + w/2 + diag_offset, y + h/2}
        }
      };
      
      GPath *tldesc_ptr = gpath_create(&tldesc_shape);
      gpath_draw_filled(ctx, tldesc_ptr);
      gpath_destroy(tldesc_ptr);
    }
    
    if (letter & SPECIAL_BRDESC_DIAG) {
      GPathInfo brdesc_path = {
        .num_points = 4,
        .points = (GPoint []) {{x+w/2, y+h/2 - stroke_width}, {x + w - stroke_width, y+h-stroke_width*2}, {x+w - stroke_width, y+h}, {x+w/2, y+h/2 + stroke_width}}
      };
      
      GPath *brdesc_ptr = gpath_create(&brdesc_path);
      gpath_draw_filled(ctx, brdesc_ptr);
      gpath_destroy(brdesc_ptr);
    }
  #endif
  
  // SEGMENT() macro returns a bitmask for the defined bit

  // Top 2
  if(letter & SEGMENT(0)){
    #ifdef SEGMENT_DEBUG
      graphics_context_set_fill_color(ctx, GColorRed);
    #endif

    graphics_fill_rect(ctx, GRect(x, y, w/2 , stroke_width), effective_corner_rad, effective_corner_rad == 0 ? GCornerNone : GCornerTopLeft);
  }
  if(letter & SEGMENT(1)) {
    #ifdef SEGMENT_DEBUG
      graphics_context_set_fill_color(ctx, GColorOrange);
    #endif
    graphics_fill_rect(ctx, GRect(x+w/2, y, w/2 , stroke_width), effective_corner_rad, effective_corner_rad == 0 ? GCornerNone : GCornerTopRight);
  }

  //First 2 Sides
  if(letter & SEGMENT(2)){
    #ifdef SEGMENT_DEBUG
      graphics_context_set_fill_color(ctx, GColorYellow);
    #endif
    graphics_fill_rect(ctx, GRect(x, y, stroke_width , h/4), effective_corner_rad, effective_corner_rad == 0 ? GCornerNone : GCornerTopLeft);
  }

  // Middle Side
  if(letter & SEGMENT(3)) {
    #ifdef SEGMENT_DEBUG
      graphics_context_set_fill_color(ctx, GColorGreen);
    #endif
    graphics_fill_rect(ctx, GRect(x+w/2 - stroke_width / 2, y, stroke_width , h/4), 0, GCornerNone);
  }
  if(letter & SEGMENT(4)) {
    #ifdef SEGMENT_DEBUG
      graphics_context_set_fill_color(ctx, GColorBlue);
    #endif
    graphics_fill_rect(ctx, GRect(x + w - stroke_width, y, stroke_width , h/4), effective_corner_rad, effective_corner_rad == 0 ? GCornerNone : GCornerTopRight);
  }

  //Second 2 Sides
  if(letter & SEGMENT(5)){
    #ifdef SEGMENT_DEBUG
      graphics_context_set_fill_color(ctx, GColorIndigo);
    #endif
    graphics_fill_rect(ctx, GRect(x, y+h/4, stroke_width , h/4), 0, GCornerNone);
  }

  if(letter & SEGMENT(6)) {
    #ifdef SEGMENT_DEBUG
      graphics_context_set_fill_color(ctx, GColorPurple);
    #endif
    graphics_fill_rect(ctx, GRect(x+w/2 - stroke_width/2, y+h/4, stroke_width , h/4), 0, GCornerNone);
  }

  if(letter & SEGMENT(7)) {
    #ifdef SEGMENT_DEBUG
      graphics_context_set_fill_color(ctx, GColorLightGray);
    #endif
    graphics_fill_rect(ctx, GRect(x+w-stroke_width, y+h/4, stroke_width , h/4), 0, GCornerNone);
  }

  // Middle 2
  if(letter & SEGMENT(8)){
    #ifdef SEGMENT_DEBUG
      graphics_context_set_fill_color(ctx, GColorDarkGray);
    #endif
    graphics_fill_rect(ctx, GRect(x, y+h/2 - stroke_width / 2, w/2 , stroke_width), 0, GCornerNone);
  }

  if(letter & SEGMENT(9)) {
    #ifdef SEGMENT_DEBUG
      graphics_context_set_fill_color(ctx, GColorBlack);
    #endif
    graphics_fill_rect(ctx, GRect(x+w/2, y+h/2 - stroke_width / 2, w/2 , stroke_width), 0, GCornerNone);
  }

  //Third 2 Sides
  if(letter & SEGMENT(10)){
    #ifdef SEGMENT_DEBUG
      graphics_context_set_fill_color(ctx, GColorRed);
    #endif
    graphics_fill_rect(ctx, GRect(x, y+h/2, stroke_width , h/4), 0, GCornerNone);
  }

  if(letter & SEGMENT(11)){
    #ifdef SEGMENT_DEBUG
      graphics_context_set_fill_color(ctx, GColorOrange);
    #endif
    graphics_fill_rect(ctx, GRect(x+w/2 - stroke_width/2, y+h/2, stroke_width , h/4), 0, GCornerNone);
  }

  if(letter & SEGMENT(12)){
    #ifdef SEGMENT_DEBUG
      graphics_context_set_fill_color(ctx, GColorYellow);
    #endif
    graphics_fill_rect(ctx, GRect(x+w-stroke_width, y+h/2, stroke_width , h/4), 0, GCornerNone);
  }

  // Last 2 Sides
  if(letter & SEGMENT(13)){
    #ifdef SEGMENT_DEBUG
      graphics_context_set_fill_color(ctx, GColorGreen);
    #endif
    graphics_fill_rect(ctx, GRect(x, y+h/4*3, stroke_width , h/4), effective_corner_rad, effective_corner_rad == 0 ? GCornerNone : GCornerBottomLeft);
  }
  if(letter & SEGMENT(14)){
    #ifdef SEGMENT_DEBUG
      graphics_context_set_fill_color(ctx, GColorBlue);
    #endif
    graphics_fill_rect(ctx, GRect(x+w/2 - stroke_width/2, y+h/4*3, stroke_width , h/4), 0, GCornerNone);
  }
  if(letter & SEGMENT(15)){
    #ifdef SEGMENT_DEBUG
      graphics_context_set_fill_color(ctx, GColorIndigo);
    #endif
    graphics_fill_rect(ctx, GRect(x+w-stroke_width, y+h/4*3, stroke_width , h/4), effective_corner_rad, effective_corner_rad == 0 ? GCornerNone : GCornerBottomRight);
  }

  // Bottom 2
  if(letter & SEGMENT(16)){
    #ifdef SEGMENT_DEBUG
      graphics_context_set_fill_color(ctx, GColorPurple);
    #endif
    graphics_fill_rect(ctx, GRect(x, y + h - stroke_width, w/2 , stroke_width), effective_corner_rad, effective_corner_rad == 0 ? GCornerNone : GCornerBottomLeft);
  }
  if(letter & SEGMENT(17)) {
    #ifdef SEGMENT_DEBUG
      graphics_context_set_fill_color(ctx, GColorLightGray);
    #endif
    graphics_fill_rect(ctx, GRect(x + w/2, y + h - stroke_width, w/2, stroke_width), effective_corner_rad, effective_corner_rad == 0 ? GCornerNone : GCornerBottomRight);
  }
}


static void update_clock(Layer *this_layer, GContext *ctx) {
  int letter_w = device_width/2 - margin - inner_margin/2;
  int letter_h = device_height/2 - margin - inner_margin/2;

  // round off sizes so we're dealing in whole divisible numbers
  letter_w = (letter_w + (2 - (letter_w % 2)));
  letter_h = (letter_h + (4 - (letter_h % 4)));

  /* x y w h*/
  GRect topLeft = GRect(
    margin,
    margin,
    letter_w, letter_h
  );
  GRect bottomLeft = GRect(
    margin,
    device_height - letter_h - margin,
    letter_w, letter_h
  );
  GRect topRight = GRect(
    device_width - letter_w - margin,
    margin,
    letter_w, letter_h
  );
  GRect bottomRight = GRect(
    device_width - letter_w - margin,
    device_height - letter_h - margin,
    letter_w, letter_h
  );

  #ifdef DEBUG
    APP_LOG(APP_LOG_LEVEL_INFO, "letter_w: %d, letter_h: %d, hour: %d, minute: %d, fh: %d, sh: %d, fm: %d, sm: %d", letter_w, letter_h, hour, min, get_first_letter(hour), get_second_letter(hour), get_first_letter(min), get_second_letter(min));
  #endif

  draw_letter(get_first_letter(hour), clock_stroke, topLeft, ctx);
  draw_letter(get_second_letter(hour), clock_stroke, topRight, ctx);
  draw_letter(get_first_letter(min), clock_stroke, bottomLeft, ctx);
  draw_letter(get_second_letter(min), clock_stroke, bottomRight, ctx);

  // graphics_context_set_fill_color(ctx, GColorRed);
  // int xfs = 16;
  // int yfs = 22;
  // for (int x = 0; x < 5; x++) {
  //   for (int y = 0; y < 5; y++) {
  //     int letter = letter_from_char(0x41 + x + 5 * y);
  //     draw_letter(letter, 2, GRect(5 + (xfs+2) * x, 5 + (yfs+2) * y, xfs, yfs), ctx);
  //   }
  // }

 // if(useSeconds){
  //  graphics_fill_rect(ctx, GRect(0, device_height, device_width * sec / 60, -4), 0, GCornerNone);
 // }
}

// Handle Clock Ticks
static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  min =  tick_time->tm_min;
  hour =  tick_time->tm_hour;
  sec = tick_time->tm_sec;

  // Convert 24h to 12h style
  if (!clock_is_24h_style()) {
    hour = hour % 12;
    if (hour == 0){
      hour = 12;
    }
  }

  layer_mark_dirty(clock_layer);
}

static void inbox_dropped_callback(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}


static void in_recv_handler(DictionaryIterator *iterator, void *context)
{
  //Get Tuple
  Tuple *t = dict_read_first(iterator);
  while (t != NULL) {
    #ifdef DEBUG
      APP_LOG(APP_LOG_LEVEL_INFO, "key: %lu, value: %li", t->key, t->value->int32);
    #endif
    if (t) {
      switch (t->key) {
        case KEY_FOREGROUND:
          #ifdef PBL_COLOR
            clockColor = GColorFromHEX(t->value->int32);
          #else
            //Round the numbers
            if (t->value->int32 > 0) {
            APP_LOG(APP_LOG_LEVEL_INFO, "Rounding foreground to white");
              clockColor = GColorWhite;
            } else {
              APP_LOG(APP_LOG_LEVEL_INFO, "Rounding foreground to black");
              clockColor = GColorBlack;
            }
          #endif
          layer_mark_dirty(clock_layer);
          persist_write_int(KEY_FOREGROUND, t->value->int32);
          break;
        case KEY_BACKGROUND:
          #ifdef PBL_COLOR
            backgroundColor = GColorFromHEX(t->value->int32);
            window_set_background_color(window, backgroundColor);
          #else
            // Round the numbers
            if (t->value->int32 > 0) {
              APP_LOG(APP_LOG_LEVEL_INFO, "Rounding bg to white");
              backgroundColor = GColorWhite;
              window_set_background_color(window, backgroundColor);
            } else {
              APP_LOG(APP_LOG_LEVEL_INFO, "Rounding bg to black");
              backgroundColor = GColorBlack;
              window_set_background_color(window, backgroundColor);
            }
          #endif
          layer_mark_dirty(clock_layer);
          persist_write_int(KEY_BACKGROUND, t->value->int32);
          break;

        case KEY_THICKNESS:
          //if(margin < clock_stroke){
           // margin = clock_stroke + 5;
         // }
          clock_stroke = t->value->int32;
          if (clock_stroke > 15) {
            clock_stroke = 15;
          }
          if (clock_stroke < 1) {
            clock_stroke = 1;
          }
          layer_mark_dirty(clock_layer);
          persist_write_int(KEY_THICKNESS, t->value->int32);
          break;
        case KEY_CORNER_RADIUS:
          corner_radius = t->value->int32;
          if (corner_radius > 8) {
            corner_radius = 8;
          }
          if (corner_radius < 0) {
            corner_radius = 0;
          }
          layer_mark_dirty(clock_layer);
          persist_write_int(KEY_CORNER_RADIUS, t->value->int32);
          break;
        case KEY_DIGIT_SPACING:
          inner_margin = t->value->int32;
          layer_mark_dirty(clock_layer);
          persist_write_int(KEY_DIGIT_SPACING, t->value->int32);
          break;
        case KEY_DIGIT_MARGIN:
          #ifdef PBL_ROUND
            margin = 28;
          #else
            margin = t->value->int32;
          #endif
          layer_mark_dirty(clock_layer);
          persist_write_int(KEY_DIGIT_MARGIN, t->value->int32);
          break;

      }
    }
    t = dict_read_next(iterator);
  }
}

void handle_init(void) {
  window = window_create();
  clock_layer = layer_create(GRect(0, 0, device_width, device_height));
  app_message_register_inbox_received((AppMessageInboxReceived) in_recv_handler);
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
  app_message_register_inbox_dropped((AppMessageInboxDropped) inbox_dropped_callback);

  if (!persist_exists (KEY_STORAGE_VERSION)) {
     //setup defaults.
    persist_write_int(KEY_STORAGE_VERSION, APP_STORAGE_VERSION);
    persist_write_int(KEY_FOREGROUND, DEFAULT_FG_COLOR);
    persist_write_int(KEY_BACKGROUND, DEFAULT_BG_COLOR);
    persist_write_int(KEY_THICKNESS, DEFAULT_THICKNESS);
    persist_write_bool(KEY_USE_SECONDS, false);
    persist_write_int(KEY_DIGIT_MARGIN, DEFAULT_DIGIT_MARGIN);
    persist_write_int(KEY_CORNER_RADIUS, DEFAULT_CORNER_RADIUS);
    persist_write_int(KEY_DIGIT_SPACING, DEFAULT_DIGIT_SPACING);
  }

  // if the storage version is less than the current app version, upgrade
  if (persist_read_int(KEY_STORAGE_VERSION) < APP_STORAGE_VERSION) {
    persist_write_int(KEY_STORAGE_VERSION, APP_STORAGE_VERSION);
  }

  // upgrade from v1->v2
  if (persist_read_int(KEY_STORAGE_VERSION) < STORAGE_VERSION_V2) {
    persist_write_int(KEY_DIGIT_MARGIN, DEFAULT_DIGIT_MARGIN);
  }

  int color = persist_read_int(KEY_FOREGROUND);
  int background = persist_read_int(KEY_BACKGROUND);
  clock_stroke = persist_read_int(KEY_THICKNESS);
  corner_radius = persist_read_int(KEY_CORNER_RADIUS);
  inner_margin = persist_read_int(KEY_DIGIT_SPACING);
  margin = persist_read_int(KEY_DIGIT_MARGIN);
  
  if (inner_margin < 10) {
    inner_margin = DEFAULT_DIGIT_SPACING; 
  }

  #ifdef PBL_ROUND
    if (margin < 32) {
      margin = 32;
    }
  #endif

  APP_LOG(APP_LOG_LEVEL_INFO, "Config: color: %d, bg: %d, stroke: %d, margin: %d, corner rad: %d",color, background, clock_stroke, margin, corner_radius);

  #ifdef PBL_COLOR
    clockColor = GColorFromHEX(color);
    backgroundColor = GColorFromHEX(background);
  #else
    // Round the numbers for B/W
    if (color > 0) {
      clockColor = GColorWhite;
    } else {
      clockColor = GColorBlack;
    }

    if (background > 0) {
      backgroundColor = GColorWhite;
    } else {
      backgroundColor = GColorBlack;
    }
  #endif

  tick_timer_service_subscribe(MINUTE_UNIT | HOUR_UNIT, tick_handler);

  window_set_background_color(window, backgroundColor);
  layer_set_update_proc(clock_layer, update_clock);
  window_stack_push(window, true);
  layer_add_child(window_get_root_layer(window), clock_layer);
}

void handle_deinit(void) {
  tick_timer_service_unsubscribe();
  layer_destroy(clock_layer);
  window_destroy(window);
}

int main(void) {
  handle_init();
  app_event_loop();
  handle_deinit();
}
