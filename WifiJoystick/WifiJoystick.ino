#include <ArduinoJson.h>
#include <ArduinoJson.hpp>
#include <WiFiNINA.h>

struct MovementCommand {
  int x;
  int y;
  int speed;
};

static int JOYSTICK_BUTTON_PIN = 9;
static int JOYSTICK_X_AXIS_PIN = A0;
static int JOYSTICK_Y_AXIS_PIN = A1;

static int JOYSTICK_X_AXIS_CENTER = 515;
static int JOYSTICK_Y_AXIS_CENTER = 505;
static int JOYSTICK_MIN_VALUE = 0;
static int JOYSTICK_MAX_VALUE = 115;
static int MOVEMENT_COMMAND_MINIMUM_SPEED = 5;
static int MOVEMENT_COMMAND_MAXIMUM_SPEED = 64;

static int WIFI_CONNECTION_SUCCESS_LED_PIN = 3;
static int WIFI_CONNECTION_FAILURE_LED_PIN = 4;

static int CAR_NETWORK_CONNECTION_SUCCESS_LED_PIN = 5;
static int CAR_NETWORK_CONNECTION_FAILURE_LED_PIN = 6;

char targetSSID[] = "YOUR_SSID_HERE";
char targetNetworkPassword[] = "YOUR_TARGET_NETWORK_PASSWORD_HERE";

int connectionStatus =  WL_IDLE_STATUS;
static unsigned int CONNECTION_WAIT_MILLISECONDS = 6000;

IPAddress carIP(192,168,1,4);
static unsigned int targetCarPort = 80;
WiFiClient client;

struct MovementCommand currentMovementCommand;
struct MovementCommand filteredMovementCommand;

StaticJsonDocument<100> buildMovementCommandDoc(struct MovementCommand commandStruct){  
  StaticJsonDocument<100> doc;
  StaticJsonDocument<50> movementCommand;
  movementCommand["x"] = commandStruct.x;
  movementCommand["y"] = commandStruct.y;
  movementCommand["speed"] = commandStruct.speed;
  doc["movementCommand"] = movementCommand;
  doc["senderType"] = "joystick";
  return doc;
}

void setup() {
  pinMode(JOYSTICK_BUTTON_PIN, INPUT_PULLUP);
  pinMode(WIFI_CONNECTION_SUCCESS_LED_PIN, OUTPUT);
  pinMode(WIFI_CONNECTION_FAILURE_LED_PIN, OUTPUT);
  pinMode(CAR_NETWORK_CONNECTION_SUCCESS_LED_PIN, OUTPUT);
  pinMode(CAR_NETWORK_CONNECTION_FAILURE_LED_PIN, OUTPUT);

  //Initially set connection pins to fail to indicate we haven't connected successfully yet
  setWifiConnectionPinsToFailure();
  setCarNetworkConnectionPinsToFailure();

  Serial.begin(9600);
  while (connectionStatus != WL_CONNECTED){
    connectionStatus = tryConnectionToWifi();
  }

  Serial.println("Connection to wifi network successful!!");
}

void loop() {
  if(WiFi.status() == WL_CONNECTED){
    setWifiConnectionPinsToSuccess();
  } else {
    setWifiConnectionPinsToFailure();
    tryConnectionToWifi();
    return;
  }

  if(client.connect(carIP, targetCarPort) == false){
    Serial.println("Client not connected to target ip!");
    setCarNetworkConnectionPinsToFailure();
    return;
  }

  int xVal = analogRead(JOYSTICK_X_AXIS_PIN);
  int yVal = analogRead(JOYSTICK_Y_AXIS_PIN);
  int buttonVal = digitalRead(JOYSTICK_BUTTON_PIN);

  int xOffCenterVal = xVal - JOYSTICK_X_AXIS_CENTER;
  int xMagnitude = map(abs(xOffCenterVal), JOYSTICK_MIN_VALUE, 512, 0, MOVEMENT_COMMAND_MAXIMUM_SPEED);

  int yOffCenterVal = yVal - JOYSTICK_Y_AXIS_CENTER;
  int yMagnitude = map(abs(yOffCenterVal), JOYSTICK_MIN_VALUE, 512, 0, MOVEMENT_COMMAND_MAXIMUM_SPEED);

  //Must send negative speed to go backwards
  if(yOffCenterVal >= 0){
    currentMovementCommand = getMovementCommandFromJoystick(xOffCenterVal, yOffCenterVal, max(xMagnitude, yMagnitude));
    filteredMovementCommand = filterMovementCommandFromJoystick(currentMovementCommand, MOVEMENT_COMMAND_MINIMUM_SPEED);
  } else {
    currentMovementCommand = getMovementCommandFromJoystick(xOffCenterVal, yOffCenterVal, max(xMagnitude, yMagnitude) * -1);
    filteredMovementCommand = filterMovementCommandFromJoystick(currentMovementCommand, MOVEMENT_COMMAND_MINIMUM_SPEED);
  }
  
  //displayMovementCommand(currentMovementCommand);
  //displayMovementCommand(filteredMovementCommand);

  StaticJsonDocument<100> currentDoc = buildMovementCommandDoc(filteredMovementCommand);
  
  sendDocToCar(currentDoc, client);

  // Check HTTP status
  char status[32] = {0};
  client.readBytesUntil('\r', status, sizeof(status));
  // It should be "HTTP/1.0 200 OK" or "HTTP/1.1 200 OK"
  if (strcmp(status + 9, "200 OK") == 0) {
    setCarNetworkConnectionPinsToSuccess();
    client.stop();    
  } else {
    Serial.print(F("Unexpected response: "));
    Serial.println(status);
    setCarNetworkConnectionPinsToFailure();
    client.stop();
  }
}
