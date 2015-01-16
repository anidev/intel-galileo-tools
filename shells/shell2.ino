#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <sys/prctl.h>

int procRead;
int procWrite;
pid_t childPid;

void spawnShell() {
  int readPipe[2];
  int writePipe[2];
  pipe(readPipe);
  pipe(writePipe);
  childPid = fork();
  if(childPid == 0) {
    prctl(PR_SET_PDEATHSIG, SIGHUP);
    dup2(writePipe[0], STDIN_FILENO);
    dup2(readPipe[1], STDOUT_FILENO);
    dup2(readPipe[1], STDERR_FILENO);
    close(readPipe[0]);
    close(readPipe[1]);
    close(writePipe[0]);
    close(writePipe[1]);
    FILE* f = fopen("/test", "a");
    fprintf(f, "testing\n");
    fclose(f);
    char* argv[] = {"sh", "-i", NULL};
    execv("/bin/sh", argv);
  } else {
    FILE* pidFile = fopen("/.shellpid", "w+");
    fprintf(pidFile, "%d", childPid);
    fclose(pidFile);
    close(readPipe[1]);
    close(writePipe[0]);
    procRead = readPipe[0];
    procWrite = writePipe[1];
    fcntl(procRead, F_SETFL, O_NONBLOCK);
    fcntl(procWrite, F_SETFL, O_NONBLOCK);
    char* cmd = "stty -F /dev/ttyGS0 isig icrnl onlcr opost\n";
    write(procWrite, cmd, strlen(cmd));
  }
}

void readPrintPipe() {
  char buf[32];
  int total = 0;
  while((total = read(procRead, buf, 32)) != -1) {
    Serial.write((uint8_t*)buf, total);
    if(total < 32) {
      break;
    }
  }
}

void serialSend() {
  while(Serial.available() > 0) {
    char data = (char)Serial.read();
    write(procWrite, &data, 1);
    //Serial.print((int)data);
  }
}

void setup() {
  spawnShell();
  Serial.begin(9600);
  Serial.println("");
  Serial.println("Enter commands, 1024 character limit, newline after command");
}

void loop() {
  readPrintPipe();
  serialSend();
}
