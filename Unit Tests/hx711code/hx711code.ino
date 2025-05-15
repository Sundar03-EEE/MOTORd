#include <HX711.h>
#define DT_pin 2
#define SCK_pin 3
HX711 scale;
float calibration_factor= 139200;

void setup() {
  Serial.begin(115200);  
  scale.begin(DT_pin, SCK_pin);
    if (!scale.is_ready()) {
        Serial.println("HX711 not ready! Check wiring.");
        while (1);
    }
    
    Serial.println("Taring... Please wait.");
     scale.set_gain(128);
     scale.tare();// Reset the scale to zero
     
    Serial.println("Tare complete. Place a known weight.");

}

void loop() {
  long raw = scale.get_units(20);  // Take 10 readings and average
   float weight=raw/calibration_factor;
    if (weight < 0) {  // ✅ Fix Negative Values
       weight *= -1;
    }
          Serial.print("Raw Value: ");
         Serial.println(raw);
       Serial.print("weight in kg: ");
        Serial.println(weight);
    delay(500);
  

}
