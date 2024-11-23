#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <stdint.h>

// NRF24L01 Setup
#define CE_PIN 9
#define CSN_PIN 10
RF24 radio(CE_PIN, CSN_PIN);

// Joystick Axes
#define JOY_X A0
#define JOY_Y A1

// Button Mappings
#define BUTTON_TRIANGLE 2
#define BUTTON_CIRCLE 3
#define BUTTON_X 4
#define BUTTON_SQUARE 5
#define BUTTON_SELECT 6
#define BUTTON_START 7

// Address for communication
const byte address[6] = "00001";

// State variables
bool isBound = false; // Tracks whether the transmitter is bound to a receiver

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
void readJoystick(DataPacket& packet);
void readButtons(DataPacket& packet);
void sendData(const DataPacket& packet);
void handleBinding();

void setup() {
    Serial.begin(9600);
    Serial.println("Initializing transmitter...");

    initializeNRF();

    // Pin setup for buttons
    pinMode(BUTTON_TRIANGLE, INPUT_PULLUP);
    pinMode(BUTTON_CIRCLE, INPUT_PULLUP);
    pinMode(BUTTON_X, INPUT_PULLUP);
    pinMode(BUTTON_SQUARE, INPUT_PULLUP);
    pinMode(BUTTON_SELECT, INPUT_PULLUP);
    pinMode(BUTTON_START, INPUT_PULLUP);

    Serial.println("Transmitter ready!");
}

void loop() {
    handleBinding();

    if (isBound) {
        DataPacket packet;
        readJoystick(packet);
        readButtons(packet);
        sendData(packet);
        delay(100); // Adjust transmission frequency as needed
    }
}

// Function to initialize the NRF24L01
void initializeNRF() {
    radio.begin();
    radio.openWritingPipe(address);
    radio.setDataRate(RF24_250KBPS);
    radio.setRetries(15, 15);  // Maximum retries and delay
    radio.setPALevel(RF24_PA_LOW);
    radio.stopListening();
}

// Function to read joystick values
void readJoystick(DataPacket& packet) {
    packet.x = analogRead(JOY_X);
    packet.y = analogRead(JOY_Y);
}

// Function to read buttons
void readButtons(DataPacket& packet) {
    packet.triangle = digitalRead(BUTTON_TRIANGLE) == LOW;
    packet.circle = digitalRead(BUTTON_CIRCLE) == LOW;
    packet.buttonX = digitalRead(BUTTON_X) == LOW; // Renamed to buttonX
    packet.square = digitalRead(BUTTON_SQUARE) == LOW;
}

// Function to send data via NRF24L01
void sendData(const DataPacket& packet) {
    // Print raw data byte-by-byte
    Serial.println("Sending Raw Data:");
    uint8_t* rawData = (uint8_t*)&packet;
    for (size_t i = 0; i < sizeof(DataPacket); i++) {
        Serial.print(rawData[i], HEX);
        Serial.print(" ");
    }
    Serial.println();

    bool success = radio.write(&packet, sizeof(packet));
    Serial.println(success ? "Data sent successfully!" : "Failed to send data!");
}



// Function to handle binding and disconnecting
void handleBinding() {
    if (digitalRead(BUTTON_START) == LOW) {
        isBound = true;
        Serial.println("Binding to receiver...");
        delay(500); // Debounce delay
    }

    if (digitalRead(BUTTON_SELECT) == LOW) {
        isBound = false;
        Serial.println("Disconnecting from receiver...");
        delay(500); // Debounce delay
    }
}
