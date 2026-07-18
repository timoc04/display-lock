// === Seeed Studio XIAO ESP32C3 – Sacoa Spark trigger ===
// De ingang moet minimaal TRIGGER_HOLD_MS gesloten blijven.
// Na een trigger moet de ingang eerst stabiel worden losgelaten.

const int PIN_PULSE = D1;

// Hoe lang de stroomkring gesloten moet blijven
const unsigned long TRIGGER_HOLD_MS = 50;

// Hoe lang de stroomkring geopend moet zijn voordat opnieuw triggeren mag
const unsigned long RELEASE_HOLD_MS = 300;

// Extra minimale tijd tussen twee triggers
const unsigned long TRIGGER_COOLDOWN_MS = 2000;

bool armed = true;

unsigned long lowStartedAt = 0;
unsigned long highStartedAt = 0;
unsigned long lastTriggerAt = 0;

void setup() {
  pinMode(PIN_PULSE, INPUT_PULLUP);
  Serial.begin(9600);

  // Voorkom dat opstartinformatie als trigger wordt gezien
  delay(1000);
}

void loop() {
  const int state = digitalRead(PIN_PULSE);
  const unsigned long now = millis();

  if (armed) {
    if (state == LOW) {
      // Start het meten zodra de stroomkring sluit
      if (lowStartedAt == 0) {
        lowStartedAt = now;
      }

      const bool longEnoughClosed =
          now - lowStartedAt >= TRIGGER_HOLD_MS;

      const bool cooldownFinished =
          now - lastTriggerAt >= TRIGGER_COOLDOWN_MS;

      if (longEnoughClosed && cooldownFinished) {
        // Stuur herkenbaar bericht in plaats van één losse byte
        Serial.println("TRIGGER");

        lastTriggerAt = now;
        armed = false;
        lowStartedAt = 0;
        highStartedAt = 0;
      }
    } else {
      // Elke onderbreking maakt de gemeten sluiting ongeldig
      lowStartedAt = 0;
    }
  } else {
    // Na een trigger moet de ingang eerst stabiel HIGH zijn
    if (state == HIGH) {
      if (highStartedAt == 0) {
        highStartedAt = now;
      }

      if (now - highStartedAt >= RELEASE_HOLD_MS) {
        armed = true;
        highStartedAt = 0;
      }
    } else {
      highStartedAt = 0;
    }
  }

  delay(5);
}