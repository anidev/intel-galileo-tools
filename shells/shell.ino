#include <cstdio>

void printLines(FILE* file) {
  int c = EOF;
  while((c = fgetc(file)) != EOF) {
    if(c == '\n'){
      Serial.write('\r');
    }
    Serial.write(c);
  }
}

// Return string length, not including terminating character
int readSerialLine(char* buf, int len) {
  int i = 0;
  while(true) {
    if(i == len - 1) {
      buf[i] = '\0';
      break;
    }
    int data = Serial.read();
    if(data < 0) {
      continue;
    }
    if(data == 0x08 || data == 0x7F) { // backspace or delete
      if(i == 0) {
        // Do not delete past start of buffer
        continue;
      }
      i--;
      buf[i] = '\0';
      Serial.print("\x8 \x8");
      continue;
    }
    buf[i] = data;
    if(buf[i] == '\n' || buf[i] == '\r') {
      buf[i] = '\0';
    }
    if(buf[i] == '\0') {
      break;
    }
    Serial.print(buf[i]);
    i++;
  }
  Serial.write("\r\n");
  return i;
}

void setup() {
  Serial.begin(9600);
  Serial.println("");
  Serial.println("Enter commands, 1024 character limit, newline after command");
}

void loop() {
  Serial.print("> ");
  char cmd[1024];
  int total = readSerialLine(cmd, 1024);
  if(total > 0) {
/*    Serial.println("Running:");
    Serial.println(cmd);
    Serial.println("Length:");
    Serial.println(strlen(cmd));*/
    FILE* output = popen(cmd, "r");
    printLines(output);
    pclose(output);
  }
}
