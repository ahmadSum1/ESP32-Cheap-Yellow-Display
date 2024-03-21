// # ============================================================ 
// # Hardware related setup
// #
// # Setup the pins that control the RGB LED
// output:
//   - platform: ledc
//     pin: GPIO4
//     id: led_red
//     inverted: true
//   - platform: ledc
//     pin: GPIO16
//     id: led_green
//     inverted: true
//   - platform: ledc
//     pin: GPIO17
//     id: led_blue
//     inverted: true


#define CYD_LED_BLUE 17
#define CYD_LED_RED 4
#define CYD_LED_GREEN 16

#include <RGBLed.h>

RGBLed led(CYD_LED_RED, CYD_LED_GREEN, CYD_LED_BLUE, RGBLed::COMMON_CATHODE);

void setup() { }

void loop() {
	
	// Flash red color, interval 100ms
	led.flash(RGBLed::RED, 100);

	// Flash green color, interval 100ms
	led.flash(0, 255, 0, 100);

	// Flash red color, during 250ms with interval 100ms
	led.flash(RGBLed::RED, 250, 100);

	// Flash green color, during 250ms with interval 100ms
	led.flash(0, 255, 0, 250, 100);
}
