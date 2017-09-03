struct MidiOutMessage {
  byte inputProgram;
  byte program1;
  byte program2;
  
};

const byte PROGRAM_NONE = 255;
const int NUM_PROGRAMS = 12;
const int INPUT_BUFFER_SIZE = 2;
const byte PROGRAM_CHANGE = 0xC0;

/*
 * EHX 45000 Programs
 */ 
const byte FF_PLAY = 0x64;
const byte FF_TRACK1_MUTE = 0x65;
const byte FF_TRACK2_MUTE = 0x66;
const byte FF_TRACK3_MUTE = 0x67;
const byte FF_TRACK4_MUTE = 0x68;
const byte FF_TRACKM_MUTE = 0x7B;
const byte FF_REC = 0x69;
const byte FF_NEW_LOOP = 0x6e;

/*
 * Media Player Triggers
 */
const byte REMOTE_PLAY_TRIGGER_PROGRAM = 0x06;
const byte REMOTE_STOP_TRIGGER_PROGRAM = 0x07;

const byte REMOTE_PLAY_TRIGGER2_PROGRAM = 0x08;
const byte REMOTE_STOP_TRIGGER2_PROGRAM = 0x09;

const byte REMOTE_PLAY_TRIGGER3_PROGRAM = 0x0a;
const byte REMOTE_STOP_TRIGGER3_PROGRAM = 0x0b;

byte input[INPUT_BUFFER_SIZE];
int pos = 0;

int REMOTE_PLAY_PIN1 = 8;
int REMOTE_PLAY_PIN2 = 9;

MidiOutMessage programs[NUM_PROGRAMS];

void setup() {
  Serial.begin(31250);  // 31250
  pinMode(REMOTE_PLAY_PIN1, OUTPUT);
  pinMode(REMOTE_PLAY_PIN2, OUTPUT);
  digitalWrite(REMOTE_PLAY_PIN1, LOW);
  digitalWrite(REMOTE_PLAY_PIN2, LOW);
  mapPrograms();
}

void mapPrograms() {
  programs[0] = {0x00, FF_PLAY, PROGRAM_NONE};
  programs[1] = {0x01, FF_REC, PROGRAM_NONE};
  programs[2] = {0x02, FF_TRACK1_MUTE, FF_TRACK2_MUTE};
  programs[3] = {0x03, FF_TRACK3_MUTE, FF_TRACK4_MUTE};
  programs[4] = {0x04, FF_TRACKM_MUTE, PROGRAM_NONE};
  programs[5] = {0x05, FF_PLAY, PROGRAM_NONE};
  programs[6] = {0x06, FF_REC, PROGRAM_NONE};
  programs[7] = {0x07, FF_NEW_LOOP, PROGRAM_NONE};
  programs[8] = {0x08, FF_PLAY, PROGRAM_NONE};
  programs[9] = {0x09, FF_PLAY, PROGRAM_NONE};
  programs[10] = {0x0a, PROGRAM_NONE, PROGRAM_NONE};
  programs[11] = {0x0b, PROGRAM_NONE, PROGRAM_NONE};
}

void loop() {
  if (Serial.available() > 0) {
    // read the incoming data:
    input[pos] = Serial.read();
    pos++;
  }

  if(pos == 2) {
    pos = 0;
    if(input[0] == PROGRAM_CHANGE) {
      byte inputProgram = input[1];

      if(!executeProgram(inputProgram)) {
        sendProgramChange(inputProgram);
      }

      triggerCDJ(inputProgram);
    }   
  }
}

bool executeProgram(byte inputProgram) {
  int programsExecuted = 0;
  if(inputProgram != PROGRAM_NONE) {
    for(int i = 0; i < NUM_PROGRAMS; i++) {
      if(programs[i].inputProgram == inputProgram) {
        sendMessage(programs[i]);
        programsExecuted++;
      }
    }
  }

  return programsExecuted > 0;
}

void sendMessage(MidiOutMessage message){
    if(message.program1 != PROGRAM_NONE) {
      sendProgramChange(message.program1);
    }
    
    if(message.program2 != PROGRAM_NONE) {
      sendProgramChange(message.program2);
    }
}

void sendProgramChange(byte program) {
  writeByte(PROGRAM_CHANGE);
  writeByte(program);
}

void writeByte(byte data) {
  Serial.write(data);
}

void sendCDJRemoteStart() {
  digitalWrite(REMOTE_PLAY_PIN2, LOW); 
  digitalWrite(REMOTE_PLAY_PIN1, HIGH); 
  delay(10);
  digitalWrite(REMOTE_PLAY_PIN1, LOW); 
}

void sendCDJRemoteStop() {
  digitalWrite(REMOTE_PLAY_PIN1, LOW); 
  digitalWrite(REMOTE_PLAY_PIN2, HIGH); 
  delay(10);
  digitalWrite(REMOTE_PLAY_PIN2, LOW); 
}

void triggerCDJ(byte inputProgram) {
  if(inputProgram == REMOTE_PLAY_TRIGGER_PROGRAM) {
    sendCDJRemoteStart();
  }
  else if(inputProgram == REMOTE_STOP_TRIGGER_PROGRAM) {
    sendCDJRemoteStop();
  }
  else if(inputProgram == REMOTE_PLAY_TRIGGER2_PROGRAM) {
    sendCDJRemoteStart();
  }
  else if(inputProgram == REMOTE_STOP_TRIGGER2_PROGRAM) {
    sendCDJRemoteStop();
  } 
  else if(inputProgram == REMOTE_PLAY_TRIGGER3_PROGRAM) {
    sendCDJRemoteStart();
  }
  else if(inputProgram == REMOTE_STOP_TRIGGER3_PROGRAM) {
    sendCDJRemoteStop();
  } 
}

