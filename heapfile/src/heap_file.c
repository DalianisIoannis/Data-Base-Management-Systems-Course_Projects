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
  return HP_OK;
}

HP_ErrorCode HP_CreateFile(const char *filename) {
  //insert code here
  int file_desc;
  char* data;
  BF_Block *block;
  BF_Block_Init(&block);
  
  CALL_BF(BF_CreateFile(filename));

  CALL_BF(BF_OpenFile(filename, &file_desc));
  CALL_BF(BF_AllocateBlock(file_desc, block));
  data=BF_Block_GetData(block);
  memcpy(data, "HeapF", 5);
  
  BF_Block_SetDirty(block);
  CALL_BF(BF_UnpinBlock(block));
  BF_Block_Destroy(&block);
  return HP_OK;
}

HP_ErrorCode HP_OpenFile(const char *fileName, int *fileDesc){
  //insert code here
  CALL_BF(BF_OpenFile(fileName, fileDesc));
  return HP_OK;
}

HP_ErrorCode HP_CloseFile(int fileDesc) {
  //insert code here
  CALL_BF(BF_CloseFile(fileDesc));
  return HP_OK;
}

HP_ErrorCode HP_InsertEntry(int fileDesc, Record record) {
  //insert code here
  int block_counter;
  char* data;
  BF_Block *block;
  BF_Block_Init(&block);
  
  CALL_BF(BF_GetBlockCounter(fileDesc, &block_counter));

  if(block_counter==1){
    CALL_BF(BF_AllocateBlock(fileDesc, block));
    data=BF_Block_GetData(block);
    memset(data, 1, 1); //einai to 1o mplok
    // memset(data+1, record.id, 1);
    memcpy(data+1+4, record.name, 15);
    memcpy(data+15+1+4, record.surname, 20);
    memcpy(data+15+20+1+4, record.city, 20);
  }
  else{
    CALL_BF(BF_GetBlock(fileDesc, block_counter-1, block));
    data=BF_Block_GetData(block);
    if(data[0]!=8){
      memset(data, data[0]+1, 1);
      memcpy(data+1+data[0]*59+4, record.name, 15);
      memcpy(data+1+data[0]*59+4+15, record.surname, 20);
      memcpy(data+1+data[0]*59+4+15+20, record.city, 20);
    }
    else{//block full
      CALL_BF(BF_UnpinBlock(block));
      CALL_BF(BF_AllocateBlock(fileDesc, block));
      data=BF_Block_GetData(block);
      memset(data, 1, 1); //einai to 1o mplok
      // memset(data+1, record.id, 1);
      memcpy(data+1+4, record.name, 15);
      memcpy(data+15+1+4, record.surname, 20);
      memcpy(data+15+20+1+4, record.city, 20);
      
    }
  }
  BF_Block_SetDirty(block);
  CALL_BF(BF_UnpinBlock(block));
  BF_Block_Destroy(&block);
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
