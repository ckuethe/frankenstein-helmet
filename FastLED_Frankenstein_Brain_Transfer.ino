/*
A "brain transfer" effect for a theatrical production of Young Frankenstein.

According to the script, the two characters are each wearing some sort of hat or
helmet connected to the machine by a large hose or cable. At the appropriate moment,
a switch is thrown, lightning flashes, and other things happen to indicate transfer
of consciousness. This effect takes between 10 and 20 seconds.

In this implementation, I use two strips of WS2812B LEDs with 60 LEDs per meter
contained inside 1/2" ID clear vinyl tubing. The tubing is attached at one end
to a 1/8" aluminum bulkhead, and to adjustable hard-hats on the other end with
waterproof compression penetrators (lemme tell ya, that was a pain to wire.)

The main processor is an ESP8266. It was the cheapest microcontroller I had in
stock. Perhaps future versions could use the WiFi for control, but this was
absolutely satisfactory for the production for which it was designed.
*/

#include <FastLED.h>

// How many LEDs are in the strip?
#define NUM_LEDS 300
// How many LEDs are inside of the hose
#define NUM_HOSE_LEDS 192
// How big is the chunk/group/block of LEDs that move?
// This has a large impact on animation speed: calling FastLED.show() once
// for each LED is a lot slower than setting 300 LEDs in memory and calling
// show() once. But maybe you want that delay to make the animation proceed
// at a particular pace
#define BLKSZ 15

// There are two strings of LEDs, one for the side connected to
// the doctor and one for the side connected to the monster.
#define DOCTOR_DATA_PIN D1
#define MONSTER_DATA_PIN D2

// A button to trigger the effect. This could be triggered by Igor throwing
// a big switch on stata
#define GO_BUTTON_PIN D5

// FastLED is pretty smart. I could break this up a lot of different ways:
// one big string, two strings, two strings with different zones on each string...
CRGB G_DOCTOR_LEDS[NUM_LEDS];
CRGB G_MONSTER_LEDS[NUM_LEDS];

// What color to make the LEDs for each character
CRGB G_DOCTOR_COLOR = CRGB::Purple;
CRGB G_MONSTER_COLOR = CRGB::Green;

// used to index into the LED string
int ctr;

// set to false if you want this to just start going as soon as power is applied
bool G_WAIT_FOR_GO_BUTTON = true;

void IRAM_ATTR go_button_isr() {
  G_WAIT_FOR_GO_BUTTON = false;
}

// Kind of like FastLED.showColor() but only for one helmet. This function only
// sets the element in the LED array, does not actually transmit the color command
void fill_helmet(bool doctor) {
  CRGB color = doctor ? G_DOCTOR_COLOR : G_MONSTER_COLOR;
  CRGB *target = doctor ? G_DOCTOR_LEDS : G_MONSTER_LEDS;
  for (ctr = NUM_HOSE_LEDS; ctr < NUM_LEDS; ctr++) {
    target[ctr] = color;
  }
}

// Set the helmet colors to start the effect
void helmet_init() {
  fill_helmet(true);
  fill_helmet(false);
  FastLED.show();
}

#define DTIM 1
#ifdef USE_PACKETS
// visualize the brain transfer by sending a packet through the hose, like a cylon flasher
void send_monster_packet() {
  Serial.printf("[%6d] Starting monster packet\r\n", millis());
  // Send a packet from the monster
  for (int blk = (NUM_HOSE_LEDS / BLKSZ) - 1; blk >= 0; blk--) {
    for (ctr = 0; ctr < BLKSZ; ctr++) {
      G_MONSTER_LEDS[blk * BLKSZ + ctr] = G_MONSTER_COLOR;
    }

    FastLED.show();
    // delay(DTIM);

    for (ctr = 0; ctr < BLKSZ; ctr++) {
      G_MONSTER_LEDS[blk * BLKSZ + ctr] = CRGB::Black;
    }
  }

  // delay(DTIM);

  for (int blk = 0; blk < (NUM_HOSE_LEDS / BLKSZ) - 1; blk++) {
    for (ctr = 0; ctr < BLKSZ; ctr++) {
      G_DOCTOR_LEDS[blk * BLKSZ + ctr] = G_MONSTER_COLOR;
    }

    FastLED.show();
    // delay(DTIM);

    for (ctr = 0; ctr < BLKSZ; ctr++) {
      G_DOCTOR_LEDS[blk * BLKSZ + ctr] = CRGB::Black;
    }
  }
  Serial.printf("[%6d] Finished monster packet\r\n", millis());
}

void send_doctor_packet() {
  Serial.printf("[%6d] Starting doctor packet\r\n", millis());
  // Send a packet from the monster
  for (int blk = (NUM_HOSE_LEDS / BLKSZ) - 1; blk >= 0; blk--) {
    for (ctr = 0; ctr < BLKSZ; ctr++) {
      G_DOCTOR_LEDS[blk * BLKSZ + ctr] = G_DOCTOR_COLOR;
    }

    FastLED.show();
    delay(DTIM);

    for (ctr = 0; ctr < BLKSZ; ctr++) {
      G_DOCTOR_LEDS[blk * BLKSZ + ctr] = CRGB::Black;
    }
  }

  delay(DTIM);

  for (int blk = 0; blk < (NUM_HOSE_LEDS / BLKSZ) - 1; blk++) {
    for (ctr = 0; ctr < BLKSZ; ctr++) {
      G_MONSTER_LEDS[blk * BLKSZ + ctr] = G_DOCTOR_COLOR;
    }

    FastLED.show();
    delay(DTIM);

    for (ctr = 0; ctr < BLKSZ; ctr++) {
      G_MONSTER_LEDS[blk * BLKSZ + ctr] = CRGB::Black;
    }
  }
  Serial.printf("[%6d] Finished doctor packet\r\n", millis());
}
#endif

// visualize the brain transfer by sending a flow. the tube fills up, then drains into the other helmet
void send_doctor_flow() {
  Serial.printf("[%6d] Starting doctor flow\r\n", millis());

  // fill up the doctor hose
  for (int blk = (NUM_HOSE_LEDS / BLKSZ) - 1; blk >= 0; blk--) {
    for (ctr = 0; ctr < BLKSZ; ctr++) {
      G_DOCTOR_LEDS[blk * BLKSZ + ctr] = G_DOCTOR_COLOR;
    }
    FastLED.show();
    //    delay(DTIM);
  }

  // delay(DTIM);
  // fill up the monster hose
  for (int blk = 0; blk < (NUM_HOSE_LEDS / BLKSZ) - 1; blk++) {
    for (ctr = 0; ctr < BLKSZ; ctr++) {
      G_MONSTER_LEDS[blk * BLKSZ + ctr] = G_DOCTOR_COLOR;
    }

    FastLED.show();
    // delay(DTIM);
  }

  // clear the doctor hose
  for (int blk = (NUM_HOSE_LEDS / BLKSZ) - 1; blk >= 0; blk--) {
    for (ctr = 0; ctr < BLKSZ; ctr++) {
      G_DOCTOR_LEDS[blk * BLKSZ + ctr] = CRGB::Black;
    }
    FastLED.show();
    //    delay(DTIM);
  }

  // delay(DTIM);
  // clear  the monster hose
  for (int blk = 0; blk < (NUM_HOSE_LEDS / BLKSZ) - 1; blk++) {
    for (ctr = 0; ctr < BLKSZ; ctr++) {
      G_MONSTER_LEDS[blk * BLKSZ + ctr] = CRGB::Black;
    }

    FastLED.show();
    // delay(DTIM);
  }
  Serial.printf("[%6d] Finished doctor flow\r\n", millis());
}

void send_monster_flow() {
  Serial.printf("[%6d] Starting monster flow\r\n", millis());

  // fill up the monster hose
  for (int blk = (NUM_HOSE_LEDS / BLKSZ) - 1; blk >= 0; blk--) {
    for (ctr = 0; ctr < BLKSZ; ctr++) {
      G_MONSTER_LEDS[blk * BLKSZ + ctr] = G_MONSTER_COLOR;
    }
    FastLED.show();
    //    delay(DTIM);
  }

  // delay(DTIM);
  // fill up the monster hose
  for (int blk = 0; blk < (NUM_HOSE_LEDS / BLKSZ) - 1; blk++) {
    for (ctr = 0; ctr < BLKSZ; ctr++) {
      G_DOCTOR_LEDS[blk * BLKSZ + ctr] = G_MONSTER_COLOR;
    }

    FastLED.show();
    // delay(DTIM);
  }

  // clear the doctor hose
  for (int blk = (NUM_HOSE_LEDS / BLKSZ) - 1; blk >= 0; blk--) {
    for (ctr = 0; ctr < BLKSZ; ctr++) {
      G_MONSTER_LEDS[blk * BLKSZ + ctr] = CRGB::Black;
    }
    FastLED.show();
    //    delay(DTIM);
  }

  delay(DTIM);
  // clear  the monster hose
  for (int blk = 0; blk < (NUM_HOSE_LEDS / BLKSZ) - 1; blk++) {
    for (ctr = 0; ctr < BLKSZ; ctr++) {
      G_DOCTOR_LEDS[blk * BLKSZ + ctr] = CRGB::Black;
    }

    FastLED.show();
    // delay(DTIM);
  }
  Serial.printf("[%6d] Finished monster flow\r\n", millis());
}

// All kinds of flashy business, then fading out
void helmet_finale() {
  static int pat_num = 0;
  int nleds[3][2] = {
    { NUM_HOSE_LEDS, 44 },
    { NUM_HOSE_LEDS + 44, 37 },
    { NUM_HOSE_LEDS + 44 + 37, 27 }
  };
  CRGB colors[] = { G_MONSTER_COLOR, G_DOCTOR_COLOR , CRGB::White};  //, CRGB::White };
  int ncolors = sizeof(colors) / sizeof(colors[0]);

  for (ctr = 0; ctr < 5; ctr++) {
    for (int c = 0; c < ncolors; c++) {
      FastLED.showColor(colors[c]);
      // FastLED.show();
      delay(100);
    }
  }

  // Fade out
  for (int brt = 255; brt >= 0; brt -= 16) {
    FastLED.showColor(CRGB::Green, brt);
    delay(50);
  }

  FastLED.showColor(CRGB::Black, 0);
  for (ctr = 0; ctr < NUM_LEDS; ctr++) {
    G_DOCTOR_LEDS[ctr] = CRGB::Black;
    G_MONSTER_LEDS[ctr] = CRGB::Black;
  }
  FastLED.show();
}

// there are three rows/wraps of LED tape around each helmet. They have different
// number in each row so here's a helper to set the row properly.
void helmet_set_row(bool doctor, int row, CRGB color) {
  int nleds[3][2] = {
    { NUM_HOSE_LEDS, 44 },
    { NUM_HOSE_LEDS + 44, 37 },
    { NUM_HOSE_LEDS + 44 + 37, 27 }
  };

  CRGB *helmet = doctor ? G_DOCTOR_LEDS : G_MONSTER_LEDS;

  for (ctr = nleds[row][0]; ctr < (nleds[row][0] + nleds[row][1]); ctr++) {
    helmet[ctr] = color;
  }
}


// And here we goooooooooooooo!
void setup() {
  Serial.begin(115200);

  Serial.println("\nCreating LEDs\r\n");
  // Hook up all the LED strings
  FastLED.addLeds<WS2812B, MONSTER_DATA_PIN, GRB>(G_MONSTER_LEDS, NUM_LEDS);
  FastLED.addLeds<WS2812B, DOCTOR_DATA_PIN, GRB>(G_DOCTOR_LEDS, NUM_LEDS);

  Serial.println("Turning off LEDs\r\n");
  // And turn off all the lights
  FastLED.showColor(CRGB::Black, 0);
  FastLED.show();
  FastLED.setBrightness(255);  // 0-255

  // Create the waiting blinkylight
  pinMode(LED_BUILTIN, OUTPUT);

  // Create the go button...
  pinMode(GO_BUTTON_PIN, INPUT_PULLUP);

  Serial.println("Waiting\r\n");
  // And wait for someone to push the button
  if (G_WAIT_FOR_GO_BUTTON)
    attachInterrupt(GO_BUTTON_PIN, go_button_isr, FALLING);

  int now, last_toggle = 0;
  while (G_WAIT_FOR_GO_BUTTON) {
    now = millis();
    if (abs(now - last_toggle) > 100) {
      last_toggle = now;
      digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    }
    delay(50);
  }

  Serial.println("Go!\r\n");
  // don't need the interrupt any more
  detachInterrupt(GO_BUTTON_PIN);

  // and turn off the waiting light
  digitalWrite(LED_BUILTIN, LOW);

  Serial.println("helmet_init()\r\n");
  // light up the helmets
  helmet_init();
  // delay(1000);
}


void loop() {
  static int phase = 0;
  unsigned int snoozle = 0, default_snoozle = 250;
  static int doctor_row = 2;
  static int monster_row = 2;

  // conveniently I repeat five steps, three times, so I can use this switch statement
  // to call the right routine. The 16th stage is the finale, and then the program drops
  // into a do-nothing section from which there is no escape.
  if (phase < 42)
    Serial.printf("loop start, phase=%d dr=%d mr=%d\r\n", phase, doctor_row, monster_row);
  snoozle = default_snoozle;
  switch (phase) {
    case 0:
    case 5:
    case 10:
      helmet_set_row(true, doctor_row, CRGB::Black);
      break;

    case 1:
    case 6:
    case 11:
      send_doctor_flow();
      break;

    case 2:
    case 7:
    case 12:
      helmet_set_row(false, monster_row, G_DOCTOR_COLOR);
      monster_row--;
      break;

    case 3:
    case 8:
    case 13:
      send_monster_flow();
      break;

    case 4:
    case 9:
    case 14:
      helmet_set_row(true, doctor_row, G_MONSTER_COLOR);
      doctor_row--;
      break;

    case 15:
      FastLED.showColor(CRGB::Black, 0);
      Serial.printf("finale\r\n");
      helmet_finale();
      break;

    default:
      Serial.printf("End of effect: timing %fs\r\n", millis() / 1000);
      phase = 42;
      break;
  }

  if (doctor_row < 0)
    doctor_row = 0;
  if (monster_row < 0)
    monster_row = 0;

  if (phase < 42)
    Serial.printf("loop end, phase=%d dr=%d mr=%d\r\n", phase, doctor_row, monster_row);

  FastLED.show();
  delay(snoozle);
  phase++;
}  // loop()
