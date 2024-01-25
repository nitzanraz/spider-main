// Define the pins for your relay modules
const int relayPin1 = 22; // Change this to your first relay module pin
const int relayPin2 = 3; // Change this to your second relay module pin

// Initialize variables to keep track of relay states
bool relayState1 = false;
bool relayState2 = false;

// Function to setup the relay
void setup_relay(int relay_id) {
  if (relay_id == 1) {
    pinMode(relayPin1, OUTPUT);
  } else if (relay_id == 2) {
    pinMode(relayPin2, OUTPUT);
  }
}

// Function to toggle the relay
void toggle(int relay_id, bool on) {
  if (relay_id == 1) {
    digitalWrite(relayPin1, on ? HIGH : LOW);
    relayState1 = on;
  } else if (relay_id == 2) {
    digitalWrite(relayPin2, on ? HIGH : LOW);
    relayState2 = on;
  }
}

void setup_2_relay() {
  setup_relay(1); // Initialize relay 1
  setup_relay(2); // Initialize relay 2
}

void relay_loop() {
  // Toggle relay 1 state every second
  if (millis() % 1000 > 500) {
    //relayState1 = !relayState1;
    toggle(1, true);
  }else{
    toggle(1, false);
  }

  // Toggle relay 2 state every second
  if (millis() % 1000 == 0) {
    relayState2 = !relayState2;
    //toggle(2, relayState2);
  }
}
