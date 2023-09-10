/***
 * EXAMPLE OF MULTITHREAD WITH BUTTONS CLASS
 */

#include <thread>
#include <iostream>
#include <atomic>
#include <queue>
#include <mutex>
#include <unistd.h>
#include <mqtt/client.h>
#include <mqtt/async_client.h>
#include <json/json.h>
#include "./LSM6DSOX/LSM6DSOX.h"
#include "./IAQTracker/IAQTracker.h"
#include "./BME688/BME688.h"
#include "./TFTDriver/display_driver.h"
#include "./APDS9660/APDS9660_lib.h"
#include "./custom_gpio/custom_gpio.h"
#include "./PWMDriver/custom_PWM.h"
#include "./TFTDriver/fonts/FreeMono12pt7b.h"
#include "./TFTDriver/fonts/FreeMono9pt7b.h"
#include "./TFTDriver/icons.h"
#include "./thread_signals/thread_queue.h"
#include "./thread_signals/thread_flag.h"

//Maximum occupation allowed
#define MAX_OCCUPATION 100

//Time variables defined for MQTT
#define TIMEOUT 2
#define KEEPALIVE 500

//Values of joystick presses
#define JOY_UP 0
#define JOY_DOWN 1
#define JOY_LEFT 2
#define JOY_RIGHT 3
#define JOY_CENTER 4

//Run variable
static uint8_t on = 1;

//Widely used colors
static uint8_t white_color[] = { 0xFF, 0xFF };
static uint8_t red_color[] = { 0xf8, 0x00 };
static uint8_t green_color[] = { 0x07, 0xe0 };

//Coordinates of the square in which the letters will be put
static uint8_t text_square[4] = { 25, 30, 192, 212 };

//States
enum {
	HOME, TEMP, OCC, BRIGHT
} state;

//Acceleration data struct
typedef struct {
	float x;
	float y;
	float z;
} acceleration_val;

//Gas sensor data structure
typedef struct {
	float temp;
	float press;
	float humid;
	float iaq;
} gas_meas;

//Data queues for syncronization

Thread_queue<acceleration_val> accel_q;

Thread_queue<color_data> rgb_q;

Thread_queue<uint8_t> prox_q;

Thread_queue<gas_meas> gas_q;

Thread_queue<uint8_t> gesture;

Thread_queue<uint8_t> joystick_button;

//Flag to syncronize the MQTT thread
Thread_flag mqtt_sync = Thread_flag();

//Atomic variables for synchronization between threads
std::atomic<int> occ_data(0);
std::atomic<float> selected_temp(25);
std::atomic<int> brightness(25);
std::atomic<bool> pollution_danger(false);
std::atomic<bool> mqtt_connect(false);

//Thread function prototypes

void LSM6DSOX_thread();

void APDS9660_thread();

void BME688_thread();

void joystick_thread();

void mqtt_thread();

//Display writers

//Prints home page
void home_page(float temperature, float humidity, float iaq, int occup);

//Prints temperature section of home page
void print_home_temp(float temperature, uint8_t color[]);

//Prints humidity section of home page
void print_home_humid(float humidity);

//Prints IAQ section of home page
void print_home_air_quality(float iaq);

//Prints occupation section of home page
void print_home_occup(int occup);

//Prints button section of home page
void print_door_button(uint8_t color[]);

//Prints light simulation section of display
void print_light_sim(float illum);

//Prints Temperature selection page
void temp_page(float temperature);

//Prints Occupation page
void occ_page(int occupation);

//Prints button section to exit program page
void print_off_button(uint8_t color[]);

//Erases the display screen
void erase_display();

int main() {

	occ_data = 0;
	bool light_auto = true;
	bool door_auto = true;
	bool alarm_on;

	bool pwm_cycle = false;

	CustomGPIO::GPIO mqtt_up(12);
	CustomGPIO::GPIO auto_bright(16);
	CustomGPIO::GPIO alarm_active(26);
	CustomGPIO::GPIO doors(19);

	mqtt_up.setInput();
	auto_bright.setInput();
	alarm_active.setInput();
	doors.setInput();

	CustomPWM::PWMDriver pwm_driver(1);

	pwm_driver.setProperties(5000000, 2500000);

	std::thread accel_thread(LSM6DSOX_thread);

	std::thread color_thread(APDS9660_thread);

	std::thread gas_thread(BME688_thread);

	std::thread joy_thread(joystick_thread);

	std::thread thingsboard_th(mqtt_thread);

	Display_driver::init_display();

	gas_meas gas;
	acceleration_val accel_data;

	uint8_t button_pressed = 5;

	state = HOME;

	while (on) {

		if (!mqtt_up.read() && !mqtt_connect) {
			mqtt_sync.set();
			mqtt_connect = true;
		} else if (mqtt_up.read() && mqtt_connect) {
			mqtt_sync.set();
			mqtt_connect = false;
		}

		if (!doors.read() && !door_auto) {
			door_auto = true;
		} else if (doors.read() && door_auto) {
			door_auto = false;
		}

		if (auto_bright.read()) {
			light_auto = false;
		} else {
			light_auto = true;
		}

		if (alarm_active.read()) {
			alarm_on = false;
		} else {
			alarm_on = true;
		}

		switch (state) {
		case HOME:

			gas = gas_q.back_clear();

			home_page(gas.temp, gas.humid, gas.iaq, occ_data);

			if (light_auto)
				print_light_sim(rgb_q.back_clear().clear);
			else
				print_light_sim(brightness);

			button_pressed = joystick_button.pop(100);

			if (button_pressed == JOY_RIGHT) {
				erase_display();
				state = TEMP;
			} else if (button_pressed == JOY_CENTER) {
				accel_data = accel_q.back_clear();
				if ((abs(accel_data.x) > 0.5 || abs(accel_data.z) > 0.5)
						&& door_auto) {
					print_door_button(red_color);
				} else {
					print_door_button(green_color);
				}
			}

			break;

		case TEMP:

			temp_page(selected_temp);

			button_pressed = joystick_button.pop(100);

			if (light_auto)
				print_light_sim(rgb_q.back_clear().clear);
			else
				print_light_sim(brightness);

			if (button_pressed == JOY_RIGHT) {
				erase_display();
				state = OCC;
			} else if (button_pressed == JOY_LEFT) {
				erase_display();
				state = HOME;
			} else if (button_pressed == JOY_UP) {
				selected_temp = selected_temp + 0.5;
			} else if (button_pressed == JOY_DOWN) {
				selected_temp = selected_temp - 0.5;
			}

			break;

		case OCC:

			occ_page(occ_data);

			button_pressed = joystick_button.pop(100);

			if (light_auto)
				print_light_sim(rgb_q.back_clear().clear);
			else
				print_light_sim(brightness);

			if (button_pressed == JOY_RIGHT) {
				erase_display();
				state = BRIGHT;
			} else if (button_pressed == JOY_LEFT) {
				erase_display();
				state = TEMP;
			}

			break;

		case BRIGHT:

			if (light_auto)
				print_light_sim(rgb_q.back_clear().clear);
			else
				print_light_sim(brightness);

			button_pressed = joystick_button.pop(100);

			if (button_pressed == JOY_LEFT) {
				erase_display();
				state = OCC;
			} else if (button_pressed == JOY_CENTER) {
				erase_display();
				on = 0;
			} else if (button_pressed == JOY_UP) {
				brightness = brightness - 0.1 * OUTDOOR_ILLUMINANCE;
			} else if (button_pressed == JOY_DOWN) {
				brightness = brightness + 0.1 * OUTDOOR_ILLUMINANCE;
			}

			break;
		}

		if (pollution_danger && alarm_on) {
			if (pwm_cycle) {
				pwm_driver.disable();
				pwm_cycle = false;
			} else {
				pwm_driver.enable();
				pwm_cycle = true;
			}
		} else {
			pwm_driver.disable();
		}

	}

	accel_thread.join();
	color_thread.join();
	gas_thread.join();
	Display_driver::uninit();
	mqtt_sync.set();
	thingsboard_th.join();

	printf("Activate buzzer");

	pwm_driver.enable();

	std::this_thread::sleep_for(std::chrono::seconds(1));

	pwm_driver.disable();

	joy_thread.join();

	printf("Threads finished");

	return 0;
}

void message_callback(mqtt::const_message_ptr msg) {
	std::cout << "MSG_RECEIVED: " << msg->get_payload_str() << std::endl;
}

void mqtt_thread() {

	std::string address = "tfg-ec-not-free.westeurope.cloudapp.azure.com",
			client_name = "TicLpG0VoIoSKIVa0eqG";

	mqtt::async_client client(address, client_name);

	auto conOps =
			mqtt::connect_options_builder().user_name(client_name).connect_timeout(
					std::chrono::seconds(TIMEOUT)).keep_alive_interval(
					std::chrono::milliseconds(KEEPALIVE)).clean_session(true).finalize();

	client.set_message_callback(message_callback);

	bool connected = false;
	Json::Value telemetry_object;
	Json::FastWriter fastWriter;
	std::string msg;

	while (on) {

		if (!mqtt_connect && !connected)
			mqtt_sync.wait();

		if (mqtt_connect && !connected) {

			if (client.connect(conOps)->wait_for(1000)) {
				std::cout << "Connected Successfully" << std::endl;
				connected = true;
			} else {
				std::cerr << "Error in connection" << std::endl;
			}

		}

		if (connected) {

			telemetry_object["red"] = rgb_q.back().red;

			telemetry_object["green"] = rgb_q.back().green;

			telemetry_object["blue"] = rgb_q.back().blue;

			telemetry_object["clear"] = rgb_q.back().clear;

			telemetry_object["x"] = accel_q.back().x;

			telemetry_object["y"] = accel_q.back().y;

			telemetry_object["z"] = accel_q.back().z;

			telemetry_object["temp"] = gas_q.back().temp;

			telemetry_object["press"] = gas_q.back().press;

			telemetry_object["humid"] = gas_q.back().humid;
			telemetry_object["occupation"] = occ_data.load();

			if (gas_q.back().iaq != -1)
				telemetry_object["iaq"] = gas_q.back().iaq;

			msg = fastWriter.write(telemetry_object);

			client.publish("v1/devices/me/telemetry", msg);

			mqtt_sync.wait(5000);

		}
		if (!mqtt_connect && connected) {
			if (client.disconnect(2000)->wait_for(1000)) {
				std::cout << "Connected Successfully" << std::endl;
				connected = false;
			}
		}

	}
}

void joystick_thread() {

	CustomGPIO::GPIO up(25);
	CustomGPIO::GPIO down(7);
	CustomGPIO::GPIO right(18);
	CustomGPIO::GPIO left(24);
	CustomGPIO::GPIO puls(23);

	CustomGPIO::GPIO joystick[] = { up, down, left, right, puls };

	for (int i = 0; i < 5; i++) {
		joystick[i].setInput(CustomGPIO::GPIO_INT_RISING);
	}

	while (on) {
		int button_pressed = CustomGPIO::GPIO::waits(joystick, 5, 100);
		if (button_pressed >= 0) {
			std::this_thread::sleep_for(std::chrono::milliseconds(20));
			if (joystick[button_pressed].read() == 1) {
				joystick_button.push(button_pressed);
				printf("Button_pressed: %d\n", button_pressed);
			}
		}
	}

}

void LSM6DSOX_thread() {

	LSM6DSOX accel(LSM6DSOX_52_HZ_ODR, LSM6DSOX_52_HZ_ODR, ACC_2_G_FSR,
			GYR_250_DPS_FSR);

	int i = 0;

	acceleration_val accel_data;

	while (on) {

		accel.get_acc_values(&accel_data.x, &accel_data.y, &accel_data.z);

		accel_q.push(accel_data);

		i++;

		std::this_thread::sleep_for(std::chrono::milliseconds(50));

	}

	return;

}

void APDS9660_thread() {

	APDS9660_Master::conf_proximity(3, 3);
	APDS9660_Master::conf_rgbc(3);

	APDS9660_Master::conf_gesture(3, 5, 5);

	uint8_t valid_ges = 0;

	uint8_t prox = 0;

	color_data color;

	while (on) {

		prox = APDS9660_Master::read_proximity();

		color = APDS9660_Master::read_rgbc();

		valid_ges = APDS9660_Master::check_gesture();

		if (valid_ges) {

			uint8_t gest = APDS9660_Master::read_ges_fifo_ud();
			if (gest == UP) {
				occ_data++;
			} else if (gest == DOWN) {
				occ_data--;
			}
		}

		prox_q.push(prox);

		rgb_q.push(color);

		std::this_thread::sleep_for(std::chrono::milliseconds(50));
	}

	return;
}

void BME688_thread() {

	BME688 gas_sensor(25, 0);

	gas_sensor.init();

	gas_sensor.set_oversamplings(OVSP_4_X, OVSP_4_X, OVSP_4_X);

	gas_sensor.set_temp_offset(8);

	gas_sensor.set_heater_configurations(true, 500, 100);

	sleep(0.1);

	IAQTracker tracker;

	float temp = 0, press = 0, humid = 0, gas_resistance = 0, iaq = 0;

	gas_meas data;

	while (on) {

		gas_sensor.get_data_one_measure(&temp, &press, &humid, &gas_resistance);

		if (tracker.get_IAQ(&iaq, temp, humid, gas_resistance) >= 0) {
			data.iaq = iaq;
			if (iaq > 200) {
				pollution_danger = true;
			} else if (iaq <= 200 && pollution_danger == true) {
				pollution_danger = false;
			}
		} else {
			data.iaq = -1;
		}

		data.temp = temp;

		data.press = press;

		data.humid = humid;

		gas_q.push(data);

		std::this_thread::sleep_for(std::chrono::milliseconds(50));

	}

	return;

}

void home_page(float temperature, float humidity, float iaq, int occup) {

	uint8_t low_temp_color[] = { 0x00, 0x1f };
	uint8_t high_temp_color[] = { 0xf8, 0x00 };

	if (temperature == selected_temp) {
		print_home_temp(temperature, white_color);
	} else if (temperature < selected_temp) {
		print_home_temp(temperature, low_temp_color);
	} else if (temperature > selected_temp) {
		print_home_temp(temperature, high_temp_color);
	}

	print_home_humid(humidity);

	print_home_air_quality(iaq);

	print_home_occup(occup);

	Display_driver::draw_icon(0, 175, 240, divider, sizeof(divider) / 2);

	print_door_button(white_color);

	Display_driver::draw_icon(195, 120 - 18, 25, mini_temp,
			sizeof(mini_temp) / 2);

	Display_driver::draw_icon(225, 120 - 7, 15, arrow_right,
			sizeof(arrow_right) / 2);

}

void print_door_button(uint8_t color[]) {

	char phrase[] = "Abrir puertas";

	Display_driver::write_fast_string(50, 195, phrase, color, FreeMono9pt7b);

	Display_driver::draw_icon_col(105, 210, 30, button_image,
			sizeof(button_image) / 2, color);

}

void print_off_button(uint8_t color[]) {

	char phrase[] = "Apagar";

	Display_driver::write_fast_string(85, 195, phrase, color, FreeMono9pt7b);

	Display_driver::draw_icon_col(105, 210, 30, button_image,
			sizeof(button_image) / 2, color);

}

void occ_page(int occupation) {

	char occ[8];

	sprintf(occ, "%03d/%d", occupation, MAX_OCCUPATION);
	Display_driver::draw_icon(120 - (strlen(occ) * 14) / 2, 120 - 7, 4 * 14,
			occ_eraser, sizeof(occ_eraser) / 4);

	Display_driver::write_fast_string(120 - (strlen(occ) * 14) / 2, 120 - 7,
			occ, white_color, FreeMono12pt7b);

	Display_driver::draw_icon(120 - 7, 50, 15, person_icon,
			sizeof(person_icon) / 2);

	Display_driver::draw_icon(225, 120 - 7, 15, arrow_right,
			sizeof(arrow_right) / 2);

	Display_driver::draw_icon(0, 120 - 7, 15, arrow_left,
			sizeof(arrow_left) / 2);

	Display_driver::draw_icon(15, 120 - 18, 25, mini_temp,
			sizeof(mini_temp) / 2);

	Display_driver::draw_icon(195, 120 - 17, 30, light_icon,
			sizeof(light_icon) / 2);
}

void temp_page(float temperature) {

	char temp[12];

	sprintf(temp, "Temp: %.2fC", temperature);

	Display_driver::write_fast_string(37, 120 - 7, temp, white_color,
			FreeMono12pt7b);

	Display_driver::draw_icon(120 - 35, 40, 70, upward_triangle,
			sizeof(upward_triangle) / 2);

	Display_driver::draw_icon(120 - 35, 240 - 40 - 38, 70, downward_triangle,
			sizeof(downward_triangle) / 2);

	Display_driver::draw_icon(225, 120 - 7, 15, arrow_right,
			sizeof(arrow_right) / 2);

	Display_driver::draw_icon(0, 120 - 7, 15, arrow_left,
			sizeof(arrow_left) / 2);

	Display_driver::draw_icon(12, 120 - 9, 25, home_icon,
			sizeof(home_icon) / 2);

	Display_driver::draw_icon(210, 120 - 14, 15, person_icon,
			sizeof(person_icon) / 2);
}

void print_light_sim(float illum) {

	float brightness_needed;

	if (illum < OUTDOOR_ILLUMINANCE)
		brightness_needed = (1 - illum / OUTDOOR_ILLUMINANCE);
	else
		brightness_needed = 0;

	char bright_val[12];
	uint8_t color1;
	uint8_t color2;

	sprintf(bright_val, "Bright: %d%%", (int) (brightness_needed * 100));

	uint8_t colorG = 0x3F * brightness_needed;

	uint8_t colorRB = 0x1F * brightness_needed;

	color1 = colorRB << 3 | colorG >> 3;

	color2 = colorG << 5 | colorRB;

	uint8_t bright[] = { color1, color2 };

	Display_driver::draw_icon_col(0, 0, 240, top_row, sizeof(top_row) / 2,
			bright);

	if (state == BRIGHT) {
		Display_driver::draw_icon(50 + 7 * 14, 120 - 14, 84, text_eraser,
				sizeof(text_eraser) / 2);
		Display_driver::write_fast_string(50, 120 - 14, bright_val, white_color,
				FreeMono12pt7b);

		Display_driver::draw_icon(120 - 35, 40 - 7, 70, upward_triangle,
				sizeof(upward_triangle) / 2);

		Display_driver::draw_icon(120 - 35, 240 - 40 - 38 - 7, 70,
				downward_triangle, sizeof(downward_triangle) / 2);

		Display_driver::draw_icon(0, 120 - 7, 15, arrow_left,
				sizeof(arrow_left) / 2);

		Display_driver::draw_icon(15, 120 - 14, 15, person_icon,
				sizeof(person_icon) / 2);
		print_off_button(white_color);
	}

}

void print_home_temp(float temperature, uint8_t color[]) {

	int top_sep = 45, top_margin = 12;

	char temp[12];

	sprintf(temp, "Temp: %.2fC", temperature);
	Display_driver::draw_icon(text_square[0] + 6 * 14, top_sep + top_margin, 84,
			text_eraser, sizeof(text_eraser) / 2);
	Display_driver::write_fast_string(text_square[0], top_sep + top_margin,
			temp, color, FreeMono12pt7b);

}

void print_home_humid(float humidity) {

	int top_sep = 45, top_margin = 12, line_height = 30;

	char humid[11];

	sprintf(humid, "Hum: %.2f%%", humidity);
	Display_driver::draw_icon(text_square[0] + 5 * 14,
			top_sep + line_height + top_margin, 84, text_eraser,
			sizeof(text_eraser) / 2);
	Display_driver::write_fast_string(text_square[0],
			top_sep + line_height + top_margin, humid, white_color,
			FreeMono12pt7b);

}

void print_home_air_quality(float iaq) {

	int top_sep = 45, top_margin = 12, line_height = 30;

	char air_quality[12];

	uint8_t iaq_color[] = { 0xFF, 0xFF };

	if (iaq >= 0) {
		sprintf(air_quality, "IAQ: %.2f", iaq);

		if (iaq > 100 && iaq < 200) {
			iaq_color[0] = 0xfe;
			iaq_color[1] = 0xe0;
		} else if (iaq >= 200) {
			iaq_color[0] = 0xf8;
			iaq_color[1] = 0x00;
		}

	} else {
		sprintf(air_quality, "IAQ: init");
		iaq_color[0] = 0xf8;
		iaq_color[1] = 0x00;
	}
	Display_driver::draw_icon(text_square[0] + 5 * 14,
			top_sep + 2 * line_height + top_margin, 84, text_eraser,
			sizeof(text_eraser) / 2);

	Display_driver::write_fast_string(text_square[0],
			top_sep + 2 * line_height + top_margin, air_quality, iaq_color,
			FreeMono12pt7b);

}

void print_home_occup(int occup) {

	int top_sep = 45, top_margin = 12, line_height = 30;

	char occ[11];

	sprintf(occ, "Aforo: %d", occup);
	Display_driver::draw_icon(text_square[0] + 6 * 14,
			top_sep + 3 * line_height + top_margin, 84, text_eraser,
			sizeof(text_eraser) / 2);
	Display_driver::write_fast_string(text_square[0],
			top_sep + 3 * line_height + top_margin, occ, white_color,
			FreeMono12pt7b);

}

void erase_display() {

	uint8_t background_color[] = { BACKGROUND, BACKGROUND };

	Display_driver::draw_icon_col(0, 30, 240, erase, sizeof(erase) / 2,
			background_color);

}
