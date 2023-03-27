
/**
 * @file main
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include "lvgl/lvgl.h"
#include "lvgl/demos/lv_demos.h"
#include "lv_drivers/display/fbdev.h"
#include "lv_drivers/indev/evdev.h"
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void lv_port_disp_init(const char *path);
static void lv_port_indev_init(const char *path, bool show_cursor);
static void cursor_set_hidden(bool en);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *      VARIABLES
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

char temp_h_s[100];
char temp_l_s[100];
char temp_n_s[100];
int high_te = 0;
int low_te = 100;
int now_te = 0;

lv_obj_t *meter;
// lv_meter_indicator_t *indic_min;
// lv_meter_indicator_t *indic_hour;
// LV_IMG_DECLARE(img_hand_min)
// LV_IMG_DECLARE(img_hand_hour)

lv_obj_t *test_btn;
lv_obj_t *test_btn_label;
lv_obj_t *write_img_button;
lv_obj_t *write_img_button_label;
lv_obj_t *status_label;
lv_obj_t *temp_high_label;
lv_obj_t *temp_low_label;
lv_obj_t *temp_now_label;
lv_obj_t *cont1;

int test_rtc_status = 0;

pthread_t thread1_t;
static void *thread1(void *paramater)
{
  int count = 0;
  int return_val;
  lv_label_set_text(status_label, "testing eth0");
  for (size_t i = 0; i < 3; i++)
  {
    return_val = system("./produce_test/eth0test.sh");
    if (return_val == 0)
    {
      lv_obj_add_state(lv_obj_get_child(cont1, 0), LV_STATE_CHECKED);
      break;
    }
    sleep(1);
  }
  lv_label_set_text(status_label, "testing uart");
  for (size_t i = 0; i < 3; i++)
  {
    return_val = system("./produce_test/uarttest.sh");
    if (return_val == 0)
    {
      lv_obj_add_state(lv_obj_get_child(cont1, 1), LV_STATE_CHECKED);
      break;
    }
    sleep(1);
  }
  lv_label_set_text(status_label, "testing i2c");
  for (size_t i = 0; i < 3; i++)
  {
    return_val = system("./produce_test/i2ctest.sh");
    if (return_val == 0)
    {
      lv_obj_add_state(lv_obj_get_child(cont1, 2), LV_STATE_CHECKED);
      break;
    }
    sleep(1);
  }
  lv_label_set_text(status_label, "testing upan");
  for (size_t i = 0; i < 3; i++)
  {
    return_val = system("./produce_test/upantest.sh");
    if (return_val == 0)
    {
      lv_obj_add_state(lv_obj_get_child(cont1, 3), LV_STATE_CHECKED);
      break;
    }
    sleep(1);
  }
  lv_label_set_text(status_label, "testing blue");
  for (size_t i = 0; i < 3; i++)
  {
    return_val = system("./produce_test/blueztest.sh");
    if (return_val == 0)
    {
      lv_obj_add_state(lv_obj_get_child(cont1, 4), LV_STATE_CHECKED);
      break;
    }
    sleep(1);
  }
  lv_label_set_text(status_label, "testing wifi");
  for (size_t i = 0; i < 3; i++)
  {
    return_val = system("./produce_test/wifitest.sh");
    if (return_val == 0)
    {
      lv_obj_add_state(lv_obj_get_child(cont1, 5), LV_STATE_CHECKED);
      break;
    }
    sleep(1);
  }
  lv_label_set_text(status_label, "testing cputemp");
  system("echo \"scale=2400; 4*a(1)\" | bc -l -q >> /dev/null 2>&1  &");
  system("echo \"scale=2400; 4*a(1)\" | bc -l -q >> /dev/null 2>&1  &");
  system("echo \"scale=2400; 4*a(1)\" | bc -l -q >> /dev/null 2>&1  &");
  system("echo \"scale=2400; 4*a(1)\" | bc -l -q >> /dev/null 2>&1  &");

  for (size_t i = 0; i < 35; i++)
  {
    sleep(1);
  }
  if (high_te < 70)
  {
    lv_obj_add_state(lv_obj_get_child(cont1, 6), LV_STATE_CHECKED);
  }

  lv_label_set_text(status_label, "test over!");

  lv_obj_clear_state(test_btn, LV_STATE_DISABLED);
  lv_obj_clear_state(write_img_button, LV_STATE_DISABLED);
  lv_label_set_text(test_btn_label, "test rtc");
  test_rtc_status = 1;

  return NULL;
}

pthread_t thread4_t;
static void *thread4(void *paramater)
{
  while (1)
  {
    int fd = open("/sys/class/thermal/thermal_zone0/temp", O_RDONLY);
    char buf[32];
    int tempp;
    read(fd, buf, 32);
    close(fd);
    tempp = atoi(buf) / 1000;
    now_te = tempp;
    if (tempp > high_te)
    {
      high_te = tempp;
    }
    if (tempp < low_te)
    {
      low_te = tempp;
    }
    sprintf(temp_h_s, "h temp:%d", high_te);
    sprintf(temp_l_s, "l temp:%d", low_te);
    sprintf(temp_n_s, "n temp:%d", now_te);
    lv_label_set_text(temp_high_label, temp_h_s);
    lv_label_set_text(temp_low_label, temp_l_s);
    lv_label_set_text(temp_now_label, temp_n_s);
    sleep(1);
  }
  return NULL;
}

volatile int dd_flage = 0;
pthread_t thread3_t;
static void *thread3(void *paramater)
{
  system("dd if=/mnt/os.img of=/dev/mmcblk0 bs=4M");
  system("sync");
  dd_flage = 0;
  return NULL;
}

pthread_t thread2_t;
static void *thread2(void *paramater)
{
  lv_label_set_text(status_label, "writeing ");
  dd_flage = 1;
  pthread_create(&thread3_t, NULL, thread3, NULL);
  size_t counti = 0;
  while (1)
  {
    if (dd_flage)
    {
      switch (counti)
      {
      case 0:
        lv_label_set_text(status_label, "writeing .");
        counti++;
        break;
      case 1:
        lv_label_set_text(status_label, "writeing ..");
        counti++;
        break;
      case 2:
        lv_label_set_text(status_label, "writeing ...");
        counti = 0;
        break;
      default:
        counti = 0;
        break;
      }
      sleep(1);
    }
    else
    {
      lv_label_set_text(status_label, "write over");
      lv_obj_clear_state(write_img_button, LV_STATE_DISABLED);
      lv_obj_clear_state(test_btn, LV_STATE_DISABLED);
      lv_label_set_text(write_img_button_label, "write img");
      break;
    }
  }
  return NULL;
}

// int ret = pthread_create(&thread1_t, NULL, thread1, NULL);
// if(ret != 0)
// {
// 	printf("Failed to create thread1.\n");
// 	return -1;
// }

static void event_handler1(lv_event_t *e)
{
  lv_event_code_t code = lv_event_get_code(e);

  if (code == LV_EVENT_CLICKED)
  {
    // printf("Clicked\r\n");
    lv_obj_add_state(write_img_button, LV_STATE_DISABLED);
    lv_obj_add_state(test_btn, LV_STATE_DISABLED);
    lv_label_set_text(write_img_button_label, "writeing!");
    pthread_create(&thread2_t, NULL, thread2, NULL);
  }
  else if (code == LV_EVENT_VALUE_CHANGED)
  {
    printf("Toggled\r\n");
  }
}

static void event_handler(lv_event_t *e)
{
  lv_event_code_t code = lv_event_get_code(e);

  if (code == LV_EVENT_CLICKED)
  {

    if (test_rtc_status == 0)
    {
      // printf("Clicked\r\n");
      printf("\r\n  get button status :%d\r\n", lv_obj_get_state(test_btn));
      lv_label_set_text(test_btn_label, "testing!");
      lv_obj_add_state(test_btn, LV_STATE_DISABLED);
      lv_obj_add_state(write_img_button, LV_STATE_DISABLED);
      lv_label_set_text(status_label, "testing wait");

      pthread_create(&thread1_t, NULL, thread1, NULL);

      // system("./produce_test/rtctest.sh");
    }
    else
    {
      system("./produce_test/rtctest.sh");
    }
  }
  else if (code == LV_EVENT_VALUE_CHANGED)
  {
    printf("Toggled\r\n");
  }
}

static lv_style_t style_radio;
static lv_style_t style_radio_chk;
static uint32_t active_index_1 = 0;
static uint32_t active_index_2 = 0;

// static void radio_event_handler(lv_event_t * e)
// {
//     uint32_t * active_id = lv_event_get_user_data(e);
//     lv_obj_t * cont = lv_event_get_current_target(e);
//     lv_obj_t * act_cb = lv_event_get_target(e);
//     lv_obj_t * old_cb = lv_obj_get_child(cont, *active_id);

//     // /*Do nothing if the container was clicked*/
//     // if(act_cb == cont) return;

//     // lv_obj_clear_state(old_cb, LV_STATE_CHECKED);   /*Uncheck the previous radio button*/
//     // lv_obj_add_state(act_cb, LV_STATE_CHECKED);     /*Uncheck the current radio button*/

//     // *active_id = lv_obj_get_index(act_cb);

//     // printf("Selected radio buttons: %d, %d", (int)active_index_1, (int)active_index_2);
// }

static void radiobutton_create(lv_obj_t *parent, const char *txt)
{
  lv_obj_t *obj = lv_checkbox_create(parent);
  lv_checkbox_set_text(obj, txt);
  lv_obj_add_flag(obj, LV_OBJ_FLAG_EVENT_BUBBLE);
  lv_obj_add_style(obj, &style_radio, LV_PART_INDICATOR);
  lv_obj_add_style(obj, &style_radio_chk, LV_PART_INDICATOR | LV_STATE_CHECKED);
  lv_obj_add_state(obj, LV_STATE_DISABLED);
}

/**
 * Checkboxes as radio buttons
 */
void lv_example_checkbox_x(void)
{
  /* The idea is to enable `LV_OBJ_FLAG_EVENT_BUBBLE` on checkboxes and process the
   * `LV_EVENT_CLICKED` on the container.
   * A variable is passed as event user data where the index of the active
   * radiobutton is saved */

  lv_style_init(&style_radio);
  lv_style_set_radius(&style_radio, LV_RADIUS_CIRCLE);

  lv_style_init(&style_radio_chk);
  lv_style_set_bg_img_src(&style_radio_chk, NULL);

  uint32_t i;
  char buf[32];

  cont1 = lv_obj_create(lv_scr_act());
  lv_obj_set_flex_flow(cont1, LV_FLEX_FLOW_COLUMN);
  lv_obj_set_size(cont1, 120, 220);

  // lv_obj_add_event_cb(cont1, radio_event_handler, LV_EVENT_CLICKED, &active_index_1);

  lv_snprintf(buf, sizeof(buf), "eth0", 1);
  radiobutton_create(cont1, buf);
  lv_snprintf(buf, sizeof(buf), "uart", 2);
  radiobutton_create(cont1, buf);
  lv_snprintf(buf, sizeof(buf), "i2c", 3);
  radiobutton_create(cont1, buf);
  lv_snprintf(buf, sizeof(buf), "upan", 4);
  radiobutton_create(cont1, buf);
  lv_snprintf(buf, sizeof(buf), "bluez", 5);
  radiobutton_create(cont1, buf);
  lv_snprintf(buf, sizeof(buf), "wifi", 6);
  radiobutton_create(cont1, buf);
  lv_snprintf(buf, sizeof(buf), "cputemp", 7);
  radiobutton_create(cont1, buf);

  // for (i = 0;i < 5;i++) {
  //     lv_snprintf(buf, sizeof(buf), "A %d", (int)i + 1);
  //     radiobutton_create(cont1, buf);

  // }
  /*Make the first checkbox checked*/
  // lv_obj_add_state(lv_obj_get_child(cont1, 0), LV_STATE_CHECKED);                                                             ///////////////////

  // lv_obj_t * cont2 = lv_obj_create(lv_scr_act());
  // lv_obj_set_flex_flow(cont2, LV_FLEX_FLOW_COLUMN);
  // lv_obj_set_size(cont2, lv_pct(40), lv_pct(80));
  // lv_obj_set_x(cont2, lv_pct(50));
  // lv_obj_add_event_cb(cont2, radio_event_handler, LV_EVENT_CLICKED, &active_index_2);

  // for (i = 0;i < 3;i++) {
  //     lv_snprintf(buf, sizeof(buf), "B %d", (int)i + 1);
  //     radiobutton_create(cont2, buf);
  // }

  // /*Make the first checkbox checked*/
  // lv_obj_add_state(lv_obj_get_child(cont2, 0), LV_STATE_CHECKED);
}

// void lv_example_label_x(void)
// {
//     // /*Create a style for the shadow*/
//     // static lv_style_t style_shadow;
//     // lv_style_init(&style_shadow);
//     // lv_style_set_text_opa(&style_shadow, LV_OPA_30);
//     // lv_style_set_text_color(&style_shadow, lv_color_black());

//     // /*Create a label for the shadow first (it's in the background)*/
//     // lv_obj_t * shadow_label = lv_label_create(lv_scr_act());
//     // lv_obj_add_style(shadow_label, &style_shadow, 0);

//     /*Create the main label*/
//     lv_obj_t * status_label = lv_label_create(lv_scr_act());
//     lv_label_set_text(status_label, "please");

//     // /*Set the same text for the shadow label*/
//     // lv_label_set_text(shadow_label, lv_label_get_text(status_label));

//     // /*Position the main label*/
//     // lv_obj_align(status_label, LV_ALIGN_CENTER, 0, 0);

//     // /*Shift the second label down and to the right by 2 pixel*/
//     // lv_obj_align_to(shadow_label, status_label, LV_ALIGN_TOP_LEFT, 2, 2);
//     lv_obj_t * status_label = lv_label_create(lv_scr_act());
//     lv_label_set_text(status_label, "please");
//     lv_obj_set_pos(status_label,140, 40);
// }

int main(int argc, char **argv)
{

  /*Initialize LVGL*/
  lv_init();

  /*Initialize the HAL (display, input devices, tick) for LVGL*/
  lv_port_disp_init(argv[1]);
  lv_port_indev_init(argv[2], true);

  cursor_set_hidden(true);

  // meter = lv_meter_create(lv_scr_act());
  // lv_obj_set_size(meter, 220, 220);
  // lv_obj_center(meter);

  // /*Create a scale for the minutes*/
  // /*61 ticks in a 360 degrees range (the last and the first line overlaps)*/
  // lv_meter_scale_t * scale_min = lv_meter_add_scale(meter);
  // lv_meter_set_scale_ticks(meter, scale_min, 61, 1, 10, lv_palette_main(LV_PALETTE_GREY));
  // lv_meter_set_scale_range(meter, scale_min, 0, 60, 360, 270);

  // /*Create another scale for the hours. It's only visual and contains only major ticks*/
  // lv_meter_scale_t * scale_hour = lv_meter_add_scale(meter);
  // lv_meter_set_scale_ticks(meter, scale_hour, 12, 0, 0, lv_palette_main(LV_PALETTE_GREY));               /*12 ticks*/
  // lv_meter_set_scale_major_ticks(meter, scale_hour, 1, 2, 20, lv_color_black(), 10);    /*Every tick is major*/
  // lv_meter_set_scale_range(meter, scale_hour, 1, 12, 330, 300);       /*[1..12] values in an almost full circle*/

  // /*Add a the hands from images*/
  // indic_min = lv_meter_add_needle_img(meter, scale_min, &img_hand_min, 5, 5);
  // indic_hour = lv_meter_add_needle_img(meter, scale_hour, &img_hand_hour, 5, 5);

  lv_example_checkbox_x();

  status_label = lv_label_create(lv_scr_act());
  lv_label_set_text(status_label, "please test!");
  lv_obj_set_pos(status_label, 130, 40);

  temp_high_label = lv_label_create(lv_scr_act());

  lv_obj_set_pos(temp_high_label, 130, 60);

  temp_low_label = lv_label_create(lv_scr_act());

  lv_obj_set_pos(temp_low_label, 130, 80);

  temp_now_label = lv_label_create(lv_scr_act());

  lv_obj_set_pos(temp_now_label, 130, 100);


  sprintf(temp_h_s, "h temp:%d", high_te);
  sprintf(temp_l_s, "l temp:%d", low_te);
  sprintf(temp_n_s, "n temp:%d", now_te);
  lv_label_set_text(temp_high_label, temp_h_s);
  lv_label_set_text(temp_low_label, temp_l_s);
  lv_label_set_text(temp_now_label, temp_n_s);


  pthread_create(&thread4_t, NULL, thread4, NULL);



  test_btn = lv_btn_create(lv_scr_act());
  test_btn_label = lv_label_create(test_btn);
  lv_label_set_text(test_btn_label, "start test!");
  lv_obj_center(test_btn_label);
  lv_obj_add_event_cb(test_btn, event_handler, LV_EVENT_ALL, NULL);
  lv_obj_set_size(test_btn, 200, 80);
  lv_obj_set_pos(test_btn, 20, 240);

  write_img_button = lv_btn_create(lv_scr_act());
  write_img_button_label = lv_label_create(write_img_button);
  lv_label_set_text(write_img_button_label, "write img");
  lv_obj_center(write_img_button_label);
  lv_obj_add_event_cb(write_img_button, event_handler1, LV_EVENT_ALL, NULL);
  lv_obj_set_size(write_img_button, 90, 40);
  lv_obj_set_pos(write_img_button, 130, 170);

  // lv_obj_t *bar1 = lv_bar_create(lv_scr_act());
  // lv_obj_set_size(bar1, 200, 20);
  // lv_obj_align(bar1, LV_ALIGN_CENTER, 0, 0);
  // lv_bar_set_anim_time(bar1, 2000);
  // lv_bar_set_value(bar1, 100, LV_ANIM_ON);

  while (1)
  {
    lv_timer_handler();
    usleep(1 * 1000);
  }

  return 0;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

uint32_t custom_tick_get(void)
{
  struct timespec ts;

  clock_gettime(CLOCK_MONOTONIC, &ts);

  uint32_t tick = ts.tv_sec * 1000 + ts.tv_nsec / 1000000;

  return tick;
}

static void lv_port_disp_init(const char *path)
{
  /*Linux frame buffer device init*/
  fbdev_init(path);

  uint32_t width, height;
  fbdev_get_sizes(&width, &height);

  printf("fbdev: %" PRIu32 " x %" PRIu32 "\n", width, height);

  /*A small buffer for LittlevGL to draw the screen's content*/
  uint32_t buf_size = width * height;
  lv_color_t *buf = malloc(buf_size * sizeof(lv_color_t));
  LV_ASSERT_MALLOC(buf);

  /*Initialize a descriptor for the buffer*/
  static lv_disp_draw_buf_t disp_buf;
  lv_disp_draw_buf_init(&disp_buf, buf, NULL, buf_size);

  /*Initialize and register a display driver*/
  static lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv);
  disp_drv.draw_buf = &disp_buf;
  disp_drv.flush_cb = fbdev_flush;
  disp_drv.hor_res = width;
  disp_drv.ver_res = height;
  lv_disp_drv_register(&disp_drv);
}

static void lv_port_indev_init(const char *path, bool show_cursor)
{
  evdev_init();
  evdev_set_file(path);

  static lv_indev_drv_t indev_drv;

  /*Register a encoder input device*/
  lv_indev_drv_init(&indev_drv);
  indev_drv.type = LV_INDEV_TYPE_POINTER;
  indev_drv.read_cb = evdev_read;
  lv_indev_t *indev = lv_indev_drv_register(&indev_drv);

  if (show_cursor)
  {
    /*Set a cursor for the mouse*/
    LV_IMG_DECLARE(mouse_cursor_icon);                  /*Declare the image file.*/
    lv_obj_t *cursor_obj = lv_img_create(lv_scr_act()); /*Create an image object for the cursor */
    lv_img_set_src(cursor_obj, &mouse_cursor_icon);     /*Set the image source*/
    lv_indev_set_cursor(indev, cursor_obj);             /*Connect the image  object to the driver*/
  }
}

static void cursor_set_hidden(bool en)
{
  // hide cursor echo -e "\033[?25l"
  // show cursor echo -e "\033[?25h"
  int ret = system(en ? "echo -e \"\033[?25l\"" : "echo -e \"\033[?25h\"");
  printf(__func__);
  printf(ret == 0 ? " OK" : " ERROR");
  printf("\n");
}
