
int inputPin0 = 6;
int outputPin1 = 7;
int outputPin2 = 8;
int outputPin3 = 9;
int outputPin4 = 10;
int outputPin5 = 11;

void setup() {
  
    pinMode(inputPin0, INPUT);//pin mode setup for both buttons
    pinMode(outputPin1, OUTPUT);
    pinMode(outputPin2, OUTPUT);
    pinMode(outputPin3, OUTPUT);
    pinMode(outputPin4, OUTPUT);
    pinMode(outputPin5, OUTPUT);
    
    digitalWrite(outputPin1,LOW);
    digitalWrite(outputPin2,LOW);
    digitalWrite(outputPin3,LOW);
    digitalWrite(outputPin4,LOW);
    digitalWrite(outputPin5,LOW);
    
    Serial.begin(9600);

    
}

bool pressed = false;
int state = 0;

void loop() {
  
    while(pressed == false)
    {
       pressed = digitalRead(inputPin0); 
    }
     if(pressed == true)
    {
      state++;
      pressed = false;
      delay(200);
    } 

    if(state == 6)
    {
        digitalWrite(outputPin1,LOW);
        digitalWrite(outputPin2,LOW);
        digitalWrite(outputPin3,LOW);
        digitalWrite(outputPin4,LOW);
        digitalWrite(outputPin5,LOW);
        state = 0;
    }
    
    switch(state)
    {
      case 1:
            digitalWrite(outputPin1,HIGH);
            break;
      case 2:
            digitalWrite(outputPin2,HIGH);
            break;
      case 3:
            digitalWrite(outputPin3,HIGH);
            break;
      case 4:
            digitalWrite(outputPin4,HIGH);
            break; 
      case 5:
            digitalWrite(outputPin5,HIGH);
            break;
    }
    

    
}
