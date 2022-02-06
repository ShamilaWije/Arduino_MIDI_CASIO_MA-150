//midi channels
const int CHANNEL1 = 0;

// notes
const int NOTE_ON_CMD = 144;
const int NOTE_OFF_CMD = 128;
const int NOTE_VELOCITY = 127;

// keyboard matrix
const int NUM_ROWS = 8;
const int NUM_COLS = 7;

// 74HC595 pins
const int dataPin = 4;   //74HC595 pin 14
const int latchPin = 3;  //74HC595 pin 12
const int clockPin = 2;  //74HC595 pin 11

// Row input pins
const int row1Pin = 5;
const int row2Pin = 6;
const int row3Pin = 7;
const int row4Pin = 8;
const int row5Pin = 9;
const int row6Pin = 10;
const int row7Pin = 11;
const int row8Pin = 12;

// MIDI baud rate
const int SERIAL_RATE = 9600;

uint8_t keyToMidiMap[NUM_ROWS][NUM_COLS];
boolean keyPressed[NUM_ROWS][NUM_COLS];

// bitmasks for scanning columns
int bits[] =
{ 
  B00000001,
  B00000010,
  B00000100,
  B00001000,
  B00010000,
  B00100000,
  B01000000,

};

void setup() {

  int note = 24;

  for(int col = 0; col < NUM_COLS; ++col){
    for(int row = 0; row < NUM_ROWS; ++row){
      keyToMidiMap[row][col] = note;
      keyPressed[row][col] = false;
      note++;
    }
  }
  pinMode(dataPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(latchPin, OUTPUT);
  pinMode(row1Pin, INPUT);
  pinMode(row2Pin, INPUT);
  pinMode(row3Pin, INPUT);
  pinMode(row4Pin, INPUT);
  pinMode(row5Pin, INPUT);
  pinMode(row6Pin, INPUT);
  pinMode(row7Pin, INPUT);
  pinMode(row8Pin, INPUT);

  Serial.begin(SERIAL_RATE);  

}

void loop() {
  for (int colCtr = 0; colCtr < NUM_COLS; ++colCtr){
    //scan next column
    scanColumn(colCtr);

    //get row values at this column
    int rowValue[NUM_ROWS];
    rowValue[0] = digitalRead(row1Pin);
    rowValue[1] = digitalRead(row2Pin);
    rowValue[2] = digitalRead(row3Pin);
    rowValue[3] = digitalRead(row4Pin);
    rowValue[4] = digitalRead(row5Pin);
    rowValue[5] = digitalRead(row6Pin);
    rowValue[6] = digitalRead(row7Pin);
    rowValue[7] = digitalRead(row8Pin);

    // process keys pressed
    for(int rowCtr=0; rowCtr<NUM_ROWS; ++rowCtr){
      if(rowValue[rowCtr] != 0 && !keyPressed[rowCtr][colCtr]){
        keyPressed[rowCtr][colCtr] = true;
        sendMidiMessage(NOTE_ON_CMD, CHANNEL1, keyToMidiMap[rowCtr][colCtr], NOTE_VELOCITY);
      }
    }

    // process keys released
    for(int rowCtr=0; rowCtr<NUM_ROWS; ++rowCtr){
      if(rowValue[rowCtr] == 0 && keyPressed[rowCtr][colCtr]){
        keyPressed[rowCtr][colCtr] = false;
        sendMidiMessage(NOTE_OFF_CMD, CHANNEL1, keyToMidiMap[rowCtr][colCtr], NOTE_VELOCITY);
      }
    }
  }

}
void sendMidiMessage(int cmd, int channel, int lsb, int msb) {
  Serial.write(cmd + channel); // send command plus the channel number
  Serial.write(lsb); // least significant bit 
  Serial.write(msb); // most significant bit
}
void scanColumn(int colNum){
  digitalWrite(latchPin, LOW);
  if(0 <= colNum && colNum <= 7)
  {
    shiftOut(dataPin, clockPin, LSBFIRST, B00000000); 
    shiftOut(dataPin, clockPin, LSBFIRST, bits[colNum]);
  }
  digitalWrite(latchPin, HIGH);
}
