#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bf.h"
#include "heap_file.h"


#define CALL_BF(call)       \
{                           \
  BF_ErrorCode code = call; \
  if (code != BF_OK) {         \
    BF_PrintError(code);    \
    return HP_ERROR;        \
  }                         \
}

HP_ErrorCode HP_Init() {
  //insert code here
  printf("HP_Init done.\n");
  return HP_OK;
}

HP_ErrorCode HP_CreateFile(const char *filename) {
  //insert code here
  printf("In HP_CreateFile me filename %s.\n",filename);
  BF_Block *block;
  // printf("Edo1.\n");
  BF_Block_Init(&block);
  // printf("Edo2.\n");
  CALL_BF(BF_CreateFile(filename));
  BF_Block_Destroy(&block);
  printf("Out of HP_CreateFile.\n");
  return HP_OK;
}

HP_ErrorCode HP_OpenFile(const char *fileName, int *fileDesc){
  //insert code here
  CALL_BF(BF_OpenFile(fileName, fileDesc));
  printf("Out of HP_OpenFile.\n");
  return HP_OK;
}

HP_ErrorCode HP_CloseFile(int fileDesc) {
  //insert code here
  printf("In HP_CloseFile me fileDesc %d.\n",fileDesc);
  CALL_BF(BF_CloseFile(fileDesc));
  // CALL_BF(BF_Close());
  printf("Out of HP_CloseFile.\n");
  return HP_OK;
}

HP_ErrorCode HP_InsertEntry(int fileDesc, Record record) {
  //insert code here
  BF_Block *block;
  BF_Block_Init(&block);

  printf("Record name %s.\n",record.name);
  printf("to size tou record se byte einai: %ld\n", sizeof(record));
  char* data;
  printf("to size tou data se byte einai: %ld\n", sizeof(data));
  CALL_BF(BF_AllocateBlock(fileDesc, block));
  printf("Edo1\n");
  data = BF_Block_GetData(block);
  printf("Edo2\n");
  int block_counter;
  CALL_BF(BF_GetBlockCounter(fileDesc, &block_counter));
  printf("To block_counter einai %d.\n", block_counter);
  // AN TO BLOCK_COUNTER EINAI 1 TOTE YPARXEI 1 BLOCK HDH
  // #######################################################

  memset(data, 1, 1);
  memset(data+sizeof(int), 2, 1);
  // memcpy(data+0*64, &record, 64);
  // memcpy(data+0*60, &record, 60);
  int my_num=*data;
  printf("To data[0] einai: %d\n",my_num);
  printf("To data[1] einai: %d\n",my_num+1);
  // NA VALO TORA AMESOS META APO AUTA NA DEXETAI TO NAME TOU RECORD
  // printf("to size tou data se byte einai: %ld\n", sizeof(data));
  
  // #######################################################
  BF_Block_SetDirty(block);
  // printf("Edo3\n");
  CALL_BF(BF_UnpinBlock(block));
  // printf("Edo4\n");

  BF_Block_Destroy(&block);
  printf("Edo5\n");
  return HP_OK;
}

HP_ErrorCode HP_PrintAllEntries(int fileDesc, char *attrName, void* value) {
  //insert code here
  return HP_OK;
}

HP_ErrorCode HP_GetEntry(int fileDesc, int rowId, Record *record) {
  //insert code here
  return HP_OK;
}
