#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include "ftd2xx.h"
#include "ruby.h"

// typedef unsigned short WORD;
// typedef unsigned char BYTE;

VALUE rb_mOpenDMX;
FT_HANDLE ftHandle = NULL;
BYTE bDmxData[513];
BOOL stop;
pthread_t threadId = 0;

VALUE method_init(VALUE self);
VALUE method_close(VALUE self);
VALUE method_set_dmx_data(VALUE self, VALUE data);

FT_STATUS initDMX();
BOOL startThread();
void stopThread();

void Init_opendmx() {
  rb_mOpenDMX = rb_define_module("OpenDMX");
  rb_define_singleton_method(rb_mOpenDMX, "init", method_init, 0);
  rb_define_singleton_method(rb_mOpenDMX, "close", method_close, 0);
  rb_define_singleton_method(rb_mOpenDMX, "set_dmx_data", method_set_dmx_data, 1);
}

VALUE method_init(VALUE self) {
  initDMX();
  startThread();
  return Qtrue;
}

VALUE method_close(VALUE self) {
  stopThread();
  return Qtrue;
}

VALUE method_set_dmx_data(VALUE self, VALUE data) {
  data = rb_check_array_type(data);
  if (data != Qnil) {
    int length = RARRAY_LEN(data);
    length = (length > 512) ? 512 : length;
    int i;
    for (i = 0; i < length; i++) {
      VALUE entry = rb_ary_entry(data, i);
      int value = FIX2INT(entry);
      value = (value > 255) ? 255 : value;
      value = (value < 0) ? 0 : value;
      bDmxData[i + 1] = (BYTE) value;
    }
  }

  return Qtrue;
}

FT_STATUS initDMX()
{
  memset(bDmxData, 0, 513);

  int iNumDevs = 0;
  FT_STATUS ftStatus;

  ftStatus = FT_ListDevices(&iNumDevs, NULL, FT_LIST_NUMBER_ONLY);
  if(ftStatus != FT_OK) {
    printf("Error: FT_ListDevices(%d)\n", (int)ftStatus);
    return ftStatus;
  }
  printf("Found %d devices.\n", iNumDevs);

  ftStatus = FT_Open(0, &ftHandle);
  if(ftStatus != FT_OK){
    /*
      This can fail if the ftdi_sio driver is loaded
      use lsmod to check this and rmmod ftdi_sio to remove
      also rmmod usbserial
    */
    printf("Error FT_Open(%d)\n", (int)ftStatus);
    printf("Use lsmod to check if ftdi_sio (and usbserial) are present.\n");
    printf("If so, unload them using rmmod, as they conflict with ftd2xx.\n");
    return ftStatus;
  }

  printf("Opened device\n");

  if((ftStatus = FT_ResetDevice(ftHandle)) != FT_OK) {
    printf("Error FT_ResetDevice(%d)\n", (int)ftStatus);
    return ftStatus;
  }

  if((ftStatus = FT_SetBaudRate(ftHandle, 250000)) != FT_OK) {
    printf("Error FT_SetBaudRate(%d)\n", (int)ftStatus);
    return ftStatus;
  }

  if((ftStatus = FT_SetDataCharacteristics(ftHandle, FT_BITS_8, FT_STOP_BITS_2, FT_PARITY_NONE)) != FT_OK) {
    printf("Error FT_SetDataCharacteristics(%d)\n", (int)ftStatus);
    return ftStatus;
  }

  if((ftStatus = FT_SetFlowControl(ftHandle, FT_FLOW_NONE, 0, 0)) != FT_OK) {
    printf("Error FT_SetFlowControl(%d)\n", (int)ftStatus);
    return ftStatus;
  }

  if((ftStatus = FT_ClrRts(ftHandle)) != FT_OK) {
    printf("Error FT_ClrRts(%d)\n", (int)ftStatus);
    return ftStatus;
  }

  if((ftStatus = FT_Purge(ftHandle, FT_PURGE_TX)) != FT_OK) {
    printf("Error FT_Purge(%d)\n", (int)ftStatus);
    return ftStatus;
  }

  if((ftStatus = FT_Purge(ftHandle, FT_PURGE_RX)) != FT_OK) {
    printf("Error FT_Purge(%d)\n", (int)ftStatus);
    return ftStatus;
  }

  printf("Configured device\n");
  return FT_OK;
}

void* sendThread(void *arg)
{
  FT_STATUS ftStatus;
  DWORD bytesWritten;

  while(!stop) {
    if ((ftStatus = FT_SetBreakOn(ftHandle)) != FT_OK) {
      printf("Error FT_SetBreakOn(%d)\n", (int)ftStatus);
      break;
    }

    usleep(100);

    if((ftStatus = FT_SetBreakOff(ftHandle)) != FT_OK) {
      printf("Error FT_SetBreakOff(%d)\n", (int)ftStatus);
      break;
    }

    if((ftStatus = FT_Write(ftHandle, &bDmxData, 513, &bytesWritten)) != FT_OK) {
      printf("Error FT_Write(%d)\n", (int)ftStatus);
      break;
    }

    usleep(30000);
  }

  printf("Thread stopped\n");

  return NULL;
}

BOOL startThread()
{
  stop = FALSE;
  int err = pthread_create(&threadId, NULL, &sendThread, NULL);
  if (err != 0)
  {
    printf("Can't create thread :[%s]", strerror(err));
    return FALSE;
  }

  printf("Thread created successfully\n");
  return TRUE;
}

void stopThread()
{
  stop = TRUE;
  pthread_join(threadId, NULL);
}