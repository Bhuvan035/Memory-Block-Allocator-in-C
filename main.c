#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

struct block1 {
  bool allocated;
  int payload;
};
struct heap {
  char value;
  struct block1* block;
  bool isheader;
};

struct heap heapList[127];

void initialization() {
  heapList[0].block =
      (struct block1*)malloc(sizeof(struct block1));  // Allocate memory
  heapList[0].isheader = true;
  heapList[0].block->allocated = false;
  heapList[0].block->payload = 126;
  heapList[0].value = '\0';
  for (int i = 1; i < 127; i++) {
    heapList[i].block =
        (struct block1*)malloc(sizeof(struct block1));  // Allocate memory

    heapList[i].isheader = false;
    heapList[i].block->allocated = false;
    heapList[i].block->payload = 0;
    heapList[i].value = '\0';
  }
}

void mallocfunc(int pay) {
  int mindifference = 999999;
  int minindex = 0;
  for (int i = 0; i < 127; i++) {
    // printf("index %d",i);
    if (heapList[i].isheader == true) {
      if (heapList[i].block->allocated == false) {
        // printf("bro %d", i);
        // fflush(stdout);
        if (heapList[i].block->payload >= pay) {
          if (heapList[i].block->payload - pay < mindifference) {
            mindifference = heapList[i].block->payload - pay;
            minindex = i;
          }
        }
      }
      i += heapList[i].block->payload;
    }
  }
  // printf("Printing minindex: %d",minindex);
  if (heapList[minindex + pay + 1].isheader == false &&
      heapList[minindex + pay + 2].isheader == false) {
    heapList[minindex + pay + 1].block->allocated = false;
    heapList[minindex + pay + 1].isheader = true;
    heapList[minindex + pay + 1].block->payload =
        (heapList[minindex].block->payload) - pay - 1;
  }
  heapList[minindex].block->allocated = true;
  heapList[minindex].block->payload = pay;
  printf("%d", minindex + 1);
  /*printf("\n");
  for(int i=0;i<127;i++){
    printf("%d ",heapList[i].isheader);
  }*/
}

void blocklistfunc() {
  for (int i = 0; i < 127; i++) {
    if (heapList[i].isheader == true) {
      if (heapList[i].block->allocated == true) {
        printf("%d, %d, %s\n", i + 1, heapList[i].block->payload, "allocated");
      } else {
        printf("%d, %d, %s\n", i + 1, heapList[i].block->payload, "free");
      }
    }
  }
}

void writememfunc(int addr, char* inp) {
  for (int i = 0; inp[i] != '\0'; i++) {
    heapList[addr].value = inp[i];
    addr++;
  }
}

void printmemfunc(int addr, int inputsize) {
  for (int i = addr; i < (addr + inputsize); i++) {
    printf("%x ", heapList[i].value);
  }
}

void freefunc(int paystart) {
  int header = paystart - 1;
  heapList[header].block->allocated = false;
  int pay = heapList[header].block->payload;
  if (heapList[header + pay + 1].block->allocated == false) {
    heapList[header].block->payload +=
        heapList[header + pay + 1].block->payload + 1;
    heapList[header + pay + 1].isheader = false;
    heapList[header + pay + 1].block->payload = 0;
    heapList[header + pay + 1].block->allocated = false;
  }
}

void reallocfunc(int paystart, int newpay) {
  int header = paystart - 1;
  if (!heapList[header].block->allocated) {
    return;
  }
  if (newpay < heapList[header].block->payload) {
    int curpay = heapList[header].block->payload;
    heapList[header].block->payload = newpay;
    if (heapList[header + newpay + 1].isheader == false &&
        heapList[header + newpay + 2].isheader == false) {
      heapList[header + newpay + 1].block->allocated = false;
      heapList[header + newpay + 1].isheader = true;
      heapList[header + newpay + 1].block->payload = curpay - newpay - 1;
      int nextheaderaddr1 = header + newpay + 1 + curpay - newpay;
      if (heapList[nextheaderaddr1].isheader == true &&
          heapList[nextheaderaddr1].block->allocated == false) {
        heapList[header + newpay + 1].block->payload +=
            heapList[nextheaderaddr1].block->payload + 1;
        heapList[nextheaderaddr1].isheader = false;
        heapList[nextheaderaddr1].block->payload = 0;
        heapList[nextheaderaddr1].block->allocated = false;
      }

      // bro
    }
  } else {
    if (newpay > heapList[header].block->payload) {
      int nextheaderaddr = paystart + (heapList[header].block->payload);
      int temp = ((heapList[nextheaderaddr].block->payload) +
                  (heapList[header].block->payload) + 1);
      if ((heapList[nextheaderaddr].block->allocated == false) &&
          (((heapList[nextheaderaddr].block->payload) +
            (heapList[header].block->payload) + 1) >= newpay)) {
        heapList[header].block->allocated = true;
        heapList[header].block->payload = newpay;
        heapList[nextheaderaddr].block->allocated = false;
        heapList[nextheaderaddr].block->payload = 0;
        heapList[nextheaderaddr].isheader = false;

        if (heapList[header + newpay + 1].isheader == false &&
            heapList[header + newpay + 2].isheader == false) {
          heapList[header + newpay + 1].isheader = true;
          heapList[header + newpay + 1].block->allocated = false;
          heapList[header + newpay + 1].block->payload = temp - newpay - 1;
        }

      } else {
        freefunc(paystart);
        mallocfunc(newpay);
      }
    }
  }
}

/*
void reallocfunc(int addr, int payload) {
  if (!heapList[addr - 1].block->allocated) {
    return;
  }
  // blocklistfunc();
  if (payload < heapList[addr - 1].block->payload) {
    heapList[heapList[addr - 1].block->payload - payload + 1].isheader = true;
    heapList[heapList[addr - 1].block->payload - payload + 1].block->allocated =
        false;
    heapList[heapList[addr - 1].block->payload - payload + 1].block->payload =
        heapList[addr - 1].block->payload - payload - 1;
    heapList[addr - 1].block->payload = payload;
  } else {
    int oldPayload = heapList[addr - 1].block->payload;
    /*printf("Next header addres : %d\n",
           addr + heapList[addr - 1].block->payload);
    printf("IhHeader? : %d\n",
           heapList[addr + heapList[addr - 1].block->payload].isheader);
    if (!heapList[addr + heapList[addr - 1].block->payload].block->allocated) {
      int freePayload =
          heapList[addr + heapList[addr - 1].block->payload].block->payload;
      heapList[addr + heapList[addr - 1].block->payload].isheader = false;
      int i;
      for (i = 0; i < (payload - oldPayload); i++) {
        heapList[i].block->allocated = true;
      }
      heapList[addr - 1].block->payload = payload;
      heapList[addr + heapList[addr - 1].block->payload].isheader = true;
      heapList[addr + heapList[addr - 1].block->payload].block->allocated =
          false;
      heapList[addr + heapList[addr - 1].block->payload].block->payload =
          freePayload - (payload - oldPayload);
    } else {
      freefunc(addr);
      mallocfunc(payload);
    }
  }
}







void reallocfunc();
void freefunc();*/

void loop() {
  do {
    char input[100];
    char command[128] = "";
    char args1[128] = "";
    char args2[128] = "";
    printf("> ");
    fgets(input, 80, stdin);
    int command_count = sscanf(input, "%s %s %s", command, args1, args2);
    /*printf("command - %s ",command);
    printf("args1 - %s ",args1);
    printf("args2 - %s ",args2);
    //printf("command - %s ",args);*/
    if (strcmp(command, "malloc") == 0) {
      mallocfunc(atoi(args1));
    } else if (strcmp(command, "realloc") == 0) {
      reallocfunc(atoi(args1), atoi(args2));
    } else if (strcmp(command, "free") == 0) {
      freefunc(atoi(args1));
    } else if (strcmp(command, "blocklist") == 0) {
      blocklistfunc();
    } else if (strcmp(command, "writemem") == 0) {
      writememfunc(atoi(args1), args2);
    } else if (strcmp(command, "printmem") == 0) {
      printmemfunc(atoi(args1), atoi(args2));
    } else if (strcmp(command, "quit") == 0) {
      break;
    } else {
      printf("Invalid Command\n");
    }
  } while (1);
}

int main() {
  initialization();
  loop();
  return 0;
}