#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

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
  return HP_OK;
}

HP_ErrorCode HP_CreateFile(const char *filename) {
  int file_desc;
  char* data;
  BF_Block *block;
  
  BF_Block_Init(&block);
  CALL_BF(BF_CreateFile(filename));
  CALL_BF(BF_OpenFile(filename, &file_desc));

  CALL_BF(BF_AllocateBlock(file_desc, block));
  data=BF_Block_GetData(block);

  // first block of heap file first initializes with zeros
  // and then takes the context HeapF
  memset(data, 0, BF_BLOCK_SIZE);
  memcpy(data, "HeapF", sizeof("HeapF"));

  BF_Block_SetDirty(block);
  CALL_BF(BF_UnpinBlock(block));

  CALL_BF(BF_CloseFile(file_desc));
  BF_Block_Destroy(&block);
  return HP_OK;
}

HP_ErrorCode HP_OpenFile(const char *fileName, int *fileDesc){
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
  int block_counter;
  char* data;
  char name[15], surname[20], city[20];
  BF_Block *block;
  BF_Block_Init(&block);

  // printf("DINO %d %s %s %s\n",record.id, record.name, record.surname, record.city);
  
  CALL_BF(BF_GetBlockCounter(fileDesc, &block_counter));
  // printf("To block_counter einai %d\n", block_counter);
  // CALL_BF(BF_GetBlock(fileDesc, block_counter-1, block));

  // if(block_counter%100==1){
  if(block_counter==1){
    // yparxei mono to periexomeno HPFile
    CALL_BF(BF_AllocateBlock(fileDesc, block));
    data=BF_Block_GetData(block);

    memset(data, 0, BF_BLOCK_SIZE);
    memset(data, 1, 1); //einai to 1o mplok
    memcpy(data+1, &record, sizeof(Record));

    // memcpy(&my_id, data+1, 4);
    // memcpy(name, data+1+4, 15);
    // memcpy(surname, data+1+4+15, 20);
    // memcpy(city, data+1+4+15+20, 20);
    // printf("TYPONO STO Insert case1 %d %s %s %s\n",my_id, name, surname, city);

    BF_Block_SetDirty(block);
    CALL_BF(BF_UnpinBlock(block));
    
  }
  else{
    // periptosi pou xoraei
    CALL_BF(BF_GetBlock(fileDesc, block_counter-1, block));
    data=BF_Block_GetData(block);
    int temp=data[0];
    if(temp!=8){
      memset(data, temp+1, 1);
      // memcpy(data+1+temp*sizeof(&record), &record, sizeof(&record));
      // memcpy(data+1+temp*sizeof(Record), &record, sizeof(&record));
      memcpy(data+1+temp*sizeof(Record), &record, sizeof(Record));

      // memcpy(&my_id, data+temp*sizeof(Record)+1, 4);
      // memcpy(name, data+temp*sizeof(Record)+4+1, 15);
      // memcpy(surname, data+1+temp*sizeof(Record)+4+15, 20);
      // memcpy(city, data+1+temp*sizeof(Record)+4+15+20, 20);
      // printf("TYPONO STO Insert case2 %d %s %s %s\n",my_id ,name, surname, city);

      BF_Block_SetDirty(block);
      CALL_BF(BF_UnpinBlock(block));
    }
    else{//block full make new block
      // BF_Block_SetDirty(block);
      CALL_BF(BF_UnpinBlock(block));
    
      CALL_BF(BF_AllocateBlock(fileDesc, block));
      data=BF_Block_GetData(block);

      memset(data, 0, BF_BLOCK_SIZE);
      memset(data, 1, 1); //einai to 1o mplok
      // memcpy(data+1, &record, sizeof(&record));
      memcpy(data+1, &record, sizeof(Record));

      // memcpy(&my_id, data+1, 4);
      // memcpy(name, data+1+4, 15);
      // memcpy(surname, data+1+4+15, 20);
      // memcpy(city, data+1+4+15+20, 20);
      // printf("TYPONO STO Insert case3 %d %s %s %s\n",my_id, name, surname, city);

      BF_Block_SetDirty(block);
      CALL_BF(BF_UnpinBlock(block));
    }
  }

  // BF_Block_SetDirty(block);
  // CALL_BF(BF_UnpinBlock(block));
  BF_Block_Destroy(&block);
  return HP_OK;
}

HP_ErrorCode HP_PrintAllEntries(int fileDesc, char *attrName, void* value) {
  // print oles tis EGGRAFES(record) pou sto attrname exoun timi 
  // value. An to value einai NULL tote ektiponei oles tis eggrafes 
  char* data;
  int block_counter;
  char characteristic[5];
  int block_entries;
  char name[15], surname[20], city[20];
  Record record_fetched;
  BF_Block *block;
  BF_Block_Init(&block);

  CALL_BF(BF_GetBlockCounter(fileDesc, &block_counter));//synolo olon ton mplok sto HPFIle
  // printf("block counter %d\n", block_counter);

  CALL_BF(BF_GetBlock(fileDesc, 0, block));
  data=BF_Block_GetData(block);
  memcpy(characteristic, data, 5);
  printf("To characteristic einai %s kai to block_counter %d\n", characteristic, block_counter);
  CALL_BF(BF_UnpinBlock(block));

  for(int i=1; i<block_counter; i++){
    CALL_BF(BF_GetBlock(fileDesc, i, block));
    data=BF_Block_GetData(block);
    // mplok entries to plithos eggrafon
    block_entries=data[0];
    for(int j=0; j<block_entries; j++){
      // printf("tora eimai stin eggrafi %d\n",j);
      memcpy(&record_fetched, data+1+j*sizeof(Record), sizeof(Record));
      if(value==NULL){
        printf("TYPONO %d %s %s %s\n",record_fetched.id, record_fetched.name, record_fetched.surname, record_fetched.city);
      }
      else{
        if( strcmp(attrName, "id")==0 && record_fetched.id==(long)value){
          printf("TYPONO %d %s %s %s\n",record_fetched.id, record_fetched.name, record_fetched.surname, record_fetched.city);
        }

        else if( strcmp(attrName, "name")==0 && strcmp(record_fetched.name, (char*)value)==0 ){
          printf("TYPONO %d %s %s %s\n",record_fetched.id, record_fetched.name, record_fetched.surname, record_fetched.city);
        }

        else if(strcmp(attrName, "surname")==0 && strcmp(record_fetched.surname, (char*)value)==0){
          printf("TYPONO %d %s %s %s\n",record_fetched.id, record_fetched.name, record_fetched.surname, record_fetched.city);
        }

        else if( strcmp(attrName, "city")==0 && strcmp(record_fetched.city, (char*)value)==0){
          printf("TYPONO %d %s %s %s\n",record_fetched.id, record_fetched.name, record_fetched.surname, record_fetched.city);
        }
        
      }
      // printf("\n");
    }
    CALL_BF(BF_UnpinBlock(block));
  }
  // CALL_BF(BF_UnpinBlock(block));
  BF_Block_Destroy(&block);
  printf("\n");
  return HP_OK;
}

HP_ErrorCode HP_GetEntry(int fileDesc, int rowId, Record *record) {
  //insert code here
  // thelo eggrafi pou vrisketai sthn rowId thesi tou arxeiou sorou
  // ousiastika rowId-1
  // exo thn eggrafi pou thelo alla prepei prota na pairno 
  // to block sto opoio yparxei
  // apla diavazo dedomena ara de xreiazetai Dirty

  int block_counter;
  char* data;
  BF_Block *block;
  BF_Block_Init(&block);

  // mplok pou thelo
  block_counter=ceil( (float)(rowId)/8 );
  printf("H eggrafi einai sto mplok %d\n", block_counter);

  CALL_BF(BF_GetBlock(fileDesc, block_counter, block));
  data=BF_Block_GetData(block);
  int my_temp=rowId%8-1;
  printf("einai %d\n",my_temp);
  memcpy(record, data + 1 + my_temp*sizeof(Record), sizeof(Record));

  CALL_BF(BF_UnpinBlock(block));
  BF_Block_Destroy(&block);

  return HP_OK;
}
