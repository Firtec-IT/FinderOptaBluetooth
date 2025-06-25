/* -------------------------------------------------------- */
/*                        FIRTEC.IT                         */
/*              CONNESSIONE BT CON FINDER OPTA              */
/*                      V. 1.0 180625                       */
/* -------------------------------------------------------- */

#include <ArduinoBLE.h>
#include <string.h>
#include <stdio.h>

bool DEBUG = true;

/* -------------------------------------------------------- */
/*       DEFINIZIONI UUID SERVIZIO E CARATTERISTICHE        */
/* -------------------------------------------------------- */

// UUID del Servizio principale
const char SERVICE_UUID[] = "19B10000-xxxx-xxxx-xxxx-xxxxxxxxxxxx";
BLEService finderOptaService(SERVICE_UUID);

// UUID per la Caratteristica degli Input Digitali
const char DIGITAL_CHARACTERISTIC_UUID[] = "19B10001-xxxx-xxxx-xxxx-xxxxxxxxxxxx";

// UUID per la Caratteristica degli Input Analogici
const char ANALOG_CHARACTERISTIC_UUID[] = "19B10002-xxxx-xxxx-xxxx-xxxxxxxxxxxx";

// UUID per la Caratteristica Informazioni
const char INFO_CHARACTERISTIC_UUID[] = "19B10003-xxxx-xxxx-xxxx-xxxxxxxxxxxx";

// UUID standard per Client Characteristic Configuration Descriptor (CCCD)
const char CCCD_UUID[] = "00002902-0000-1000-8000-00805f9b34fb";

/* -------------------------------------------------------- */
/*                    CONFIGURAZIONE PIN                    */
/* -------------------------------------------------------- */

// Input digitali del Finder Opta: (Es. A0, A1, A2, A3, A4, A5, A6)
const int digitalInputPins[] = {A0, A1, A2, A3, A4, A5, A6};
const int NUM_DIGITAL_INPUTS = sizeof(digitalInputPins) / sizeof(digitalInputPins[0]);
byte prevDigitalInputBytes[NUM_DIGITAL_INPUTS];

// Input analogici del Finder Opta: (Es. A7)
const int analogInputPins[] = {A7};
const int MY_NUM_ANALOG_INPUTS = sizeof(analogInputPins) / sizeof(analogInputPins[0]);
float prevAnalogVoltages[MY_NUM_ANALOG_INPUTS];
const int MAX_VOLTAGE_STRING_LENGTH = 10;
char analogVoltageStrings[MY_NUM_ANALOG_INPUTS][MAX_VOLTAGE_STRING_LENGTH];

// Caratteristica per gli input digitali
BLECharacteristic digitalInputsCharacteristic(DIGITAL_CHARACTERISTIC_UUID, BLERead | BLENotify, NUM_DIGITAL_INPUTS);

// Caratteristica per gli input analogici
BLECharacteristic analogInputCharacteristic(ANALOG_CHARACTERISTIC_UUID, BLERead | BLENotify, MY_NUM_ANALOG_INPUTS * MAX_VOLTAGE_STRING_LENGTH);

// Caratteristica per l'informazioni
const int MAX_WORD_LENGTH = 20;
char currentInfoString[MAX_WORD_LENGTH + 1];
char prevCurrentInfoString[MAX_WORD_LENGTH + 1];
BLECharacteristic infoStringCharacteristic(INFO_CHARACTERISTIC_UUID, BLERead | BLENotify, MAX_WORD_LENGTH);

void setup() {
  Serial.begin(9600);
  while (!Serial);

  // Inizializza input/output
  analogReadResolution(12);
  pinMode(BTN_USER, INPUT);
  pinMode(LED_D0, OUTPUT);
  pinMode(LED_D1, OUTPUT);
  pinMode(LED_D2, OUTPUT);
  pinMode(LED_D3, OUTPUT);
  for (int i = 0; i < NUM_DIGITAL_INPUTS; i++) {
    pinMode(digitalInputPins[i], INPUT);
  }

  // Inizializza il modulo BLE
  if (!BLE.begin()) {
    if (DEBUG) { Serial.println("Avvio BLE fallito"); }
    while (true);
  }

  // Configura il dispositivo BLE
  BLE.setLocalName("Firtec_FinderOpta");
  BLE.setAdvertisedService(finderOptaService);
  finderOptaService.addCharacteristic(digitalInputsCharacteristic);
  finderOptaService.addCharacteristic(analogInputCharacteristic);
  finderOptaService.addCharacteristic(infoStringCharacteristic);
  BLE.addService(finderOptaService);

  // Inizializza i valori di default (Input digitali)
  byte initialDigitalValues[NUM_DIGITAL_INPUTS];
  for(int i = 0; i < NUM_DIGITAL_INPUTS; i++) {
    initialDigitalValues[i] = 0x00;
  }
  
  // Inizializza i valori di default (Input analogici)
  for (int i = 0; i < MY_NUM_ANALOG_INPUTS; i++) {
    prevAnalogVoltages[i] = -999.0;
  }

  updateDigitalCharacteristic(true);
  updateAnalogCharacteristic(true);

  // Inizializza i valori di default (Stringa informazioni)
  strcpy(currentInfoString, "0.0.0.0");
  strcpy(prevCurrentInfoString, "");
  infoStringCharacteristic.writeValue(currentInfoString);
  strcpy(prevCurrentInfoString, currentInfoString);
  
  // Avvia la connessione BT
  BLE.advertise();
  if (DEBUG) { Serial.println("BLE Avviato..."); }

  // Accende il LED_D0 per indicare che la connessione BT è attiva
  digitalWrite(LED_D0, HIGH);
}

void loop() {
  BLEDevice central = BLE.central();

  if (central) {
    if (DEBUG) { 
      Serial.print("Connesso al dispositivo: ");
      Serial.println(central.address());
    }

    // Accende il LED_D1 per indicare che il dispositivo è connesso
    digitalWrite(LED_D1, HIGH);

    while (central.connected()) {
      //Indirizzo IP
      strcpy(currentInfoString, "192.168.1.23");
      infoStringCharacteristic.writeValue(currentInfoString);

      //Controllo input digitali
      updateDigitalCharacteristic(false);

      //Controllo input analogici
      updateAnalogCharacteristic(false);

      delay(100);
    }

    if (DEBUG) {
      Serial.print("Disconnesso dal dispositivo: ");
      Serial.println(central.address());
      // Spegne il LED_D1 per indicare che il dispositivo è disconnesso
      digitalWrite(LED_D1, LOW);
    }
  }
}

void updateDigitalCharacteristic(bool forceUpdate) {
  // Input digitali
  byte currentDigitalInputBytes[NUM_DIGITAL_INPUTS];
  bool digitalChanged = false;
  for (int i = 0; i < NUM_DIGITAL_INPUTS; i++) {
    if (digitalRead(digitalInputPins[i]) == LOW) {
      // Input OFF
      currentDigitalInputBytes[i] = 0x00;
    } else {
      // Input ON (pulsante premuto o contatto chiuso)
      currentDigitalInputBytes[i] = 0x01;
    }
    if (currentDigitalInputBytes[i] != prevDigitalInputBytes[i]) {
      digitalChanged = true;
    }
  }
  
  // Invia la notifica BLE
  if (digitalChanged || forceUpdate) {
    if (DEBUG) {
      Serial.print("Inputs digitali aggiornati: ");
      for (int i = 0; i < NUM_DIGITAL_INPUTS; i++) {
        Serial.print(currentDigitalInputBytes[i], HEX);
        Serial.print(" ");
      }
      Serial.println();
    }
    digitalInputsCharacteristic.writeValue(currentDigitalInputBytes, NUM_DIGITAL_INPUTS);
    memcpy(prevDigitalInputBytes, currentDigitalInputBytes, NUM_DIGITAL_INPUTS);
  }
}

void updateAnalogCharacteristic(bool forceUpdate) {
  // Variabili per il calcolo della tensione
  float VOLTAGE_MAX = 3.3;
  float RESOLUTION = 4095.0;
  float DIVIDER = 0.3034;

  const float ANALOG_VOLTAGE_CHANGE_THRESHOLD = 0.10;
  bool analogChanged = false;
  char fullAnalogDataString[MY_NUM_ANALOG_INPUTS * MAX_VOLTAGE_STRING_LENGTH];
  fullAnalogDataString[0] = '\0';

  for (int i = 0; i < MY_NUM_ANALOG_INPUTS; i++) {
    int terminalValue = analogRead(analogInputPins[i]);
    float currentVoltage = terminalValue * (VOLTAGE_MAX / RESOLUTION) / DIVIDER;

    // Controlla se il valore è cambiato oltre la soglia o se l'aggiornamento è forzato
    if (forceUpdate || abs(currentVoltage - prevAnalogVoltages[i]) >= ANALOG_VOLTAGE_CHANGE_THRESHOLD) {
      analogChanged = true;
      sprintf(analogVoltageStrings[i], "%.2f", currentVoltage); 
    } else {
      sprintf(analogVoltageStrings[i], "%.2f", prevAnalogVoltages[i]);
    }
    
    strcat(fullAnalogDataString, analogVoltageStrings[i]);
    if (i < MY_NUM_ANALOG_INPUTS - 1) {
      strcat(fullAnalogDataString, ",");
    }
  }

  // Invia la notifica BLE
  if (analogChanged || forceUpdate) {
    analogInputCharacteristic.writeValue(fullAnalogDataString); 
    
    // Aggiorna i valori precedenti con i valori attuali
    for (int i = 0; i < MY_NUM_ANALOG_INPUTS; i++) {
        int terminalValue = analogRead(analogInputPins[i]); 
        prevAnalogVoltages[i] = terminalValue * (VOLTAGE_MAX / RESOLUTION) / DIVIDER;
    }

    if (DEBUG) {
      Serial.print("Inputs analogici aggiornati: ");
      Serial.println(fullAnalogDataString);
    }
  }
}
