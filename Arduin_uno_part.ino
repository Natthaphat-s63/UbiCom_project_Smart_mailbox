#include <Keypad.h>
#include <Servo.h>

Servo myservo; 

const byte ROWS = 4; //four rows
const byte COLS = 4; //three columns
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

// connect the pins from right to left to pin 2, 3, 4, 5,6,7,8,9
byte rowPins[ROWS] = {5,4,3,2}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {10,8,7,6}; //connect to the column pinouts of the keypad
char pass[6] = {'1','2','3','4','5','6'};
//static char pwd[6];
//static bool locked=false;
//static int idx = 0;
//static int old_state=1;
//int countpwdErr= 0;
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

void setup() {
  pinMode(11, INPUT);
  myservo.attach(9);
  Serial.begin(115200);
  myservo.write(0); 
}
int chkpwd(char password[]) {
  int count=1;
  for(int i =0;i<6;i++)
  {
    count *= (password[i] == pass[i]);
  }
  return count;
}
void loop() {
  static char pwd[6];
  static bool locked=false;
  static int idx = 0;
  static int old_state=1;
  static int countErr= 0;
  char key = keypad.getKey();
 // Serial.print(key);
  if (key != NO_KEY) {
   if (idx <= 5 && key != '#' && key !='*') {
      pwd[idx] = key;
      idx++;
    }
    else if (key == '#')
    {
      if(chkpwd(pwd)){
         locked=false;
      }else{
        countErr++;
      }
      memset(pwd, 0, 6);
      idx=0;
    }
    else if(key == '*')
    {
      locked=true;
      memset(pwd, 0, 6);
      idx=0;
    }
    
  }
  int val = digitalRead(11);
  if(locked){
    if((val && !old_state)){
      delay(100);
      Serial.print('c');
      Serial.flush();
    }else if(countErr ==3){
      countErr=0;
      delay(100);
      Serial.print('a');
      Serial.flush();
    }
    myservo.write(90);
  }else{
    myservo.write(0); 
  }
  old_state = val;
}
