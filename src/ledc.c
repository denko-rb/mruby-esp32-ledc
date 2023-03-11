#include <mruby.h>
#include <mruby/value.h>

#include "soc/io_mux_reg.h"
#include "driver/gpio.h"
#include "driver/ledc.h"

static mrb_value
mrb_esp32_ledc_timer_config(mrb_state *mrb, mrb_value self) {
  mrb_value group, timer, res, freq;

  mrb_get_args(mrb, "oooo", &group, &timer, &res, &freq);

  if (!mrb_fixnum_p(group) || !mrb_fixnum_p(timer) || !mrb_fixnum_p(res) || !mrb_fixnum_p(freq)) {
    return mrb_nil_value();
  }

  // Timer config
  ledc_timer_config_t ledc_timer = {
      .speed_mode       = mrb_fixnum(group),
      .timer_num        = mrb_fixnum(timer),
      .duty_resolution  = mrb_fixnum(res),
      .freq_hz          = mrb_fixnum(freq),
      .clk_cfg          = LEDC_AUTO_CLK
  };
  ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

  return self;
}

static mrb_value
mrb_esp32_ledc_channel_config(mrb_state *mrb, mrb_value self) {
  // Only set pin and channel for now. Can add more config later.
  mrb_value pin, group, timer, ch;

  mrb_get_args(mrb, "oooo", &pin, &group, &timer, &ch);

  if (!mrb_fixnum_p(pin) || !mrb_fixnum_p(group) || !mrb_fixnum_p(timer) || !mrb_fixnum_p(ch)) {
    return mrb_nil_value();
  }

  // Channel config
  ledc_channel_config_t ledc_channel = {
      .gpio_num       = mrb_fixnum(pin),
      .speed_mode     = mrb_fixnum(group),
      .timer_sel      = mrb_fixnum(timer),
      .channel        = mrb_fixnum(ch),
      .intr_type      = LEDC_INTR_DISABLE,
      .duty           = 0, // Set duty to 0%
      .hpoint         = 0
  };
  ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));

  return self;
}

static mrb_value
mrb_esp32_ledc_write(mrb_state *mrb, mrb_value self) {
  mrb_value group, ch, duty;

  mrb_get_args(mrb, "ooo", &group, &ch, &duty);

  if (!mrb_fixnum_p(group) || !mrb_fixnum_p(ch) || !mrb_fixnum_p(duty)) {
    return mrb_nil_value();
  }
  
  // Write the duty cycle.
  ESP_ERROR_CHECK(ledc_set_duty(mrb_fixnum(group), mrb_fixnum(ch), mrb_fixnum(duty)));
  ESP_ERROR_CHECK(ledc_update_duty(mrb_fixnum(group), mrb_fixnum(ch)));
  
  return self;
}

static mrb_value
mrb_esp32_ledc_detach(mrb_state *mrb, mrb_value self) {
  mrb_value pin;

  mrb_get_args(mrb, "o", &pin);

  if (!mrb_fixnum_p(pin)) {
    return mrb_nil_value();
  }
  
  // PIN_FUNC_GPIO = 2. Normal GPIO.
  // Last param is output inversion.
  gpio_iomux_out(mrb_fixnum(pin), PIN_FUNC_GPIO, false);
    
  return self;
}

void
mrb_mruby_esp32_ledc_gem_init(mrb_state* mrb)
{
  struct RClass *esp32, *ledc, *constants;

  esp32 = mrb_define_module(mrb, "ESP32");

  ledc = mrb_define_module_under(mrb, esp32, "LEDC");
  mrb_define_module_function(mrb, ledc, "timer_config", mrb_esp32_ledc_timer_config, MRB_ARGS_REQ(4));
  mrb_define_module_function(mrb, ledc, "channel_config", mrb_esp32_ledc_channel_config, MRB_ARGS_REQ(4));
  mrb_define_module_function(mrb, ledc, "write", mrb_esp32_ledc_write, MRB_ARGS_REQ(3));
  mrb_define_module_function(mrb, ledc, "detach", mrb_esp32_ledc_write, MRB_ARGS_REQ(1));

  constants = mrb_define_module_under(mrb, esp32, "Constants");

#define define_const(SYM) \
  do { \
    mrb_define_const(mrb, constants, #SYM, mrb_fixnum_value(SYM)); \
  } while (0)

  // LEDC channel groups.
  // High speed not available on some devices.
  define_const(LEDC_HIGH_SPEED_MODE);
  define_const(LEDC_LOW_SPEED_MODE);
  
  // LEDC channel numbers. 8 channels per group.
  define_const(LEDC_CHANNEL_0);
  define_const(LEDC_CHANNEL_1);
  define_const(LEDC_CHANNEL_2);
  define_const(LEDC_CHANNEL_3);
  define_const(LEDC_CHANNEL_4);
  define_const(LEDC_CHANNEL_5);
  define_const(LEDC_CHANNEL_6);
  define_const(LEDC_CHANNEL_7);
  define_const(LEDC_CHANNEL_MAX);

  // LEDC timer numbers. 4 timers per group of 8 channels.
  define_const(LEDC_TIMER_0);
  define_const(LEDC_TIMER_1);
  define_const(LEDC_TIMER_2);
  define_const(LEDC_TIMER_3);
  define_const(LEDC_TIMER_MAX);

  // LEDC timer resolutions.
  define_const(LEDC_TIMER_1_BIT);
  define_const(LEDC_TIMER_2_BIT);
  define_const(LEDC_TIMER_3_BIT);
  define_const(LEDC_TIMER_4_BIT);
  define_const(LEDC_TIMER_5_BIT);
  define_const(LEDC_TIMER_6_BIT);
  define_const(LEDC_TIMER_7_BIT);
  define_const(LEDC_TIMER_8_BIT);
  define_const(LEDC_TIMER_9_BIT);
  define_const(LEDC_TIMER_10_BIT);
  define_const(LEDC_TIMER_11_BIT);
  define_const(LEDC_TIMER_12_BIT);
  define_const(LEDC_TIMER_13_BIT);
  define_const(LEDC_TIMER_14_BIT);
  define_const(LEDC_TIMER_15_BIT);
  define_const(LEDC_TIMER_16_BIT);
  define_const(LEDC_TIMER_17_BIT);
  define_const(LEDC_TIMER_18_BIT);
  define_const(LEDC_TIMER_19_BIT);
  define_const(LEDC_TIMER_20_BIT);
  define_const(LEDC_TIMER_BIT_MAX);
}

void
mrb_mruby_esp32_ledc_gem_final(mrb_state* mrb)
{
}
