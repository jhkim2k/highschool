// author: Kwang-Hyun Park (akaii@kw.ac.kr)

#include <stdio.h>
#include <conio.h>
#include "roboid.h"

#define NUM_HAMSTERS 4

// Example00: Scan
void example_scan(void) {
	scan(); // print serial ports
}

// Example01: Simple Movement
void example_simple_movement(void) {
	hamster_create();

	hamster_wheels(50, 50); // move forward
	wait(500); // 0.5 seconds

	hamster_wheels(-50, -50); // move backward
	wait(500); // 0.5 seconds

	hamster_stop(); // stop

	// don't forget to dispose
	dispose_all(); // disconnect and release memory
}

// Example02: Basic Movement
void example_basic_movement(void) {
	int i;

	hamster_create();

	for(i = 0; i < 10; ++i) {
		hamster_wheels(50, 50); // move forward
		wait(500); // 0.5 seconds

		hamster_wheels(-50, -50); // move backward
		wait(500); // 0.5 seconds

		hamster_wheels(-50, 50); // turn left
		wait(500); // 0.5 seconds
	}
	hamster_stop(); // stop

	// don't forget to dispose
	dispose_all(); // disconnect and release memory
}

// Example03: Acceleration
void example_acceleration(void) {
	int i;

	hamster_create();

	// acceleration
	for(i = 0; i < 50; ++i) {
		hamster_wheels(i, i);
		wait(20); // 20 msec
	}

	// 0.5 seconds
	wait(500);

	// deceleration
	for(i = 50; i >= 0; --i) {
		hamster_wheels(i, i);
		wait(20); // 20 msec
	}

	// don't forget to dispose
	dispose_all(); // disconnect and release memory
}

// Example04: Led
void example_led(void) {
	hamster_create();

	hamster_leds(HAMSTER_LED_RED, HAMSTER_LED_GREEN); // turn on both LEDs
	wait(500); // 0.5 seconds

	hamster_left_led(0); // turn off left LED
	wait(500); // 0.5 seconds

	hamster_left_led(HAMSTER_LED_BLUE); // turn on left LED
	wait(500); // 0.5 seconds

	hamster_right_led(0); // turn off right LED
	wait(500); // 0.5 seconds

	hamster_right_led(HAMSTER_LED_YELLOW); // turn on right LED
	wait(500); // 0.5 seconds

	hamster_leds(0, 0); // turn off both LEDs

	// don't forget to dispose
	dispose_all(); // disconnect and release memory
}

// Example05: Siren
void example_siren(void) {
	int i, hz;

	hamster_create();

	for(i = 0; i < 10; ++i) {
		for(hz = 500; hz <= 700; hz += 10) {
			hamster_buzzer(hz);
			wait(20); // 20 msec
		}
	}
	hamster_buzzer(0); // clear buzzer

	// don't forget to dispose
	dispose_all(); // disconnect and release memory
}

// Example06: Melody
void example_melody(void) {
	int i, j;

	hamster_create();

	for(i = 0; i < 2; ++i) {
		for(j = 0; j < 2; ++j) {
			hamster_note(HAMSTER_NOTE_C_4, 0.5);
			hamster_note(HAMSTER_NOTE_E_4, 0.5);
			hamster_note(HAMSTER_NOTE_G_4, 0.5);
		}
		for(j = 0; j < 3; ++j) {
			hamster_note(HAMSTER_NOTE_A_4, 0.5);
		}
		hamster_note(HAMSTER_NOTE_G_4, 1);
		hamster_note(0, 0.5f);

		for(j = 0; j < 3; ++j) {
			hamster_note(HAMSTER_NOTE_F_4, 0.5);
		}
		for(j = 0; j < 3; ++j) {
			hamster_note(HAMSTER_NOTE_E_4, 0.5);
		}
		for(j = 0; j < 3; ++j) {
			hamster_note(HAMSTER_NOTE_D_4, 0.5);
		}
		hamster_note(HAMSTER_NOTE_C_4, 1);
		hamster_note(0, 0.5);

		hamster_tempo(120); // speed up
	}

	// don't forget to dispose
	dispose_all(); // disconnect and release memory
}

// Example07: Sensor
void example_sensor(void) {
	int i;

	hamster_create();

	for(i = 0; i < 500; ++i) { // 10 seconds
		printf("Signal Strength: %d\n", hamster_signal_strength());
		printf("Proximity: %d, %d\n", hamster_left_proximity(), hamster_right_proximity());
		printf("Floor: %d, %d\n", hamster_left_floor(), hamster_right_floor());
		printf("Acceleration: %d, %d, %d\n", hamster_acceleration_x(), hamster_acceleration_y(), hamster_acceleration_z());
		printf("Light: %d\n", hamster_light());
		printf("Temperature: %d\n", hamster_temperature());
		printf("Input: %d, %d\n\n", hamster_input_a(), hamster_input_b());

		wait(20); // 20 msec
	}

	// don't forget to dispose
	dispose_all(); // disconnect and release memory
}

// Example08: Staggering Walk
void example_staggering_walk(void) {
	int acc, count;

	hamster_create();

	for(count = 10; count > 0; ) {
		hamster_wheels(-100, -100); // move backward
		acc = hamster_acceleration_x();
		if(acc > 2000 || acc < -2000) {
			hamster_wheels(100, 100); // move forward
			wait(250); // 0.25 seconds

			hamster_wheels(-50, 50); // turn left
			wait(500); // 0.5 seconds

			-- count;
		}
	}
	hamster_stop(); // stop

	// don't forget to dispose
	dispose_all(); // disconnect and release memory
}

// Example09: Theremin Sound
void example_theremin_sound(void) {
	double pitch = 0;
	int proximity, i;

	hamster_create();

	for(i = 0; i < 500; ++i) { // 10 seconds
		proximity = hamster_left_proximity();
		if(proximity < 10) {
			proximity = 0;
		}
		pitch = (pitch * 9 + proximity) / 10.0;
		hamster_pitch(pitch);

		wait(20); // 20 msec
	}
	hamster_pitch(0); // clear sound

	// don't forget to dispose
	dispose_all(); // disconnect and release memory
}

// Example10: Simple Hand Follower
void example_simple_hand_follower(void) {
	int proximity, left_speed, right_speed, i;

	hamster_create();

	for(i = 0; i < 500; ++i) { // 10 seconds
		// left wheel
		proximity = hamster_left_proximity();
		if(proximity > 15) {
			left_speed = (40 - proximity) * 4;
		} else {
			left_speed = 0;
		}

		// right wheel
		proximity = hamster_right_proximity();
		if(proximity > 15) {
			right_speed = (40 - proximity) * 4;
		} else {
			right_speed = 0;
		}

		hamster_wheels(left_speed, right_speed);
		wait(20); // 20 msec
	}
	hamster_stop(); // stop

	// don't forget to dispose
	dispose_all(); // disconnect and release memory
}

// Example11: Builtin Line Tracer
void example_builtin_line_tracer(void) {
	hamster_create();

	hamster_line_tracer_mode(HAMSTER_LINE_TRACER_MODE_BLACK_LEFT_SENSOR);
	wait(5000); // 5 seconds

	hamster_line_tracer_speed(8); // speed up
	wait(5000); // 5 seconds

	hamster_line_tracer_mode(HAMSTER_LINE_TRACER_MODE_BLACK_RIGHT_SENSOR);
	wait(5000); // 5 seconds

	hamster_line_tracer_speed(5); // speed down
	wait(5000); // 5 seconds

	hamster_line_tracer_mode(HAMSTER_LINE_TRACER_MODE_BLACK_BOTH_SENSORS);
	wait(5000); // 5 seconds

	hamster_line_tracer_mode(0); // clear line tracer mode

	// don't forget to dispose
	dispose_all(); // disconnect and release memory
}

// Example12: Builtin Line Tracer Intersection
void example_builtin_line_tracer_intersection(void) {
	hamster_create();
	
	// move to a front intersection
	hamster_line_tracer_mode(HAMSTER_LINE_TRACER_MODE_BLACK_MOVE_FORWARD);

	// move to a left intersection
	hamster_line_tracer_mode(HAMSTER_LINE_TRACER_MODE_BLACK_TURN_LEFT);

	// move to a right intersection
	hamster_line_tracer_mode(HAMSTER_LINE_TRACER_MODE_BLACK_TURN_RIGHT);

	// u-turn and move to an intersection
	hamster_line_tracer_mode(HAMSTER_LINE_TRACER_MODE_BLACK_UTURN);
		
	// don't forget to dispose
	dispose_all(); // disconnect and release memory
}

// Example13: Hand Detector
void example_hand_detector(void) {
	hamster_create();

	// wait while the value of the left proximity is less than 50
	while(hamster_left_proximity() < 50) {
		wait(10); // 10 msec
	}

	hamster_beep(); // beep

	hamster_wheels(-50, -50); // move backward
	wait(500); // 0.5 seconds

	hamster_stop(); // stop

	// don't forget to dispose
	dispose_all(); // disconnect and release memory
}

// Example14: Move On Board
// This example requires a Hamster board (http://hamster.school/en/tutorial/class or http://hamster.school/ko/tutorial/class)
void example_move_on_board(void) {
	hamster_create();

	// move one space forward
	hamster_board_forward();

	// turn left once
	hamster_board_left();

	// move one space forward
	hamster_board_forward();

	// turn right once
	hamster_board_right();

	// move one space forward
	hamster_board_forward();

	// don't forget to dispose
	dispose_all(); // disconnect and release memory
}

// Example15: Extension - Button
// This example requires a Hamster's extension kit
void example_extension_button(void) {
	int i;

	hamster_create();

	// set a port B to the digital input mode
	hamster_io_mode_b(HAMSTER_IO_MODE_DIGITAL_INPUT);

	for(i = 0; i < 500; ++i) { // 10 seconds
		if(hamster_input_b() == 0) {
			hamster_buzzer(1000);
		} else {
			hamster_buzzer(0);
		}
		wait(20); // 20 msec
	}

	// don't forget to dispose
	dispose_all(); // disconnect and release memory
}

// Example16: Extension - Led Blink
// This example requires a Hamster's extension kit
void example_extension_led_blink(void) {
	int i;

	hamster_create();

	// set a port A to the digital output mode
	hamster_io_mode_a(HAMSTER_IO_MODE_DIGITAL_OUTPUT);

	for(i = 0; i < 5; ++i) {
		hamster_output_a(1); // turn on
		wait(1000); // 1 second

		hamster_output_a(0); // turn off
		wait(1000); // 1 second
	}

	// don't forget to dispose
	dispose_all(); // disconnect and release memory
}

// Example17: Extension - Potentiometer
// This example requires a Hamster's extension kit
void example_extension_potentiometer(void) {
	int potentiometer, i;

	hamster_create();

	// set a port A to the analog input mode
	hamster_io_mode_a(HAMSTER_IO_MODE_ANALOG_INPUT);

	for(i = 0; i < 500; ++i) { // 10 seconds
		potentiometer = hamster_input_a();
		hamster_buzzer(potentiometer);
		wait(20); // 20 msec
	}

	// don't forget to dispose
	dispose_all(); // disconnect and release memory
}

// Example18: Extension - Smooth Led
// This example requires a Hamster's extension kit
void example_extension_smooth_led(void) {
	int i, j;

	hamster_create();

	// set a port A to the PWM output mode
	hamster_io_mode_a(HAMSTER_IO_MODE_PWM_OUTPUT);

	for(i = 0; i < 3; ++i) {
		// turn on
		for(j = 0; j <= 255; j += 5) {
			hamster_output_a(j);
			wait(20); // 20 msec
		}

		// turn off
		for(j = 255; j >= 0; j -= 5) {
			hamster_output_a(j);
			wait(20); // 20 msec
		}
	}

	// don't forget to dispose
	dispose_all(); // disconnect and release memory
}

// Example19: Extension - Servo
// This example requires a Hamster's extension kit
void example_extension_servo(void) {
	int i;

	hamster_create();

	// set a port A to the servo output mode
	hamster_io_mode_a(HAMSTER_IO_MODE_SERVO_OUTPUT);

	for(i = 0; i < 3; ++i) {
		hamster_output_a(180); // move to 180 degrees
		wait(1000); // 1 second

		hamster_output_a(10); // move to home
		wait(1000); // 1 second
	}

	// don't forget to dispose
	dispose_all(); // disconnect and release memory
}

// Example20: Dual Theremin Sound
void example_dual_theremin_sound(void) {
	double pitch1 = 0, pitch2 = 0;
	int proximity, i;

	Hamster* hamster1 = hamster_create();
	Hamster* hamster2 = hamster_create();

	// wait until two robots are ready
	wait_until_ready();

	for(i = 0; i < 500; ++i) { // 10 seconds
		proximity = hamster1->left_proximity();
		if(proximity < 10) proximity = 0;
		pitch1 = (pitch1 * 9 + proximity) / 10.0;
		hamster1->pitch(pitch1);

		proximity = hamster2->left_proximity();
		if(proximity < 10) proximity = 0;
		pitch2 = (pitch2 * 9 + proximity) / 10.0;
		hamster2->pitch(pitch2);

		wait(20); // 20 msec
	}

	// clear sounds
	hamster1->pitch(0);
	hamster2->pitch(0);

	// don't forget to dispose
	dispose_all(); // disconnect and release memory
}

// Example21: Mutiple Basic Movement
void example_multiple_basic_movement(void) {
	Hamster* hamsters[NUM_HAMSTERS];
	int i, j;

	for(i = 0; i < NUM_HAMSTERS; ++i) {
		hamsters[i] = hamster_create();
	}

	// wait until four robots are ready
	wait_until_ready();

	for(i = 0; i < 10; ++i) {
		// move forward
		for(j = 0; j < NUM_HAMSTERS; ++j) {
			hamsters[j]->wheels(50, 50);
		}
		wait(500); // 0.5 seconds

		// move backward
		for(j = 0; j < NUM_HAMSTERS; ++j) {
			hamsters[j]->wheels(-50, -50);
		}
		wait(500); // 0.5 seconds

		// turn left
		for(j = 0; j < NUM_HAMSTERS; ++j) {
			hamsters[j]->wheels(-50, 50);
		}
		wait(500); // 0.5 seconds
	}

	// stop
	for(i = 0; i < NUM_HAMSTERS; ++i) {
		hamsters[i]->stop();
	}

	// don't forget to dispose
	dispose_all(); // disconnect and release memory
}

// Example22: Multiple Hand Follower
void example_multiple_hand_follower(void) {
	Hamster* hamsters[NUM_HAMSTERS];
	int proximity, left_speed, right_speed;
	int i, j;

	for(i = 0; i < NUM_HAMSTERS; ++i) {
		hamsters[i] = hamster_create();
	}

	// wait until four robots are ready
	wait_until_ready();

	for(i = 0; i < 500; ++i) { // 10 seconds
		for(j = 0; j < NUM_HAMSTERS; ++j) {
			// left wheel
			proximity = hamsters[j]->left_proximity();
			if(proximity > 15) {
				left_speed = (40 - proximity) * 4;
			} else {
				left_speed = 0;
			}

			// right wheel
			proximity = hamsters[j]->right_proximity();
			if(proximity > 15) {
				right_speed = (40 - proximity) * 4;
			} else {
				right_speed = 0;
			}
			hamsters[j]->wheels(left_speed, right_speed);
		}
		wait(20); // 20 msec
	}

	// stop
	for(i = 0; i < NUM_HAMSTERS; ++i) {
		hamsters[i]->stop();
	}

	// don't forget to dispose
	dispose_all(); // disconnect and release memory
}

// Example23: Advanced - Sensor
void sensor_execute(void* arg) {
	printf("Signal Strength: %d\n", hamster_signal_strength());
	printf("Proximity: %d, %d\n", hamster_left_proximity(), hamster_right_proximity());
	printf("Floor: %d, %d\n", hamster_left_floor(), hamster_right_floor());
	printf("Acceleration: %d, %d, %d\n", hamster_acceleration_x(), hamster_acceleration_y(), hamster_acceleration_z());
	printf("Light: %d\n", hamster_light());
	printf("Temperature: %d\n", hamster_temperature());
	printf("Input: %d, %d\n\n", hamster_input_a(), hamster_input_b());
}

void example_advanced_sensor(void) {
	hamster_create();

	// set a periodic (20 msec) callback
	set_executable(sensor_execute, NULL);

	wait(10000); // 10 seconds

	// don't forget to dispose
	dispose_all(); // disconnect and release memory
}

// Example24: Advanced - Theremin Sound
void theremin_sound_execute(void* arg) {
	double* pitch = (double*)arg;
	int proximity = hamster_left_proximity();
	if(proximity < 10) {
		proximity = 0;
	}
	*pitch = (*pitch * 9 + proximity) / 10.0;
	hamster_pitch(*pitch);
}

void example_advanced_theremin_sound(void) {
	double pitch = 0;

	hamster_create();

	// set a periodic (20 msec) callback
	set_executable(theremin_sound_execute, &pitch);

	wait(10000); // 10 seconds
	hamster_pitch(0); // clear note

	// don't forget to dispose
	dispose_all(); // disconnect and release memory
}

// Example25: Advanced - Simple Hand Follower
int calc_speed(int proximity) {
	return (proximity > 15) ? (40 - proximity) * 4 : 0;
}

void simple_hand_follower_execute(void* arg) {
	int left_speed = calc_speed(hamster_left_proximity());
	int right_speed = calc_speed(hamster_right_proximity());
	hamster_wheels(left_speed, right_speed);
}

void example_advanced_simple_hand_follower(void) {
	hamster_create();

	// set a periodic (20 msec) callback
	set_executable(simple_hand_follower_execute, NULL);

	wait(10000); // 10 seconds
	hamster_stop(); // stop

	// don't forget to dispose
	dispose_all(); // disconnect and release memory
}

// Example26: Advanced - Hand Detector
int hand_detector_evaluate(void* arg) {
	return (hamster_left_proximity() > 50) ? 1 : 0;
}

void example_advanced_hand_detector(void) {
	hamster_create();

	// wait until the value of the left proximity is greater than 50
	wait_until(hand_detector_evaluate, NULL);

	hamster_beep(); // beep

	hamster_wheels(-50, -50); // move backward
	wait(500); // 0.5 seconds

	hamster_stop(); // stop

	// don't forget to dispose
	dispose_all(); // disconnect and release memory
}

// Example27: Advanced - Extension - Button
// This example requires a Hamster's extension kit
void button_execute(void* arg) {
	if(hamster_input_b() == 0) {
		hamster_buzzer(1000);
	} else {
		hamster_buzzer(0);
	}
}

void example_advanced_extension_button(void) {
	hamster_create();

	// set a port B to the digital input mode
	hamster_io_mode_b(HAMSTER_IO_MODE_DIGITAL_INPUT);

	// set a periodic (20 msec) callback
	set_executable(button_execute, NULL);

	wait(10000); // 10 seconds

	// don't forget to dispose
	dispose_all(); // disconnect and release memory
}

// Example28: Advanced - Extension - Potentiometer
// This example requires a Hamster's extension kit
void potentiometer_execute(void* arg) {
	hamster_buzzer(hamster_input_a());
}

void example_advanced_extension_potentiometer(void) {
	hamster_create();

	// set a port A to the analog input mode
	hamster_io_mode_a(HAMSTER_IO_MODE_ANALOG_INPUT);

	// set a periodic (20 msec) callback
	set_executable(potentiometer_execute, NULL);

	wait(10000); // 10 seconds

	// don't forget to dispose
	dispose_all(); // disconnect and release memory
}

// Example29: Advanced - Dual Theremin Sound
struct dual_theremin_sound {
	Hamster* hamster1;
	Hamster* hamster2;
	double pitch1, pitch2;
};

void dual_theremin_sound_execute(void* arg) {
	struct dual_theremin_sound* context = (struct dual_theremin_sound*)arg;
	int proximity = context->hamster1->left_proximity();
	if(proximity < 10) proximity = 0;
	context->pitch1 = (context->pitch1 * 9 + proximity) / 10.0;
	context->hamster1->pitch(context->pitch1);

	proximity = context->hamster2->left_proximity();
	if(proximity < 10) proximity = 0;
	context->pitch2 = (context->pitch2 * 9 + proximity) / 10.0;
	context->hamster2->pitch(context->pitch2);
}

void example_advanced_dual_theremin_sound(void) {
	struct dual_theremin_sound context;

	context.hamster1 = hamster_create();
	context.hamster2 = hamster_create();
	context.pitch1 = 0;
	context.pitch2 = 0;

	// wait until two robots are ready
	wait_until_ready();

	// set a periodic (20 msec) callback
	set_executable(dual_theremin_sound_execute, &context);

	wait(10000); // 10 seconds

	// clear sounds
	context.hamster1->pitch(0);
	context.hamster2->pitch(0);

	// don't forget to dispose
	dispose_all(); // disconnect and release memory
}

// Example30: Advanced - Multiple Hand Follower
void multiple_hand_follower_execute(void* arg) {
	Hamster** hamsters = (Hamster**)arg;
	int left_speed, right_speed, i;

	for(i = 0; i < NUM_HAMSTERS; ++i) {
		// left wheel
		left_speed = calc_speed(hamsters[i]->left_proximity());
		right_speed = calc_speed(hamsters[i]->right_proximity());
		hamsters[i]->wheels(left_speed, right_speed);
	}
}

void example_advanced_multiple_hand_follower(void) {
	Hamster* hamsters[NUM_HAMSTERS];
	int i;

	for(i = 0; i < NUM_HAMSTERS; ++i) {
		hamsters[i] = hamster_create();
	}

	// wait until four robots are ready
	wait_until_ready();

	// set a periodic (20 msec) callback
	set_executable(multiple_hand_follower_execute, hamsters);

	wait(10000); // 10 seconds

	// stop
	for(i = 0; i < NUM_HAMSTERS; ++i) {
		hamsters[i]->stop();
	}

	// don't forget to dispose
	dispose_all(); // disconnect and release memory
}

// Example31: Keyboard Controller
// Ctrl + Break to terminate
#define UP 72
#define DOWN 80
#define LEFT 75
#define RIGHT 77

void example_keyboard_controller(void) {
	hamster_create();

	while(1) {
	    switch(_getch()) {
	    	case UP: hamster_wheels(30, 30); break; // move forward
	    	case DOWN: hamster_wheels(-30, -30); break; // move backward
	    	case LEFT: hamster_wheels(-30, 30); break; // turn left
	    	case RIGHT: hamster_wheels(30, -30); break; // turn right
	    	case ' ': hamster_stop(); // stop
		}
		wait(10); // 10 msec
	}
}

// Example33: Keyboard Piano
// Ctrl + Break to terminate
void play(int pitch) {
	hamster_pitch(0); // break for the same pitch
	wait(50);
    hamster_pitch(pitch);
}

void example_keyboard_piano(void) {
	hamster_create();
	
	while(1) {
	    switch(_getch()) {
	    	case ' ': hamster_pitch(0); break;
	    	case 'a': play(HAMSTER_NOTE_C_4); break;
    		case 'w': play(HAMSTER_NOTE_C_SHARP_4); break;
    		case 's': play(HAMSTER_NOTE_D_4); break;
    		case 'e': play(HAMSTER_NOTE_E_FLAT_4); break;
    		case 'd': play(HAMSTER_NOTE_E_4); break;
    		case 'f': play(HAMSTER_NOTE_F_4); break;
    		case 't': play(HAMSTER_NOTE_F_SHARP_4); break;
    		case 'g': play(HAMSTER_NOTE_G_4); break;
    		case 'y': play(HAMSTER_NOTE_G_SHARP_4); break;
    		case 'h': play(HAMSTER_NOTE_A_4); break;
    		case 'u': play(HAMSTER_NOTE_B_FLAT_4); break;
    		case 'j': play(HAMSTER_NOTE_B_4); break;
    		case 'k': play(HAMSTER_NOTE_C_5); break;
    		case 'o': play(HAMSTER_NOTE_C_SHARP_5); break;
    		case 'l': play(HAMSTER_NOTE_D_5); break;
    		case 'p': play(HAMSTER_NOTE_E_FLAT_5); break;
    		case ';': play(HAMSTER_NOTE_E_5); break;
    		case '\'': play(HAMSTER_NOTE_F_5); break;
		}
		wait(10); // 10 msec
	}
}

int main(int argc, char *argv[]) {
//	example_scan();
//	example_simple_movement();
//	example_basic_movement();
//	example_acceleration();
//	example_led();
//	example_siren();
//	example_melody();
//	example_sensor();
//	example_staggering_walk();
//	example_theremin_sound();
//	example_simple_hand_follower();
//	example_builtin_line_tracer();
//	example_builtin_line_tracer_intersection();
//	example_hand_detector();
	example_move_on_board();
//	example_extension_button();
//	example_extension_led_blink();
//	example_extension_potentiometer();
//	example_extension_smooth_led();
//	example_extension_servo();
//	example_dual_theremin_sound();
//	example_multiple_basic_movement();
//	example_multiple_hand_follower();
//	example_advanced_sensor();
//	example_advanced_theremin_sound();
//	example_advanced_simple_hand_follower();
//	example_advanced_hand_detector();
//	example_advanced_extension_button();
//	example_advanced_extension_potentiometer();
//	example_advanced_dual_theremin_sound();
//	example_advanced_multiple_hand_follower();
//	example_keyboard_controller();
//	example_keyboard_piano();

	return 0;
}
