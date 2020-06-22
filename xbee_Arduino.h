#ifndef XBEE_ARDUINO_H
#define XBEE_ARDUINO_H

class xbee {
public:
	/* Initializes xbee serial connection
	 * speed - baudrate of the serial connecction in bps
	 * buffer - initial buffer
	*/
	void begin(int speed/*(, char buffer[]*/);
	// Reead the data and store it in bufferin
	// If data is valid, set entry to TRUE
	void xbee_in_array();
	// Sets the destinations address
	void set_destination(char destionation_address[]);
	int endbyte_pos();
	// Return whether data is valid and reset the entry
	bool xbee_detected();
	String array_concat(int start, int end);
	char buffer(int posi);
	String getdata();
	uint8_t checksum(int start, uint8_t end, char buffer_type);
	// Sends data contained in the string
	void sendata(String data);
	// Sends data contained in the string
	void sendata(String data, int length, char endCh, bool enable_acks);
	// Returns frame associated with the message
	void getFrame(char output_Buff[], char data, int length, char endCh, bool enable_acks);
	// Reads data received and returns as a String
	String receive();
	// Creates route between the source and destination (for multi hop networks)
	void create_route(char dest_address[], char dest_address16[], int numHops, char route[]);

private:
	char destination_address64[8]; // Destination 64-bit address
	char destination_address16[2] = {0xFF, 0xFE}; // Destination 16-bit adderss
	char bufferin[300]; // Input buffer
	char bufferout[300]; // Output buffer
	size_t count=0;
	size_t end_byte=0;
	bool entry;
	bool flag; // whether we know data length or not
	size_t data_width;
	int data_start_index; // index at which data starts
	int data_end_index; // index at which data ends
};

#endif // !XBEE_ARDUINO_H