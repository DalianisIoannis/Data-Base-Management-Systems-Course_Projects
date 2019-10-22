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
  // prepei na diavazei thn pliroforia tou protou mplok
  // an h plhroforia einai oxi gia HPFile mhnyma lathous
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
    // make new block
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
  // print oles tis EGGRAFES(record) pou sto attrname
  // exoun timi value. An to value einai NULL
  // tote ektiponei oles tis eggrafes pou iparxoun
  printf("Edose value %s\n", value);
  char* data;
  int block_counter;
  char characteristic[5];
  int block_entries;
  BF_Block *block;
  BF_Block_Init(&block);

  CALL_BF(BF_GetBlockCounter(fileDesc, &block_counter));//synolo olon ton mplok sto HPFIle
  printf("block counter %d\n", block_counter);

  CALL_BF(BF_GetBlock(fileDesc, 0, block));
  data=BF_Block_GetData(block);
  memcpy(characteristic, data, 5);
  printf("To characteristic einai %s\n", characteristic);
  CALL_BF(BF_UnpinBlock(block));

  for(int i=1; i<block_counter; i++){
      CALL_BF(BF_GetBlock(fileDesc, i, block));
      data=BF_Block_GetData(block);
      // prepei na pairno mono otan protompaino sto mplok to data[0]
      if(i==1){
        block_entries=data[0];
      }
      printf("to mplok entries einai %d\n",block_entries);
      for(int j=0; j<block_entries; j++){
        char name[15], surname[20], city[20];
        memcpy(name, data+1+j*59+4, 15);
        memcpy(surname, data+1+j*59+4+15, 20);
        memcpy(city, data+1+j*59+4+15+20, 20);
        printf("TYPONO %s %s %s\n",name, surname, city);
      }

    CALL_BF(BF_UnpinBlock(block));
  }
  // CALL_BF(BF_UnpinBlock(block));
  BF_Block_Destroy(&block);
  return HP_OK;
}

HP_ErrorCode HP_GetEntry(int fileDesc, int rowId, Record *record) {
  //insert code here
  // thelo eggrafi pou vrisketai sthn rowId thesi tou arxeiou sorou
  // ousiastika rowId-1
  // exo thn eggrafi pou thelo alla prepei prota na pairno 
  // to block sto opoio yparxei
  // apla diavazo dedomena ara de xreiazetai Dirty

  // printf("Mpike.\n");
  // int block_counter;
  // char* data;
  // BF_Block *block;
  // BF_Block_Init(&block);

  // CALL_BF(BF_GetBlock(fileDesc, block_counter-1, block));

  // CALL_BF(BF_UnpinBlock(block));
  // BF_Block_Destroy(&block);
  // printf("Vgike.\n");

  return HP_OK;
}
