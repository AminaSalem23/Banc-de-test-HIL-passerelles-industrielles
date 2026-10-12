
#include <Arduino.h>

// ==================== BROCHES ====================
const int RELAY_PIN = 27;
const int VOLTAGE_DAC_PIN = 25;  // DAC1 : generateur de tension
const int CURRENT_DAC_PIN = 26; // DAC2 : generateur de courant

// ==================== VALEURS ====================
int voltageValue = 175;
int currentValue = 13;

bool relayState = false;

// ==================== INITIALISATION ====================
void setup() {
  Serial.begin(115200);

  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);

  dacWrite(VOLTAGE_DAC_PIN, voltageValue);
  dacWrite(CURRENT_DAC_PIN, currentValue);

  Serial.println();
  Serial.println("========================================");
  Serial.println("   FIRMWARE ESP32 - BANC DE TEST HIL");
  Serial.println("========================================");
  Serial.println("Commandes disponibles :");
  Serial.println("RELAY_ON");
  Serial.println("RELAY_OFF");
  Serial.println("VOLTAGE 0 a 255");
  Serial.println("CURRENT 0 a 255");
  Serial.println("STATUS");
  Serial.println("========================================");
}

// ==================== COMMANDE DU RELAIS ====================
void controlRelay(bool state) {
  relayState = state;
  digitalWrite(RELAY_PIN, state ? HIGH : LOW);

  if (state) {
    Serial.println("Relais active (GPIO 27 = HIGH)");
  } else {
    Serial.println("Relais desactive (GPIO 27 = LOW)");
  }
}

// ==================== GENERATEUR DE TENSION ====================
void setVoltage(int value) {
  if (value < 0 || value > 255) {
    Serial.println("Erreur : la valeur DAC doit etre entre 0 et 255.");
    return;
  }

  voltageValue = value;
  dacWrite(VOLTAGE_DAC_PIN, voltageValue);

  float v_dac = (voltageValue / 255.0) * 3.3;
  float v_out = v_dac * 3.0; // Gain AOP suppose egal a 3

  Serial.println("--- Generateur de tension ---");
  Serial.print("Valeur DAC : ");
  Serial.println(voltageValue);

  Serial.print("Tension DAC theorique : ");
  Serial.print(v_dac, 2);
  Serial.println(" V");

  Serial.print("Sortie AOP theorique : ");
  Serial.print(v_out, 2);
  Serial.println(" V");
}

// ==================== GENERATEUR DE COURANT ====================
void setCurrent(int value) {
  if (value < 0 || value > 255) {
    Serial.println("Erreur : la valeur DAC doit etre entre 0 et 255.");
    return;
  }

  currentValue = value;
  dacWrite(CURRENT_DAC_PIN, currentValue);

  float v_gpio = (currentValue / 255.0) * 3.3;
  float i_out = 4.0 + (v_gpio / 3.3) * 16.0;

  Serial.println("--- Generateur de courant ---");
  Serial.print("Valeur DAC : ");
  Serial.println(currentValue);

  Serial.print("Tension DAC theorique : ");
  Serial.print(v_gpio, 2);
  Serial.println(" V");

  Serial.print("Courant de boucle theorique : ");
  Serial.print(i_out, 2);
  Serial.println(" mA");
}

// ==================== ETAT DU SYSTEME ====================
void showStatus() {
  Serial.println("--- Etat du banc de test ---");

  Serial.print("Relais : ");
  Serial.println(relayState ? "ACTIF" : "INACTIF");

  Serial.print("DAC tension (GPIO 25) : ");
  Serial.println(voltageValue);

  Serial.print("DAC courant (GPIO 26) : ");
  Serial.println(currentValue);
}

// ==================== TRAITEMENT DES COMMANDES ====================
void processCommand(String command) {
  command.trim();
  command.toUpperCase();

  if (command == "RELAY_ON") {
    controlRelay(true);
  }
  else if (command == "RELAY_OFF") {
    controlRelay(false);
  }
  else if (command == "STATUS") {
    showStatus();
  }
  else if (command.startsWith("VOLTAGE ")) {
    int value = command.substring(8).toInt();
    setVoltage(value);
  }
  else if (command.startsWith("CURRENT ")) {
    int value = command.substring(8).toInt();
    setCurrent(value);
  }
  else {
    Serial.println("Commande inconnue.");
    Serial.println("Utilisez RELAY_ON, RELAY_OFF, VOLTAGE n, CURRENT n ou STATUS.");
  }
}

// ==================== BOUCLE PRINCIPALE ====================
void loop() {
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    processCommand(command);
  }
}
