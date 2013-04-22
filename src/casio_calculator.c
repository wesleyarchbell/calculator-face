#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"


#define MY_UUID { 0x20, 0xF4, 0x86, 0x60, 0xD8, 0x62, 0x4A, 0xD1, 0x9A, 0xC9, 0x6B, 0xFF, 0x70, 0x54, 0x65, 0xD5 }
PBL_APP_INFO(MY_UUID,
             "Calculator Face", "Plastik Apps",
             1, 0, /* App version */
             RESOURCE_ID_IMAGE_MENU_ICON,
             APP_INFO_WATCH_FACE);

Window window;
TextLayer dayOfWeek;
TextLayer pmAm;
TextLayer currentTime;
BitmapLayer watchFaceLayer;
BmpContainer watchFaceImage;


void setupTextLayer( TextLayer *layer, Window *parent, int x, int y, int width, int height, GFont font )
{
    text_layer_init(layer, window.layer.frame);
    text_layer_set_text_color(layer, GColorBlack);
    text_layer_set_background_color(layer, GColorClear);
    layer_set_frame(&layer->layer, GRect(x, y, width, height));
    text_layer_set_font(layer, font );
    layer_add_child(&parent->layer, &layer->layer);
}


void handle_init(AppContextRef ctx) {
  (void)ctx;

  window_init(&window, "Calculator Face");
  window_stack_push(&window, true /* Animated */);
  window_set_background_color(&window, GColorBlack);

  resource_init_current_app(&APP_RESOURCES);

  //setup watch face image and add layer to window
  bmp_init_container(RESOURCE_ID_IMAGE_WATCH_FACE, &watchFaceImage);
  bitmap_layer_init(&watchFaceLayer, GRect(0,0, 144, 168));
  layer_add_child( &window.layer, &watchFaceLayer.layer);
  bitmap_layer_set_bitmap(&watchFaceLayer, &(watchFaceImage.bmp));

  GFont font1 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_DIGITAL_45));
  GFont font2 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_DIGITAL_21));
  GFont font3 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_DIGITAL_17));

  setupTextLayer(&pmAm, &window, 23, 13, 144-120, 21, font3);
  setupTextLayer(&dayOfWeek, &window, 100, 10, 144-100, 21, font2);
  setupTextLayer(&currentTime, &window, 15, 18, 144-4, 45, font1);
}

void handle_minute_tick(AppContextRef ctx, PebbleTickEvent *t)
{
  (void)ctx;

  PblTm *tickTime = t->tick_time;

  static char dateText[] = "Sun";
  static char pmAmText[] = "PM AM";
  static int lastShownDate = -1;
  int theDay = tickTime->tm_yday;
  if (theDay != lastShownDate)
  {
    lastShownDate = theDay;
    string_format_time(dateText, sizeof(dateText), "%a", tickTime );
    string_format_time(pmAmText, sizeof(pmAmText), "%p", tickTime);
    text_layer_set_text(&dayOfWeek, dateText);
    text_layer_set_text(&pmAm, pmAmText);
  }

  static char timeText[] = "00:00";
  const char *timeFormat = clock_is_24h_style() ? "%R" : "%I:%M";
  string_format_time(timeText, sizeof(timeText), timeFormat, tickTime);
  text_layer_set_text(&currentTime, timeText);
}



void pbl_main(void *params) {
  PebbleAppHandlers handlers =
  {
    .init_handler = &handle_init,
    .tick_info =
    {
      .tick_handler = &handle_minute_tick,
      .tick_units = MINUTE_UNIT
    }
  };
  app_event_loop(params, &handlers);
}
