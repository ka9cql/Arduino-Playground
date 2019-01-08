/*
 * IRrecvDump - Display which button on the IR Remote was pressed
 */

#include <IRremote.h>

#define IR_KEY_IGNORE      0x00
#define IR_KEY_REPEAT      0x01
#define IR_KEY_CH_MINUS    0x0a
#define IR_KEY_CH          0x0b
#define IR_KEY_CH_PLUS     0x0c
#define IR_KEY_PREVIOUS    0x0d
#define IR_KEY_NEXT        0x0e
#define IR_KEY_PLAY_PAUSE  0x0f
#define IR_KEY_VOLUME_DOWN 0x10
#define IR_KEY_VOLUME_UP   0x11
#define IR_KEY_EQ          0x12
#define IR_KEY_100_PLUS    0x13
#define IR_KEY_200_PLUS    0x14

typedef struct key_arr_s {
  long int keycode;
  int return_code;
} KEY_ARR;

// "NEC"-type remote button definitions -
KEY_ARR key_array []= {
  { 0xFF6897, '0' },
  { 0xFF30CF, '1' },
  { 0xFF18E7, '2' },
  { 0xFF7A85, '3' },
  { 0xFF10EF, '4' },
  { 0xFF38C7, '5' },
  { 0xFF5AA5, '6' },
  { 0xFF42BD, '7' },
  { 0xFF4AB5, '8' },
  { 0xFF52AD, '9' },
  { 0xFFA25D, IR_KEY_CH_MINUS },
  { 0xFF629D, IR_KEY_CH },
  { 0xFFE21D, IR_KEY_CH_PLUS },
  { 0xFF22DD, IR_KEY_PREVIOUS },
  { 0xFF02FD, IR_KEY_NEXT },
  { 0xFFC23D, IR_KEY_PLAY_PAUSE },
  { 0xFFE01F, IR_KEY_VOLUME_DOWN },
  { 0xFFA857, IR_KEY_VOLUME_UP },
  { 0xFF906F, IR_KEY_EQ },
  { 0xFF9867, IR_KEY_100_PLUS },
  { 0xFFB04F, IR_KEY_200_PLUS }
};



/* 
*  Your IR receiver should be connected to the pin defined here
*/
int RECV_PIN = 13;

IRrecv irrecv(RECV_PIN);

decode_results results;

void setup()
{
  Serial.begin(9600);
  irrecv.enableIRIn(); // Start the receiver
}


int irCheck(decode_results *results) {
  
  // Call this after IRrecv::decode()
  int retVal=IR_KEY_IGNORE;
  int i=0;
  int MAX_LOOP=(sizeof(key_array)/sizeof(KEY_ARR));
  
  int count = results->rawlen;
  if (results->decode_type != NEC) {
    Serial.print("UNKNOWN REMOTE TYPE!!");

    Serial.print(results->value, HEX);
    Serial.print(" (");
    Serial.print(results->bits, DEC);
    Serial.println(" bits)");
    //Serial.print("Raw (");
    Serial.println(count, DEC);
    //Serial.print("): ");
  
  #if 0
    for (int i = 1; i < count; i++) {
      if (i & 1) {
        Serial.print(results->rawbuf[i]*USECPERTICK, DEC);
      }
      else {
        Serial.write('-');
        Serial.print((unsigned long) results->rawbuf[i]*USECPERTICK, DEC);
      }
      Serial.print(" ");
    }
  #endif
  
    Serial.println();
  }

  Serial.print("key_array size: ");
  Serial.println(MAX_LOOP, DEC);
  for (i=0; i < MAX_LOOP; i++)
  {
    if (key_array[i].keycode == results->value)
    {
      retVal=key_array[i].return_code;
      Serial.print("Found key: 0x");
      Serial.print(retVal, HEX);
      Serial.println("h");
      return(retVal);
    }
  }

  switch(results->value)
  {
  
    case 0xFFFFFFFF:
      // "Repeat"
      Serial.println("REPEAT LAST KEY!");
      retVal=IR_KEY_REPEAT;
    break;

    default:
      // Unknown
    break;
  }

  return retVal;
}

void loop() {
  if (irrecv.decode(&results)) {
    if (irCheck(&results) == IR_KEY_IGNORE)
    {
      Serial.println("UNKNOWN KEY!");
    }
    irrecv.resume(); // Receive the next value
  }
}
