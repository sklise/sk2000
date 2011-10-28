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
int noteDuration[] = {200,200,200,200,200,200,200,200};
int noteRest[] = {50,50,50,50,50,50,50,50};

// Selector switch
int pinState1 = 0;
int pinState2 = 0;
int pinState3 = 0;
int lastSelect = 0;
int switchPos = 0;
int noteStep = 0;
boolean resetButton = false;
boolean backwards = false;
boolean programMode = false;

int notePressed = 0;
int prevPressed = 0;
int noteStart = 0;
int noteEnd = 0;
int restStart = 0;
int restEnd = 0;

void setup()
{
  // MUX Control Pins need to be outputs
  pinMode(CONTROL0, OUTPUT);
  pinMode(CONTROL1, OUTPUT);
  pinMode(CONTROL2, OUTPUT);
  pinMode(CONTROL3, OUTPUT);
  pinMode(16, INPUT);
  pinMode(10, OUTPUT);
  pinMode(9, OUTPUT);
  digitalWrite(16, HIGH);
  
  Serial.begin(9600);
}

void loop()
{
  addressPin(3);
  mux2array[3] = digitalRead(16);
  if(mux2array[3] == 1){
    if(programMode == true){
      programMode = false;
      digitalWrite(9, LOW);
    } else {
      programMode = true;
      digitalWrite(9, HIGH);
      noteStep = 0;
    }
  }
  
  if(programMode == true){
    addressPin(1);
    notePressed = digitalRead(16);
    
    // Press exit
    if(notePressed == 1 && prevPressed == 0){
      noteEnd = millis();
      restStart = millis();
      noteDuration[noteStep] = noteEnd-noteStart;
      if(noteStep == 7){
        noteStep = 0;
      } else {
        noteStep++;
      }
    } else if (notePressed == 0 && prevPressed == 1){
      restEnd = millis();
      if(noteStep > 0){
        noteRest[noteStep-1] = restEnd-restStart;
      } else
      {
        noteRest[7] = restEnd-restStart;
      }
      noteStart = millis();
    }
    
    prevPressed = notePressed;
  } else {
    resetButton = false;
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
    
    // **************************
    // Step Through Notes
    // **************************
    
    
    // Read Pot Value
    addressPin(noteStep);
    mux0array[noteStep] = analogRead(14);
    
    // Light the LED
    muxWrite(1,noteStep);
    digitalWrite(15, HIGH);
    delay(100);
    digitalWrite(15, LOW);
    
    // Play the Tone
    int j = noteStep+8;
    muxWrite(1,j);
    tone(15,tones[((switchPos-1)*12)+int(map(mux0array[noteStep],0,1023,0,11))],100);
    delay(noteDuration[noteStep]);
    
    // Listen for Reset
    addressPin(0);
    mux2array[0] = digitalRead(16);
    if(mux2array[0] == 1){
      resetButton = true;
    }
    
    // Listen for Backwards
    addressPin(2);
    mux2array[2] = digitalRead(16);
    if(mux2array[2] == 1){
      if(backwards == true)
      {
        backwards = false;
        digitalWrite(10,LOW);
      } else {
        backwards = true;
        digitalWrite(10,HIGH);
      }
    }
    
    delay(noteRest[noteStep]);
    
    // Reset the the sequence if the reset button was clicked.
    if(resetButton == true){
      noteStep = 0;
    } else if (backwards == true){
      if(noteStep == 0){
        noteStep = 7;
      } else{
        noteStep--;
      }
    } else if(noteStep == 7){
      noteStep = 0;
    } else {
      noteStep++;
    }
  }
  // **************************
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
