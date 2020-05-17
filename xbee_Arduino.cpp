#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <Arduino.h> 
#include "HardwareSerial.h"
#include "SoftwareSerial.h"
#include <xbee_Arduino.h>

// SoftwareSerial Serial(0, 1);

void xbee::xbee_in_array() {
	if (Serial.available()>0) {
		bufferin[count] = Serial.read()&0xFF;
		if (count>=3 && bufferin[count-3]==0x7E && !flag){ // If we reached the beginnig of the data
			flag = true; // set the flag to notify that we have calculated data length
			data_width = bufferin[count - 2]*256 + bufferin[count - 1];
			data_start_index = count;
			data_end_index = data_start_index + data_width - 1;	
		}
		// check if data has no error
		if ((checksum(data_start_index, data_end_index + 1,'I') == 0xFF) && (count >= data_width+3)) {
			end_byte = count;
			entry = true;
			count = 0;
			flag = false;
		}
		else {
			count++;
		}
	}
}

void xbee::set_destination(char destionation_address[]) {
	for (int i = 0; i < 8; i++) {
		bufferout[5 + i] = destionation_address[i];
		destination_address64[i] = destionation_address[i];
	}
	// bufferout[13] = destination_address16[0]; //0xFF; // Destination 16 bit address high
	// bufferout[14] = destination_address16[0]; //0xFE; // Destination 16 bit address low
	// bufferout[15] = 0x00;
	// bufferout[16] = 0x01;
	// destination_address16[0] = 0xFF;
	// destination_address16[1] = 0xFE;
}

bool xbee::xbee_detected() {
	if (entry) {
		entry = false;
		return true;
	}
	else {
		return false;
	}
}

int xbee::endbyte_pos() {
	return end_byte;
}

char xbee::buffer(int posi) {
	return bufferin[posi];
}

String xbee::array_concat(int start, int end) {
	String concat;
	for (size_t i = start; i <= end; i++) concat += bufferin[i]&0xFF;
	return concat;
}

String xbee::getdata() {
	String data;
	if (data_width==0x07) {
		for (size_t i = data_start_index; i <= data_end_index; i++) {
			data += bufferin[i];
			bufferin[i] = ' ';
		}
	}
	else {
		for (size_t i = data_start_index + 4; i <= data_end_index; i++) {
			data += bufferin[i];
			bufferin[i] = ' ';
		}
	}
	return data;
}

void xbee::begin(int speed) {
	Serial.begin(speed);
	// for (size_t i = 0; i < 8; i++) bufferout[i+5] = buffer[i];
}

uint8_t xbee::checksum(int start, uint8_t end, char buffer_type) {
	int sum = 0;	
	if (buffer_type == 'I')	for (int r = start; r <= end; r++) sum += bufferin[r] & 0xFF; //xbeeFrame is the array where are stored the bytes' frame
	if (buffer_type == 'O')	for (int r = start; r <= end; r++) sum += bufferout[r] & 0xFF; 
	return (0xFF - sum);
}

void xbee::sendata(String data) {
	char buff[300];
	bufferout[0] = 0x7E;
	bufferout[1] = (int)floor((data.length() + 14) / 256);
	bufferout[2] = (data.length() + 14) % 256;
	bufferout[3] = 0x10;
	bufferout[4] = 0x00;
	bufferout[15] = 0x00;
	bufferout[16] = 0x01;
	
	data.toCharArray(buff,data.length()+1);
	
	for (int i = 0; i < data.length(); i++) {
		bufferout[i + 17] = buff[i];
	}
	bufferout[data.length() + 17] = checksum(3, 17 + data.length() - 1, 'O');
	for (size_t i = 0; i < data.length() + 18; i++) {
		Serial.write(bufferout[i] & 0xFF);
	}
	Serial.println();
	// for (size_t i = 0; i < data.length() + 18; i++) {
	// 	Serial.print(bufferout[i] & 0xFF, HEX);
	// }
	// Serial.println();
}

void xbee::sendata(String data, int length, char endCh, bool enable_acks) {
	char buff[300];
	bufferout[0] = 0x7E;
	bufferout[1] = (int)floor((length + 14) / 256);
	bufferout[2] = (length + 14) % 256;
	bufferout[3] = 0x10;
	bufferout[4] = 0x00;
	bufferout[15] = 0x00;
	if (enable_acks) {
		bufferout[16] = 0x00;
	} else {
		bufferout[16] = 0x01;
	}
	
	data.toCharArray(buff,length+1);
	
	for (int i = 0; i < length-1; i++) {
		bufferout[i + 17] = buff[i];
	}
	bufferout[length + 16] = endCh;
	bufferout[length + 17] = checksum(3, 17 + length - 1, 'O');
	for (size_t i = 0; i < length + 18; i++) {
		Serial.write(bufferout[i] & 0xFF);
	}
	Serial.println();
}

String xbee::receive() {
	String message = "";
	while (Serial.available() > 0) {
		message += Serial.read();
	}
	return message;
}

void xbee::create_route(char dest_address[], char dest_address16[], int numHops, char route[]) {
	Serial.println("Creating route...");
	destination_address16[0] = dest_address16[0];
	destination_address16[1] = dest_address16[1];
	bufferout[0] = 0x7E;
	bufferout[1] = (int)floor((17 + 2 * numHops - 3) / 256);
	bufferout[2] = (17 + 2 * numHops - 3) % 256;
	bufferout[3] = 0x21;
	bufferout[4] = 0x00;
	set_destination(dest_address);
	bufferout[13] = dest_address16[0];
	bufferout[14] = dest_address16[1];
	bufferout[15] = 0x00; // Route command options
	bufferout[16] = (char)numHops;
	for (int i = 0; i < numHops; i++) {
		bufferout[17 + 2 * i] = route[2 * i];
		bufferout[17 + 2 * i + 1] = route[2 * i + 1];
	}
	bufferout[17 + 2 * numHops] = checksum(3, 17 + 2 * numHops - 1, 'O');	
	for (size_t i = 0; i <= 17 + 2 * numHops; i++) {
		Serial.write(bufferout[i] & 0xFF);
	}
	Serial.println();
	for (size_t i = 0; i <= 17 + 2 * numHops; i++) {
		Serial.print(bufferout[i] & 0xFF, HEX);
	}
	Serial.println();
	Serial.println("Route created...");
}