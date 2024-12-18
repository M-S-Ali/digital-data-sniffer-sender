#define samplingTime 2065
#define initialDelay samplingTime/2
#define dataLength 320

enum state {SAMPLING, PRINTING, IDLE};

volatile state systemState = IDLE;
volatile uint16_t bitCount = 0;
volatile uint8_t data[dataLength/8] = {0};


void setup() {
  Serial.begin(9600);
  EINT0_init();
  TIMER1_init();
  sei(); // enable all global interrupts
}

void loop() {
  if(systemState == PRINTING){
    for (int i=0; i < dataLength/8; i++){
      // Serial.print(data[i/8]&(1<<(7-(i%8)))?1:0);
      Serial.print(data[i],HEX);
      Serial.print(" ");
    }
    Serial.println();
    bitCount = 0;
    for (int i=0; i < dataLength/8; i++)
      data[i] = 0;
    EIMSK |= (1 << INT0); // enable external interrupt
    systemState = IDLE;
  }
}

void EINT0_init(){
  // ensure initialisation
  EICRA = 0; // ensure external interrupt control register A is set to 0
  EIMSK = 0; // ensure external interrupt mask register is set to 0
  DDRD &= ~(1 << DDD2); // ensure PIN D2 is set as input for interrupt

  EICRA |= (1 << ISC01); // SET External Interrupt 0 Sense Control bit 1 to enable falling edge detection
  EIMSK |= (1 << INT0);  // SET external interrupt 0 mask to 1 to enable interrupt
  PORTD |= (1 << PORTD2); // SET PORTD PIN 2 for internal pull-up
}

void TIMER1_init(){
  // ensure initialisation
  TCCR1A = 0; // ensure Timer/Counter Control Register1 A is set to 0
  TCCR1B = 0; // ensure Timer/Counter Control Register1 B is set to 0
  TCCR1C = 0; // ensure Timer/Counter Control Register1 C is set to 0

  TCCR1B |= (1 << WGM12); // SET wave generation mode to Clear compare match output to clear on compare match (CTC)
  TCCR1B |= (1 << CS10); // SET clock select bit to set prescalar as 1
  DDRB |= (1 << DDB1); // SET Data Direction Register of Port B pin 1 (Arduino pin D9) to set as output
  OCR1A = samplingTime; // Set Output Compare Register for sampling time
}

ISR(INT0_vect){
  if(systemState == IDLE){
    TCNT1 = 0;
    while (TCNT1 <= initialDelay){}
    TCNT1 = 0;
    TCCR1A |= (1 << COM1A0); // SET compare output mode 1 for channel A to toggle for compare match
    TIMSK1 |= (1 << OCIE1A); // enable timer interrupt
    EIMSK &= ~(1 << INT0); // disable external interrupt
    systemState = SAMPLING;
  }
}

ISR(TIMER1_COMPA_vect){
  if(systemState == SAMPLING){
    if(bitCount<dataLength){
      if (PIND & (1 << PIND2))
        data[bitCount/8] |= (1 << (7 - (bitCount % 8)));
      bitCount++;
    } else if(bitCount == dataLength){
      TIMSK1 &= ~(1 << OCIE1A);
      TCCR1A &= ~(1 << COM1A0); // SET compare output mode 1 for channel A to toggle for compare match
      systemState = PRINTING;
    }
  }
}
