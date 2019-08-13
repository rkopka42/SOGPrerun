/* SOG Prerun
 *  
 * Lüfter für eine Weile laufen lassen nach Aktivierung. Dazu mit LED die Zeit darstellen
 * 
 * 01 - Farb LED
 * 02 - Pulslänge einer LED
 * 03 - Pulslänge, auf INT umstellen
 * 04 - Messung Motorspannung
 * 
 */

//#define ATTINY

#ifdef ATTINY
 #define LED_RT 4 // PB4 Pin3
 #define SOG 1    // PB1  Pin6
 #define INP 2    // A1 Pin7 PB2
#else
 #define LED_RT 11
 #define SOG 3
 #define INP 4
#endif

// jetzt nicht mehr zu Farben zugeordnet
#define TIME_RTGN 10000
#define TIME_GN   10000
#define TIME_BL   60000

static unsigned long next_LED=0;
static unsigned long next_state=0;
unsigned long now_;
bool          last_LED_on=false;
unsigned long led_on=100;
unsigned long led_off=500;

void setup() 
{
  #ifndef ATTINY
  //Serial.begin(115200);
  #endif
  
  pinMode(LED_RT, OUTPUT);
  pinMode(SOG, OUTPUT);
  pinMode(INP, INPUT_PULLUP);

  digitalWrite(SOG,0);
  digitalWrite(LED_RT,0);

  now_=millis();
  next_state = TIME_RTGN + now_;
}

void loop() 
{
  static int state=0;
  bool eing;
  
  now_ = millis();
  eing = !digitalRead(INP); // wegen Taster mit PullUp, sonst nicht invertiert. Analoge Messung nicht wichtig, soweit

 // Serial.print(eing);
 // Serial.println();
        
  switch(state)
  {
    case 0:
      if (eing)   // kein Einschalten
      {
        led_on  = 1;
        led_off = 4000; 
        state = 4; 
      }
      else
      {
        //SOG Messung fertig
        digitalWrite(SOG,1);
        state = 1;
      }
      break;
      
    case 1:
      if (now_ > next_state)  // lang genug her
      {
        led_on  = 2000;
        led_off = 1; 
        state = 2; 
        next_state = TIME_GN + now_;
      }
      else
      {
        unsigned long delta;
        unsigned long factor;
        
        delta = (next_state - now_);
        factor = delta * 1000 / TIME_RTGN;
        led_on  = (1000-factor) * 500 / 1000;
        led_off = (factor/2)    * 500 / 1000 + 100; 
      }
      break;
      
    case 2:
      if (now_ > next_state)  // lang genug her
      {
        led_on  = 100;
        led_off = 1900;
        state = 3; 
        next_state = TIME_BL + now_;
        digitalWrite(SOG,0);
      }
      break;
      
    case 3:    
      if (now_ > next_state or !eing)  // lang genug her oder SOG schon aus
      {
        led_on  = 1;
        led_off = 4000; 
        state = 4; 
      }
      break;
    
    case 4:   // ab hier nichts mehr machen außer dem LED Blinken unten
    default:
      break;
  }
   
  // LED Pulsen
  if (next_LED < millis())
  {
    if (last_LED_on)
    {
      digitalWrite(LED_RT, LOW);   // turn the LED on (HIGH is the voltage level)
      next_LED = millis() +  led_off;
    }
    else
    {
      digitalWrite(LED_RT, HIGH);    // turn the LED off by making the voltage LOW
      next_LED = millis() + led_on;
    }
    last_LED_on = !last_LED_on;
  }
} // Ende loop()
