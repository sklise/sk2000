// Names for MUX Control Pins
#define CONTROL0 5    
#define CONTROL1 4
#define CONTROL2 3
#define CONTROL3 2

// Arrays for data from the MUX
int mux0array[16];
// mux0array
// 0-7: 10k pots as Analog In
// 8-9: Rotary Encoder
int mux1array[16];
int mux2array[16];

int tones1[8];

void setup()
{
  // MUX Control Pins need to be outputs
  pinMode(CONTROL0, OUTPUT);
  pinMode(CONTROL1, OUTPUT);
  pinMode(CONTROL2, OUTPUT);
  pinMode(CONTROL3, OUTPUT);
  
  
  Serial.begin(9600);
}

void loop()
{  
  readMux(mux0array,0);

  //The following lines are for printing out results of array0
  Serial.print("mux0array: ");
  for (int i=0; i<16; i++)
  {
    Serial.print(mux0array[i]);
    Serial.print("-");
  }
  Serial.println();  //line feed
  
  //This for loop is used to scroll through the SECOND multiplexer
  for (int i=0; i<8; i++)
  {
    muxWrite(1,i);
    digitalWrite(15, HIGH);
    delay(100);
    digitalWrite(15, LOW);
    int j = i+8;
    muxWrite(1,j);
    tone(15,mux0array[i],100);
    delay(100);
  }
  
}

void muxWrite(int mux, int muxPin)
{
  switch(mux){
    case 0:
      pinMode(14, OUTPUT);
      pinMode(15, INPUT);
      pinMode(16, INPUT);
      break;
    case 1:
      pinMode(14, INPUT);
      pinMode(15, OUTPUT);
      pinMode(16, INPUT);    
    break;
      pinMode(14, INPUT);
      pinMode(15, INPUT);
      pinMode(16, OUTPUT);  
    default:      
  }
  
  digitalWrite(CONTROL0, (muxPin&15)>>3); //S3
  digitalWrite(CONTROL1, (muxPin&7)>>2);  //S2
  digitalWrite(CONTROL2, (muxPin&3)>>1);  //S1
  digitalWrite(CONTROL3, (muxPin&1));     //S0
}

void readMux(int muxarray[], int pinNum)
{
  //This for loop is used to scroll through and store the 16 inputs on the FIRST multiplexer
  for (int i=0; i<16; i++)
  {
    //The following 4 commands set the correct logic for the control pins to select the desired input
    //See the Arduino Bitwise AND Reference: http://www.arduino.cc/en/Reference/BitwiseAnd
    //See the Aruino Bitshift Reference: http://www.arduino.cc/en/Reference/Bitshift
    digitalWrite(CONTROL0, (i&15)>>3); 
    digitalWrite(CONTROL1, (i&7)>>2);  
    digitalWrite(CONTROL2, (i&3)>>1);  
    digitalWrite(CONTROL3, (i&1));     
    
    //Read and store the input value at a location in the array
    muxarray[i] = analogRead(pinNum);
  }
}
