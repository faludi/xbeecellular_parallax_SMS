// ------ Libraries and Definitions ------
#include "simpletools.h"
#include "abdrive.h"
#include "fdserial.h"

// ------ Global Variables and Objects ------
int item;
int index_ctr;
int wL;
char * command = "\0                                                                                                                               ";
int start_time;
int wR;
int roam;
int secs_elapsed;
char * command_received = "\0                                                                                                                                ";
int char_in;
int status_sent;
fdserial *fdser;
char *__scBfr;
int __ssIdx, __stIdx;
int str_comp(char *__strA, char *__strB) { return strcmp(__strA, __strB); }

// ------ Function Declarations ------
void timer();
void clear_input_string();
void Serial_Receive();
void check_whiskers();
void check_commands();
void roam2();

// ------ Main Program ------
int main() {
  fdser = fdserial_open(11, 10, 0, 9600);
  // Main Program: Whiskers Roaming SMS
  freqout(4, 150, 3000);
  dprint(fdser, "Robot v1.2 ready...");
  while(!fdserial_txEmpty(fdser));
  pause(5);
  command_received = ("..........");
  roam = 0;
  drive_speed(0, 0);
  cog_run(timer, 128);
  cog_run(Serial_Receive, 128);
  while(1) {
    check_whiskers();
    check_commands();
    if (roam) {
      roam2();
    }

  }

}

// ------ Functions ------
void timer() {
  // creates a clock for other functions
  while(1) {
    pause(1000);
    secs_elapsed++;
  }
}

void clear_input_string() {
  // string must be cleared with junk characters
  for (item = 1; item <= 10; item += abs(1)) {
    command[(item>strlen(command)?strlen(command):item)-1] = ((46) & 0xFF)
    ;
  }// stop robot after each command
  drive_speed(0, 0);
}

void Serial_Receive() {
  // stores characters from the UART in a global string
  index_ctr = 1;
  start_time = secs_elapsed;
  while(1) {
    char_in = fdserial_rxChar(fdser);
    if (secs_elapsed - start_time > 2) {
      index_ctr = 1;
      start_time = secs_elapsed;
    }

    command_received[(index_ctr>strlen(command_received)?strlen(command_received):index_ctr)-1] = (char_in & 0xFF)
    ;__stIdx = 0;
    for(__ssIdx = (1-1); __ssIdx <= (index_ctr <= strlen(command_received)?index_ctr:strlen(command_received))-1; __ssIdx++) {
    __scBfr[__stIdx] = command_received[__ssIdx]; __stIdx++; }
    __scBfr[__stIdx] = 0;
    strcpy(command, __scBfr);
    index_ctr = index_ctr + 1;
  }
}

void check_whiskers() {
  // check for collisions and transmit status
  wL = input(7);
  wR = input(8);
  if (wL == 1  && wR == 1) {
    status_sent = 0;
  }

  if (wL == 0  && status_sent == 0) {
    dprint(fdser, "left whisker");
    while(!fdserial_txEmpty(fdser));
    pause(5);
    status_sent = 1;
  }
  else if (wR == 0  && status_sent == 0) {
    dprint(fdser, "right whisker");
    while(!fdserial_txEmpty(fdser));
    pause(5);
    status_sent = 1;
  }
}

void check_commands() {
  // check for and execute received commands
  if ((str_comp(command, ("beep")) == 0)  || (str_comp(command, ("Beep")) == 0)) {
    freqout(4, 500, 4000);
    freqout(4, 500, 8000);
    clear_input_string();
  }
  else if ((str_comp(command, ("roam")) == 0)  || (str_comp(command, ("Roam")) == 0)) {
    roam = 1;
    clear_input_string();
  }else if ((str_comp(command, ("stop")) == 0)  || (str_comp(command, ("Stop")) == 0)) {
    roam = 0;
    clear_input_string();
  }else if ((str_comp(command, ("forward")) == 0)  || (str_comp(command, ("Forward")) == 0)) {
    drive_speed(64, 64);
    pause(1500);
    clear_input_string();
  }else if ((str_comp(command, ("back")) == 0)  || (str_comp(command, ("Back")) == 0)) {
    drive_speed(-64, -64);
    pause(600);
    clear_input_string();
  }else if ((str_comp(command, ("left")) == 0)  || (str_comp(command, ("Left")) == 0)) {
    drive_speed(-64, 64);
    pause(300);
    clear_input_string();
  }else if ((str_comp(command, ("right")) == 0)  || (str_comp(command, ("Right")) == 0)) {
    drive_speed(64, -64);
    pause(300);
    clear_input_string();
  }
}

void roam2() {
  // automatic roaming mode
  drive_speed(64, 64);
  if (wL == 0) {
    drive_speed(-64, -64);
    pause(600);
    drive_speed(64, -64);
    pause(300);
  }
  else if (wR == 0) {
    drive_speed(-64, -64);
    pause(600);
    drive_speed(-64, 64);
    pause(300);
  }
}
