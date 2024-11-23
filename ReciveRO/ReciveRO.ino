#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <stdint.h>

// NRF24L01 Pins for Croduino NOVA32
#define CE_PIN 4  // GPIO4 (D4 on the board)
#define CSN_PIN 5 // GPIO5 (D5 on the board)
RF24 radio(CE_PIN, CSN_PIN);

// Address for communication
const byte address[6] = "00001";

// Data Packet Structure

#pragma pack(push, 1)  // Disable padding between fields
struct DataPacket {
    int16_t x;         // Joystick X-axis (16-bit signed integer)
    int16_t y;         // Joystick Y-axis (16-bit signed integer)
    uint8_t triangle;  // Triangle button state (8-bit unsigned integer)
    uint8_t circle;    // Circle button state (8-bit unsigned integer)
    uint8_t buttonX;   // X button state (8-bit unsigned integer)
    uint8_t square;    // Square button state (8-bit unsigned integer)
};
#pragma pack(pop)


// Function prototypes
void initializeNRF();
bool receiveData(DataPacket& packet);
void printData(const DataPacket& packet);

void setup() {
    Serial.begin(115200);
    Serial.println("Initializing receiver on Croduino NOVA32...");

    initializeNRF();

    Serial.println("Receiver ready!");
}

void loop() {
    DataPacket packet;
    if (receiveData(packet)) {
        printData(packet);
    }
}

// Function to initialize the NRF24L01
void initializeNRF() {
    if (!radio.begin()) {
        Serial.println("NRF24L01 initialization failed!");
        while (1); // Halt if initialization fails
    }

    radio.openReadingPipe(0, address);
    radio.setDataRate(RF24_250KBPS);
    radio.setRetries(15, 15);  // Maximum retries and delay

    radio.setPALevel(RF24_PA_LOW);
    radio.startListening();
}

// Function to receive data from the transmitter
bool receiveData(DataPacket& packet) {
    if (radio.available()) {
        radio.read(&packet, sizeof(packet));

        // Debug: Print all fields explicitly
        Serial.println("Received Data:");
        Serial.print("X: ");
        Serial.println(packet.x);  // Correctly print int16_t
        Serial.print("Y: ");
        Serial.println(packet.y);  // Correctly print int16_t
        Serial.print("Triangle: ");
        Serial.println((int)packet.triangle);  // Cast uint8_t to int
        Serial.print("Circle: ");
        Serial.println((int)packet.circle);  // Cast uint8_t to int
        Serial.print("X Button: ");
        Serial.println((int)packet.buttonX);  // Cast uint8_t to int
        Serial.print("Square: ");
        Serial.println((int)packet.square);  // Cast uint8_t to int

        return true;
    }
    return false;
}




// Function to print received data to the Serial Monitor
void printData(const DataPacket& packet) {
	return;
    Serial.print("X: ");
    Serial.print(packet.x);
    Serial.print("\tY: ");
    Serial.print(packet.y);
    Serial.print("\tTriangle: ");
    Serial.print(packet.triangle ? "PRESSED" : "RELEASED");
    Serial.print("\tCircle: ");
    Serial.print(packet.circle ? "PRESSED" : "RELEASED");
    Serial.print("\tX: ");
    Serial.print(packet.buttonX ? "PRESSED" : "RELEASED");
    Serial.print("\tSquare: ");
    Serial.println(packet.square ? "PRESSED" : "RELEASED");
}
