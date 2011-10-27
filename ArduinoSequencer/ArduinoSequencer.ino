// Names for MUX Control Pins
#define CONTROL0 5    
#define CONTROL1 4
#define CONTROL2 3
#define CONTROL3 2

// mux0array
// 0-7: 10k pots as Analog In
// 8-9: Rotary Encoder
int mux0array[16];
// mux1array
int mux1array[16];
// mux2array :: DIGITAL IN
// 0-3: buttons
// 4-6: 
int mux2array[16];

int tones[] = {31,31,33,35,37,39,41,44,46,49,52,55,58,62,65,69,73,78,82,87,93,98,104,110,117,123,131,139,147,156,165,175,185,196,208,220,233,247,262,277,294,311,330,349,370,392,415,440,466,494,523,554,587,622,659,698,740,784,831,880,932};

// Selector switch
int pinState1 = 0;
int pinState2 = 0;
int pinState3 = 0;
int lastSelect = 0;
int switchPos = 0;

void setup()
{
  // MUX Control Pins need to be outputs
  pinMode(CONTROL0, OUTPUT);
  pinMode(CONTROL1, OUTPUT);
  pinMode(CONTROL2, OUTPUT);
  pinMode(CONTROL3, OUTPUT);
  pinMode(16, INPUT);
  digitalWrite(16, HIGH);
  
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
    
    tone(15,tones[((switchPos-1)*12)+int(map(mux0array[i],0,1023,0,11))],100);
    delay(100);
  }
  
  // **************************
  // Selector
  // **************************
  pinMode(16, INPUT);
  digitalWrite(16,LOW);
  addressPin(4);
  pinState1 = digitalRead(16);
  addressPin(5);
  pinState2 = digitalRead(16);
  addressPin(6);
  pinState3 = digitalRead(16);
  if(pinState1 == HIGH){
    if(pinState2 == HIGH){switchPos = 2;}
    else {switchPos = 1;}
  }  
  else if(pinState2 == HIGH)
  {
    if(pinState1 == HIGH){switchPos = 2;}
    else if(pinState3 == HIGH){switchPos = 4;}
    else {switchPos = 3;}
  }
  else if(pinState3 ==HIGH)
  {
    
    if(pinState2 == HIGH){switchPos = 4;}
    else {switchPos = 5;}
  }
    
  //Now that we know which position our switch is in, let's print it to the terminal.
  //But first, we'll make sure the switch has moved
  if(switchPos != lastSelect)
  {
    Serial.print("Switch Position:");
    Serial.println(switchPos);
  }      
  //Now remember that for the next sweep.
  lastSelect = switchPos;

  // buttons
  Serial.print("Buttons ");
  for(int i=0; i<4; i++)
  {
    addressPin(i);
    mux2array[i] = digitalRead(16);
    Serial.print(mux2array[i]);
    Serial.print("-");
  }
  Serial.println();
}

void muxWrite(int mux, int muxPin)
{
  switch(mux){
    case 0:
      pinMode(14, OUTPUT);
      pinMode(15, INPUT);
      break;
    case 1:
      pinMode(14, INPUT);
      pinMode(15, OUTPUT);
    break;
    default:
      pinMode(14, INPUT);
      pinMode(15, INPUT);
  }
  
  addressPin(muxPin);
}

void readMux(int muxarray[], int pinNum)
{
  //This for loop is used to scroll through and store the 16 inputs on the FIRST multiplexer
  for (int i=0; i<16; i++)
  {
    addressPin(i);    

    //Read and store the input value at a location in the array
    muxarray[i] = analogRead(pinNum);
  }
}

void addressPin(int p)
{
  digitalWrite(CONTROL0, (p&15)>>3); 
  digitalWrite(CONTROL1, (p&7)>>2);  
  digitalWrite(CONTROL2, (p&3)>>1);  
  digitalWrite(CONTROL3, (p&1)); 
}
