void sendDocToCar(StaticJsonDocument<100> doc, WiFiClient client){
  client.println(F("POST / HTTP/1.1"));
  client.println(F("Host: 192.168.1.4"));
  client.println(F("Content-Type: application/json"));
  client.println(F("Referer: http://192.168.1.4/"));
  client.print(F("Content-Length: "));
  client.println(measureJson(doc));
  client.println(F("Connection: close"));
  serializeJson(doc, client);
  client.println();
  client.println();
}

int tryConnectionToWifi(){
  Serial.print("Attempting to connect to WPA SSID: ");
  Serial.println(targetSSID);
  int connectionStatus = WiFi.begin(targetSSID, targetNetworkPassword);
  delay(CONNECTION_WAIT_MILLISECONDS);
  Serial.print("Connection status is: ");
  Serial.println(connectionStatus);
  return connectionStatus;
}

void setWifiConnectionPinsToSuccess(){
  digitalWrite(WIFI_CONNECTION_SUCCESS_LED_PIN, HIGH);
  digitalWrite(WIFI_CONNECTION_FAILURE_LED_PIN, LOW);
}

void setWifiConnectionPinsToFailure(){
  digitalWrite(WIFI_CONNECTION_SUCCESS_LED_PIN, LOW);
  digitalWrite(WIFI_CONNECTION_FAILURE_LED_PIN, HIGH);
}

void setCarNetworkConnectionPinsToSuccess(){
  digitalWrite(CAR_NETWORK_CONNECTION_SUCCESS_LED_PIN, HIGH);
  digitalWrite(CAR_NETWORK_CONNECTION_FAILURE_LED_PIN, LOW);
}

void setCarNetworkConnectionPinsToFailure(){
  digitalWrite(CAR_NETWORK_CONNECTION_SUCCESS_LED_PIN, LOW);
  digitalWrite(CAR_NETWORK_CONNECTION_FAILURE_LED_PIN, HIGH);
}