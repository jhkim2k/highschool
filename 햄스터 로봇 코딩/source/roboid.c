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

#include <process.h>
#include <stdio.h>
#include <windows.h>
#include <signal.h>
#include <sys/timeb.h>
#include "roboid.h"

#ifdef _MSC_VER
#define _STRCPY(dst, sz, src) strcpy_s((dst), (sz), (src))
#define _STRCAT(dst, sz, src) strcat_s((dst), (sz), (src))
#define _STRNCPY(dst, sz, src, n) strncpy_s((dst), (sz), (src), (n))
#define _STRICMP(s1, s2) _stricmp((s1), (s2))
#else
#define _STRCPY(dst, sz, src) strcpy((dst), (src))
#define _STRCAT(dst, sz, src) strcat((dst), (src))
#define _STRNCPY(dst, sz, src, n) strncpy((dst), (src), (n))
#define _STRICMP(s1, s2) stricmp((s1), (s2))
#endif

#ifdef _WIN64
#define _LONG long long
#else
#define _LONG long
#endif

/*------------------------------
  SERIAL
------------------------------*/

#define _SERIAL_BAUDRATE_110 110
#define _SERIAL_BAUDRATE_300 300
#define _SERIAL_BAUDRATE_600 600
#define _SERIAL_BAUDRATE_1200 1200
#define _SERIAL_BAUDRATE_4800 4800
#define _SERIAL_BAUDRATE_9600 9600
#define _SERIAL_BAUDRATE_14400 14400
#define _SERIAL_BAUDRATE_19200 19200
#define _SERIAL_BAUDRATE_38400 38400
#define _SERIAL_BAUDRATE_57600 57600
#define _SERIAL_BAUDRATE_115200 115200
#define _SERIAL_BAUDRATE_128000 128000
#define _SERIAL_BAUDRATE_256000 256000

#define _SERIAL_DATABITS_5 5
#define _SERIAL_DATABITS_6 6
#define _SERIAL_DATABITS_7 7
#define _SERIAL_DATABITS_8 8

#define _SERIAL_STOPBITS_1 1
#define _SERIAL_STOPBITS_2 2
#define _SERIAL_STOPBITS_1_5 3

#define _SERIAL_PARITY_NONE 0
#define _SERIAL_PARITY_ODD 1
#define _SERIAL_PARITY_EVEN 2
#define _SERIAL_PARITY_MARK 3
#define _SERIAL_PARITY_SPACE 4

#define _SERIAL_PURGE_RXABORT 0x0002
#define _SERIAL_PURGE_RXCLEAR 0x0008
#define _SERIAL_PURGE_TXABORT 0x0001
#define _SERIAL_PURGE_TXCLEAR 0x0004

#define _SERIAL_MASK_RXCHAR 1
#define _SERIAL_MASK_RXFLAG 2
#define _SERIAL_MASK_TXEMPTY 4
#define _SERIAL_MASK_CTS 8
#define _SERIAL_MASK_DSR 16
#define _SERIAL_MASK_RLSD 32
#define _SERIAL_MASK_BREAK 64
#define _SERIAL_MASK_ERR 128
#define _SERIAL_MASK_RING 256

#define _SERIAL_FLOWCONTROL_NONE 0
#define _SERIAL_FLOWCONTROL_RTSCTS_IN 1
#define _SERIAL_FLOWCONTROL_RTSCTS_OUT 2
#define _SERIAL_FLOWCONTROL_XONXOFF_IN 4
#define _SERIAL_FLOWCONTROL_XONXOFF_OUT 8

#define _SERIAL_ERROR_FRAME 0x0008
#define _SERIAL_ERROR_OVERRUN 0x0002
#define _SERIAL_ERROR_PARITY 0x0004

#define _SERIAL_ERROR_PORT_BUSY -1
#define _SERIAL_ERROR_PORT_NOT_FOUND -2
#define _SERIAL_ERROR_PERMISSION_DENIED -3
#define _SERIAL_ERROR_INCORRECT_SERIAL_PORT -4

#define _SERIAL_PARAMS_FLAG_IGNPAR 1
#define _SERIAL_PARAMS_FLAG_PARMRK 2

#define _TEMP_CHAR_BUFFER_SIZE 256
#define _SERIAL_BUFFER_SIZE 32768

struct _serial {
	_LONG port_handle;
	int port_opened;
	char* buffer;
	int buffer_size;
	int offset;
};

char** _serial_window_get_serial_port_names(const char* keyword, int* count);
_LONG _serial_window_open_port(const char* port_name);
int _serial_window_close_port(_LONG port_handle);
int _serial_window_set_params(_LONG port_handle, int baud_rate, int byte_size, int stop_bits, int parity, int set_rts, int set_dtr, int flags);
int _serial_window_set_flow_control_mode(_LONG port_handle, int mask);
int _serial_window_purge_port(_LONG port_handle, int flags);
int _serial_window_count_read_bytes(_LONG port_handle);
int _serial_window_read_bytes(_LONG port_handle, unsigned char* buffer, int buffer_size);
int _serial_window_write_bytes(_LONG port_handle, const unsigned char* buffer, int buffer_size);

struct _serial* _serial_create(void);
void _serial_dispose(struct _serial* serial);
int _serial_open(struct _serial* serial, const char* port_name, int baud_rate, int flow_control);
void _serial_close(struct _serial* serial);
void _serial_clear(struct _serial* serial);
int _serial_read_string_until(struct _serial* serial, char* buffer, int buffer_size, char delimiter);
int _serial_write(const struct _serial* serial, const char* buffer, int buffer_size);

char** _serial_window_get_serial_port_names(const char* keyword, int* count) {
	HKEY result;
	LPCSTR sub_key = "HARDWARE\\DEVICEMAP\\SERIALCOMM\\";
	char** names = NULL;
	int names_count = 0;
	
	if(RegOpenKeyExA(HKEY_LOCAL_MACHINE, sub_key, 0, KEY_READ, &result) == ERROR_SUCCESS) {
		int has_more_elements = 1;
		char value_name[_TEMP_CHAR_BUFFER_SIZE];
		DWORD keys_count = 0, value_name_size, enum_result;
		
		while(has_more_elements == 1) {
			value_name_size = _TEMP_CHAR_BUFFER_SIZE;
			enum_result = RegEnumValueA(result, keys_count, value_name, &value_name_size, NULL, NULL, NULL, NULL);
			if(enum_result == ERROR_SUCCESS) {
				++ keys_count;
				if(keyword == NULL || strstr(value_name, keyword) != NULL) {
					++ names_count;
				}
			} else if(enum_result == ERROR_NO_MORE_ITEMS) {
				has_more_elements = 0;
			} else {
				has_more_elements = 0;
			}
		}
		if(names_count > 0) {
			byte data[_TEMP_CHAR_BUFFER_SIZE];
			DWORD data_size;
			int names_index = 0, i;
			DWORD j;
			
			names = (char**)malloc(sizeof(char*) * names_count);
			for(i = 0; i < names_count; ++i) {
				names[i] = (char*)malloc(sizeof(char) * _TEMP_CHAR_BUFFER_SIZE);
			}
			for(j = 0; j < keys_count; ++j) {
				value_name_size = _TEMP_CHAR_BUFFER_SIZE;
				data_size = _TEMP_CHAR_BUFFER_SIZE;
				enum_result = RegEnumValueA(result, j, value_name, &value_name_size, NULL, NULL, data, &data_size);
				if(enum_result == ERROR_SUCCESS) {
					if(keyword == NULL || strstr(value_name, keyword) != NULL) {
						_STRCPY(names[names_index], _TEMP_CHAR_BUFFER_SIZE, (const char*)data);
						++ names_index;
					}
				}
			}
		}
		CloseHandle(result);
	}
	*count = names_count;
	return names;
}

_LONG _serial_window_open_port(const char* port_name) {
	const char* prefix= "\\\\.\\";
	char port_full_name[_TEMP_CHAR_BUFFER_SIZE];
	HANDLE comm;

	_STRCPY(port_full_name, _TEMP_CHAR_BUFFER_SIZE, prefix);
	_STRCAT(port_full_name, _TEMP_CHAR_BUFFER_SIZE, port_name);

	comm = CreateFileA((LPCSTR)port_full_name,
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_FLAG_OVERLAPPED,
		0);

	if(comm != INVALID_HANDLE_VALUE) {
		DCB dcb = { 0 };
		dcb.DCBlength = sizeof(DCB);
		if(!GetCommState(comm, &dcb)) {
			CloseHandle(comm);
			comm = (HANDLE)_SERIAL_ERROR_INCORRECT_SERIAL_PORT;
		}
	} else {
		DWORD error_value = GetLastError();
		if(error_value == ERROR_ACCESS_DENIED) {
			comm = (HANDLE)_SERIAL_ERROR_PORT_BUSY;
		} else if(error_value == ERROR_FILE_NOT_FOUND) {
			comm = (HANDLE)_SERIAL_ERROR_PORT_NOT_FOUND;
		}
	}
	return (_LONG)comm;
}

int _serial_window_close_port(_LONG port_handle) {
	return CloseHandle((HANDLE)port_handle) ? 1 : 0;
}

int _serial_window_set_params(_LONG port_handle, int baud_rate, int byte_size, int stop_bits, int parity, int set_rts, int set_dtr, int flags) {
	HANDLE comm = (HANDLE)port_handle;
	int return_value = 0;
	DCB dcb = { 0 };

	dcb.DCBlength = sizeof(DCB);
	if(GetCommState(comm, &dcb)) {
		dcb.BaudRate = baud_rate;
		dcb.ByteSize = byte_size;
		dcb.StopBits = stop_bits;
		dcb.Parity = parity;

		if(set_rts == 1) {
			dcb.fRtsControl = RTS_CONTROL_ENABLE;
		} else {
			dcb.fRtsControl = RTS_CONTROL_DISABLE;
		}
		if(set_dtr == 1) {
			dcb.fDtrControl = DTR_CONTROL_ENABLE;
		} else {
			dcb.fDtrControl = DTR_CONTROL_DISABLE;
		}
		dcb.fOutxCtsFlow = FALSE;
		dcb.fOutxDsrFlow = FALSE;
		dcb.fDsrSensitivity = FALSE;
		dcb.fTXContinueOnXoff = TRUE;
		dcb.fOutX = FALSE;
		dcb.fInX = FALSE;
		dcb.fErrorChar = FALSE;
		dcb.fNull = FALSE;
		dcb.fAbortOnError = FALSE;
		dcb.XonLim = 2048;
		dcb.XoffLim = 512;
		dcb.XonChar = (char)17; //DC1
		dcb.XoffChar = (char)19; //DC3

		if(SetCommState(comm, &dcb)) {
			COMMTIMEOUTS comm_timeouts = { 0 };
			comm_timeouts.ReadIntervalTimeout = 0;
			comm_timeouts.ReadTotalTimeoutConstant = 100;
			comm_timeouts.ReadTotalTimeoutMultiplier = 0;
			comm_timeouts.WriteTotalTimeoutConstant = 0;
			comm_timeouts.WriteTotalTimeoutMultiplier = 0;
			if(SetCommTimeouts(comm, &comm_timeouts)) {
				return_value = 1;
			}
		}
	}
	return return_value;
}

int _serial_window_set_flow_control_mode(_LONG port_handle, int mask) {
	HANDLE comm = (HANDLE)port_handle;
	int return_value = 0;
	DCB dcb = { 0 };

	dcb.DCBlength = sizeof(DCB);
	if(GetCommState(comm, &dcb)) {
		dcb.fRtsControl = RTS_CONTROL_ENABLE;
		dcb.fOutxCtsFlow = FALSE;
		dcb.fOutX = FALSE;
		dcb.fInX = FALSE;
		if(mask != _SERIAL_FLOWCONTROL_NONE) {
			if((mask & _SERIAL_FLOWCONTROL_RTSCTS_IN) == _SERIAL_FLOWCONTROL_RTSCTS_IN) {
				dcb.fRtsControl = RTS_CONTROL_HANDSHAKE;
			}
			if((mask & _SERIAL_FLOWCONTROL_RTSCTS_OUT) == _SERIAL_FLOWCONTROL_RTSCTS_OUT) {
				dcb.fOutxCtsFlow = TRUE;
			}
			if((mask & _SERIAL_FLOWCONTROL_XONXOFF_IN) == _SERIAL_FLOWCONTROL_XONXOFF_IN) {
				dcb.fInX = TRUE;
			}
			if((mask & _SERIAL_FLOWCONTROL_XONXOFF_OUT) == _SERIAL_FLOWCONTROL_XONXOFF_OUT) {
				dcb.fOutX = TRUE;
			}
		}
		if(SetCommState(comm, &dcb)) {
			return_value = 1;
		}
	}
	return return_value;
}

int _serial_window_purge_port(_LONG port_handle, int flags) {
	return PurgeComm((HANDLE)port_handle, (DWORD)flags) ? 1 : 0;
}

int _serial_window_count_read_bytes(_LONG port_handle) {
	HANDLE comm = (HANDLE)port_handle;
	DWORD errors;
	COMSTAT comstat = { 0 };
	int return_value = -1;

	if(ClearCommError(comm, &errors, &comstat)) {
		return_value = comstat.cbInQue;
	} else {
		return_value = -1;
	}
	return return_value;
}

int _serial_window_read_bytes(_LONG port_handle, unsigned char* buffer, int buffer_size) {
	HANDLE comm = (HANDLE)port_handle;
	DWORD number_of_bytes_transferred;
	DWORD number_of_bytes_read;
	OVERLAPPED overlapped = { 0 };
	int return_value = 0;

	overlapped.hEvent = CreateEventA(NULL, 1, 0, NULL);
	if(ReadFile(comm, buffer, (DWORD)buffer_size, &number_of_bytes_read, &overlapped)) {
		return_value = number_of_bytes_read;
	} else if(GetLastError() == ERROR_IO_PENDING) {
		if(WaitForSingleObject(overlapped.hEvent, INFINITE) == WAIT_OBJECT_0) {
			if(GetOverlappedResult(comm, &overlapped, &number_of_bytes_transferred, 0)) {
				return_value = number_of_bytes_transferred;
			}
		}
	}
	CloseHandle(overlapped.hEvent);
	return return_value;
}

int _serial_window_write_bytes(_LONG port_handle, const unsigned char* buffer, int buffer_size) {
	HANDLE comm = (HANDLE)port_handle;
	DWORD number_of_bytes_transferred;
	DWORD number_of_bytes_written;
	OVERLAPPED overlapped = { 0 };
	int return_value = 0;

	overlapped.hEvent = CreateEventA(NULL, 1, 0, NULL);
	if(WriteFile(comm, buffer, buffer_size, &number_of_bytes_written, &overlapped)) {
		return_value = 1;
	} else if(GetLastError() == ERROR_IO_PENDING) {
		if(WaitForSingleObject(overlapped.hEvent, INFINITE) == WAIT_OBJECT_0) {
			if(GetOverlappedResult(comm, &overlapped, &number_of_bytes_transferred, 0)) {
				return_value = 1;
			}
		}
	}
	CloseHandle(overlapped.hEvent);
	return return_value;
}

struct _serial* _serial_create(void) {
	struct _serial* serial = (struct _serial*)malloc(sizeof(struct _serial));
	serial->port_handle = 0;
	serial->port_opened = 0;
	serial->buffer = NULL;
	serial->buffer_size = 0;
	serial->offset = 0;
	return serial;
}

void _serial_dispose(struct _serial* serial) {
	if(serial == NULL) return;
	_serial_close(serial); // free buffer
	free(serial);
}

int _serial_open(struct _serial* serial, const char* port_name, int baud_rate, int flow_control) {
	_LONG port_handle;

	if(serial == NULL) return 0;
	if(serial->port_opened == 1) return 0;
	
	port_handle = _serial_window_open_port(port_name);
	if(port_handle == _SERIAL_ERROR_PORT_BUSY) return 0;
	else if(port_handle == _SERIAL_ERROR_PORT_NOT_FOUND) return 0;
	else if(port_handle == _SERIAL_ERROR_PERMISSION_DENIED) return 0;
	else if(port_handle == _SERIAL_ERROR_INCORRECT_SERIAL_PORT) return 0;
	
	serial->port_handle = port_handle;
	serial->port_opened = 1;
	serial->offset = 0;
	
	if(serial->buffer != NULL) {
		free(serial->buffer);
		serial->buffer = NULL;
	}
	serial->buffer_size = _SERIAL_BUFFER_SIZE;
	serial->buffer = (char*)malloc(sizeof(char) * _SERIAL_BUFFER_SIZE);
	_serial_window_set_params(port_handle, baud_rate, _SERIAL_DATABITS_8, 0, _SERIAL_PARITY_NONE, 1, 1, 0);
	_serial_window_set_flow_control_mode(port_handle, flow_control);
	return 1;
}

void _serial_close(struct _serial* serial) {
	if(serial == NULL) return;
	if(serial->port_opened == 0) return;

	serial->buffer_size = 0;
	if(serial->buffer != NULL) {
		free(serial->buffer);
		serial->buffer = NULL;
	}
	if(_serial_window_close_port(serial->port_handle) == 1) {
		serial->port_opened = 0;
	}
	serial->offset = 0;
	serial->port_handle = 0;
}

void _serial_clear(struct _serial* serial) {
	if(serial == NULL) return;
	if(serial->port_opened == 0) return;
	
	serial->offset = 0;
	_serial_window_purge_port(serial->port_handle, _SERIAL_PURGE_RXCLEAR | _SERIAL_PURGE_RXABORT | _SERIAL_PURGE_TXCLEAR | _SERIAL_PURGE_TXABORT);
}

int _serial_read_string_until(struct _serial* serial, char* buffer, int buffer_size, char delimiter) {
	_LONG port_handle;
	int to_read, read_bytes;
	int size, found, i;
	char* temp;

	if(serial == NULL) return 0;
	if(serial->port_opened == 0) return 0;

	port_handle = serial->port_handle;
	to_read = _serial_window_count_read_bytes(port_handle);
	while(to_read > 0) {
		if(serial->buffer_size < serial->offset + to_read) {
			size = (serial->offset + to_read) * 2;
			temp = (char*)malloc(sizeof(char) * size);
			_STRNCPY(temp, size, serial->buffer, serial->buffer_size);
			free(serial->buffer);
			serial->buffer = temp;
			serial->buffer_size = size;
		}
		read_bytes = _serial_window_read_bytes(port_handle, (unsigned char*)(serial->buffer + serial->offset), to_read);
		serial->offset += read_bytes;

		found = -1;
		for(i = 0; i < serial->offset; ++i) {
			if(serial->buffer[i] == delimiter) {
				found = i;
				break;
			}
		}
		if(found != -1) {
			int to_copy = found + 1;
			if(to_copy < buffer_size) {
				_STRNCPY(buffer, buffer_size, serial->buffer, to_copy);
				_STRNCPY(serial->buffer, serial->buffer_size, serial->buffer + to_copy, serial->offset - to_copy + 1);
				serial->offset -= to_copy;
				return to_copy;
			}
		}
		to_read = _serial_window_count_read_bytes(port_handle);
	}
	return 0;
}

int _serial_write(const struct _serial* serial, const char* buffer, int buffer_size) {
	if(serial == NULL) return 0;
	if(serial->port_opened == 0) return 0;
	return _serial_window_write_bytes(serial->port_handle, (const unsigned char*)buffer, buffer_size);
}

/*------------------------------
  CONNECTOR
------------------------------*/

#define _CONNECTION_STATE_NONE 0
#define _CONNECTION_STATE_CONNECTING 1
#define _CONNECTION_STATE_CONNECTED 2
#define _CONNECTION_STATE_CONNECTION_LOST 3
#define _CONNECTION_STATE_DISCONNECTED 4
#define _CONNECTION_STATE_DISPOSED 5
#define _CONNECTION_RESULT_FOUND 1
#define _CONNECTION_RESULT_NOT_CONNECTED 2
#define _CONNECTION_RESULT_NOT_AVAILABLE 3

#define _CONNECTOR_INFO_BUFFER_SIZE 20
#define _CONNECTOR_BUFFER_SIZE 256
#define _TIMEOUT 100 // milliseconds
#define _RETRY 10

#define _VALID_PACKET_LENGTH 54
#define _MOTORING_PACKET_LENGTH 54
#define _CR 13

#define _DEFAULT_ADDRESS "000000000000"

struct _connector;
typedef int (*_CHECK_CONNECTION)(struct _connector* connector, struct _serial* serial);

struct _connector {
	struct _serial* serial;
	char* tag;
	int index;
	int packet_length;
	char delimiter;
	char* address;
	char* port_name;
	int found;
	int connected;
	int checking_timeout;
	double timestamp;
	char* buffer;
	_CHECK_CONNECTION check_connection;
};

struct _connector* _connector_create(const char* tag, int index, int packet_length, char delimiter);
void _connector_dispose(struct _connector* connector);
int _connector_open(struct _connector* connector, const char* port_name, int baud_rate, int flow_control);
int _connector_check_port(struct _connector* connector, struct _serial* serial);
int _connector_open_port(struct _connector* connector, const char* port_name, int baud_rate, int flow_control);
void _connector_close(struct _connector* connector);
int _connector_is_connected(const struct _connector* connector);
const char* _connector_get_port_name(const struct _connector* connector);
const char* _connector_get_address(const struct _connector* connector);
void _connector_set_address(const struct _connector* connector, const char* address);
void _connector_set_connection_state(struct _connector* connector, int state);
int _connector_read_packet(const struct _connector* connector, struct _serial* serial, const char* start_bytes);
void _connector_write(const struct _connector* connector, const char* buffer, int buffer_size);
int _connector_read(struct _connector* _connector);
void _connector_print_state(const struct _connector* connector, int state);
void _connector_print_error(const struct _connector* connector, int error_code);

double _get_timestamp(struct timeb* t) {
	ftime(t);
	return (double)t->time + (double)t->millitm / 1000.0;
}

struct _connector* _connector_create(const char* tag, int index, int packet_length, char delimiter) {
	struct _connector* connector = (struct _connector*)malloc(sizeof(struct _connector));
	
	connector->serial = NULL;
	connector->index = index;
	connector->packet_length = packet_length;
	connector->delimiter = delimiter;
	
	connector->tag = (char*)malloc(sizeof(char) * _CONNECTOR_INFO_BUFFER_SIZE);
	connector->address = (char*)malloc(sizeof(char) * _CONNECTOR_INFO_BUFFER_SIZE);
	connector->port_name = (char*)malloc(sizeof(char) * _CONNECTOR_INFO_BUFFER_SIZE);
	_STRCPY(connector->tag, _CONNECTOR_INFO_BUFFER_SIZE, tag);
	_STRCPY(connector->address, _CONNECTOR_INFO_BUFFER_SIZE, _DEFAULT_ADDRESS);
	_STRCPY(connector->port_name, _CONNECTOR_INFO_BUFFER_SIZE, "");
	
	connector->found = 0;
	connector->connected = 0;
	connector->checking_timeout = 0;
	connector->timestamp = 0;
	
	connector->buffer = (char*)malloc(sizeof(char) * _CONNECTOR_BUFFER_SIZE);
	connector->check_connection = NULL;

	return connector;
}

void _connector_dispose(struct _connector* connector) {
	if(connector == NULL) return;

	_connector_close(connector); // close serial
	if(connector->tag != NULL) {
		free(connector->tag);
		connector->tag = NULL;
	}
	if(connector->address != NULL) {
		free(connector->address);
		connector->address = NULL;
	}
	if(connector->port_name != NULL) {
		free(connector->port_name);
		connector->port_name = NULL;
	}
	if(connector->buffer != NULL) {
		free(connector->buffer);
		connector->buffer = NULL;
	}
	connector->check_connection = NULL;
	free(connector);
}

int _connector_open(struct _connector* connector, const char* port_name, int baud_rate, int flow_control) {
	int result = _CONNECTION_RESULT_NOT_AVAILABLE;

	if(connector == NULL) return _CONNECTION_RESULT_NOT_AVAILABLE;
	if(port_name == NULL) {
		int port_count = 0;
		char** port_names = _serial_window_get_serial_port_names(NULL, &port_count);
		if(port_count > 0 && port_names != NULL) {
			int i;
	
			for(i = 0; i < port_count; ++i) {
				result = _connector_open_port(connector, port_names[i], baud_rate, flow_control);
				if(result != _CONNECTION_RESULT_NOT_AVAILABLE) {
					break;
				}
			}
			for(i = 0; i < port_count; ++i) {
				free(port_names[i]);
			}
			free(port_names);
		}
	} else {
		result = _connector_open_port(connector, port_name, baud_rate, flow_control);
	}
	if(result == _CONNECTION_RESULT_NOT_AVAILABLE) {
		_connector_print_error(connector, result);
	}
	return result;
}

int _connector_check_port(struct _connector* connector, struct _serial* serial) {
	int read_bytes1, read_bytes2;

	_connector_read_packet(connector, serial, NULL);
	read_bytes1 = _connector_read_packet(connector, serial, NULL);
	read_bytes2 = _connector_read_packet(connector, serial, NULL);

	if(read_bytes2 != 0) {
		if(read_bytes2 == connector->packet_length) {
			if(connector->check_connection != NULL) {
				return connector->check_connection(connector, serial);
			}
		} else if(read_bytes1 != 0 && read_bytes2 == 2) {
			_connector_print_error(connector, _CONNECTION_RESULT_NOT_CONNECTED);
			return _CONNECTION_RESULT_NOT_CONNECTED;
		}
	}
	return _CONNECTION_RESULT_NOT_AVAILABLE;
}

int _connector_open_port(struct _connector* connector, const char* port_name, int baud_rate, int flow_control) {
	struct _serial* serial = _serial_create();
	if(_serial_open(serial, port_name, baud_rate, flow_control) == 1) {
		int result;
		
		_serial_clear(serial);
		_STRCPY(connector->port_name, _CONNECTOR_INFO_BUFFER_SIZE, port_name);
		
		result = _connector_check_port(connector, serial);
		if(result != _CONNECTION_RESULT_NOT_AVAILABLE) {
			connector->serial = serial;
			return result;
		}
		_serial_close(serial);
	}
	_serial_dispose(serial);
	serial = NULL;
	
	return _CONNECTION_RESULT_NOT_AVAILABLE;
}

void _connector_close(struct _connector* connector) {
	if(connector == NULL) return;
	if(connector->serial != NULL) {
		_serial_dispose(connector->serial);
		connector->serial = NULL;
	}
	connector->connected = 0;
	_connector_print_state(connector, _CONNECTION_STATE_DISPOSED);
}

int _connector_is_connected(const struct _connector* connector) {
	if(connector == NULL) return 0;
	return connector->connected;
}

const char* _connector_get_port_name(const struct _connector* connector) {
	if(connector == NULL) return "";
	return connector->port_name;
}

const char* _connector_get_address(const struct _connector* connector) {
	if(connector == NULL) return _DEFAULT_ADDRESS;
	return connector->address;
}

void _connector_set_address(const struct _connector* connector, const char* address) {
	if(connector == NULL) return;
	_STRCPY(connector->address, _CONNECTOR_INFO_BUFFER_SIZE, address);
}

void _connector_set_connection_state(struct _connector* connector, int state) {
	if(connector == NULL) return;
	connector->connected = (state == _CONNECTION_STATE_CONNECTED) ? 1 : 0;
	if(connector->found == 0 && connector->connected == 1) {
		connector->found = 1;
	}
	if(connector->found == 1) {
		_connector_print_state(connector, state);
	}
}

int _connector_read_packet(const struct _connector* connector, struct _serial* serial, const char* start_bytes) {
	if(connector != NULL) {
		char* buffer = connector->buffer;
		int read_bytes = 0, i;

		for(i = 0; i < _RETRY; ++i) {
			Sleep(10);
			if((read_bytes = _serial_read_string_until(serial, buffer, _CONNECTOR_BUFFER_SIZE, connector->delimiter)) != 0) {
				if(start_bytes == NULL) {
					return read_bytes;
				}
				if(buffer[0] == start_bytes[0] && buffer[1] == start_bytes[1]) {
					return read_bytes;
				}
			}
		}
	}
	return 0;
}

void _connector_write(const struct _connector* connector, const char* buffer, int buffer_size) {
	if(connector == NULL || connector->serial == NULL) return;
	_serial_write(connector->serial, buffer, buffer_size);
}

int _connector_read(struct _connector* connector) {
	int read_bytes;

	if(connector == NULL || connector->serial == NULL) return 0;
	read_bytes = _serial_read_string_until(connector->serial, connector->buffer, _CONNECTOR_BUFFER_SIZE, connector->delimiter);
	if(read_bytes == connector->packet_length) {
		if(connector->found == 0) {
			if(connector->check_connection != NULL) {
				connector->check_connection(connector, connector->serial);
			}
		} else if(connector->connected == 0) {
			_connector_set_connection_state(connector, _CONNECTION_STATE_CONNECTED);
		}
		connector->checking_timeout = 0;
		connector->timestamp = 0;
		return read_bytes;
	} else if(connector->connected == 1) {
		struct timeb time;
		double t;

		t = _get_timestamp(&time);
		if(connector->checking_timeout == 0) {
			connector->checking_timeout = 1;
			connector->timestamp = t;
		} else if(t - connector->timestamp > _TIMEOUT) {
			_serial_clear(connector->serial);
			_connector_set_connection_state(connector, _CONNECTION_STATE_CONNECTION_LOST);
		}
	}
	return 0;
}

void _connector_print_state(const struct _connector* connector, int state) {
	switch(state) {
		case _CONNECTION_STATE_CONNECTED:
			printf("%s[%d] Connected: %s\n", connector->tag, connector->index, connector->port_name);
			break;
		case _CONNECTION_STATE_CONNECTION_LOST:
			printf("%s[%d] Connection lost\n", connector->tag, connector->index);
			break;
		case _CONNECTION_STATE_DISCONNECTED:
			printf("%s[%d] Disconnected\n", connector->tag, connector->index);
			break;
		case _CONNECTION_STATE_DISPOSED:
			printf("%s[%d] Disposed\n", connector->tag, connector->index);
			break;
	}
}

void _connector_print_error(const struct _connector* connector, int error_code) {
	switch(error_code) {
		case _CONNECTION_RESULT_NOT_AVAILABLE:
			printf("%s[%d] No available USB to BLE bridge\n", connector->tag, connector->index);
			break;
		case _CONNECTION_RESULT_NOT_CONNECTED:
			printf("%s[%d] Not connected\n", connector->tag, connector->index);
			break;
	}
}

/*------------------------------
  DEVICE
------------------------------*/

#define MIN(a, b) ((a) < (b) ? (a) : (b))

struct _device {
	char* name;
	int id;
	int device_type;
	int data_type;
	int data_size;
	int data_len;
	void* data;
	float min_value;
	float max_value;
	float initial_value;
	int event;
	int fired;
	int written;
	void (*reset)(struct _device* device);
	int (*read)(const struct _device* device);
	int (*read_at)(const struct _device* device, int index);
	int (*read_array)(const struct _device* device, int* data, int length);
	float (*read_float)(const struct _device* device);
	float (*read_float_at)(const struct _device* device, int index);
	int (*read_float_array)(const struct _device* device, float* data, int length);
	int (*write)(struct _device* device, int data);
	int (*write_at)(struct _device* device, int index, int data);
	int (*write_array)(struct _device* device, const int* data, int length);
	int (*write_float)(struct _device* device, float data);
	int (*write_float_at)(struct _device* device, int index, float data);
	int (*write_float_array)(struct _device* device, const float* data, int length);
	int (*put)(struct _device* device, int data);
	int (*put_at)(struct _device* device, int index, int data);
	int (*put_array)(struct _device* device, const int* data, int length);
	int (*put_float)(struct _device* device, float data);
	int (*put_float_at)(struct _device* device, int index, float data);
	int (*put_float_array)(struct _device* device, const float* data, int length);
};

struct _device* _device_create(int id, const char* name, int device_type, int data_type, int data_size, float min_value, float max_value, float initial_value);
void _device_dispose(struct _device* device);
const char* _device_get_name(const struct _device* device);
void _device_set_name(struct _device* device, const char* name);
int _device_get_id(const struct _device* device);
int _device_get_device_type(const struct _device* device);
int _device_get_data_type(const struct _device* device);
int _device_get_data_size(const struct _device* device);
int _device_e(const struct _device* device);
void _device_reset(struct _device* device);
int _device_read(const struct _device* device);
int _device_read_at(const struct _device* device, int index);
int _device_read_array(const struct _device* device, int* data, int length);
float _device_read_float(const struct _device* device);
float _device_read_float_at(const struct _device* device, int index);
int _device_read_float_array(const struct _device* device, float* data, int length);
int _device_write(struct _device* device, int data);
int _device_write_at(struct _device* device, int index, int data);
int _device_write_array(struct _device* device, const int* data, int length);
int _device_write_float(struct _device* device, float data);
int _device_write_float_at(struct _device* device, int index, float data);
int _device_write_float_array(struct _device* device, const float* data, int length);
int _device_put(struct _device* device, int data);
int _device_put_at(struct _device* device, int index, int data);
int _device_put_array(struct _device* device, const int* data, int length);
int _device_put_float(struct _device* device, float data);
int _device_put_float_at(struct _device* device, int index, float data);
int _device_put_float_array(struct _device* device, const float* data, int length);
void _device_update_device_state(struct _device* device);

void _int_device_reset(struct _device* device) {
	int* this_data = (int*)device->data;
	
	if(this_data != NULL) {
		int this_len = device->data_len, i;
		int initial_value = (int)device->initial_value;
		
		for(i = 0; i < this_len; ++i) {
			this_data[i] = initial_value;
		}
	}
	device->event = 0;
	device->fired = 0;
	device->written = 0;
}

int _int_device_read(const struct _device* device) {
	int* this_data;
	
	if(device->data_len <= 0) return 0;
	this_data = (int*)device->data;
	if(this_data == NULL) return 0;
	
	return this_data[0];
}

int _int_device_read_at(const struct _device* device, int index) {
	int* this_data;

	if(index < 0 || index >= device->data_len) return 0;
	this_data = (int*)device->data;
	if(this_data == NULL) return 0;
	
	return this_data[index];
}

int _int_device_read_array(const struct _device* device, int* data, int length) {
	int* this_data;
	int this_len, len;

	if(data == NULL) return 0;
	if(length <= 0) return 0;
	
	this_len = device->data_len;
	if(this_len <= 0) return 0;
	this_data = (int*)device->data;
	if(this_data == NULL) return 0;
	
	len = MIN(this_len, length);
	memcpy(data, this_data, sizeof(int) * len);
	return len;
}

float _int_device_read_float(const struct _device* device) {
	return (float)_int_device_read(device);
}

float _int_device_read_float_at(const struct _device* device, int index) {
	return (float)_int_device_read_at(device, index);
}

int _int_device_read_float_array(const struct _device* device, float* data, int length) {
	int* this_data;
	int this_len, len, i;

	if(data == NULL) return 0;
	if(length <= 0) return 0;
	
	this_len = device->data_len;
	if(this_len <= 0) return 0;
	this_data = (int*)device->data;
	if(this_data == NULL) return 0;
	
	len = MIN(this_len, length);
	for(i = 0; i < len; ++i) {
		data[i] = (float)this_data[i];
	}
	return len;
}

int _int_device_write(struct _device* device, int data) {
	int* this_data;
	int min_value, max_value;

	if(device->device_type == DEVICE_TYPE_SENSOR || device->device_type == DEVICE_TYPE_EVENT) return 0;
	if(device->data_len <= 0) return 0;
	this_data = (int*)device->data;
	if(this_data == NULL) return 0;
	
	min_value = (int)device->min_value;
	max_value = (int)device->max_value;
	if(data < min_value) data = min_value;
	else if(data > max_value) data = max_value;
	
	this_data[0] = data;
	device->fired = 1;
	device->written = 1;
	return 1;
}

int _int_device_write_at(struct _device* device, int index, int data) {
	int* this_data;
	int min_value, max_value;

	if(device->device_type == DEVICE_TYPE_SENSOR || device->device_type == DEVICE_TYPE_EVENT) return 0;
	if(index < 0 || index >= device->data_len) return 0;
	this_data = (int*)device->data;
	if(this_data == NULL) return 0;
	
	min_value = (int)device->min_value;
	max_value = (int)device->max_value;
	if(data < min_value) data = min_value;
	else if(data > max_value) data = max_value;
	
	this_data[index] = data;
	device->fired = 1;
	device->written = 1;
	return 1;
}

int _int_device_write_array(struct _device* device, const int* data, int length) {
	int* this_data;
	int this_len, len, i;
	int value, min_value, max_value;

	if(device->device_type == DEVICE_TYPE_SENSOR || device->device_type == DEVICE_TYPE_EVENT) return 0;
	if(data == NULL) return 0;
	if(length <= 0) return 0;
	
	this_len = device->data_len;
	if(this_len <= 0) return 0;
	this_data = (int*)device->data;
	if(this_data == NULL) return 0;
	
	len = MIN(this_len, length);
	min_value = (int)device->min_value;
	max_value = (int)device->max_value;
	for(i = 0; i < len; ++i) {
		value = data[i];
		if(value < min_value) value = min_value;
		else if(value > max_value) value = max_value;
		this_data[i] = value;
	}
	device->fired = 1;
	device->written = 1;
	return len;
}

int _int_device_write_float(struct _device* device, float data) {
	return _int_device_write(device, (int)data);
}

int _int_device_write_float_at(struct _device* device, int index, float data) {
	return _int_device_write_at(device, index, (int)data);
}

int _int_device_write_float_array(struct _device* device, const float* data, int length) {
	int* this_data;
	int this_len, len, i;
	int value, min_value, max_value;

	if(device->device_type == DEVICE_TYPE_SENSOR || device->device_type == DEVICE_TYPE_EVENT) return 0;
	if(data == NULL) return 0;
	if(length <= 0) return 0;
	
	this_len = device->data_len;
	if(this_len <= 0) return 0;
	this_data = (int*)device->data;
	if(this_data == NULL) return 0;
	
	len = MIN(this_len, length);
	min_value = (int)device->min_value;
	max_value = (int)device->max_value;
	for(i = 0; i < len; ++i) {
		value = (int)data[i];
		if(value < min_value) value = min_value;
		else if(value > max_value) value = max_value;
		this_data[i] = value;
	}
	device->fired = 1;
	device->written = 1;
	return len;
}

int _int_device_put(struct _device* device, int data) {
	int* this_data;

	if(device->data_len <= 0) return 0;
	this_data = (int*)device->data;
	if(this_data == NULL) return 0;
	
	this_data[0] = data;
	device->fired = 1;
	return 1;
}

int _int_device_put_at(struct _device* device, int index, int data) {
	int* this_data;

	if(index < 0 || index >= device->data_len) return 0;
	this_data = (int*)device->data;
	if(this_data == NULL) return 0;
	
	this_data[index] = data;
	device->fired = 1;
	return 1;
}

int _int_device_put_array(struct _device* device, const int* data, int length) {
	int* this_data;
	int this_len, len;

	if(data == NULL) return 0;
	if(length <= 0) return 0;
	
	this_len = device->data_len;
	if(this_len <= 0) return 0;
	this_data = (int*)device->data;
	if(this_data == NULL) return 0;
	
	len = MIN(this_len, length);
	memcpy(this_data, data, sizeof(int) * len);
	device->fired = 1;
	return len;
}

int _int_device_put_float(struct _device* device, float data) {
	return _int_device_put(device, (int)data);
}

int _int_device_put_float_at(struct _device* device, int index, float data) {
	return _int_device_put_at(device, index, (int)data);
}

int _int_device_put_float_array(struct _device* device, const float* data, int length) {
	int* this_data;
	int this_len, len, i;

	if(data == NULL) return 0;
	if(length <= 0) return 0;
	
	this_len = device->data_len;
	if(this_len <= 0) return 0;
	this_data = (int*)device->data;
	if(this_data == NULL) return 0;
	
	len = MIN(this_len, length);
	for(i = 0; i < len; ++i) {
		this_data[i] = (int)data[i];
	}
	device->fired = 1;
	return len;
}

void _float_device_reset(struct _device* device) {
	float* this_data = (float*)device->data;
	
	if(this_data != NULL) {
		int this_len = device->data_len, i;
		float initial_value = device->initial_value;
		
		for(i = 0; i < this_len; ++i) {
			this_data[i] = initial_value;
		}
	}
	device->event = 0;
	device->fired = 0;
	device->written = 0;
}

int _float_device_read_array(const struct _device* device, int* data, int length) {
	float* this_data;
	int this_len, len, i;

	if(data == NULL) return 0;
	if(length <= 0) return 0;
	
	this_len = device->data_len;
	if(this_len <= 0) return 0;
	this_data = (float*)device->data;
	if(this_data == NULL) return 0;
	
	len = MIN(this_len, length);
	for(i = 0; i < len; ++i) {
		data[i] = (int)this_data[i];
	}
	return len;
}

float _float_device_read_float(const struct _device* device) {
	float* this_data;

	if(device->data_len <= 0) return 0.0f;
	this_data = (float*)device->data;
	if(this_data == NULL) return 0.0f;
	
	return this_data[0];
}

float _float_device_read_float_at(const struct _device* device, int index) {
	float* this_data;

	if(index < 0 || index >= device->data_len) return 0.0f;
	this_data = (float*)device->data;
	if(this_data == NULL) return 0.0f;
	
	return this_data[index];
}

int _float_device_read(const struct _device* device) {
	return (int)_float_device_read_float(device);
}

int _float_device_read_at(const struct _device* device, int index) {
	return (int)_float_device_read_float_at(device, index);
}

int _float_device_read_float_array(const struct _device* device, float* data, int length) {
	float* this_data;
	int this_len, len;

	if(data == NULL) return 0;
	if(length <= 0) return 0;
	
	this_len = device->data_len;
	if(this_len <= 0) return 0;
	this_data = (float*)device->data;
	if(this_data == NULL) return 0;
	
	len = MIN(this_len, length);
	memcpy(data, this_data, sizeof(float) * len);
	return len;
}

int _float_device_write_array(struct _device* device, const int* data, int length) {
	float* this_data;
	int this_len, len, i;
	float value, min_value, max_value;

	if(device->device_type == DEVICE_TYPE_SENSOR || device->device_type == DEVICE_TYPE_EVENT) return 0;
	if(data == NULL) return 0;
	if(length <= 0) return 0;
	
	this_len = device->data_len;
	if(this_len <= 0) return 0;
	this_data = (float*)device->data;
	if(this_data == NULL) return 0;
	
	len = MIN(this_len, length);
	min_value = device->min_value;
	max_value = device->max_value;
	for(i = 0; i < len; ++i) {
		value = (float)data[i];
		if(value < min_value) value = min_value;
		else if(value > max_value) value = max_value;
		this_data[i] = value;
	}
	device->fired = 1;
	device->written = 1;
	return len;
}

int _float_device_write_float(struct _device* device, float data) {
	float* this_data;
	float min_value, max_value;

	if(device->device_type == DEVICE_TYPE_SENSOR || device->device_type == DEVICE_TYPE_EVENT) return 0;
	if(device->data_len <= 0) return 0;
	this_data = (float*)device->data;
	if(this_data == NULL) return 0;
	
	min_value = device->min_value;
	max_value = device->max_value;
	if(data < min_value) data = min_value;
	else if(data > max_value) data = max_value;
	
	this_data[0] = data;
	device->fired = 1;
	device->written = 1;
	return 1;
}

int _float_device_write_float_at(struct _device* device, int index, float data) {
	float* this_data;
	float min_value, max_value;

	if(device->device_type == DEVICE_TYPE_SENSOR || device->device_type == DEVICE_TYPE_EVENT) return 0;
	if(index < 0 || index >= device->data_len) return 0;
	this_data = (float*)device->data;
	if(this_data == NULL) return 0;
	
	min_value = device->min_value;
	max_value = device->max_value;
	if(data < min_value) data = min_value;
	else if(data > max_value) data = max_value;
	
	this_data[index] = data;
	device->fired = 1;
	device->written = 1;
	return 1;
}

int _float_device_write(struct _device* device, int data) {
	return _float_device_write_float(device, (float)data);
}

int _float_device_write_at(struct _device* device, int index, int data) {
	return _float_device_write_float_at(device, index, (float)data);
}

int _float_device_write_float_array(struct _device* device, const float* data, int length) {
	float* this_data;
	int this_len, len, i;
	float value, min_value, max_value;

	if(device->device_type == DEVICE_TYPE_SENSOR || device->device_type == DEVICE_TYPE_EVENT) return 0;
	if(data == NULL) return 0;
	if(length <= 0) return 0;
	
	this_len = device->data_len;
	if(this_len <= 0) return 0;
	this_data = (float*)device->data;
	if(this_data == NULL) return 0;
	
	len = MIN(this_len, length);
	min_value = device->min_value;
	max_value = device->max_value;
	for(i = 0; i < len; ++i) {
		value = data[i];
		if(value < min_value) value = min_value;
		else if(value > max_value) value = max_value;
		this_data[i] = value;
	}
	device->fired = 1;
	device->written = 1;
	return len;
}

int _float_device_put_array(struct _device* device, const int* data, int length) {
	float* this_data;
	int this_len, len, i;

	if(data == NULL) return 0;
	if(length <= 0) return 0;
	
	this_len = device->data_len;
	if(this_len <= 0) return 0;
	this_data = (float*)device->data;
	if(this_data == NULL) return 0;
	
	len = MIN(this_len, length);
	for(i = 0; i < len; ++i) {
		this_data[i] = (float)data[i];
	}
	device->fired = 1;
	return len;
}

int _float_device_put_float(struct _device* device, float data) {
	float* this_data;

	if(device->data_len <= 0) return 0;
	this_data = (float*)device->data;
	if(this_data == NULL) return 0;
	
	this_data[0] = data;
	device->fired = 1;
	return 1;
}

int _float_device_put_float_at(struct _device* device, int index, float data) {
	float* this_data;

	if(index < 0 || index >= device->data_len) return 0;
	this_data = (float*)device->data;
	if(this_data == NULL) return 0;
	
	this_data[index] = data;
	device->fired = 1;
	return 1;
}

int _float_device_put(struct _device* device, int data) {
	return _float_device_put_float(device, (float)data);
}

int _float_device_put_at(struct _device* device, int index, int data) {
	return _float_device_put_float_at(device, index, (float)data);
}

int _float_device_put_float_array(struct _device* device, const float* data, int length) {
	float* this_data;
	int this_len, len;

	if(data == NULL) return 0;
	if(length <= 0) return 0;
	
	this_len = device->data_len;
	if(this_len <= 0) return 0;
	this_data = (float*)device->data;
	if(this_data == NULL) return 0;
	
	len = MIN(this_len, length);
	memcpy(this_data, data, sizeof(float) * len);
	device->fired = 1;
	return len;
}

struct _device* _device_create(int id, const char* name, int device_type, int data_type, int data_size, float min_value, float max_value, float initial_value) {
	if(data_type == DATA_TYPE_INTEGER || data_type == DATA_TYPE_FLOAT) {
		struct _device* device = (struct _device*)malloc(sizeof(struct _device));
		int len;

		if(name == NULL) name = "";
		len = strlen(name) + 1;
		device->name = (char*)malloc(sizeof(char) * len);
		_STRCPY(device->name, len, name);

		device->id = id & 0xfff00fff;
		device->device_type = device_type;
		device->data_type = data_type;
		device->data_size = data_size;
		device->data_len = 0;
		device->data = NULL;
		device->min_value = min_value;
		device->max_value = max_value;
		device->initial_value = initial_value;
		device->event = 0;
		device->fired = 0;
		device->written = 0;

		if(data_type == DATA_TYPE_INTEGER) {
			device->reset = _int_device_reset;
			device->read = _int_device_read;
			device->read_at = _int_device_read_at;
			device->read_array = _int_device_read_array;
			device->read_float = _int_device_read_float;
			device->read_float_at = _int_device_read_float_at;
			device->read_float_array = _int_device_read_float_array;
			device->write = _int_device_write;
			device->write_at = _int_device_write_at;
			device->write_array = _int_device_write_array;
			device->write_float = _int_device_write_float;
			device->write_float_at = _int_device_write_float_at;
			device->write_float_array = _int_device_write_float_array;
			device->put = _int_device_put;
			device->put_at = _int_device_put_at;
			device->put_array = _int_device_put_array;
			device->put_float = _int_device_put_float;
			device->put_float_at = _int_device_put_float_at;
			device->put_float_array = _int_device_put_float_array;
		
			if(data_size > 0) {
				int* data;
				int i;

				data = (int*)malloc(sizeof(int) * data_size);
				for(i = 0; i < data_size; ++i) {
					data[i] = (int)initial_value;
				}
				device->data = data;
				device->data_len = data_size;
			}
		} else if(data_type == DATA_TYPE_FLOAT) {
			device->reset = _float_device_reset;
			device->read = _float_device_read;
			device->read_at = _float_device_read_at;
			device->read_array = _float_device_read_array;
			device->read_float = _float_device_read_float;
			device->read_float_at = _float_device_read_float_at;
			device->read_float_array = _float_device_read_float_array;
			device->write = _float_device_write;
			device->write_at = _float_device_write_at;
			device->write_array = _float_device_write_array;
			device->write_float = _float_device_write_float;
			device->write_float_at = _float_device_write_float_at;
			device->write_float_array = _float_device_write_float_array;
			device->put = _float_device_put;
			device->put_at = _float_device_put_at;
			device->put_array = _float_device_put_array;
			device->put_float = _float_device_put_float;
			device->put_float_at = _float_device_put_float_at;
			device->put_float_array = _float_device_put_float_array;
		
			if(data_size > 0) {
				float* data;
				int i;

				data = (float*)malloc(sizeof(float) * data_size);
				for(i = 0; i < data_size; ++i) {
					data[i] = initial_value;
				}
				device->data = data;
				device->data_len = data_size;
			}
		}
		return device;
	}
	return NULL;
}

void _device_dispose(struct _device* device) {
	if(device == NULL) return;
	
	if(device->name != NULL) {
		free(device->name);
		device->name = NULL;
	}
	if(device->data_type == DATA_TYPE_INTEGER) {
		int* data = (int*)device->data;
		if(data != NULL) {
			free(data);
			device->data = NULL;
		}
	} else if(device->data_type == DATA_TYPE_FLOAT) {
		float* data = (float*)device->data;
		if(data != NULL) {
			free(data);
			device->data = NULL;
		}
	}
	free(device);
}

const char* _device_get_name(const struct _device* device) {
	if(device == NULL) return "";
	return device->name;
}

void _device_set_name(struct _device* device, const char* name) {
	int len;

	if(device == NULL) return;
	if(name == NULL) name = "";
	if(device->name != NULL) {
		free(device->name);
	}
	len = strlen(name) + 1;
	device->name = (char*)malloc(sizeof(char) * len);
	_STRCPY(device->name, len, name);
}

int _device_get_id(const struct _device* device) {
	if(device == NULL) return 0;
	return device->id;
}

int _device_get_device_type(const struct _device* device) {
	if(device == NULL) return 0;
	return device->device_type;
}

int _device_get_data_type(const struct _device* device) {
	if(device == NULL) return 0;
	return device->data_type;
}

int _device_get_data_size(const struct _device* device) {
	if(device == NULL) return 0;
	return device->data_size;
}

int _device_e(const struct _device* device) {
	if(device == NULL) return 0;
	return device->event;
}

void _device_reset(struct _device* device) {
	if(device == NULL) return;
	device->reset(device);
}

int _device_read(const struct _device* device) {
	if(device == NULL) return 0;
	return device->read(device);
}

int _device_read_at(const struct _device* device, int index) {
	if(device == NULL) return 0;
	return device->read_at(device, index);
}

int _device_read_array(const struct _device* device, int* data, int length) {
	if(device == NULL) return 0;
	return device->read_array(device, data, length);
}

float _device_read_float(const struct _device* device) {
	if(device == NULL) return 0.0f;
	return device->read_float(device);
}

float _device_read_float_at(const struct _device* device, int index) {
	if(device == NULL) return 0.0f;
	return device->read_float_at(device, index);
}

int _device_read_float_array(const struct _device* device, float* data, int length) {
	if(device == NULL) return 0;
	return device->read_float_array(device, data, length);
}

int _device_write(struct _device* device, int data) {
	if(device == NULL) return 0;
	return device->write(device, data);
}

int _device_write_at(struct _device* device, int index, int data) {
	if(device == NULL) return 0;
	return device->write_at(device, index, data);
}

int _device_write_array(struct _device* device, const int* data, int length) {
	if(device == NULL) return 0;
	return device->write_array(device, data, length);
}

int _device_write_float(struct _device* device, float data) {
	if(device == NULL) return 0;
	return device->write_float(device, data);
}

int _device_write_float_at(struct _device* device, int index, float data) {
	if(device == NULL) return 0;
	return device->write_float_at(device, index, data);
}

int _device_write_float_array(struct _device* device, const float* data, int length) {
	if(device == NULL) return 0;
	return device->write_float_array(device, data, length);
}

int _device_put(struct _device* device, int data) {
	if(device == NULL) return 0;
	return device->put(device, data);
}

int _device_put_at(struct _device* device, int index, int data) {
	if(device == NULL) return 0;
	return device->put_at(device, index, data);
}

int _device_put_array(struct _device* device, const int* data, int length) {
	if(device == NULL) return 0;
	return device->put_array(device, data, length);
}

int _device_put_float(struct _device* device, float data) {
	if(device == NULL) return 0;
	return device->put_float(device, data);
}

int _device_put_float_at(struct _device* device, int index, float data) {
	if(device == NULL) return 0;
	return device->put_float_at(device, index, data);
}

int _device_put_float_array(struct _device* device, const float* data, int length) {
	if(device == NULL) return 0;
	return device->put_float_array(device, data, length);
}

void _device_update_device_state(struct _device* device) {
	if(device == NULL) return;
	device->event = device->fired;
	device->fired = 0;
}

/*------------------------------
  UTIL
------------------------------*/

int _hex_char_to_value(char character);
int _value_to_hex(char* buffer, int index, int value, int bytes);
int _hex_to_value(const char* str, int start, int end);

char _HEX_DIGITS[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };

int _hex_char_to_value(char character) {
	switch(character) {
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			return (int)(character - '0');
		case 'a':
		case 'b':
		case 'c':
		case 'd':
		case 'e':
		case 'f':
			return (int)(character - 'a' + 10);
		case 'A':
		case 'B':
		case 'C':
		case 'D':
		case 'E':
		case 'F':
			return (int)(character - 'A' + 10);
	}
	return ' ';
}

int _value_to_hex(char* buffer, int index, int value, int bytes) {
	int high, low, val, i;

	for(i = 0; i < bytes; ++i) {
		val = value >> ((bytes - i - 1) * 8);
		high = (val >> 4) & 0xf;
		low = val & 0xf;
		buffer[index++] = _HEX_DIGITS[high];
		buffer[index++] = _HEX_DIGITS[low];
	}
	return index;
}

int _hex_to_value(const char* str, int start, int end) {
	int result = 0, i;

	for(i = start; i < end; ++i) {
		result <<= 4;
		result += _hex_char_to_value(str[i]);
	}
	return result;
}

/*------------------------------
  ROBOT
------------------------------*/

struct _robot;
typedef void (*_REQUEST_MOTORING_DATA)(struct _robot* robot);
typedef void (*_UPDATE_SENSORY_DEVICE_STATE)(struct _robot* robot);
typedef void (*_UPDATE_MOTORING_DEVICE_STATE)(struct _robot* robot);
typedef void (*_RESET)(struct _robot* robot);
typedef void (*_DISPOSE)(struct _robot* robot);

struct _robot {
	int index;
	char* name;
	char* write_buffer;
	int devices_size;
	struct _device** devices;
	struct _connector* connector;
	int alive;
	int running;
	int ready;
	int thread_alive;
	HANDLE thread_handle;
	_REQUEST_MOTORING_DATA request_motoring_data;
	_UPDATE_SENSORY_DEVICE_STATE update_sensory_device_state;
	_UPDATE_MOTORING_DEVICE_STATE update_motoring_device_state;
	_RESET reset;
	_DISPOSE dispose;
};

void _robot_init(struct _robot* robot, int index, const char* name, int write_buffer_size) {
	if(robot != NULL) {
		int len;
		robot->index = index;
		if(name == NULL) name = "";
		len = strlen(name) + 1;
		robot->name = (char*)malloc(sizeof(char) * len);
		_STRCPY(robot->name, len, name);
		robot->write_buffer = (char*)malloc(sizeof(char) * write_buffer_size);
		robot->alive = 0;
		robot->running = 0;
		robot->ready = 0;
		robot->thread_alive = 0;
		robot->thread_handle = 0;
	}
}

void _robot_dispose(struct _robot* robot) {
	if(robot != NULL) {
		struct _device** devices = robot->devices;
		int size = robot->devices_size, i;

		if(robot->connector != NULL) {
			_connector_dispose(robot->connector);
			robot->connector = NULL;
		}
		if(robot->name != NULL) {
			free(robot->name);
			robot->name = NULL;
		}
		if(robot->write_buffer != NULL) {
			free(robot->write_buffer);
			robot->write_buffer = NULL;
		}
		for(i = 0; i < size; ++i) {
			_device_dispose(devices[i]);
			devices[i] = NULL;
		}
		free(devices);
		robot->devices = NULL;
		robot->devices_size = 0;
	}
}

const char* _robot_get_name(struct _robot* robot) {
	if(robot == NULL) return "";
	return robot->name;
}

void _robot_set_name(struct _robot* robot, const char* name) {
	if(robot != NULL) {
		int len;
		if(robot->name != NULL) {
			free(robot->name);
		}
		if(name == NULL) name = "";
		len = strlen(name) + 1;
		robot->name = (char*)malloc(sizeof(char) * len);
		_STRCPY(robot->name, len, name);
	}
}

int _robot_get_index(struct _robot* robot) {
	if(robot == NULL) return 0;
	return robot->index;
}

void _robot_reset(struct _robot* robot) {
	if(robot != NULL) {
		struct _device** devices = robot->devices;
		int size = robot->devices_size, i;
		for(i = 0; i < size; ++i) {
			_device_reset(devices[i]);
		}
	}
}

void _robot_clear_written(struct _robot* robot) {
	if(robot != NULL) {
		struct _device** devices = robot->devices;
		int size = robot->devices_size, i;
		for(i = 0; i < size; ++i) {
			devices[i]->written = 0;
		}
	}
}

struct _device* _robot_find_device(struct _robot* robot, int device_id) {
	if(robot == NULL) return NULL;
	if(robot->devices == NULL) return NULL;
	return robot->devices[device_id & 0xffff];
}

int _robot_e(struct _robot* robot, int device_id) {
	struct _device* device = _robot_find_device(robot, device_id);
	return _device_e(device);
}

int _robot_read(struct _robot* robot, int device_id) {
	struct _device* device = _robot_find_device(robot, device_id);
	return _device_read(device);
}

int _robot_read_at(struct _robot* robot, int device_id, int index) {
	struct _device* device = _robot_find_device(robot, device_id);
	return _device_read_at(device, index);
}

int _robot_read_array(struct _robot* robot, int device_id, int* data, int length) {
	struct _device* device = _robot_find_device(robot, device_id);
	return _device_read_array(device, data, length);
}

float _robot_read_float(struct _robot* robot, int device_id) {
	struct _device* device = _robot_find_device(robot, device_id);
	return _device_read_float(device);
}

float _robot_read_float_at(struct _robot* robot, int device_id, int index) {
	struct _device* device = _robot_find_device(robot, device_id);
	return _device_read_float_at(device, index);
}

int _robot_read_float_array(struct _robot* robot, int device_id, float* data, int length) {
	struct _device* device = _robot_find_device(robot, device_id);
	return _device_read_float_array(device, data, length);
}

int _robot_write(struct _robot* robot, int device_id, int data) {
	struct _device* device = _robot_find_device(robot, device_id);
	return _device_write(device, data);
}

int _robot_write_at(struct _robot* robot, int device_id, int index, int data) {
	struct _device* device = _robot_find_device(robot, device_id);
	return _device_write_at(device, index, data);
}

int _robot_write_array(struct _robot* robot, int device_id, const int* data, int length) {
	struct _device* device = _robot_find_device(robot, device_id);
	return _device_write_array(device, data, length);
}

int _robot_write_float(struct _robot* robot, int device_id, float data) {
	struct _device* device = _robot_find_device(robot, device_id);
	return _device_write_float(device, data);
}

int _robot_write_float_at(struct _robot* robot, int device_id, int index, float data) {
	struct _device* device = _robot_find_device(robot, device_id);
	return _device_write_float_at(device, index, data);
}

int _robot_write_float_array(struct _robot* robot, int device_id, const float* data, int length) {
	struct _device* device = _robot_find_device(robot, device_id);
	return _device_write_float_array(device, data, length);
}

#define _NUM_ROBOT_GROUPS 1

struct _robot_group {
	int robots_size;
	int robots_count;
	struct _robot** robots;
};

struct _robot_group** _robot_groups = NULL;

void _robot_group_dispose_all(void) {
	if(_robot_groups != NULL) {
		struct _robot_group* group;
		int i;
		for(i = 0; i < _NUM_ROBOT_GROUPS; ++i) {
			group = _robot_groups[i];
			if(group != NULL) {
				if(group->robots != NULL) {
					free(group->robots);
					group->robots = NULL;
				}
				free(group);
				_robot_groups[i] = NULL;
			}
		}
		free(_robot_groups);
		_robot_groups = NULL;
	}
}

struct _robot_group* _robot_group_get(int group_index) {
	struct _robot_group* group;
	
	if(group_index < 0 || group_index >= _NUM_ROBOT_GROUPS) return NULL;
	if(_robot_groups == NULL) {
		int i;
		_robot_groups = (struct _robot_group**)malloc(sizeof(struct _robot_group*) * _NUM_ROBOT_GROUPS);
		for(i = 0; i < _NUM_ROBOT_GROUPS; ++i) {
			_robot_groups[i] = NULL;
		}
	}
	group = _robot_groups[group_index];
	if(group == NULL) {
		group = (struct _robot_group*)malloc(sizeof(struct _robot_group));
		group->robots_size = 1;
		group->robots_count = 0;
		group->robots = (struct _robot**)malloc(sizeof(struct _robot*));
		group->robots[0] = NULL;
		_robot_groups[group_index] = group;
	}
	return group;
}

struct _robot* _robot_group_get_robot(int group_index, int robot_index) {
	struct _robot_group* group = _robot_group_get(group_index);
	if(group == NULL) return NULL;
	if(robot_index < 0 || robot_index >= group->robots_count) return NULL;
	return group->robots[robot_index];
}

void _robot_group_add_robot(int group_index, struct _robot* robot) {
	if(robot != NULL) {
		struct _robot_group* group = _robot_group_get(group_index);
		if(group != NULL) {
			if(group->robots_size <= group->robots_count) {
				int size = group->robots_size * 2, i;
				struct _robot** temp = (struct _robot**)malloc(sizeof(struct _robot*) * size);
				for(i = 0; i < size; ++i) {
					temp[i] = NULL;
				}
				memcpy(temp, group->robots, sizeof(struct _robot*) * group->robots_size);
				free(group->robots);
				group->robots = temp;
				group->robots_size = size;
			}
			group->robots[group->robots_count] = robot;
			group->robots_count ++;
		}
	}
}

void _robot_group_remove_robot(int group_index, struct _robot* robot) {
	if(robot != NULL) {
		struct _robot_group* group = _robot_group_get(group_index);
		if(group == NULL) return;
		if(robot->index < 0 || robot->index >= group->robots_size) return;
		group->robots[robot->index] = NULL;
	}
}

int _robot_group_count_robots(int group_index) {
	struct _robot_group* group = _robot_group_get(group_index);
	if(group == NULL) return 0;
	return group->robots_count;
}

const char* _robot_group_get_name(int group_index, int robot_index) {
	struct _robot* robot = _robot_group_get_robot(group_index, robot_index);
	return _robot_get_name(robot);
}

void _robot_group_set_name(int group_index, int robot_index, const char* name) {
	struct _robot* robot = _robot_group_get_robot(group_index, robot_index);
	_robot_set_name(robot, name);
}

void _robot_group_reset(int group_index, int robot_index) {
	struct _robot* robot = _robot_group_get_robot(group_index, robot_index);
	if(robot != NULL) {
		robot->reset(robot);
	}
}

void _robot_group_dispose(int group_index, int robot_index) {
	struct _robot* robot = _robot_group_get_robot(group_index, robot_index);
	if(robot != NULL) {
		robot->alive = 0;
		robot->reset(robot);
		robot->dispose(robot);
	}
}

int _robot_group_e(int group_index, int robot_index, int device_id) {
	struct _robot* robot = _robot_group_get_robot(group_index, robot_index);
	return _robot_e(robot, device_id);
}

int _robot_group_read(int group_index, int robot_index, int device_id) {
	struct _robot* robot = _robot_group_get_robot(group_index, robot_index);
	return _robot_read(robot, device_id);
}

int _robot_group_read_at(int group_index, int robot_index, int device_id, int index) {
	struct _robot* robot = _robot_group_get_robot(group_index, robot_index);
	return _robot_read_at(robot, device_id, index);
}

int _robot_group_read_array(int group_index, int robot_index, int device_id, int* data, int length) {
	struct _robot* robot = _robot_group_get_robot(group_index, robot_index);
	return _robot_read_array(robot, device_id, data, length);
}

float _robot_group_read_float(int group_index, int robot_index, int device_id) {
	struct _robot* robot = _robot_group_get_robot(group_index, robot_index);
	return _robot_read_float(robot, device_id);
}

float _robot_group_read_float_at(int group_index, int robot_index, int device_id, int index) {
	struct _robot* robot = _robot_group_get_robot(group_index, robot_index);
	return _robot_read_float_at(robot, device_id, index);
}

int _robot_group_read_float_array(int group_index, int robot_index, int device_id, float* data, int length) {
	struct _robot* robot = _robot_group_get_robot(group_index, robot_index);
	return _robot_read_float_array(robot, device_id, data, length);
}

int _robot_group_write(int group_index, int robot_index, int device_id, int data) {
	struct _robot* robot = _robot_group_get_robot(group_index, robot_index);
	return _robot_write(robot, device_id, data);
}

int _robot_group_write_at(int group_index, int robot_index, int device_id, int index, int data) {
	struct _robot* robot = _robot_group_get_robot(group_index, robot_index);
	return _robot_write_at(robot, device_id, index, data);
}

int _robot_group_write_array(int group_index, int robot_index, int device_id, const int* data, int length) {
	struct _robot* robot = _robot_group_get_robot(group_index, robot_index);
	return _robot_write_array(robot, device_id, data, length);
}

int _robot_group_write_float(int group_index, int robot_index, int device_id, float data) {
	struct _robot* robot = _robot_group_get_robot(group_index, robot_index);
	return _robot_write_float(robot, device_id, data);
}

int _robot_group_write_float_at(int group_index, int robot_index, int device_id, int index, float data) {
	struct _robot* robot = _robot_group_get_robot(group_index, robot_index);
	return _robot_write_float_at(robot, device_id, index, data);
}

int _robot_group_write_float_array(int group_index, int robot_index, int device_id, const float* data, int length) {
	struct _robot* robot = _robot_group_get_robot(group_index, robot_index);
	return _robot_write_float_array(robot, device_id, data, length);
}

/*------------------------------
  RUNNER
------------------------------*/

typedef void (*_EXECUTE)(void* arg);
typedef int (*_EVALUATE)(void* arg);

struct _runner {
	int started;
	int connection_required;
	int connection_checked;
	int robots_count;
	int robots_size;
	struct _robot** robots;
	_EXECUTE execute;
	void* execute_arg;
	_EVALUATE evaluate;
	void* evaluate_arg;
	int evaluate_result;
	int running;
	int thread_alive;
	HANDLE thread_handle;
};

struct _runner* _runner = NULL;

void _runner_create(void);
void _runner_shutdown(void);
void _runner_start(void);
void _runner_register_required(void);
void _runner_register_checked(void);
int _runner_is_all_checked(void);
void _runner_add_robot(struct _robot* robot);

void _runner_create(void) {
	if(_runner != NULL) return;
	_runner = (struct _runner*)malloc(sizeof(struct _runner));
	_runner->started = 0;
	_runner->connection_required = 0;
	_runner->connection_checked = 0;
	_runner->robots_count = 0;
	_runner->robots_size = 1;
	_runner->robots = (struct _robot**)malloc(sizeof(struct _robot*));
	_runner->robots[0] = NULL;
	_runner->execute = NULL;
	_runner->execute_arg = NULL;
	_runner->evaluate = NULL;
	_runner->evaluate_arg = NULL;
	_runner->evaluate_result = 0;
	_runner->running = 0;
	_runner->thread_alive = 0;
	_runner->thread_handle = 0;
}

void _runner_shutdown(void) {
	if(_runner != NULL) {
		struct _robot** robots = _runner->robots;
		int count = _runner->robots_count, i;
		struct _robot* robot;

		if(robots != NULL) {
			for(i = 0; i < count; ++i) {
				robot = robots[i];
				if(robot != NULL) {
					robot->alive = 0;
					robot->reset(robot);
				}
			}
		}

		_runner->running = 0;
		if(_runner->thread_alive == 1) {
			WaitForSingleObject(_runner->thread_handle, INFINITE);
		}

		if(robots != NULL) {
			for(i = 0; i < count; ++i) {
				robot = robots[i];
				if(robot != NULL) {
					robot->dispose(robot);
					robots[i] = NULL;
				}
			}
			free(robots);
			_runner->robots = NULL;
		}
		free(_runner);
		_runner = NULL;
	}
}

unsigned WINAPI _runner_thread_proc(void* arg) {
	struct _runner* runner = (struct _runner*)arg;
	struct _robot** robots;
	struct _robot* robot;
	int count, i;
	struct timeb time;
	double target_time;

	if(runner == NULL) return 0;
	
	runner->thread_alive = 1;
	target_time = _get_timestamp(&time);
	while(runner->running == 1) {
		if(_get_timestamp(&time) > target_time) {
			robots = runner->robots;
			count = runner->robots_count;
	
			for(i = 0; i < count; ++i) {
				robot = robots[i];
				if(robot != NULL && robot->alive == 1) {
					robot->update_sensory_device_state(robot);
				}
			}
			
			if(runner->evaluate != NULL) {
				runner->evaluate_result = runner->evaluate(runner->evaluate_arg);
				if(runner->evaluate_result == 1) {
					runner->evaluate = NULL;
				}
			}
			
			if(runner->execute != NULL) {
				runner->execute(runner->execute_arg);
			}
			
			for(i = 0; i < count; ++i) {
				robot = robots[i];
				if(robot != NULL && robot->alive == 1) {
					robot->request_motoring_data(robot);
				}
			}
			for(i = 0; i < count; ++i) {
				robot = robots[i];
				if(robot != NULL && robot->alive == 1) {
					robot->update_motoring_device_state(robot);
				}
			}
			target_time += 0.02;
		}
		Sleep(5);
	}
	runner->thread_alive = 0;
	return 0;
}

void _runner_signal_handler(int sig) {
	dispose_all();
	exit(1);
}

void _runner_start(void) {
	if(_runner == NULL) return;
	if(_runner->started == 0) {
		unsigned int thread_id;
		
		_runner->started = 1;
		_runner->running = 1;
		signal(SIGINT, _runner_signal_handler);
		_runner->thread_handle = (HANDLE)_beginthreadex(NULL,
			0,
			_runner_thread_proc,
			_runner,
			CREATE_SUSPENDED,
			&thread_id);
		if(_runner->thread_handle != 0) {
			ResumeThread(_runner->thread_handle);
		}
	}
}

void _runner_register_required(void) {
	if(_runner == NULL) {
		_runner_create();
	}
	_runner->connection_required ++;
}

void _runner_register_checked(void) {
	if(_runner == NULL) {
		_runner_create();
	}
	_runner->connection_checked ++;
}

int _runner_is_all_checked(void) {
	if(_runner == NULL) {
		_runner_create();
	}
	return (_runner->connection_checked >= _runner->connection_required) ? 1 : 0;
}

void _runner_add_robot(struct _robot* robot) {
	if(robot == NULL) return;
	if(_runner == NULL) {
		_runner_create();
	}
	if(_runner->robots_size <= _runner->robots_count) {
		int size = _runner->robots_size * 2, i;
		struct _robot** temp = (struct _robot**)malloc(sizeof(struct _robot*) * size);
		for(i = 0; i < size; ++i) {
			temp[i] = 0;
		}
		memcpy(temp, _runner->robots, sizeof(struct _robot*) * _runner->robots_size);
		free(_runner->robots);
		_runner->robots = temp;
		_runner->robots_size = size;
	}
	_runner->robots[_runner->robots_count] = robot;
	_runner->robots_count ++;
}

void _runner_remove_robot(struct _robot* robot) {
	struct _robot** robots;
	int count, i;
	
	if(robot == NULL) return;
	if(_runner == NULL) return;
	count = _runner->robots_count;
	robots = _runner->robots;
	for(i = 0; i < count; ++i) {
		if(robots[i] == robot) {
			robots[i] = NULL;
		}
	}
}

/*------------------------------
  COMMON
------------------------------*/

void scan(void) {
	int port_count = 0;
	char** port_names;
	
	printf("Serial ports:\n");

	port_names = _serial_window_get_serial_port_names(NULL, &port_count);
	if(port_count > 0 && port_names != NULL) {
		int i;
		for(i = 0; i < port_count; ++i) {
			printf("%s\n", port_names[i]);
			free(port_names[i]);
		}
		free(port_names);
	} else {
		printf("No available serial port\n");
	}
}

void set_executable(void (*execute)(void* arg), void* arg) {
	if(_runner == NULL) {
		_runner_create();
	}
	_runner->execute_arg = arg;
	_runner->execute = execute;
}

void wait(int milliseconds) {
	if(milliseconds > 0) {
		struct timeb time;
		double timeout;
		
		timeout = _get_timestamp(&time) + (double)milliseconds / 1000.0;
		while(1) {
			if(_get_timestamp(&time) >= timeout) {
				break;
			}
			Sleep(1);
		}
	}
}

void wait_until(int (*evaluate)(void* arg), void* arg) {
	if(_runner == NULL) {
		_runner_create();
	}
	_runner->evaluate_arg = arg;
	_runner->evaluate_result = 0;
	_runner->evaluate = evaluate;
	while(_runner->evaluate_result == 0) {
		Sleep(10);
	}
}

void wait_until_ready(void) {
	while(_runner_is_all_checked() == 0) {
		Sleep(10);
	}
}

void dispose_all(void) {
	_runner_shutdown();
	_robot_group_dispose_all();
}

/*------------------------------
  HAMSTER
------------------------------*/

#define _HAMSTER_LEFT_WHEEL_INDEX 0
#define _HAMSTER_RIGHT_WHEEL_INDEX 1
#define _HAMSTER_BUZZER_INDEX 2
#define _HAMSTER_OUTPUT_A_INDEX 3
#define _HAMSTER_OUTPUT_B_INDEX 4
#define _HAMSTER_TOPOLOGY_INDEX 5
#define _HAMSTER_LEFT_LED_INDEX 6
#define _HAMSTER_RIGHT_LED_INDEX 7
#define _HAMSTER_NOTE_INDEX 8
#define _HAMSTER_LINE_TRACER_MODE_INDEX 9
#define _HAMSTER_LINE_TRACER_SPEED_INDEX 10
#define _HAMSTER_IO_MODE_A_INDEX 11
#define _HAMSTER_IO_MODE_B_INDEX 12
#define _HAMSTER_CONFIG_PROXIMITY_INDEX 13
#define _HAMSTER_CONFIG_GRAVITY_INDEX 14
#define _HAMSTER_CONFIG_BAND_WIDTH_INDEX 15
#define _HAMSTER_SIGNAL_STRENGTH_INDEX 16
#define _HAMSTER_LEFT_PROXIMITY_INDEX 17
#define _HAMSTER_RIGHT_PROXIMITY_INDEX 18
#define _HAMSTER_LEFT_FLOOR_INDEX 19
#define _HAMSTER_RIGHT_FLOOR_INDEX 20
#define _HAMSTER_ACCELERATION_INDEX 21
#define _HAMSTER_LIGHT_INDEX 22
#define _HAMSTER_TEMPERATURE_INDEX 23
#define _HAMSTER_INPUT_A_INDEX 24
#define _HAMSTER_INPUT_B_INDEX 25
#define _HAMSTER_LINE_TRACER_STATE_INDEX 26

#define _GROUP_HAMSTER 0

struct _hamster_robot {
	struct _robot robot;
	int left_wheel;
	int right_wheel;
	float buzzer;
	int output_a;
	int output_b;
	int topology;
	int left_led;
	int right_led;
	int note;
	int line_tracer_mode;
	int line_tracer_speed;
	int io_mode_a;
	int io_mode_b;
	int config_proximity;
	int config_gravity;
	int config_band_width;
	int light;
	int temperature;
	double bpm;
	int line_tracer_mode_written;
	int line_tracer_flag;
	int line_tracer_state;
	int line_tracer_event;
	int board_count;
	int board_state;
};

int _hamster_check_connection(struct _connector* connector, struct _serial* serial) {
	int i, read_bytes;

	for(i = 0; i < _RETRY; ++i) {
		_serial_write(serial, "FF\r", 3);
		read_bytes = _connector_read_packet(connector, serial, "FF");
		if(read_bytes != 0) {
			char tmp[5][256];
			int index = 0, len;
			char* buf;
			char* comma;

			buf = connector->buffer;
			comma = strchr(buf, ',');
			while(comma != NULL && index < 5) {
				len = comma - buf;
				_STRNCPY(tmp[index], 256, buf, len);
				tmp[index][len] = '\0';
				index ++;
				buf = comma + 1;
				comma = strchr(buf, ',');
			}
			if(index >= 4 && buf != NULL && strlen(buf) >= 12) {
				if(_STRICMP(tmp[1], "Hamster") == 0 && _STRICMP(tmp[2], "04") == 0) {
					buf[12] = '\0';
					_connector_set_address(connector, buf);
					_connector_set_connection_state(connector, _CONNECTION_STATE_CONNECTED);
					return _CONNECTION_RESULT_FOUND;
				}
			}
			return _CONNECTION_RESULT_NOT_AVAILABLE;
		}
	}
	return _CONNECTION_RESULT_NOT_AVAILABLE;
}

void _hamster_request_motoring_data(struct _robot* robot) {
	struct _hamster_robot* hamster = (struct _hamster_robot*)robot;
	struct _device** devices = robot->devices;
	
	hamster->left_wheel = _device_read(devices[_HAMSTER_LEFT_WHEEL_INDEX]);
	hamster->right_wheel = _device_read(devices[_HAMSTER_RIGHT_WHEEL_INDEX]);
	hamster->buzzer = _device_read_float(devices[_HAMSTER_BUZZER_INDEX]);
	hamster->output_a = _device_read(devices[_HAMSTER_OUTPUT_A_INDEX]);
	hamster->output_b = _device_read(devices[_HAMSTER_OUTPUT_B_INDEX]);
	if(devices[_HAMSTER_TOPOLOGY_INDEX]->written == 1) {
		hamster->topology = _device_read(devices[_HAMSTER_TOPOLOGY_INDEX]);
	}
	if(devices[_HAMSTER_LEFT_LED_INDEX]->written == 1) {
		hamster->left_led = _device_read(devices[_HAMSTER_LEFT_LED_INDEX]);
	}
	if(devices[_HAMSTER_RIGHT_LED_INDEX]->written == 1) {
		hamster->right_led = _device_read(devices[_HAMSTER_RIGHT_LED_INDEX]);
	}
	if(devices[_HAMSTER_NOTE_INDEX]->written == 1) {
		hamster->note = _device_read(devices[_HAMSTER_NOTE_INDEX]);
	}
	if(devices[_HAMSTER_LINE_TRACER_MODE_INDEX]->written == 1) {
		hamster->line_tracer_mode = _device_read(devices[_HAMSTER_LINE_TRACER_MODE_INDEX]);
		hamster->line_tracer_mode_written = 1;
	}
	if(devices[_HAMSTER_LINE_TRACER_SPEED_INDEX]->written == 1) {
		hamster->line_tracer_speed = _device_read(devices[_HAMSTER_LINE_TRACER_SPEED_INDEX]);
	}
	if(devices[_HAMSTER_IO_MODE_A_INDEX]->written == 1) {
		hamster->io_mode_a = _device_read(devices[_HAMSTER_IO_MODE_A_INDEX]);
	}
	if(devices[_HAMSTER_IO_MODE_B_INDEX]->written == 1) {
		hamster->io_mode_b = _device_read(devices[_HAMSTER_IO_MODE_B_INDEX]);
	}
	if(devices[_HAMSTER_CONFIG_PROXIMITY_INDEX]->written == 1) {
		hamster->config_proximity = _device_read(devices[_HAMSTER_CONFIG_PROXIMITY_INDEX]);
	}
	if(devices[_HAMSTER_CONFIG_GRAVITY_INDEX]->written == 1) {
		hamster->config_gravity = _device_read(devices[_HAMSTER_CONFIG_GRAVITY_INDEX]);
	}
	if(devices[_HAMSTER_CONFIG_BAND_WIDTH_INDEX]->written == 1) {
		hamster->config_band_width = _device_read(devices[_HAMSTER_CONFIG_BAND_WIDTH_INDEX]);
	}
	_robot_clear_written(robot);
}

void _hamster_encode_motoring_packet(struct _robot* robot) {
	struct _hamster_robot* hamster = (struct _hamster_robot*)robot;
	struct _device** devices = robot->devices;
	char* buffer = robot->write_buffer;
	int index = 0, temp, i;
	const char* address;

	index = _value_to_hex(buffer, index, hamster->topology & 0x0f, 1);
	buffer[index++] = '0';
	buffer[index++] = '0';
	buffer[index++] = '1';
	buffer[index++] = '0';
	index = _value_to_hex(buffer, index, hamster->left_wheel, 1);
	index = _value_to_hex(buffer, index, hamster->right_wheel, 1);
	index = _value_to_hex(buffer, index, hamster->left_led, 1);
	index = _value_to_hex(buffer, index, hamster->right_led, 1);
	index = _value_to_hex(buffer, index, (int)(hamster->buzzer * 100), 3);
	index = _value_to_hex(buffer, index, hamster->note, 1);
	if(hamster->line_tracer_mode_written == 1) {
		if(hamster->line_tracer_mode > 0) {
			hamster->line_tracer_flag ^= 0x80;
			hamster->line_tracer_event = 1;
		}
		hamster->line_tracer_mode_written = 0;
	}
	temp = (hamster->line_tracer_mode & 0x0f) << 3;
	temp |= (hamster->line_tracer_speed - 1) & 0x07;
	temp |= hamster->line_tracer_flag;
	index = _value_to_hex(buffer, index, temp, 1);
	index = _value_to_hex(buffer, index, hamster->config_proximity, 1);
	temp = (hamster->config_gravity & 0x0f) << 4;
	temp |= hamster->config_band_width & 0x0f;
	index = _value_to_hex(buffer, index, temp, 1);
	temp = (hamster->io_mode_a & 0x0f) << 4;
	temp |= hamster->io_mode_b & 0x0f;
	index = _value_to_hex(buffer, index, temp, 1);
	index = _value_to_hex(buffer, index, hamster->output_a, 1);
	index = _value_to_hex(buffer, index, hamster->output_b, 1);
	buffer[index++] = '0';
	buffer[index++] = '0';
	buffer[index++] = '0';
	buffer[index++] = '0';
	buffer[index++] = '0';
	buffer[index++] = '0';
	buffer[index++] = '-';
	address = _connector_get_address(robot->connector);
	for(i = 0; i < 12; ++i) {
		buffer[index++] = address[i];
	}
	buffer[index++] = '\r';
	
	_connector_write(robot->connector, buffer, _MOTORING_PACKET_LENGTH);
}

int _hamster_decode_sensory_packet(struct _robot* robot, char* packet) {
	struct _hamster_robot* hamster = (struct _hamster_robot*)robot;
	struct _device** devices = robot->devices;
	char* buffer = packet;
	int value;
	
	value = _hex_to_value(buffer, 6, 8);
	value -= 0x100;
	_device_put(devices[_HAMSTER_SIGNAL_STRENGTH_INDEX], value);
	_device_put(devices[_HAMSTER_LEFT_PROXIMITY_INDEX], _hex_to_value(buffer, 8, 10)); // left proximity
	_device_put(devices[_HAMSTER_RIGHT_PROXIMITY_INDEX], _hex_to_value(buffer, 10, 12)); // right proximity
	_device_put(devices[_HAMSTER_LEFT_FLOOR_INDEX], _hex_to_value(buffer, 12, 14)); // left floor
	_device_put(devices[_HAMSTER_RIGHT_FLOOR_INDEX], _hex_to_value(buffer, 14, 16)); // right floor
	value = _hex_to_value(buffer, 16, 20);
	if(value > 0x7FFF) value -= 0x10000;
	_device_put_at(devices[_HAMSTER_ACCELERATION_INDEX], 0, value);
	value = _hex_to_value(buffer, 20, 24);
	if(value > 0x7FFF) value -= 0x10000;
	_device_put_at(devices[_HAMSTER_ACCELERATION_INDEX], 1, value);
	value = _hex_to_value(buffer, 24, 28);
	if(value > 0x7FFF) value -= 0x10000;
	_device_put_at(devices[_HAMSTER_ACCELERATION_INDEX], 2, value);
	value = _hex_to_value(buffer, 28, 30);
	if(value == 0) {
		hamster->light = _hex_to_value(buffer, 30, 34);
	} else {
		value = _hex_to_value(buffer, 30, 32);
		if(value > 0x7F) value -= 0x100;
		hamster->temperature = (int)(value / 2.0f + 24);
	}
	_device_put(devices[_HAMSTER_LIGHT_INDEX], hamster->light);
	_device_put(devices[_HAMSTER_TEMPERATURE_INDEX], hamster->temperature);
	_device_put(devices[_HAMSTER_INPUT_A_INDEX], _hex_to_value(buffer, 34, 36));
	_device_put(devices[_HAMSTER_INPUT_B_INDEX], _hex_to_value(buffer, 36, 38));
	value = _hex_to_value(buffer, 38, 40);
	if((value & 0x40) != 0) {
		if(hamster->line_tracer_event == 1) {
			if(value != 0x40) {
				hamster->line_tracer_event = 2;
			}
		}
		if(hamster->line_tracer_event == 2) {
			if(value != hamster->line_tracer_state) {
				hamster->line_tracer_state = value;
				_device_put(devices[_HAMSTER_LINE_TRACER_STATE_INDEX], value);
				if(value == 0x40) {
					hamster->line_tracer_event = 0;
				}
			}
		}
	}
	return 1;
}

int _hamster_receive(struct _robot* robot) {
	struct _connector* connector = robot->connector;
	
	if(connector != NULL) {
		int read_bytes = _connector_read(connector);
		if(read_bytes != 0) {
			if(_hamster_decode_sensory_packet(robot, connector->buffer) == 1) {
				if(robot->ready == 0) {
					robot->ready = 1;
					_runner_register_checked();
				}
			}
			return 1;
		}
	}
	return 0;
}

void _hamster_send(struct _robot* robot) {
	struct _connector* connector = robot->connector;
	
	if(connector != NULL) {
		_hamster_encode_motoring_packet(robot);
	}
}

void _hamster_update_sensory_device_state(struct _robot* robot) {
	struct _device** devices = robot->devices;
	int i;

	for(i = _HAMSTER_SIGNAL_STRENGTH_INDEX; i <= _HAMSTER_LINE_TRACER_STATE_INDEX; ++i) {
		_device_update_device_state(devices[i]);
	}
}

void _hamster_update_motoring_device_state(struct _robot* robot) {
	struct _device** devices = robot->devices;
	int i;

	for(i = _HAMSTER_LEFT_WHEEL_INDEX; i <= _HAMSTER_CONFIG_BAND_WIDTH_INDEX; ++i) {
		_device_update_device_state(devices[i]);
	}
}

void _hamster_reset(struct _robot* robot) {
	struct _hamster_robot* hamster = (struct _hamster_robot*)robot;
	
	if(hamster == NULL) return;
	_robot_reset(robot);
	
	hamster->left_wheel = 0;
	hamster->right_wheel = 0;
	hamster->buzzer = 0;
	hamster->output_a = 0;
	hamster->output_b = 0;
	hamster->topology = 0;
	hamster->left_led = 0;
	hamster->right_led = 0;
	hamster->note = 0;
	hamster->line_tracer_mode = 0;
	hamster->line_tracer_speed = 5;
	hamster->io_mode_a = 0;
	hamster->io_mode_b = 0;
	hamster->config_proximity = 2;
	hamster->config_gravity = 0;
	hamster->config_band_width = 3;
	
	hamster->light = 0;
	hamster->temperature = 0;
	hamster->line_tracer_mode_written = 0;
	hamster->line_tracer_flag = 0;
	hamster->line_tracer_state = 0;
	hamster->line_tracer_event = 0;
}

void _hamster_dispose(struct _robot* robot) {
	_robot_group_remove_robot(_GROUP_HAMSTER, robot);
	_runner_remove_robot(robot);

	robot->running = 0;
	if(robot->thread_alive == 1) {
		WaitForSingleObject(robot->thread_handle, 1000);
	}
	Sleep(100);
	_robot_dispose(robot);
	free((struct _hamster_robot*)robot);
}

unsigned WINAPI _hamster_thread_proc(void* arg) {
	struct _robot* robot = (struct _robot*)arg;
	int shutdown = 0, shutdown_count = 0;
	
	if(robot == NULL) return 0;

	robot->thread_alive = 1;
	while(1) {
		if(_hamster_receive(robot) == 1) {
			_hamster_send(robot);
			if(shutdown == 1) {
				break;
			}
		}
		if(shutdown == 1) {
			if(++shutdown_count > 5) {
				break;
			}
		}
		if(robot->running == 0) {
			shutdown = 1;
		}
		Sleep(5);
	}
	robot->thread_alive = 0;
	return 0;
}

struct _hamster_robot* _hamster_create(const char* port_name) {
	struct _hamster_robot* hamster = (struct _hamster_robot*)malloc(sizeof(struct _hamster_robot));
	struct _robot* robot = (struct _robot*)hamster;
	struct _device** devices;
	struct _connector* connector;
	unsigned int thread_id;
	
	_robot_init(robot, _robot_group_count_robots(_GROUP_HAMSTER), "Hamster", 55);
	
	hamster->left_wheel = 0;
	hamster->right_wheel = 0;
	hamster->buzzer = 0;
	hamster->output_a = 0;
	hamster->output_b = 0;
	hamster->topology = 0;
	hamster->left_led = 0;
	hamster->right_led = 0;
	hamster->note = 0;
	hamster->line_tracer_mode = 0;
	hamster->line_tracer_speed = 5;
	hamster->io_mode_a = 0;
	hamster->io_mode_b = 0;
	hamster->config_proximity = 2;
	hamster->config_gravity = 0;
	hamster->config_band_width = 3;
	
	hamster->light = 0;
	hamster->temperature = 0;
	hamster->bpm = 60;
	hamster->line_tracer_mode_written = 0;
	hamster->line_tracer_flag = 0;
	hamster->line_tracer_state = 0;
	hamster->line_tracer_event = 0;
	hamster->board_count = 0;
	hamster->board_state = 0;
	
	robot->request_motoring_data = _hamster_request_motoring_data;
	robot->update_sensory_device_state = _hamster_update_sensory_device_state;
	robot->update_motoring_device_state = _hamster_update_motoring_device_state;
	robot->reset = _hamster_reset;
	robot->dispose = _hamster_dispose;
	
	robot->devices_size = 27;
	devices = (struct _device**)malloc(sizeof(struct _device*) * 27);
	devices[0] = _device_create(HAMSTER_LEFT_WHEEL, "LeftWheel", DEVICE_TYPE_EFFECTOR, DATA_TYPE_INTEGER, 1, -100, 100, 0);
	devices[1] = _device_create(HAMSTER_RIGHT_WHEEL, "RightWheel", DEVICE_TYPE_EFFECTOR, DATA_TYPE_INTEGER, 1, -100, 100, 0);
	devices[2] = _device_create(HAMSTER_BUZZER, "Buzzer", DEVICE_TYPE_EFFECTOR, DATA_TYPE_FLOAT, 1, 0, 167772.15f, 0);
	devices[3] = _device_create(HAMSTER_OUTPUT_A, "OutputA", DEVICE_TYPE_EFFECTOR, DATA_TYPE_INTEGER, 1, 0, 255, 0);
	devices[4] = _device_create(HAMSTER_OUTPUT_B, "OutputB", DEVICE_TYPE_EFFECTOR, DATA_TYPE_INTEGER, 1, 0, 255, 0);
	devices[5] = _device_create(HAMSTER_TOPOLOGY, "Topology", DEVICE_TYPE_COMMAND, DATA_TYPE_INTEGER, 1, 0, 15, 0);
	devices[6] = _device_create(HAMSTER_LEFT_LED, "LeftLed", DEVICE_TYPE_COMMAND, DATA_TYPE_INTEGER, 1, 0, 7, 0);
	devices[7] = _device_create(HAMSTER_RIGHT_LED, "RightLed", DEVICE_TYPE_COMMAND, DATA_TYPE_INTEGER, 1, 0, 7, 0);
	devices[8] = _device_create(HAMSTER_NOTE, "Note", DEVICE_TYPE_COMMAND, DATA_TYPE_INTEGER, 1, 0, 88, 0);
	devices[9] = _device_create(HAMSTER_LINE_TRACER_MODE, "LineTracerMode", DEVICE_TYPE_COMMAND, DATA_TYPE_INTEGER, 1, 0, 15, 0);
	devices[10] = _device_create(HAMSTER_LINE_TRACER_SPEED, "LineTracerSpeed", DEVICE_TYPE_COMMAND, DATA_TYPE_INTEGER, 1, 1, 8, 5);
	devices[11] = _device_create(HAMSTER_IO_MODE_A, "IoModeA", DEVICE_TYPE_COMMAND, DATA_TYPE_INTEGER, 1, 0, 15, 0);
	devices[12] = _device_create(HAMSTER_IO_MODE_B, "IoModeB", DEVICE_TYPE_COMMAND, DATA_TYPE_INTEGER, 1, 0, 15, 0);
	devices[13] = _device_create(HAMSTER_CONFIG_PROXIMITY, "ConfigProximity", DEVICE_TYPE_COMMAND, DATA_TYPE_INTEGER, 1, 1, 7, 2);
	devices[14] = _device_create(HAMSTER_CONFIG_GRAVITY, "ConfigGravity", DEVICE_TYPE_COMMAND, DATA_TYPE_INTEGER, 1, 0, 3, 0);
	devices[15] = _device_create(HAMSTER_CONFIG_BAND_WIDTH, "ConfigBandWidth", DEVICE_TYPE_COMMAND, DATA_TYPE_INTEGER, 1, 1, 8, 3);
	devices[16] = _device_create(HAMSTER_SIGNAL_STRENGTH, "SignalStrength", DEVICE_TYPE_SENSOR, DATA_TYPE_INTEGER, 1, -128, 0, 0);
	devices[17] = _device_create(HAMSTER_LEFT_PROXIMITY, "LeftProximity", DEVICE_TYPE_SENSOR, DATA_TYPE_INTEGER, 1, 0, 255, 0);
	devices[18] = _device_create(HAMSTER_RIGHT_PROXIMITY, "RightProximity", DEVICE_TYPE_SENSOR, DATA_TYPE_INTEGER, 1, 0, 255, 0);
	devices[19] = _device_create(HAMSTER_LEFT_FLOOR, "LeftFloor", DEVICE_TYPE_SENSOR, DATA_TYPE_INTEGER, 1, 0, 255, 0);
	devices[20] = _device_create(HAMSTER_RIGHT_FLOOR, "RightFloor", DEVICE_TYPE_SENSOR, DATA_TYPE_INTEGER, 1, 0, 255, 0);
	devices[21] = _device_create(HAMSTER_ACCELERATION, "Acceleration", DEVICE_TYPE_SENSOR, DATA_TYPE_INTEGER, 3, -32768, 32767, 0);
	devices[22] = _device_create(HAMSTER_LIGHT, "Light", DEVICE_TYPE_SENSOR, DATA_TYPE_INTEGER, 1, 0, 65535, 0);
	devices[23] = _device_create(HAMSTER_TEMPERATURE, "Temperature", DEVICE_TYPE_SENSOR, DATA_TYPE_INTEGER, 1, -40, 88, 0);
	devices[24] = _device_create(HAMSTER_INPUT_A, "inputA", DEVICE_TYPE_SENSOR, DATA_TYPE_INTEGER, 1, 0, 255, 0);
	devices[25] = _device_create(HAMSTER_INPUT_B, "inputB", DEVICE_TYPE_SENSOR, DATA_TYPE_INTEGER, 1, 0, 255, 0);
	devices[26] = _device_create(HAMSTER_LINE_TRACER_STATE, "LineTracerState", DEVICE_TYPE_EVENT, DATA_TYPE_INTEGER, 1, 0, 255, 0);
	robot->devices = devices;
	robot->connector = NULL;
	robot->alive = 1;

	_robot_group_add_robot(_GROUP_HAMSTER, robot);
	_runner_add_robot(robot);
	_runner_start();
	
	_runner_register_required();
	robot->running = 1;
	robot->thread_handle = (HANDLE)_beginthreadex(NULL,
		0,
		_hamster_thread_proc,
		robot,
		CREATE_SUSPENDED,
		&thread_id);
	if(robot->thread_handle != 0) {
		ResumeThread(robot->thread_handle);
	}

	connector = _connector_create("Hamster", robot->index, _VALID_PACKET_LENGTH, _CR);
	robot->connector = connector;
	if(connector != NULL) {
		int result;
		connector->check_connection = _hamster_check_connection;
		result = _connector_open(connector, port_name, _SERIAL_BAUDRATE_115200, _SERIAL_FLOWCONTROL_RTSCTS_IN | _SERIAL_FLOWCONTROL_RTSCTS_OUT);
		if(result == _CONNECTION_RESULT_FOUND) {
			while(robot->ready == 0 && robot->running == 1) {
				Sleep(10);
			}
		} else if(result == _CONNECTION_RESULT_NOT_AVAILABLE) {
			_runner_register_checked();
		}
	}
	return hamster;
}

const char* _hamster_get_id(void) {
	return HAMSTER_ID;
}

void _hamster_wheels(int hamster_index, double left_speed, double right_speed) {
	struct _robot* robot = _robot_group_get_robot(_GROUP_HAMSTER, hamster_index);
	
	if(robot == NULL) return;
	_robot_write(robot, HAMSTER_LINE_TRACER_MODE, HAMSTER_LINE_TRACER_MODE_OFF);
	_robot_write(robot, HAMSTER_LEFT_WHEEL, (int)left_speed);
	_robot_write(robot, HAMSTER_RIGHT_WHEEL, (int)right_speed);
}

void _hamster_left_wheel(int hamster_index, double speed) {
	struct _robot* robot = _robot_group_get_robot(_GROUP_HAMSTER, hamster_index);
	
	if(robot == NULL) return;
	_robot_write(robot, HAMSTER_LINE_TRACER_MODE, HAMSTER_LINE_TRACER_MODE_OFF);
	_robot_write(robot, HAMSTER_LEFT_WHEEL, (int)speed);
}

void _hamster_right_wheel(int hamster_index, double speed) {
	struct _robot* robot = _robot_group_get_robot(_GROUP_HAMSTER, hamster_index);
	
	if(robot == NULL) return;
	_robot_write(robot, HAMSTER_LINE_TRACER_MODE, HAMSTER_LINE_TRACER_MODE_OFF);
	_robot_write(robot, HAMSTER_RIGHT_WHEEL, (int)speed);
}

void _hamster_stop(int hamster_index) {
	struct _robot* robot = _robot_group_get_robot(_GROUP_HAMSTER, hamster_index);
	
	if(robot == NULL) return;
	_robot_write(robot, HAMSTER_LINE_TRACER_MODE, HAMSTER_LINE_TRACER_MODE_OFF);
	_robot_write(robot, HAMSTER_LEFT_WHEEL, 0);
	_robot_write(robot, HAMSTER_RIGHT_WHEEL, 0);
}

int _hamster_line_tracer_mode_callback(int hamster_index) {
	struct _robot* robot = _robot_group_get_robot(_GROUP_HAMSTER, hamster_index);
	
	if(robot == NULL) return 1;
	return _robot_e(robot, HAMSTER_LINE_TRACER_STATE) && _robot_read(robot, HAMSTER_LINE_TRACER_STATE) == 0x40;
}

void _hamster_line_tracer_mode(int hamster_index, int mode, int (*callback)(void* arg)) {
	struct _robot* robot = _robot_group_get_robot(_GROUP_HAMSTER, hamster_index);
	
	if(robot == NULL) return;
	_robot_write(robot, HAMSTER_LEFT_WHEEL, 0);
	_robot_write(robot, HAMSTER_RIGHT_WHEEL, 0);
	_robot_write(robot, HAMSTER_LINE_TRACER_MODE, mode);
	
	switch(mode) {
		case HAMSTER_LINE_TRACER_MODE_BLACK_MOVE_FORWARD:
		case HAMSTER_LINE_TRACER_MODE_BLACK_TURN_LEFT:
		case HAMSTER_LINE_TRACER_MODE_BLACK_TURN_RIGHT:
		case HAMSTER_LINE_TRACER_MODE_BLACK_UTURN:
		case HAMSTER_LINE_TRACER_MODE_WHITE_MOVE_FORWARD:
		case HAMSTER_LINE_TRACER_MODE_WHITE_TURN_LEFT:
		case HAMSTER_LINE_TRACER_MODE_WHITE_TURN_RIGHT:
		case HAMSTER_LINE_TRACER_MODE_WHITE_UTURN: {
			wait_until(callback, NULL);
			break;
		}
	}
}

void _hamster_line_tracer_speed(int hamster_index, double speed) {
	struct _robot* robot = _robot_group_get_robot(_GROUP_HAMSTER, hamster_index);
	
	if(robot == NULL) return;
	_robot_write(robot, HAMSTER_LINE_TRACER_SPEED, (int)speed);
}

int _hamster_board_forward_callback(int hamster_index) {
	struct _robot* robot = _robot_group_get_robot(_GROUP_HAMSTER, hamster_index);
	struct _hamster_robot* hamster = (struct _hamster_robot*)robot;
	
	if(hamster == NULL) return 1;
	switch(hamster->board_state) {
		case 1: {
			if(hamster->board_count < 2) {
				int left_floor = _robot_read(robot, HAMSTER_LEFT_FLOOR);
				int right_floor = _robot_read(robot, HAMSTER_RIGHT_FLOOR);
				int diff = left_floor - right_floor;
				if(left_floor < 50 && right_floor < 50) {
					hamster->board_count ++;
				} else {
					hamster->board_count = 0;
				}
				_robot_write(robot, HAMSTER_LEFT_WHEEL, (int)(45 + diff * 0.25f));
				_robot_write(robot, HAMSTER_RIGHT_WHEEL, (int)(45 - diff * 0.25f));
			} else {
				hamster->board_count = 0;
				hamster->board_state = 2;
			}
			break;
		}
		case 2: {
			if(hamster->board_count < 10) {
				int diff = _robot_read(robot, HAMSTER_LEFT_FLOOR) - _robot_read(robot, HAMSTER_RIGHT_FLOOR);
				hamster->board_count ++;
				_robot_write(robot, HAMSTER_LEFT_WHEEL, (int)(45 + diff * 0.25f));
				_robot_write(robot, HAMSTER_RIGHT_WHEEL, (int)(45 - diff * 0.25f));
			} else {
				_robot_write(robot, HAMSTER_LEFT_WHEEL, 0);
				_robot_write(robot, HAMSTER_RIGHT_WHEEL, 0);
				return 1;
			}
			break;
		}
	}
	return 0;
}

void _hamster_board_forward(int hamster_index, int (*callback)(void* arg)) {
	struct _robot* robot = _robot_group_get_robot(_GROUP_HAMSTER, hamster_index);
	struct _hamster_robot* hamster = (struct _hamster_robot*)robot;
	
	if(hamster == NULL) return;
	hamster->board_count = 0;
	hamster->board_state = 1;
	_robot_write(robot, HAMSTER_LINE_TRACER_MODE, HAMSTER_LINE_TRACER_MODE_OFF);
	_robot_write(robot, HAMSTER_LEFT_WHEEL, 45);
	_robot_write(robot, HAMSTER_RIGHT_WHEEL, 45);
	wait_until(callback, NULL);
}

int _hamster_board_left_callback(int hamster_index) {
	struct _robot* robot = _robot_group_get_robot(_GROUP_HAMSTER, hamster_index);
	struct _hamster_robot* hamster = (struct _hamster_robot*)robot;
	
	if(hamster == NULL) return 1;
	switch(hamster->board_state) {
		case 1: {
			if(hamster->board_count < 2) {
				if(_robot_read(robot, HAMSTER_LEFT_FLOOR) > 50) {
					hamster->board_count ++;
				}
			} else {
				hamster->board_count = 0;
				hamster->board_state = 2;
			}
			break;
		}
		case 2: {
			if(_robot_read(robot, HAMSTER_LEFT_FLOOR) < 20) {
				hamster->board_state = 3;
			}
			break;
		}
		case 3: {
			if(hamster->board_count < 2) {
				if(_robot_read(robot, HAMSTER_LEFT_FLOOR) < 20) {
					hamster->board_count ++;
				}
			} else {
				hamster->board_count = 0;
				hamster->board_state = 4;
			}
			break;
		}
		case 4: {
			if(_robot_read(robot, HAMSTER_LEFT_FLOOR) > 50) {
				hamster->board_state = 5;
			}
			break;
		}
		case 5: {
			int diff = _robot_read(robot, HAMSTER_LEFT_FLOOR) - _robot_read(robot, HAMSTER_RIGHT_FLOOR);
			if(diff > -15) {
				_robot_write(robot, HAMSTER_LEFT_WHEEL, 0);
				_robot_write(robot, HAMSTER_RIGHT_WHEEL, 0);
				return 1;
			} else {
				_robot_write(robot, HAMSTER_LEFT_WHEEL, (int)(diff * 0.5f));
				_robot_write(robot, HAMSTER_RIGHT_WHEEL, -(int)(diff * 0.5f));
			}
			break;
		}
	}
	return 0;
}

void _hamster_board_left(int hamster_index, int (*callback)(void* arg)) {
	struct _robot* robot = _robot_group_get_robot(_GROUP_HAMSTER, hamster_index);
	struct _hamster_robot* hamster = (struct _hamster_robot*)robot;
	
	if(hamster == NULL) return;
	hamster->board_count = 0;
	hamster->board_state = 1;
	_robot_write(robot, HAMSTER_LINE_TRACER_MODE, HAMSTER_LINE_TRACER_MODE_OFF);
	_robot_write(robot, HAMSTER_LEFT_WHEEL, -45);
	_robot_write(robot, HAMSTER_RIGHT_WHEEL, 45);
	wait_until(callback, NULL);
}

int _hamster_board_right_callback(int hamster_index) {
	struct _robot* robot = _robot_group_get_robot(_GROUP_HAMSTER, hamster_index);
	struct _hamster_robot* hamster = (struct _hamster_robot*)robot;
	
	if(hamster == NULL) return 1;
	switch(hamster->board_state) {
		case 1: {
			if(hamster->board_count < 2) {
				if(_robot_read(robot, HAMSTER_RIGHT_FLOOR) > 50) {
					hamster->board_count ++;
				}
			} else {
				hamster->board_count = 0;
				hamster->board_state = 2;
			}
			break;
		}
		case 2: {
			if(_robot_read(robot, HAMSTER_RIGHT_FLOOR) < 20) {
				hamster->board_state = 3;
			}
			break;
		}
		case 3: {
			if(hamster->board_count < 2) {
				if(_robot_read(robot, HAMSTER_RIGHT_FLOOR) < 20) {
					hamster->board_count ++;
				}
			} else {
				hamster->board_count = 0;
				hamster->board_state = 4;
			}
			break;
		}
		case 4: {
			if(_robot_read(robot, HAMSTER_RIGHT_FLOOR) > 50) {
				hamster->board_state = 5;
			}
			break;
		}
		case 5: {
			int diff = _robot_read(robot, HAMSTER_RIGHT_FLOOR) - _robot_read(robot, HAMSTER_LEFT_FLOOR);
			if(diff > -15) {
				_robot_write(robot, HAMSTER_LEFT_WHEEL, 0);
				_robot_write(robot, HAMSTER_RIGHT_WHEEL, 0);
				return 1;
			} else {
				_robot_write(robot, HAMSTER_LEFT_WHEEL, -(int)(diff * 0.5f));
				_robot_write(robot, HAMSTER_RIGHT_WHEEL, (int)(diff * 0.5f));
			}
			break;
		}
	}
	return 0;
}

void _hamster_board_right(int hamster_index, int (*callback)(void* arg)) {
	struct _robot* robot = _robot_group_get_robot(_GROUP_HAMSTER, hamster_index);
	struct _hamster_robot* hamster = (struct _hamster_robot*)robot;
	
	if(hamster == NULL) return;
	hamster->board_count = 0;
	hamster->board_state = 1;
	_robot_write(robot, HAMSTER_LINE_TRACER_MODE, HAMSTER_LINE_TRACER_MODE_OFF);
	_robot_write(robot, HAMSTER_LEFT_WHEEL, 45);
	_robot_write(robot, HAMSTER_RIGHT_WHEEL, -45);
	wait_until(callback, NULL);
}

void _hamster_leds(int hamster_index, int left_color, int right_color) {
	struct _robot* robot = _robot_group_get_robot(_GROUP_HAMSTER, hamster_index);
	
	if(robot == NULL) return;
	_robot_write(robot, HAMSTER_LEFT_LED, left_color);
	_robot_write(robot, HAMSTER_RIGHT_LED, right_color);
}

void _hamster_left_led(int hamster_index, int color) {
	struct _robot* robot = _robot_group_get_robot(_GROUP_HAMSTER, hamster_index);
	
	if(robot == NULL) return;
	_robot_write(robot, HAMSTER_LEFT_LED, color);
}

void _hamster_right_led(int hamster_index, int color) {
	struct _robot* robot = _robot_group_get_robot(_GROUP_HAMSTER, hamster_index);
	
	if(robot == NULL) return;
	_robot_write(robot, HAMSTER_RIGHT_LED, color);
}

void _hamster_beep(int hamster_index) {
	struct _robot* robot = _robot_group_get_robot(_GROUP_HAMSTER, hamster_index);
	
	if(robot == NULL) return;
	_robot_write(robot, HAMSTER_NOTE, HAMSTER_NOTE_OFF);
	_robot_write(robot, HAMSTER_BUZZER, 440);
	wait(200);
	_robot_write(robot, HAMSTER_BUZZER, 0);
}

void _hamster_buzzer(int hamster_index, double hz) {
	struct _robot* robot = _robot_group_get_robot(_GROUP_HAMSTER, hamster_index);
	
	if(robot == NULL) return;
	_robot_write(robot, HAMSTER_NOTE, HAMSTER_NOTE_OFF);
	_robot_write_float(robot, HAMSTER_BUZZER, (float)hz);
}

void _hamster_tempo(int hamster_index, double bpm) {
	struct _hamster_robot* hamster = (struct _hamster_robot*)_robot_group_get_robot(_GROUP_HAMSTER, hamster_index);
	
	if(hamster == NULL) return;
	if(bpm > 0) {
		hamster->bpm = bpm;
	}
}

void _hamster_pitch(int hamster_index, double pitch) {
	struct _robot* robot = _robot_group_get_robot(_GROUP_HAMSTER, hamster_index);
	struct _hamster_robot* hamster = (struct _hamster_robot*)robot;

	if(hamster == NULL) return;
	if(pitch < 0) return;
	_robot_write(robot, HAMSTER_BUZZER, 0);
	_robot_write(robot, HAMSTER_NOTE, (int)pitch);
}

void _hamster_note(int hamster_index, double pitch, double beats) {
	struct _robot* robot = _robot_group_get_robot(_GROUP_HAMSTER, hamster_index);
	struct _hamster_robot* hamster = (struct _hamster_robot*)robot;
	
	if(hamster == NULL) return;
	if(beats > 0) {
		double bpm = hamster->bpm;
		if(bpm > 0) {
			int _pitch = (int)pitch;
			if(_pitch == 0) {
				_robot_write(robot, HAMSTER_BUZZER, 0);
				_robot_write(robot, HAMSTER_NOTE, HAMSTER_NOTE_OFF);
				wait((int)(beats * 60 * 1000 / bpm));
			} else if(_pitch > 0) {
				int timeout = (int)(beats * 60 * 1000 / bpm);
				int tail = 0;
				if(timeout > 100) {
					tail = 100;
				}
				_robot_write(robot, HAMSTER_BUZZER, 0);
				_robot_write(robot, HAMSTER_NOTE, _pitch);
				wait(timeout - tail);
				_robot_write(robot, HAMSTER_NOTE, HAMSTER_NOTE_OFF);
				if(tail > 0) {
					wait(tail);
				}
			}
		}
	}
}

void _hamster_io_mode_a(int hamster_index, int mode) {
	struct _robot* robot = _robot_group_get_robot(_GROUP_HAMSTER, hamster_index);
	
	if(robot == NULL) return;
	_robot_write(robot, HAMSTER_IO_MODE_A, mode);
}

void _hamster_io_mode_b(int hamster_index, int mode) {
	struct _robot* robot = _robot_group_get_robot(_GROUP_HAMSTER, hamster_index);
	
	if(robot == NULL) return;
	_robot_write(robot, HAMSTER_IO_MODE_B, mode);
}

void _hamster_output_a(int hamster_index, double value) {
	struct _robot* robot = _robot_group_get_robot(_GROUP_HAMSTER, hamster_index);
	
	if(robot == NULL) return;
	_robot_write(robot, HAMSTER_OUTPUT_A, (int)value);
}

void _hamster_output_b(int hamster_index, double value) {
	struct _robot* robot = _robot_group_get_robot(_GROUP_HAMSTER, hamster_index);
	
	if(robot == NULL) return;
	_robot_write(robot, HAMSTER_OUTPUT_B, (int)value);
}

int _hamster_signal_strength(int hamster_index) {
	struct _robot* robot = _robot_group_get_robot(_GROUP_HAMSTER, hamster_index);
	
	if(robot == NULL) return 0;
	return _robot_read(robot, HAMSTER_SIGNAL_STRENGTH);
}

int _hamster_left_proximity(int hamster_index) {
	struct _robot* robot = _robot_group_get_robot(_GROUP_HAMSTER, hamster_index);
	
	if(robot == NULL) return 0;
	return _robot_read(robot, HAMSTER_LEFT_PROXIMITY);
}

int _hamster_right_proximity(int hamster_index) {
	struct _robot* robot = _robot_group_get_robot(_GROUP_HAMSTER, hamster_index);
	
	if(robot == NULL) return 0;
	return _robot_read(robot, HAMSTER_RIGHT_PROXIMITY);
}

int _hamster_left_floor(int hamster_index) {
	struct _robot* robot = _robot_group_get_robot(_GROUP_HAMSTER, hamster_index);
	
	if(robot == NULL) return 0;
	return _robot_read(robot, HAMSTER_LEFT_FLOOR);
}

int _hamster_right_floor(int hamster_index) {
	struct _robot* robot = _robot_group_get_robot(_GROUP_HAMSTER, hamster_index);
	
	if(robot == NULL) return 0;
	return _robot_read(robot, HAMSTER_RIGHT_FLOOR);
}

int _hamster_acceleration_x(int hamster_index) {
	struct _robot* robot = _robot_group_get_robot(_GROUP_HAMSTER, hamster_index);
	
	if(robot == NULL) return 0;
	return _robot_read_at(robot, HAMSTER_ACCELERATION, 0);
}

int _hamster_acceleration_y(int hamster_index) {
	struct _robot* robot = _robot_group_get_robot(_GROUP_HAMSTER, hamster_index);
	
	if(robot == NULL) return 0;
	return _robot_read_at(robot, HAMSTER_ACCELERATION, 1);
}

int _hamster_acceleration_z(int hamster_index) {
	struct _robot* robot = _robot_group_get_robot(_GROUP_HAMSTER, hamster_index);
	
	if(robot == NULL) return 0;
	return _robot_read_at(robot, HAMSTER_ACCELERATION, 2);
}

int _hamster_light(int hamster_index) {
	struct _robot* robot = _robot_group_get_robot(_GROUP_HAMSTER, hamster_index);
	
	if(robot == NULL) return 0;
	return _robot_read(robot, HAMSTER_LIGHT);
}

int _hamster_temperature(int hamster_index) {
	struct _robot* robot = _robot_group_get_robot(_GROUP_HAMSTER, hamster_index);
	
	if(robot == NULL) return 0;
	return _robot_read(robot, HAMSTER_TEMPERATURE);
}

int _hamster_input_a(int hamster_index) {
	struct _robot* robot = _robot_group_get_robot(_GROUP_HAMSTER, hamster_index);
	
	if(robot == NULL) return 0;
	return _robot_read(robot, HAMSTER_INPUT_A);
}

int _hamster_input_b(int hamster_index) {
	struct _robot* robot = _robot_group_get_robot(_GROUP_HAMSTER, hamster_index);
	
	if(robot == NULL) return 0;
	return _robot_read(robot, HAMSTER_INPUT_B);
}

#define _MAX_NUM_HAMSTERS 10

#define _FUNCTION_HAMSTER(n) \
	__inline const char* _hamster_get_name_##n(void) { return _robot_group_get_name(_GROUP_HAMSTER, n); } \
	__inline void _hamster_set_name_##n(const char* name) { _robot_group_set_name(_GROUP_HAMSTER, n, name); } \
	__inline const char* _hamster_get_id_##n(void) { return HAMSTER_ID; } \
	__inline int _hamster_get_index_##n(void) { return n; } \
	__inline int _hamster_e_##n(int device_id) { return _robot_group_e(_GROUP_HAMSTER, n, device_id); } \
	__inline int _hamster_read_##n(int device_id) { return _robot_group_read(_GROUP_HAMSTER, n, device_id); } \
	__inline int _hamster_read_at_##n(int device_id, int index) { return _robot_group_read_at(_GROUP_HAMSTER, n, device_id, index); } \
	__inline int _hamster_read_array_##n(int device_id, int* data, int length) { return _robot_group_read_array(_GROUP_HAMSTER, n, device_id, data, length); } \
	__inline float _hamster_read_float_##n(int device_id) { return _robot_group_read_float(_GROUP_HAMSTER, n, device_id); } \
	__inline float _hamster_read_float_at_##n(int device_id, int index) { return _robot_group_read_float_at(_GROUP_HAMSTER, n, device_id, index); } \
	__inline int _hamster_read_float_array_##n(int device_id, float* data, int length) { return _robot_group_read_float_array(_GROUP_HAMSTER, n, device_id, data, length); } \
	__inline int _hamster_write_##n(int device_id, int data) { return _robot_group_write(_GROUP_HAMSTER, n, device_id, data); } \
	__inline int _hamster_write_at_##n(int device_id, int index, int data) { return _robot_group_write_at(_GROUP_HAMSTER, n, device_id, index, data); } \
	__inline int _hamster_write_array_##n(int device_id, const int* data, int length) { return _robot_group_write_array(_GROUP_HAMSTER, n, device_id, data, length); } \
	__inline int _hamster_write_float_##n(int device_id, float data) { return _robot_group_write_float(_GROUP_HAMSTER, n, device_id, data); } \
	__inline int _hamster_write_float_at_##n(int device_id, int index, float data) { return _robot_group_write_float_at(_GROUP_HAMSTER, n, device_id, index, data); } \
	__inline int _hamster_write_float_array_##n(int device_id, const float* data, int length) { return _robot_group_write_float_array(_GROUP_HAMSTER, n, device_id, data, length); } \
	__inline void _hamster_reset_##n(void) { _robot_group_reset(_GROUP_HAMSTER, n); } \
	__inline void _hamster_dispose_##n(void) { _robot_group_dispose(_GROUP_HAMSTER, n); } \
	__inline void _hamster_wheels_##n(double left_speed, double right_speed) { _hamster_wheels(n, left_speed, right_speed); } \
	__inline void _hamster_left_wheel_##n(double speed) { _hamster_left_wheel(n, speed); } \
	__inline void _hamster_right_wheel_##n(double speed) { _hamster_right_wheel(n, speed); } \
	__inline void _hamster_stop_##n(void) { _hamster_stop(n); } \
	__inline int _hamster_line_tracer_mode_callback_##n(void* arg) { return _hamster_line_tracer_mode_callback(n); } \
	__inline void _hamster_line_tracer_mode_##n(int mode) { _hamster_line_tracer_mode(n, mode, _hamster_line_tracer_mode_callback_##n); } \
	__inline void _hamster_line_tracer_speed_##n(double speed) { _hamster_line_tracer_speed(n, speed); } \
	__inline int _hamster_board_forward_callback_##n(void* arg) { return _hamster_board_forward_callback(n); } \
	__inline void _hamster_board_forward_##n(void) { _hamster_board_forward(n, _hamster_board_forward_callback_##n); } \
	__inline int _hamster_board_left_callback_##n(void* arg) { return _hamster_board_left_callback(n); } \
	__inline void _hamster_board_left_##n(void) { _hamster_board_left(n, _hamster_board_left_callback_##n); } \
	__inline int _hamster_board_right_callback_##n(void* arg) { return _hamster_board_right_callback(n); } \
	__inline void _hamster_board_right_##n(void) { _hamster_board_right(n, _hamster_board_right_callback_##n); } \
	__inline void _hamster_leds_##n(int left_color, int right_color) { _hamster_leds(n, left_color, right_color); } \
	__inline void _hamster_left_led_##n(int color) { _hamster_left_led(n, color); } \
	__inline void _hamster_right_led_##n(int color) { _hamster_right_led(n, color); } \
	__inline void _hamster_beep_##n(void) { _hamster_beep(n); } \
	__inline void _hamster_buzzer_##n(double hz) { _hamster_buzzer(n, hz); } \
	__inline void _hamster_tempo_##n(double bpm) { _hamster_tempo(n, bpm); } \
	__inline void _hamster_pitch_##n(double pitch) { _hamster_pitch(n, pitch); } \
	__inline void _hamster_note_##n(double pitch, double beats) { _hamster_note(n, pitch, beats); } \
	__inline void _hamster_io_mode_a_##n(int mode) { _hamster_io_mode_a(n, mode); } \
	__inline void _hamster_io_mode_b_##n(int mode) { _hamster_io_mode_b(n, mode); } \
	__inline void _hamster_output_a_##n(double value) { _hamster_output_a(n, value); } \
	__inline void _hamster_output_b_##n(double value) { _hamster_output_b(n, value); } \
	__inline int _hamster_signal_strength_##n(void) { return _hamster_signal_strength(n); } \
	__inline int _hamster_left_proximity_##n(void) { return _hamster_left_proximity(n); } \
	__inline int _hamster_right_proximity_##n(void) { return _hamster_right_proximity(n); } \
	__inline int _hamster_left_floor_##n(void) { return _hamster_left_floor(n); } \
	__inline int _hamster_right_floor_##n(void) { return _hamster_right_floor(n); } \
	__inline int _hamster_acceleration_x_##n(void) { return _hamster_acceleration_x(n); } \
	__inline int _hamster_acceleration_y_##n(void) { return _hamster_acceleration_y(n); } \
	__inline int _hamster_acceleration_z_##n(void) { return _hamster_acceleration_z(n); } \
	__inline int _hamster_light_##n(void) { return _hamster_light(n); } \
	__inline int _hamster_temperature_##n(void) { return _hamster_temperature(n); } \
	__inline int _hamster_input_a_##n(void) { return _hamster_input_a(n); } \
	__inline int _hamster_input_b_##n(void) { return _hamster_input_b(n); }

#define _FUNCTION_PTR_HAMSTER(name, n) \
	name->get_name = _hamster_get_name_##n; \
	name->set_name = _hamster_set_name_##n; \
	name->get_id = _hamster_get_id_##n; \
	name->get_index = _hamster_get_index_##n; \
	name->e = _hamster_e_##n; \
	name->read = _hamster_read_##n; \
	name->read_at = _hamster_read_at_##n; \
	name->read_array = _hamster_read_array_##n; \
	name->read_float = _hamster_read_float_##n; \
	name->read_float_at = _hamster_read_float_at_##n; \
	name->read_float_array = _hamster_read_float_array_##n; \
	name->write = _hamster_write_##n; \
	name->write_at = _hamster_write_at_##n; \
	name->write_array = _hamster_write_array_##n; \
	name->write_float = _hamster_write_float_##n; \
	name->write_float_at = _hamster_write_float_at_##n; \
	name->write_float_array = _hamster_write_float_array_##n; \
	name->reset = _hamster_reset_##n; \
	name->dispose = _hamster_dispose_##n; \
	name->wheels = _hamster_wheels_##n; \
	name->left_wheel = _hamster_left_wheel_##n; \
	name->right_wheel = _hamster_right_wheel_##n; \
	name->stop = _hamster_stop_##n; \
	name->line_tracer_mode = _hamster_line_tracer_mode_##n; \
	name->line_tracer_speed = _hamster_line_tracer_speed_##n; \
	name->board_forward = _hamster_board_forward_##n; \
	name->board_left = _hamster_board_left_##n; \
	name->board_right = _hamster_board_right_##n; \
	name->leds = _hamster_leds_##n; \
	name->left_led = _hamster_left_led_##n; \
	name->right_led = _hamster_right_led_##n; \
	name->beep = _hamster_beep_##n; \
	name->buzzer = _hamster_buzzer_##n; \
	name->tempo = _hamster_tempo_##n; \
	name->pitch = _hamster_pitch_##n; \
	name->note = _hamster_note_##n; \
	name->io_mode_a = _hamster_io_mode_a_##n; \
	name->io_mode_b = _hamster_io_mode_b_##n; \
	name->output_a = _hamster_output_a_##n; \
	name->output_b = _hamster_output_b_##n; \
	name->signal_strength = _hamster_signal_strength_##n; \
	name->left_proximity = _hamster_left_proximity_##n; \
	name->right_proximity = _hamster_right_proximity_##n; \
	name->left_floor = _hamster_left_floor_##n; \
	name->right_floor = _hamster_right_floor_##n; \
	name->acceleration_x = _hamster_acceleration_x_##n; \
	name->acceleration_y = _hamster_acceleration_y_##n; \
	name->acceleration_z = _hamster_acceleration_z_##n; \
	name->light = _hamster_light_##n; \
	name->temperature = _hamster_temperature_##n; \
	name->input_a = _hamster_input_a_##n; \
	name->input_b = _hamster_input_b_##n;

_FUNCTION_HAMSTER(0);
_FUNCTION_HAMSTER(1);
_FUNCTION_HAMSTER(2);
_FUNCTION_HAMSTER(3);
_FUNCTION_HAMSTER(4);
_FUNCTION_HAMSTER(5);
_FUNCTION_HAMSTER(6);
_FUNCTION_HAMSTER(7);
_FUNCTION_HAMSTER(8);
_FUNCTION_HAMSTER(9);

Hamster* hamster_create_port(const char* port_name) {
	if(_robot_group_count_robots(_GROUP_HAMSTER) < _MAX_NUM_HAMSTERS) {
		struct hamster* hamster = (struct hamster*)malloc(sizeof(struct hamster));
		struct _robot* robot = (struct _robot*)_hamster_create(port_name);
	
		switch(robot->index) {
			case 0: _FUNCTION_PTR_HAMSTER(hamster, 0); break;
			case 1: _FUNCTION_PTR_HAMSTER(hamster, 1); break;
			case 2: _FUNCTION_PTR_HAMSTER(hamster, 2); break;
			case 3: _FUNCTION_PTR_HAMSTER(hamster, 3); break;
			case 4: _FUNCTION_PTR_HAMSTER(hamster, 4); break;
			case 5: _FUNCTION_PTR_HAMSTER(hamster, 5); break;
			case 6: _FUNCTION_PTR_HAMSTER(hamster, 6); break;
			case 7: _FUNCTION_PTR_HAMSTER(hamster, 7); break;
			case 8: _FUNCTION_PTR_HAMSTER(hamster, 8); break;
			case 9: _FUNCTION_PTR_HAMSTER(hamster, 9); break;
		}
		return hamster;
	}
	return NULL;
}

Hamster* hamster_create(void) {
	return hamster_create_port(NULL);
}

const char* hamster_get_name(void) {
	return _robot_group_get_name(_GROUP_HAMSTER, 0);
}

void hamster_set_name(const char* name) {
	_robot_group_set_name(_GROUP_HAMSTER, 0, name);
}

const char* hamster_get_id(void) {
	return HAMSTER_ID;
}

int hamster_e(int device_id) {
	return _robot_group_e(_GROUP_HAMSTER, 0, device_id);
}

int hamster_read(int device_id) {
	return _robot_group_read(_GROUP_HAMSTER, 0, device_id);
}

int hamster_read_at(int device_id, int index) {
	return _robot_group_read_at(_GROUP_HAMSTER, 0, device_id, index);
}

int hamster_read_array(int device_id, int* data, int length) {
	return _robot_group_read_array(_GROUP_HAMSTER, 0, device_id, data, length);
}

float hamster_read_float(int device_id) {
	return _robot_group_read_float(_GROUP_HAMSTER, 0, device_id);
}

float hamster_read_float_at(int device_id, int index) {
	return _robot_group_read_float_at(_GROUP_HAMSTER, 0, device_id, index);
}

int hamster_read_float_array(int device_id, float* data, int length) {
	return _robot_group_read_float_array(_GROUP_HAMSTER, 0, device_id, data, length);
}

int hamster_write(int device_id, int data) {
	return _robot_group_write(_GROUP_HAMSTER, 0, device_id, data);
}

int hamster_write_at(int device_id, int index, int data) {
	return _robot_group_write_at(_GROUP_HAMSTER, 0, device_id, index, data);
}

int hamster_write_array(int device_id, const int* data, int length) {
	return _robot_group_write_array(_GROUP_HAMSTER, 0, device_id, data, length);
}

int hamster_write_float(int device_id, float data) {
	return _robot_group_write_float(_GROUP_HAMSTER, 0, device_id, data);
}

int hamster_write_float_at(int device_id, int index, float data) {
	return _robot_group_write_float_at(_GROUP_HAMSTER, 0, device_id, index, data);
}

int hamster_write_float_array(int device_id, const float* data, int length) {
	return _robot_group_write_float_array(_GROUP_HAMSTER, 0, device_id, data, length);
}

void hamster_reset(void) {
	_robot_group_reset(_GROUP_HAMSTER, 0);
}

void hamster_dispose(void) {
	_robot_group_dispose(_GROUP_HAMSTER, 0);
}

void hamster_wheels(double left_speed, double right_speed) {
	_hamster_wheels(0, left_speed, right_speed);
}

void hamster_left_wheel(double speed) {
	_hamster_left_wheel(0, speed);
}

void hamster_right_wheel(double speed) {
	_hamster_right_wheel(0, speed);
}

void hamster_stop(void) {
	_hamster_stop(0);
}

int _hamster_line_tracer_mode_evaluate(void* arg) {
	return _hamster_line_tracer_mode_callback(0);
}

void hamster_line_tracer_mode(int mode) {
	_hamster_line_tracer_mode(0, mode, _hamster_line_tracer_mode_evaluate);
}

void hamster_line_tracer_speed(double speed) {
	_hamster_line_tracer_speed(0, speed);
}

int _hamster_board_forward_evaluate(void* arg) {
	return _hamster_board_forward_callback(0);
}

void hamster_board_forward(void) {
	_hamster_board_forward(0, _hamster_board_forward_evaluate);
}

int _hamster_board_left_evaluate(void* arg) {
	return _hamster_board_left_callback(0);
}

void hamster_board_left(void) {
	_hamster_board_left(0, _hamster_board_left_evaluate);
}

int _hamster_board_right_evaluate(void* arg) {
	return _hamster_board_right_callback(0);
}

void hamster_board_right(void) {
	_hamster_board_right(0, _hamster_board_right_evaluate);
}

void hamster_leds(int left_color, int right_color) {
	_hamster_leds(0, left_color, right_color);
}

void hamster_left_led(int color) {
	_hamster_left_led(0, color);
}

void hamster_right_led(int color) {
	_hamster_right_led(0, color);
}

void hamster_beep(void) {
	_hamster_beep(0);
}

void hamster_buzzer(double hz) {
	_hamster_buzzer(0, hz);
}

void hamster_tempo(double bpm) {
	_hamster_tempo(0, bpm);
}

void hamster_pitch(double pitch) {
	_hamster_pitch(0, pitch);
}

void hamster_note(double pitch, double beats) {
	_hamster_note(0, pitch, beats);
}

void hamster_io_mode_a(int mode) {
	_hamster_io_mode_a(0, mode);
}

void hamster_io_mode_b(int mode) {
	_hamster_io_mode_b(0, mode);
}

void hamster_output_a(double value) {
	_hamster_output_a(0, value);
}

void hamster_output_b(double value) {
	_hamster_output_b(0, value);
}

int hamster_signal_strength(void) {
	return _hamster_signal_strength(0);
}

int hamster_left_proximity(void) {
	return _hamster_left_proximity(0);
}

int hamster_right_proximity(void) {
	return _hamster_right_proximity(0);
}

int hamster_left_floor(void) {
	return _hamster_left_floor(0);
}

int hamster_right_floor(void) {
	return _hamster_right_floor(0);
}

int hamster_acceleration_x(void) {
	return _hamster_acceleration_x(0);
}

int hamster_acceleration_y(void) {
	return _hamster_acceleration_y(0);
}

int hamster_acceleration_z(void) {
	return _hamster_acceleration_z(0);
}

int hamster_light(void) {
	return _hamster_light(0);
}

int hamster_temperature(void) {
	return _hamster_temperature(0);
}

int hamster_input_a(void) {
	return _hamster_input_a(0);
}

int hamster_input_b(void) {
	return _hamster_input_b(0);
}
