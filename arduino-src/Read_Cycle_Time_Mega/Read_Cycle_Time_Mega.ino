//const uint8_t moldInput1 = 2;
//const uint8_t moldInput2 = 3;
//const uint8_t moldInput3 = 4;
//const uint8_t moldInput4 = 5;
//const uint8_t moldInput5 = 6;
//const uint8_t moldInput6 = 7;

const uint8_t moldInput1 = 38;
const uint8_t moldInput2 = 39;
const uint8_t moldInput3 = 40;
const uint8_t moldInput4 = 41;
const uint8_t moldInput5 = 42;
const uint8_t moldInput6 = 43;

char nameMachine1[6] = "V0209";
char nameMachine2[6] = "V0304";
char nameMachine3[6] = "V0303";
char nameMachine4[6] = "V0210";
char nameMachine5[6] = "H0205";
char nameMachine6[6] = "H0302";

unsigned long tempTime1 = 0;

char dataArray[500] = "";

typedef enum {
  INACTIVE,
  ACTIVE,
} count_state_e;

typedef struct {
  uint8_t inputPin;
  uint8_t pinState;
  uint8_t lastPinState;
  unsigned long timeStart;
  unsigned long timeStop;
  unsigned long tempCycleTime; //tempcycletime
  unsigned long cycleTime;
  unsigned int cycleTimeCnt;
  char *nameStr;
  count_state_e cntState;
} machine_s;
machine_s machine1, machine2, machine3, machine4, machine5, machine6;

void setup()
{
  Serial.begin(9600);
  delay(100);
  Serial1.begin(9600);
  delay(100);

  init(&machine1, nameMachine1, moldInput1);
  Serial.println(machine1.nameStr);
  init(&machine2, nameMachine2, moldInput2);
  Serial.println(machine2.nameStr);
  init(&machine3, nameMachine3, moldInput3);
  Serial.println(machine3.nameStr);
  init(&machine4, nameMachine4, moldInput4);
  Serial.println(machine4.nameStr);
  init(&machine5, nameMachine5, moldInput5);
  Serial.println(machine5.nameStr);
  init(&machine6, nameMachine6, moldInput6);
  Serial.println(machine6.nameStr);

}

void loop()
{
  scanStatus(&machine1);
  scanStatus(&machine2);
  scanStatus(&machine3);
  scanStatus(&machine4);
  scanStatus(&machine5);
  scanStatus(&machine6);

  if ((millis() - tempTime1) >= 5000) // 5 second
  {
    cutStringData(dataArray);
    Serial1.println("printout");
    //Serial1.println(cutStringData(dataArray));
    tempTime1 = millis();
  }
}

char* cutStringData(char *data)
{
  char tmpArray[500] = "";
  char str[30] = "";
  char * pch;
  uint8_t i = 0;
  uint8_t pos = 0;

  // xac dinh vi tri cua ki tu C thu 2
  pch = strchr(data, 'C');
  while (pch != NULL)
  {
    pos = pch - data;
    pch = strchr(pch + 1, 'C');

    i++;
    if (i > 1) // neu gat vi tri ki tu C thu 2 thi thoat khoi vong lap
      break;
  }

  if (i > 1)
  {
    strncpy(str, data, pos);  // copy chuoi tu vi tri dau tien den vi tri cua ki tu C thu 2
    str[pos] = '\0';  // phải có dòng này nhé, để ngôn ngữ Arduino xem đây là một chuỗi, thay vì là một mảng ký tự

    strncpy (tmpArray, data + pos, strlen(data));
    tmpArray[strlen(data) - 1] = '\0'; // phải có dòng này nhé, để ngôn ngữ Arduino xem đây là một chuỗi, thay vì là một mảng ký tự

    strcpy(data, tmpArray);
  }
  else if (i == 1)
  {
    strcpy(str, data);
    strcpy(data, "");
  }

  Serial1.println(str);
  return str;
}

void addStringData(machine_s *machine)
{
  char hdrArray[4] = "CT,";
  char timingArray[10] = ""; // Buffer big enough for 10-character float
  char cycleCntArray[6] = "";

  if (machine->cycleTime > 5)
  {
    strcat(dataArray, hdrArray);
    strcat(dataArray, machine->nameStr);

    strcat(dataArray, ",");

    //strcat(dataArray, string2char(String((float)machine->cycleTime/1000.0)));

    strcat(dataArray, string2char(String(machine->cycleTime)));

    strcat(dataArray, ",");

    strcat(dataArray, string2char(String(machine->cycleTimeCnt)));
  }
  machine->cycleTime = 0;
}


void printResult(machine_s *machine)
{

}

void scanStatus(machine_s *machine)
{
  machine->pinState = digitalRead(machine->inputPin);

  if ( machine->lastPinState != machine->pinState)
  {
    machine->lastPinState = machine->pinState;

    if (machine->pinState == HIGH) //tin hieu dong khuon == HIGH
    {
      switch (machine->cntState)
      {
        case INACTIVE:
          machine->timeStart = millis();
          Serial.println("start counting");
          machine->cntState = ACTIVE;
          break;
        case ACTIVE:
          machine->timeStop = millis();
          machine->tempCycleTime = (machine->timeStop - machine->timeStart)/1000;
          machine->cycleTime = machine->tempCycleTime;
          machine->timeStart = 0;
          machine->timeStop = 0;
          machine->cycleTimeCnt++;
          Serial.print("stop counting, cycle time = ");
          Serial.println(machine->cycleTime);
          machine->cntState = INACTIVE;
          addStringData(machine);
          break;
        default:
          break;
      }
    }
  }
}

void init(machine_s *machine, char *name_, uint8_t pin)
{
  machine->inputPin = pin;
  pinMode(machine->inputPin, INPUT_PULLUP);
  machine->pinState = 0;
  machine->lastPinState = 0;
  machine->timeStart = 0;
  machine->timeStop = 0;
  machine->cycleTime = 0;
  machine->cycleTimeCnt = 0;
  machine->nameStr = name_;
  machine->cntState = INACTIVE;
}

char* string2char(String command)
{
  if (command.length() != 0) {
    char *p = const_cast<char*>(command.c_str());
    return p;
  }
}
