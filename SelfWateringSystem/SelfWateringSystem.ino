int MoistureLevels[] = {0, 0, 0, 0, 0, 0};
const int WaterReadings[] = {200, 202, 205, 215, 220, 210};
const int OpenAirReadings[] = {518, 520, 530, 530, 530, 530};
const int moisturePins[] = {A0, A1, A2, A3, A4, A5};

const int pumpPins[] = {2, 3, 4, 5, 6, 7};
const int numSensors = 6;
unsigned long previousMillis[numSensors] = {0};
const unsigned long interval = 8UL*60*60*1000;  // Check every 6 hours
const int largePotTime = 5000; // water large Pots for 5 seconds
const int smallPotTime = 2000; // water small pots for 2 seconds

// A0 -> 2
// A1 -> 3
// A2 -> 4
// A3 -> 5
// A4 -> 6
// A5 -> 7

int SoilMoisturePercentage = 0;
int recheckMoisture[] = {0, 0 ,0 ,0 ,0 ,0}; // if 0 no need to recheck, if 1 recheck
// #define trigPin 7
// Need to check pump 2, was not activating when set up
const int thresholds[] = {35, 45, 45, 45, 40, 35}; // values from 50, 35, 50, 50, 50, 25
const int upperLimit[] = {50, 70, 70, 70, 60, 70}; // Need a warning if I am overwatering the plants

void moistureOutputs(int sensorNumber, int moistureLevel, int OpenAirReadings, int WaterReadings) {
    Serial.print("This is sensor number: ");
    Serial.println(sensorNumber);

    Serial.print("Moisture level is: ");
    Serial.println(moistureLevel);

    SoilMoisturePercentage = map(moistureLevel, OpenAirReadings, WaterReadings, 0, 100);
    Serial.print("Percent moisture is: ");
    Serial.println(SoilMoisturePercentage);
}


void setup() {
  Serial.begin(9600); // open serial port, set the baud rate to 9600 bps
  bool allZeros = true;
  //pinMode(trigPin, OUTPUT);
  //digitalWrite(trigPin, HIGH);
  for (int i = 0; i < numSensors; i++) {
    pinMode(moisturePins[i], INPUT);
    pinMode(pumpPins[i], OUTPUT);
    digitalWrite(pumpPins[i], HIGH);  // Initially turn off all pumps
  }
}

void loop() {
  // Reads all sensors every 6 hours and decides if they need water or not
  unsigned long currentMillis = millis();
  // MoistureLevel = analogRead(A4);  //update based on the analog Pin selected
  for (int i = 0; i < numSensors; i++) { //numSensors

    // Check all 6 pumps
    if (currentMillis - previousMillis[i] >= interval) {
      //Serial.print("Hi this sensor: ");
      //Serial.println(i);
      previousMillis[i] = currentMillis;

      int moistureValue = analogRead(moisturePins[i]);
      moistureOutputs(i, moistureValue, OpenAirReadings[i], WaterReadings[i]);
      SoilMoisturePercentage = map(moistureValue, OpenAirReadings[i], WaterReadings[i], 0, 100);

      if (SoilMoisturePercentage >= thresholds[i]) {
        // since large pots will need more water than small pots
        if ((pumpPins[i] % 2) != 0) {
            //moistureOutputs(i, SoilMoisturePercentage);
            //digitalWrite(pumpPins[i], LOW);
            //delay(largePotTime);

            //digitalWrite(pumpPins[i], HIGH);
            recheckMoisture[i] = recheckMoisture[i]+1;
        }
        else if ((pumpPins[i] % 2) == 0) {
            //moistureOutputs(i, SoilMoisturePercentage);
            //digitalWrite(pumpPins[i], LOW);
            //delay(smallPotTime);

            //digitalWrite(pumpPins[i], HIGH);
            recheckMoisture[i] = recheckMoisture[i]+1;
        }
      }
      else if(SoilMoisturePercentage < thresholds[i]) {
        //moistureOutputs(i, SoilMoisturePercentage);
        recheckMoisture[i] = 0;
        }
      else if(SoilMoisturePercentage >= upperLimit[i]) {
        //moistureOutputs(i, SoilMoisturePercentage);
        recheckMoisture[i] = 0;
        String message = "Warning! sensor " + String(i) + " indicates the plant is overwatered! Please reduce pump time";
        Serial.print(message);
        }
      if (recheckMoisture[i] >= 5) {
        String message = "Warning! sensor " + String(i) + " requested water 5 times in a row! Consider increasing water";
        Serial.print(message);
      }

    }
  }
}
