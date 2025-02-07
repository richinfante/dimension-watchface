#include <pebble.h>
#include "main.h"

//#define SDK_3_0 //Enable SDK 3.0 Compatibility

#define KEY_FOREGROUND 1
#define KEY_BACKGROUND 2
#define KEY_THICKNESS 3
#define KEY_USE_SECONDS 4
#define KEY_STORAGE_VERSION 5
#define KEY_DIGIT_MARGIN 6

#define APP_STORAGE_VERSION 2

#define STORAGE_VERSION_V1 1
#define STORAGE_VERSION_V2 2

#define DEFAULT_DIGIT_MARGIN 15

Window *window;
Layer *clock_layer;

// config
int clock_stroke = 1;
int margin = DEFAULT_DIGIT_MARGIN;
GColor clockColor;
GColor backgroundColor;

// constants
int device_width = 144;
int device_height = 168;
int letter_height = 49;
int min = 0;
int hour = 0;
int sec = 0;

bool * get_second_letter(int num){
  #ifdef TEST_MODE
    if(num%2 == 0){
    return test;
  }else{
    return test2;
  }
  #endif

  num = num % 10;
  switch(num) {
    case 0:
      return zero;
    case 1:
      return one;
    case 2:
      return two;
    case 3:
      return three;
    case 4:
      return four;
    case 5:
      return five;
    case 6:
      return six;
    case 7:
      return seven;
    case 8:
      return eight;
    case 9:
      return nine;
    default:
      return dash;
  }
}

bool * get_first_letter(int num){
  return get_second_letter((int) (num / 10));
}

static void draw_letter(bool * letter, int stroke_width, GRect rect, GContext * ctx){
  graphics_context_set_stroke_color(ctx, clockColor);
  graphics_context_set_fill_color(ctx, clockColor);

  int x = rect.origin.x;
  int y = rect.origin.y;
  int w = rect.size.w;
  int h = rect.size.h;

  //Top 2
  if(letter[0] == true){
    graphics_fill_rect(ctx, GRect(x, y, w/2 , stroke_width), 0, GCornerNone);
    //graphics_draw_line(ctx, GPoint(x,y), GPoint(x+w/2, y));
  }
  if(letter[1] == true) {
    graphics_fill_rect(ctx, GRect(x+w/2, y, w/2 , stroke_width), 0, GCornerNone);
    //graphics_draw_line(ctx, GPoint(x+w/2, y), GPoint(x+w, y));
  }

  //First 2 Sides
  if(letter[2] == true){
    graphics_fill_rect(ctx, GRect(x, y, stroke_width , h/4), 0, GCornerNone);
    //graphics_draw_line(ctx, GPoint(x,y), GPoint(x, y+h/4));
  }

  //Middle Side
  if(letter[3] == true) {
    graphics_fill_rect(ctx, GRect(x+w/2 - stroke_width / 2, y, stroke_width , h/4 + 1), 0, GCornerNone);
    //graphics_draw_line(ctx, GPoint(x+w/2,y), GPoint(x+w/2, y+h/4));
  }

  if(letter[4] == true) {
    graphics_fill_rect(ctx, GRect(x+w, y, -stroke_width , h/4 + 1), 0, GCornerNone);
    //graphics_draw_line(ctx, GPoint(x+w,y), GPoint(x+w, y+h/4));
  }

  //Second 2 Sides
  if(letter[5] == true){
    graphics_fill_rect(ctx, GRect(x, y+h/4, stroke_width , h/4 + 1), 0, GCornerNone);
    //graphics_draw_line(ctx, GPoint(x,y+h/4), GPoint(x,y+h/2));
  }

  if(letter[6] == true) {
    graphics_fill_rect(ctx, GRect(x+w/2 - stroke_width/2, y+h/4, stroke_width , h/4 + 1), 0, GCornerNone);
    //graphics_draw_line(ctx, GPoint(x+w/2,y+h/4), GPoint(x+w/2,y+h/2));
  }

  if(letter[7] == true) {
    graphics_fill_rect(ctx, GRect(x+w, y+h/4, -stroke_width , h/4 + 1), 0, GCornerNone);
    //graphics_draw_line(ctx, GPoint(x+w,y+h/4), GPoint(x+w,y+h/2));
  }

  //Middle 2
  if(letter[8] == true){
    graphics_fill_rect(ctx, GRect(x, y+h/2 - stroke_width / 2, w/2 , stroke_width), 0, GCornerNone);
    //graphics_draw_line(ctx, GPoint(x,y+h/2), GPoint(x+w/2, y+h/2));
  }

  if(letter[9] == true) {
    graphics_fill_rect(ctx, GRect(x+w/2, y+h/2 - stroke_width / 2, w/2 , stroke_width), 0, GCornerNone);
    //graphics_draw_line(ctx, GPoint(x+w/2, y+h/2), GPoint(x+w, y+h/2));
  }

  //Third 2 Sides
  if(letter[10] == true){
     graphics_fill_rect(ctx, GRect(x, y+h/2, stroke_width , h/4 + 1), 0, GCornerNone);
    //graphics_draw_line(ctx, GPoint(x,y+h/2), GPoint(x, y+h/4*3));
  }

  if(letter[11] == true){
    graphics_fill_rect(ctx, GRect(x+w/2 - stroke_width/2, y+h/2, stroke_width , h/4 + 1), 0, GCornerNone);
    //graphics_draw_line(ctx, GPoint(x+w/2, y+h/2), GPoint(x+w/2, y+h/4*3));
  }

  if(letter[12] == true){
    graphics_fill_rect(ctx, GRect(x+w, y+h/2, -stroke_width , h/4 + 1), 0, GCornerNone);
    //graphics_draw_line(ctx, GPoint(x+w, y+h/2), GPoint(x+w, y+h/4*3));
  }

  //Last 2 Sides
  if(letter[13] == true){
    graphics_fill_rect(ctx, GRect(x, y+h/4*3, stroke_width , h/4 + 1), 0, GCornerNone);
    //graphics_draw_line(ctx, GPoint(x,y+h/4*3), GPoint(x, y+h));
  }

  if(letter[14] == true){
    graphics_fill_rect(ctx, GRect(x+w/2 - stroke_width/2, y+h/4*3, stroke_width , h/4 + 1), 0, GCornerNone);
    //graphics_draw_line(ctx, GPoint(x+w/2,y+h/4*3), GPoint(x+w/2, y+h));
  }

  if(letter[15] == true){
    graphics_fill_rect(ctx, GRect(x+w, y+h/4*3, -stroke_width , h/4 + 1), 0, GCornerNone);
    //graphics_draw_line(ctx, GPoint(x+w,y+h/4*3), GPoint(x+w, y+h));
  }

  //Bottom 2
  if(letter[16] == true){
    graphics_fill_rect(ctx, GRect(x, y+h, w/2 , -stroke_width), 0, GCornerNone);
    //graphics_draw_line(ctx, GPoint(x,y+h), GPoint(x+w/2, y+h));
  }
  if(letter[17] == true) {
    graphics_fill_rect(ctx, GRect(x+w/2, y+h, w/2 , -stroke_width), 0, GCornerNone);
    //graphics_draw_line(ctx, GPoint(x+w/2, y+h), GPoint(x+w, y+h));
  }
}


static void update_clock(Layer *this_layer, GContext *ctx) {
  GRect topLeft = GRect(margin, margin, device_width/2 - margin*2, device_height/2 - margin*2);
  GRect bottomLeft = GRect(margin, margin  + device_height/2, device_width/2 - margin*2, device_height/2 - margin*2);
  GRect topRight = GRect(margin + device_width/2, margin, device_width/2 - margin*2, device_height/2 - margin*2);
  GRect bottomRight = GRect(margin + device_width/2, margin + device_height/2, device_width/2 - margin*2, device_height/2 - margin*2);

  draw_letter(get_first_letter(hour), clock_stroke, topLeft, ctx);
  draw_letter(get_second_letter(hour), clock_stroke, topRight, ctx);
  draw_letter(get_first_letter(min), clock_stroke, bottomLeft, ctx);
  draw_letter(get_second_letter(min), clock_stroke, bottomRight, ctx);

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
    if(hour == 0){
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
    APP_LOG(APP_LOG_LEVEL_INFO, "key: %lu, value: %li", t->key, t->value->int32);
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
        case KEY_DIGIT_MARGIN:
          margin = t->value->int32;
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
  clock_layer = layer_create(GRect(0, 0, 144, 168));
  app_message_register_inbox_received((AppMessageInboxReceived) in_recv_handler);
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
  app_message_register_inbox_dropped((AppMessageInboxDropped) inbox_dropped_callback);

  if (!persist_exists (KEY_STORAGE_VERSION)){
     //setup defaults.
    persist_write_int(KEY_STORAGE_VERSION, APP_STORAGE_VERSION);
    persist_write_int(KEY_FOREGROUND, 0xFFFFFF);
    persist_write_int(KEY_BACKGROUND, 0x000000);
    persist_write_int(KEY_THICKNESS, 1);
    persist_write_bool(KEY_USE_SECONDS, false);
    persist_write_int(KEY_DIGIT_MARGIN, DEFAULT_DIGIT_MARGIN);
  }

  // if the storage version is less than the current app version, upgrade
  if (persist_read_int(KEY_STORAGE_VERSION) < APP_STORAGE_VERSION){
    persist_write_int(KEY_STORAGE_VERSION, APP_STORAGE_VERSION);
  }

  // upgrade from v1->v2
  if (persist_read_int(KEY_STORAGE_VERSION) < STORAGE_VERSION_V2) {
    persist_write_int(KEY_DIGIT_MARGIN, DEFAULT_DIGIT_MARGIN);
  }

  int color = persist_read_int(KEY_FOREGROUND);
  int background = persist_read_int(KEY_BACKGROUND);
  clock_stroke = persist_read_int(KEY_THICKNESS);
  margin = persist_read_int(KEY_DIGIT_MARGIN);

  APP_LOG(APP_LOG_LEVEL_INFO, "Config: color: %d, bg: %d, stroke: %d, margin: %d",color, background, clock_stroke, margin);

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

  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);

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
