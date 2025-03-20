#define FOSC 16000000
#define BAUD 9600
#define MYUBRR FOSC/16/BAUD-1

void USART_Init(unsigned int ubrr)
{
/*Set baud rate */
UBRR0H = (unsigned char)(ubrr>>8);
UBRR0L = (unsigned char)ubrr;
/*Enable receiver and transmitter */
UCSR0B = (1<<RXEN0)|(1<<TXEN0);
/* Set frame format: 8data, 2stop bit */
UCSR0C = (1<<USBS0)|(3<<UCSZ00);
}

void USART_Transmit(char b)
{
while (!(UCSR0A & (1<<UDRE0)));
UDR0 = b;
}

unsigned char USART_Receive(void)
{
/* Wait for data to be received */
while (!(UCSR0A & (1<<RXC0)));
/* Get and return received data from buffer */
return UDR0;
}

void USART_Flush()      //USART function to clear the data register
{
unsigned char dummy;
while (UCSR0A & (1<<RXC0)) dummy = UDR0;
}
 
void dc_pwm(int OP)
{
DDRD |= (1<<PD6)|(1<<PD4);    //Fast PWM output at OC0A pin: PD6
PORTD&=~(1<<4);
TCCR0A |= (1<<COM0A1) | (1<<WGM01) | (1<<WGM00);   //Non-Inverting Fast PWM mode 3 using OCRA unit
TCCR0B |= (1<<CS00);  //No-Prescalar
OCR0A =((255*OP)/100);
}

void dc_motor()
{
  int i,k,r;
  char q[2];
  char a[]="DC motor selected";
  char b[]="Enter the duty cycle";
USART_Transmit('\r');  USART_Transmit('\n');
  for(i=0;a[i]!='\0';i++) 
  {
    USART_Transmit(a[i]);
    _delay_ms(10);
  }
  USART_Flush();
  USART_Transmit('\r');  USART_Transmit('\n');
  for(i=0;b[i]!='\0';i++)
  {
    USART_Transmit(b[i]);
    _delay_ms(10);
  }
  USART_Flush();
  for(k=0;k<2;k++)
  {
    q[k]=USART_Receive();
    USART_Transmit(q[k]);
  }
  q[k]='\0';
  r=atoi(q);
  dc_pwm(r);
}

void step_motor()
{
  int i,r,k;
  char q[3];
  char c[]="stepper motor selected";
  char d[]="Enter the angle";
USART_Transmit('\r');  USART_Transmit('\n');
  USART_Flush();
  for(i=0;c[i]!='\0';i++)
  {
    USART_Transmit(c[i]);
    _delay_ms(10);
  }
  USART_Flush();
  USART_Transmit('\r');  USART_Transmit('\n');
  USART_Flush();
  for(i=0;d[i]!='\0';i++)
  {
    USART_Transmit(d[i]);
    _delay_ms(10);
  }
  USART_Flush();
  for(k=0;k<3;k++)
  {
    q[k]=USART_Receive();
    USART_Transmit(q[k]);
  }
  q[k]='\0';
  r=atoi(q);
  step_clock(r);
}

void step_clock(int r)
{
  int i;
  DDRB=(0X1e);
  PORTB&=~(0X1e);
  USART_Flush();

  for(i=0;i<(r/7.2);i++)
  {
    PORTB=(1<<0);
    _delay_ms(100);

    PORTB&=~(1<<0);
    _delay_ms(100);
    PORTB=(1<<1);
    _delay_ms(100);

    PORTB&=~(1<<1);
    _delay_ms(100);
    PORTB=(1<<2);
    _delay_ms(100);

     PORTB&=~(1<<2);
     _delay_ms(100);
    PORTB=(1<<3);
    _delay_ms(100);

    PORTB=~(1<<3);
    _delay_ms(100);
  }
} 

void step_anti(int r)
{
  int i;
  DDRB=(0X1e);
  PORTB&=~(0X1e);
  USART_Flush();

  for(i=0;i<(r/7.2);i++)
  {
    PORTB=(1<<3);
    _delay_ms(100);

    PORTB&=~(1<<3);
    _delay_ms(100);
    PORTB=(1<<2);
    _delay_ms(100);

    PORTB&=~(1<<2);
    _delay_ms(100);
    PORTB=(1<<1);
    _delay_ms(100);

     PORTB&=~(1<<1);
     _delay_ms(100);
    PORTB=(1<<0);
    _delay_ms(100);

    PORTB=~(1<<0);
    _delay_ms(100);
  }
} 

void manual_mode()
{
  int i;
   char a[]="enter the motor to select ";
   char b[]="Enter s for steppermotor d for dc motor ";
   char s[]="Do you want to continue any motor y/n: "; char t[]="No motors";
   char c,d;
   USART_Transmit('\r');  USART_Transmit('\n');
   USART_Flush();
   for(i=0;a[i]!='\0';i++)
  {
    USART_Transmit(a[i]);
    _delay_ms(10);
  }
  USART_Flush();
 l1:  USART_Transmit('\r');  USART_Transmit('\n');
 USART_Flush();
  for(i=0;b[i]!='\0';i++)
  {
    USART_Transmit(b[i]);
    _delay_ms(10);
  }
  USART_Flush();
   c=USART_Receive();
   USART_Transmit(c);
   if(c=='s')
   {
     step_motor(); 
   }
   if(c=='d')
   {
     dc_motor();  
   }
   USART_Transmit('\r');  USART_Transmit('\n');
   USART_Flush();
   for(i=0;s[i]!='\0';i++)
  {
    USART_Transmit(s[i]);
    _delay_ms(10);
  }USART_Flush();
  d=USART_Receive();
   USART_Transmit(d);
  if(d=='y')  goto l1;
  else
  {
    USART_Transmit('\r');  USART_Transmit('\n');
    USART_Flush();
    for(i=0;t[i]!='\0';i++)
  {
    USART_Transmit(t[i]);
    _delay_ms(10);
  }
  USART_Flush();
  }
}

void temperature_sensor()
{
  int i,x;
  char res[5];
  char a[]="threshold of temperature=26 c";
  char b[]="Temperature is:";
  USART_Transmit('\r');  USART_Transmit('\n');
  USART_Flush();
  for(i=0;a[i]!='\0';i++)
  {
     USART_Transmit(a[i]);
    _delay_ms(10);
  }USART_Flush();
  USART_Transmit('\r');  USART_Transmit('\n');
  USART_Flush();
  for(i=0;b[i]!='\0';i++)
  {
     USART_Transmit(b[i]);
    _delay_ms(10);
  }USART_Flush();
  ADMUX=(0x01);
ADMUX=(1<<6);  
ADCSRA=(7<<0); 
ADCSRA|=(1<<7);
ADCSRA|=(1<<6);  
while((ADCSRA & (1<<4))==0); 
x=ADC; 
x=((x*5*100)/1024);
sprintf(res,"%d",x);
  for(i=0;res[i]!='\0';i++)
  {
    USART_Transmit(res[i]);
  }
if(x>15)
{
  dc_pwm(90);  
}  
else
  dc_pwm(60);
}

void gas_sensore()
{
  int x,i;
  char res[5]; char a[]="Threshold of gas is 4%"; char b[]="Gas is : ";
   USART_Transmit('\r');  USART_Transmit('\n');
   USART_Flush();
  for(i=0;a[i]!='\0';i++)
  {
     USART_Transmit(a[i]);
    _delay_ms(10);
  }USART_Flush();
   USART_Transmit('\n');  USART_Transmit('\r');
   USART_Flush();
  for(i=0;b[i]!='\0';i++)
  {
     USART_Transmit(b[i]);
    _delay_ms(10);
  }USART_Flush();
  DDRB=(1<<5);
ADMUX=(1<<6);  
ADCSRA= (7<<0); 
ADCSRA|=(1<<7);
ADCSRA|=(1<<6);  
while((ADCSRA & (1<<4))==0); 
x=ADC; 
x=(x*100/1024);
sprintf(res,"%d",x);
USART_Flush();
 for(i=0;res[i]!='\0';i++)
  {
    USART_Transmit(res[i]);
  }
 if(x>4)
 {
   PORTB=(1<<5); 
 } 
}

void ir_sensore()
{
  char a[]="Object is detected! "; char b[]="Object is not Detected! "; int i;
  if((PINB & (1<<0))!=0)
  {
       USART_Transmit('\r');  USART_Transmit('\n');
       USART_Flush();
    for(i=0;a[i]!='\0';i++)
    {
      USART_Transmit(a[i]);
    }USART_Flush();
    step_clock(90);
  }
  
  if((PINB & (1<<0))==0)
  {
       USART_Transmit('\r');  USART_Transmit('\n');
       USART_Flush();
     for(i=0;b[i]!='\0';i++)
    {
      USART_Transmit(b[i]);
    }
    step_anti(90);  
  }
  
}

void active_mode()
{
  int i;
   char a[]="Auto mode is selected ";
   USART_Transmit('\r');  USART_Transmit('\n');
   USART_Flush();
   for(i=0;a[i]!='\0';i++)
  {
    USART_Transmit(a[i]);
    _delay_ms(10);
  }USART_Flush();
     temperature_sensor(); 
     gas_sensore();  
      ir_sensore();
}
int main()
{
  USART_Init(MYUBRR);
  int i,x,y;
  char p[]="Select a mode ";
  char q[]="Enter a for active motor m for manual mode "; char v[]="THANK YOU";
  char s[]="Do you want to continue any mode y/n: "; char t[]="No mode"; 
  char w[]="do you want to exit enter q or n "; 
  char r;
l1:  USART_Transmit('\r');  USART_Transmit('\n');
USART_Flush();
  for(i=0;p[i]!='\0';i++)
  {
    USART_Transmit(p[i]);
    _delay_ms(10);
  }USART_Flush();
  USART_Transmit('\r');  USART_Transmit('\n');
  USART_Flush();
l2:  for(i=0;q[i]!='\0';i++)
  {
    USART_Transmit(q[i]);
    _delay_ms(10);
  } USART_Flush();
  r=USART_Receive();
  USART_Transmit(r);
  if((r=='a')|(r=='A'))
  {
    active_mode();    
  }
  else if((r=='m')|(r=='M'))
  {
    manual_mode();  
  }
   USART_Transmit('\r');  USART_Transmit('\n');
   USART_Flush();
  for(i=0;s[i]!='\0';i++)
  {
    USART_Transmit(s[i]);
    _delay_ms(10);
  } USART_Flush();
  x=USART_Receive();
  USART_Transmit(x);
  if(x=='y')  goto l1;
  else 
    {
      USART_Transmit('\r');  USART_Transmit('\n');
      USART_Flush();
    for(i=0;t[i]!='\0';i++)
   {
    USART_Transmit(t[i]);
    _delay_ms(10);
   }USART_Flush();   
   }
    USART_Transmit('\r');  USART_Transmit('\n');
    USART_Flush();
  for(i=0;w[i]!='\0';i++)
  {
    USART_Transmit(w[i]);
    _delay_ms(10);
  }USART_Flush();
  y=USART_Receive();
  USART_Transmit(y);
  if(y=='q')
  {
     PORTB&=~(1<<5);
     OCR0A=0; 
      USART_Transmit('\r');  USART_Transmit('\n');
      USART_Flush();
     for(i=0;v[i]!='\0';i++)
      {
      USART_Transmit(v[i]);
    _delay_ms(10);
      }
   }
   USART_Flush();
   //else   goto l2;
}
