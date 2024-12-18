char data_r[320] = "00111111010111000001011000001101110100010100000001011000000101010010110000000001001111110101110000010110000011011101000101000000010110000001010100101100000000010011111101011100000101100000110111010001010000000101100000010101001011000000000100111111010111100001011000001101110100010100000001011000000101010010110000000001";

void sendData(char *data){
  for (int i=0;i<320;i++){
    if (data[i]=='0')
      PORTD &= ~(1<<PORTD1);
    else
      PORTD |= (1<<PORTD1);
    TCNT1 = 0;
    // while(TCNT1<2040){}
    while(TCNT1<2040){}
  }
  PORTD |= (1<<PORTD1);
}

void TIMER1_init(){
  TCCR1A = 0;
  TCCR1B = 0;//(1<<WGM12); // Clear Timer on Compare Match
  TCCR1B |= (1<<CS10);//|(1<<CS10); // set prescalar as 1
  TCCR1C = 0;
  TCNT1 = 0;
}

void TIMER0_init(){
  TCCR0A = 0;
  TCCR0B = 0;//(1<<WGM12); // Clear Timer on Compare Match
  TCCR0B |= (1<<CS02)|(1<<CS00); // set prescalar as 1024
  TCNT0 = 0;
}

void setup() { 
  DDRD |= 1<<DDD1; //set arduino pin 1 as output
  TIMER1_init();
  TIMER0_init();
  sei();
}

void loop() {
  sendData(data_r);
  for (uint8_t i=0; i<124; i++){
    //delay(16ms)
    TCNT0 = 0;
    while(TCNT0<249){}
  }
}
