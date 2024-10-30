/*
 * Part of the ROBOID project - http://hamster.school
 * Copyright (C) 2016 Kwang-Hyun Park (akaii@kw.ac.kr)
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place, Suite 330,
 * Boston, MA  02111-1307  USA
*/

#ifndef _ROBOID_H_
#define _ROBOID_H_

#define DATA_TYPE_INTEGER 4
#define DATA_TYPE_FLOAT 5

#define DEVICE_TYPE_SENSOR 0
#define DEVICE_TYPE_EFFECTOR 1
#define DEVICE_TYPE_EVENT 2
#define DEVICE_TYPE_COMMAND 3

#define HAMSTER_ID "kr.robomation.physical.hamster"

#define HAMSTER_LEFT_WHEEL 0x00400000
#define HAMSTER_RIGHT_WHEEL 0x00400001
#define HAMSTER_BUZZER 0x00400002
#define HAMSTER_OUTPUT_A 0x00400003
#define HAMSTER_OUTPUT_B 0x00400004
#define HAMSTER_TOPOLOGY 0x00400005
#define HAMSTER_LEFT_LED 0x00400006
#define HAMSTER_RIGHT_LED 0x00400007
#define HAMSTER_NOTE 0x00400008
#define HAMSTER_LINE_TRACER_MODE 0x00400009
#define HAMSTER_LINE_TRACER_SPEED 0x0040000a
#define HAMSTER_IO_MODE_A 0x0040000b
#define HAMSTER_IO_MODE_B 0x0040000c
#define HAMSTER_CONFIG_PROXIMITY 0x0040000d
#define HAMSTER_CONFIG_GRAVITY 0x0040000e
#define HAMSTER_CONFIG_BAND_WIDTH 0x0040000f

#define HAMSTER_SIGNAL_STRENGTH 0x00400010
#define HAMSTER_LEFT_PROXIMITY 0x00400011
#define HAMSTER_RIGHT_PROXIMITY 0x00400012
#define HAMSTER_LEFT_FLOOR 0x00400013
#define HAMSTER_RIGHT_FLOOR 0x00400014
#define HAMSTER_ACCELERATION 0x00400015
#define HAMSTER_LIGHT 0x00400016
#define HAMSTER_TEMPERATURE 0x00400017
#define HAMSTER_INPUT_A 0x00400018
#define HAMSTER_INPUT_B 0x00400019
#define HAMSTER_LINE_TRACER_STATE 0x0040001a

#define HAMSTER_TOPOLOGY_NONE 0
#define HAMSTER_TOPOLOGY_DAISY_CHAIN 1
#define HAMSTER_TOPOLOGY_STAR 2
#define HAMSTER_TOPOLOGY_EXTENDED_STAR 3

#define HAMSTER_LED_OFF 0
#define HAMSTER_LED_BLUE 1
#define HAMSTER_LED_GREEN 2
#define HAMSTER_LED_CYAN 3
#define HAMSTER_LED_RED 4
#define HAMSTER_LED_MAGENTA 5
#define HAMSTER_LED_YELLOW 6
#define HAMSTER_LED_WHITE 7

#define HAMSTER_LINE_TRACER_MODE_OFF 0
#define HAMSTER_LINE_TRACER_MODE_BLACK_LEFT_SENSOR 1
#define HAMSTER_LINE_TRACER_MODE_BLACK_RIGHT_SENSOR 2
#define HAMSTER_LINE_TRACER_MODE_BLACK_BOTH_SENSORS 3
#define HAMSTER_LINE_TRACER_MODE_BLACK_TURN_LEFT 4
#define HAMSTER_LINE_TRACER_MODE_BLACK_TURN_RIGHT 5
#define HAMSTER_LINE_TRACER_MODE_BLACK_MOVE_FORWARD 6
#define HAMSTER_LINE_TRACER_MODE_BLACK_UTURN 7
#define HAMSTER_LINE_TRACER_MODE_WHITE_LEFT_SENSOR 8
#define HAMSTER_LINE_TRACER_MODE_WHITE_RIGHT_SENSOR 9
#define HAMSTER_LINE_TRACER_MODE_WHITE_BOTH_SENSORS 10
#define HAMSTER_LINE_TRACER_MODE_WHITE_TURN_LEFT 11
#define HAMSTER_LINE_TRACER_MODE_WHITE_TURN_RIGHT 12
#define HAMSTER_LINE_TRACER_MODE_WHITE_MOVE_FORWARD 13
#define HAMSTER_LINE_TRACER_MODE_WHITE_UTURN 14

#define HAMSTER_IO_MODE_ANALOG_INPUT 0
#define HAMSTER_IO_MODE_DIGITAL_INPUT 1
#define HAMSTER_IO_MODE_SERVO_OUTPUT 8
#define HAMSTER_IO_MODE_PWM_OUTPUT 9
#define HAMSTER_IO_MODE_DIGITAL_OUTPUT 10

#define HAMSTER_NOTE_OFF 0
#define HAMSTER_NOTE_A_0 1
#define HAMSTER_NOTE_A_SHARP_0 2
#define HAMSTER_NOTE_B_FLAT_0 2
#define HAMSTER_NOTE_B_0 3
#define HAMSTER_NOTE_C_1 4
#define HAMSTER_NOTE_C_SHARP_1 5
#define HAMSTER_NOTE_D_FLAT_1 5
#define HAMSTER_NOTE_D_1 6
#define HAMSTER_NOTE_D_SHARP_1 7
#define HAMSTER_NOTE_E_FLAT_1 7
#define HAMSTER_NOTE_E_1 8
#define HAMSTER_NOTE_F_1 9
#define HAMSTER_NOTE_F_SHARP_1 10
#define HAMSTER_NOTE_G_FLAT_1 10
#define HAMSTER_NOTE_G_1 11
#define HAMSTER_NOTE_G_SHARP_1 12
#define HAMSTER_NOTE_A_FLAT_1 12
#define HAMSTER_NOTE_A_1 13
#define HAMSTER_NOTE_A_SHARP_1 14
#define HAMSTER_NOTE_B_FLAT_1 14
#define HAMSTER_NOTE_B_1 15
#define HAMSTER_NOTE_C_2 16
#define HAMSTER_NOTE_C_SHARP_2 17
#define HAMSTER_NOTE_D_FLAT_2 17
#define HAMSTER_NOTE_D_2 18
#define HAMSTER_NOTE_D_SHARP_2 19
#define HAMSTER_NOTE_E_FLAT_2 19
#define HAMSTER_NOTE_E_2 20
#define HAMSTER_NOTE_F_2 21
#define HAMSTER_NOTE_F_SHARP_2 22
#define HAMSTER_NOTE_G_FLAT_2 22
#define HAMSTER_NOTE_G_2 23
#define HAMSTER_NOTE_G_SHARP_2 24
#define HAMSTER_NOTE_A_FLAT_2 24
#define HAMSTER_NOTE_A_2 25
#define HAMSTER_NOTE_A_SHARP_2 26
#define HAMSTER_NOTE_B_FLAT_2 26
#define HAMSTER_NOTE_B_2 27
#define HAMSTER_NOTE_C_3 28
#define HAMSTER_NOTE_C_SHARP_3 29
#define HAMSTER_NOTE_D_FLAT_3 29
#define HAMSTER_NOTE_D_3 30
#define HAMSTER_NOTE_D_SHARP_3 31
#define HAMSTER_NOTE_E_FLAT_3 31
#define HAMSTER_NOTE_E_3 32
#define HAMSTER_NOTE_F_3 33
#define HAMSTER_NOTE_F_SHARP_3 34
#define HAMSTER_NOTE_G_FLAT_3 34
#define HAMSTER_NOTE_G_3 35
#define HAMSTER_NOTE_G_SHARP_3 36
#define HAMSTER_NOTE_A_FLAT_3 36
#define HAMSTER_NOTE_A_3 37
#define HAMSTER_NOTE_A_SHARP_3 38
#define HAMSTER_NOTE_B_FLAT_3 38
#define HAMSTER_NOTE_B_3 39
#define HAMSTER_NOTE_C_4 40
#define HAMSTER_NOTE_C_SHARP_4 41
#define HAMSTER_NOTE_D_FLAT_4 41
#define HAMSTER_NOTE_D_4 42
#define HAMSTER_NOTE_D_SHARP_4 43
#define HAMSTER_NOTE_E_FLAT_4 43
#define HAMSTER_NOTE_E_4 44
#define HAMSTER_NOTE_F_4 45
#define HAMSTER_NOTE_F_SHARP_4 46
#define HAMSTER_NOTE_G_FLAT_4 46
#define HAMSTER_NOTE_G_4 47
#define HAMSTER_NOTE_G_SHARP_4 48
#define HAMSTER_NOTE_A_FLAT_4 48
#define HAMSTER_NOTE_A_4 49
#define HAMSTER_NOTE_A_SHARP_4 50
#define HAMSTER_NOTE_B_FLAT_4 50
#define HAMSTER_NOTE_B_4 51
#define HAMSTER_NOTE_C_5 52
#define HAMSTER_NOTE_C_SHARP_5 53
#define HAMSTER_NOTE_D_FLAT_5 53
#define HAMSTER_NOTE_D_5 54
#define HAMSTER_NOTE_D_SHARP_5 55
#define HAMSTER_NOTE_E_FLAT_5 55
#define HAMSTER_NOTE_E_5 56
#define HAMSTER_NOTE_F_5 57
#define HAMSTER_NOTE_F_SHARP_5 58
#define HAMSTER_NOTE_G_FLAT_5 58
#define HAMSTER_NOTE_G_5 59
#define HAMSTER_NOTE_G_SHARP_5 60
#define HAMSTER_NOTE_A_FLAT_5 60
#define HAMSTER_NOTE_A_5 61
#define HAMSTER_NOTE_A_SHARP_5 62
#define HAMSTER_NOTE_B_FLAT_5 62
#define HAMSTER_NOTE_B_5 63
#define HAMSTER_NOTE_C_6 64
#define HAMSTER_NOTE_C_SHARP_6 65
#define HAMSTER_NOTE_D_FLAT_6 65
#define HAMSTER_NOTE_D_6 66
#define HAMSTER_NOTE_D_SHARP_6 67
#define HAMSTER_NOTE_E_FLAT_6 67
#define HAMSTER_NOTE_E_6 68
#define HAMSTER_NOTE_F_6 69
#define HAMSTER_NOTE_F_SHARP_6 70
#define HAMSTER_NOTE_G_FLAT_6 70
#define HAMSTER_NOTE_G_6 71
#define HAMSTER_NOTE_G_SHARP_6 72
#define HAMSTER_NOTE_A_FLAT_6 72
#define HAMSTER_NOTE_A_6 73
#define HAMSTER_NOTE_A_SHARP_6 74
#define HAMSTER_NOTE_B_FLAT_6 74
#define HAMSTER_NOTE_B_6 75
#define HAMSTER_NOTE_C_7 76
#define HAMSTER_NOTE_C_SHARP_7 77
#define HAMSTER_NOTE_D_FLAT_7 77
#define HAMSTER_NOTE_D_7 78
#define HAMSTER_NOTE_D_SHARP_7 79
#define HAMSTER_NOTE_E_FLAT_7 79
#define HAMSTER_NOTE_E_7 80
#define HAMSTER_NOTE_F_7 81
#define HAMSTER_NOTE_F_SHARP_7 82
#define HAMSTER_NOTE_G_FLAT_7 82
#define HAMSTER_NOTE_G_7 83
#define HAMSTER_NOTE_G_SHARP_7 84
#define HAMSTER_NOTE_A_FLAT_7 84
#define HAMSTER_NOTE_A_7 85
#define HAMSTER_NOTE_A_SHARP_7 86
#define HAMSTER_NOTE_B_FLAT_7 86
#define HAMSTER_NOTE_B_7 87
#define HAMSTER_NOTE_C_8 88

typedef struct hamster {
	const char* (*get_name)(void);
	void (*set_name)(const char* name);
	const char* (*get_id)(void);
	int (*get_index)(void);
	int (*e)(int device_id);
	int (*read)(int device_id);
	int (*read_at)(int device_id, int index);
	int (*read_array)(int device_id, int* data, int length);
	float (*read_float)(int device_id);
	float (*read_float_at)(int device_id, int index);
	int (*read_float_array)(int device_id, float* data, int length);
	int (*write)(int device_id, int data);
	int (*write_at)(int device_id, int index, int data);
	int (*write_array)(int device_id, const int* data, int length);
	int (*write_float)(int device_id, float data);
	int (*write_float_at)(int device_id, int index, float data);
	int (*write_float_array)(int device_id, const float* data, int length);
	void (*reset)(void);
	void (*dispose)(void);
	void (*wheels)(double left_speed, double right_speed);
	void (*left_wheel)(double speed);
	void (*right_wheel)(double speed);
	void (*stop)(void);
	void (*line_tracer_mode)(int mode);
	void (*line_tracer_speed)(double speed);
	void (*board_forward)(void);
	void (*board_left)(void);
	void (*board_right)(void);
	void (*leds)(int left_color, int right_color);
	void (*left_led)(int color);
	void (*right_led)(int color);
	void (*beep)(void);
	void (*buzzer)(double hz);
	void (*tempo)(double bpm);
	void (*pitch)(double pitch);
	void (*note)(double pitch, double beats);
	void (*io_mode_a)(int mode);
	void (*io_mode_b)(int mode);
	void (*output_a)(double value);
	void (*output_b)(double value);
	int (*signal_strength)(void);
	int (*left_proximity)(void);
	int (*right_proximity)(void);
	int (*left_floor)(void);
	int (*right_floor)(void);
	int (*acceleration_x)(void);
	int (*acceleration_y)(void);
	int (*acceleration_z)(void);
	int (*light)(void);
	int (*temperature)(void);
	int (*input_a)(void);
	int (*input_b)(void);
} Hamster;

void scan(void);
void set_executable(void (*execute)(void* arg), void* arg);
void wait(int milliseconds);
void wait_until(int (*evaluate)(void* arg), void* arg);
void wait_until_ready(void);
void dispose_all(void);

Hamster* hamster_create(void);
Hamster* hamster_create_port(const char* port_name);
const char* hamster_get_name(void);
void hamster_set_name(const char* name);
const char* hamster_get_id(void);
int hamster_e(int device_id);
int hamster_read(int device_id);
int hamster_read_at(int device_id, int index);
int hamster_read_array(int device_id, int* data, int length);
float hamster_read_float(int device_id);
float hamster_read_float_at(int device_id, int index);
int hamster_read_float_array(int device_id, float* data, int length);
int hamster_write(int device_id, int data);
int hamster_write_at(int device_id, int index, int data);
int hamster_write_array(int device_id, const int* data, int length);
int hamster_write_float(int device_id, float data);
int hamster_write_float_at(int device_id, int index, float data);
int hamster_write_float_array(int device_id, const float* data, int length);
void hamster_reset(void);
void hamster_dispose(void);
void hamster_wheels(double left_speed, double right_speed);
void hamster_left_wheel(double speed);
void hamster_right_wheel(double speed);
void hamster_stop(void);
void hamster_line_tracer_mode(int mode);
void hamster_line_tracer_speed(double speed);
void hamster_board_forward(void);
void hamster_board_left(void);
void hamster_board_right(void);
void hamster_leds(int left_color, int right_color);
void hamster_left_led(int color);
void hamster_right_led(int color);
void hamster_beep(void);
void hamster_buzzer(double hz);
void hamster_tempo(double bpm);
void hamster_pitch(double pitch);
void hamster_note(double pitch, double beats);
void hamster_io_mode_a(int mode);
void hamster_io_mode_b(int mode);
void hamster_output_a(double value);
void hamster_output_b(double value);
int hamster_signal_strength(void);
int hamster_left_proximity(void);
int hamster_right_proximity(void);
int hamster_left_floor(void);
int hamster_right_floor(void);
int hamster_acceleration_x(void);
int hamster_acceleration_y(void);
int hamster_acceleration_z(void);
int hamster_light(void);
int hamster_temperature(void);
int hamster_input_a(void);
int hamster_input_b(void);

#endif
