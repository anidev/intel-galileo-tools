#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <sys/prctl.h>

int pty;
pid_t childPid = 0;

void spawnShell() {
  Serial.println("Starting shell...");
  pty = posix_openpt(O_RDWR);
  grantpt(pty);
  unlockpt(pty);
  childPid = fork();
  if(childPid == 0) {
    prctl(PR_SET_PDEATHSIG, SIGHUP);
    int ptySlave = open(ptsname(pty), O_RDWR);
    close(pty);
    dup2(ptySlave, STDIN_FILENO);
    dup2(ptySlave, STDOUT_FILENO);
    dup2(ptySlave, STDERR_FILENO);
    char* sh;
    if(access("/bin/bash", F_OK) != -1) {
      sh = "bash";
    } else {
      sh = "sh";
    }
    char* argv[] = {sh, "-li", NULL};
    char cmd[16];
    sprintf(cmd, "/bin/%s", sh);
    execv(cmd, argv);
  } else {
    FILE* pidFile = fopen("/.shellpid", "w+");
    fprintf(pidFile, "%d", childPid);
    fclose(pidFile);
    fcntl(pty, F_SETFL, O_NONBLOCK);
  }
}

void readPrintPipe() {
  char buf[32];
  int total = 0;
  while((total = read(pty, buf, 32)) != -1) {
    Serial.write((uint8_t*)buf, total);
    if(total < 32) {
      break;
    }
  }
}

void serialSend() {
  while(Serial.available() > 0) {
    char data = (char)Serial.read();
    write(pty, &data, 1);
  }
}

void setup() {
  Serial.begin(9600);
  Serial.println("");
  Serial.println("Enter commands, 1024 character limit, newline after command");
  spawnShell();
}

void loop() {
  readPrintPipe();
  serialSend();
  delay(1); // delay 1ms, without this the sketch takes up 85-95% cpu
}
