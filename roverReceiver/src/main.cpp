#include <Arduino.h>
#include <SPI.h>
#include <RH_RF69.h>
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define PIN 5

#define RF69_FREQ 900.0

  #define RFM69_CS      8
  #define RFM69_INT     3
  #define RFM69_RST     4
  #define LED           13

RH_RF69 rf69(RFM69_CS, RFM69_INT);

Adafruit_NeoPixel strip = Adafruit_NeoPixel(3, PIN, NEO_RGBW + NEO_KHZ800);

void blink(char color, unsigned int waitTime);

void setup() {
  Serial.begin(115200);
  //while(!Serial);

  pinMode(LED, OUTPUT);
  pinMode(RFM69_RST, OUTPUT);
  digitalWrite(RFM69_RST, LOW);

    // manual reset
  digitalWrite(RFM69_RST, HIGH);
  delay(10);
  digitalWrite(RFM69_RST, LOW);
  delay(10);

  if (!rf69.init()) {
    //Serial.println("RFM69 radio init failed");
    while (1);
  }
  Serial.println("RFM69 radio init OK!");
  if (!rf69.setFrequency(RF69_FREQ)) {
    //Serial.println("setFrequency failed");
  }
  rf69.setTxPower(20, true);  // range from 14-20 for power, 2nd arg must be true for 69HCW
  uint8_t key[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
  rf69.setEncryptionKey(key);

  pinMode(LED, OUTPUT);

  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
}
void loop(){
  String word, xcoord, ycoord;
  char temp[20];
  int i = 0, xcoordint, ycoordint;
  if (rf69.available()) {
      // Should be a message for us now
      uint8_t buf[RH_RF69_MAX_MESSAGE_LEN];
      uint8_t len = sizeof(buf);
      if (rf69.recv(buf, &len)) {
        if (!len) return;
        buf[len] = 0;
        //Serial.print("Received [");
        //Serial.print(len);
        //Serial.print("]: ");
        Serial.println((char*)buf);
        //temp = (char) buf;
        for(int i = 0; i<20; i++){
          temp[i] = (char) buf[i];
        }
        word = temp;
        while(word.charAt(i) != '*'){
          if(word.charAt(i) >= '0' && word.charAt(i) <= '9') xcoord += word.charAt(i);
          i++;
        }
        i++;
        while(word.charAt(i) != '*'){
          if(word.charAt(i) >= '0' && word.charAt(i) <= '9') ycoord += word.charAt(i);
          i++;
        }
        /*for(int i = 0; i<5; i++){
          if(word.charAt(i) >= '0' && word.charAt(i) <= '9') xcoord += word.charAt(i);
        }
        for(int i = 5; i<10; i++){
          if(word.charAt(i) >= '0' && word.charAt(i) <= '9') ycoord += word.charAt(i);
        }*/
        //xcoord = word.substring(0, 4);
        //ycoord = word.substring(8, 13);
        xcoordint = xcoord.toInt();
        ycoordint = ycoord.toInt();
        Serial.print(xcoordint); Serial.print(", "); Serial.println(ycoordint);
        //Serial.println(word);
        //Serial.print(", ");
        //Serial.print("RSSI: ");
        //Serial.println(rf69.lastRssi(), DEC);
        //blink('r', (unsigned int) 200);
        digitalWrite(13, HIGH);
      } else {
        Serial.println("Receive failed");
      }
   }
   else digitalWrite(13, LOW);
}

void blink(char color, unsigned int waitTime){

  uint32_t c = 0;

  if(color == 'r') c = strip.Color(0, 255, 0);  //red
  else if(color == 'w') c = strip.Color(0, 0, 0, 255);  //white
  else if(color == 'b') c = strip.Color(0, 0, 255);  //blue
  else if(color == 'g') c = strip.Color(255, 0, 0);  //green

  for(unsigned int i=0; i<strip.numPixels(); i++){
    strip.setPixelColor(i, c);
    strip.show();
    //delay(50);
  }

  delay(waitTime);

  for(unsigned int i=0; i<strip.numPixels(); i++){
    strip.setPixelColor(i, strip.Color(0, 0, 0));
    strip.show();
    //delay(50);
  }
}
