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
  int first_counter=0;
  BF_Block *block;
  
  CALL_BF(BF_CreateFile(filename));

  CALL_BF(BF_OpenFile(filename, &file_desc));

  BF_Block_Init(&block);
  CALL_BF(BF_AllocateBlock(file_desc, block));

  data=BF_Block_GetData(block);

  // first block of heap file takes the context HeapF
  memcpy(data, "HeapF", sizeof("HeapF"));

  BF_Block_SetDirty(block);
  CALL_BF(BF_UnpinBlock(block));
  CALL_BF(BF_CloseFile(file_desc));
  BF_Block_Destroy(&block);
  return HP_OK;
}

HP_ErrorCode HP_OpenFile(const char *fileName, int *fileDesc){
  char* data;
  BF_Block *block;
  int block_counter;
  char type[5];
  CALL_BF(BF_OpenFile(fileName, fileDesc));
  const int int_temp=*fileDesc;
  
  BF_Block_Init(&block);
  CALL_BF(BF_GetBlockCounter(int_temp, &block_counter));
  CALL_BF(BF_GetBlock(int_temp, block_counter-1, block));
  data=BF_Block_GetData(block);

  memcpy(type, data, 5);

  CALL_BF(BF_UnpinBlock(block));
  BF_Block_Destroy(&block);
  if(strcmp("HeapF",type)==0){
    return HP_OK;
  }
  else{
    printf("The file is not an Heap File.\n");
    return HP_ERROR;
  }
}

HP_ErrorCode HP_CloseFile(int fileDesc) {
  CALL_BF(BF_CloseFile(fileDesc));
  return HP_OK;
}

HP_ErrorCode HP_InsertEntry(int fileDesc, Record record) {
  int block_counter;
  char* data;
  int temp;
  BF_Block *block;
  BF_Block_Init(&block);
  
  CALL_BF(BF_GetBlockCounter(fileDesc, &block_counter));

  if(block_counter==1){
    // i now insert first record in Heap File
    CALL_BF(BF_AllocateBlock(fileDesc, block));
    data=BF_Block_GetData(block);

    temp=1;
    memcpy(data, &temp, sizeof(int));
    memcpy(data+sizeof(int), &record, sizeof(Record));
  }
  else{
    // there are records in the block
    CALL_BF(BF_GetBlock(fileDesc, block_counter-1, block));
    data=BF_Block_GetData(block);
    memcpy(&temp, data, sizeof(int));
    int free_space=BF_BLOCK_SIZE-sizeof(int)-temp*sizeof(Record);

    if(free_space>=sizeof(Record)){
      // current block has free space
      memcpy(data+sizeof(int)+temp*sizeof(Record), &record, sizeof(Record));
      // memcpy(data+BF_BLOCK_SIZE-free_space, &record, sizeof(Record));
      temp+=1;
      memcpy(data, &temp, sizeof(int));
      
    }
    else{
      //block full make new block
      CALL_BF(BF_UnpinBlock(block));
    
      CALL_BF(BF_AllocateBlock(fileDesc, block));
      data=BF_Block_GetData(block);

      temp=1;
      memcpy(data, &temp, sizeof(int));
      memcpy(data+sizeof(int), &record, sizeof(Record));
    }
  }
  BF_Block_SetDirty(block);
  CALL_BF(BF_UnpinBlock(block));
  BF_Block_Destroy(&block);
  return HP_OK;
}

void Print_Record(const Record *record){
  printf("%d,\"%s\",\"%s\",\"%s\"\n",record->id, record->name, record->surname, record->city);
  return;
}

HP_ErrorCode HP_PrintAllEntries(int fileDesc, char *attrName, void* value) {
  char* data;
  int block_counter;
  int block_entries;
  Record record_fetched;
  BF_Block *block;
  BF_Block_Init(&block);

  // all blocks in Heap File
  CALL_BF(BF_GetBlockCounter(fileDesc, &block_counter));
  for(int i=1; i<block_counter; i++){
    CALL_BF(BF_GetBlock(fileDesc, i, block));
    data=BF_Block_GetData(block);
    // block_entries are records of a block
    memcpy(&block_entries, data, sizeof(int));
    for(int j=0; j<block_entries; j++){
      memcpy(&record_fetched, data+sizeof(int)+j*sizeof(Record), sizeof(Record));
      if(value==NULL){
        Print_Record(&record_fetched);
      }
      else{
        if( strcmp(attrName, "id")==0 && record_fetched.id==(long)value){
          Print_Record(&record_fetched);
        }
        else if( strcmp(attrName, "name")==0 && strcmp(record_fetched.name, (char*)value)==0 ){
          Print_Record(&record_fetched);
        }
        else if(strcmp(attrName, "surname")==0 && strcmp(record_fetched.surname, (char*)value)==0){
          Print_Record(&record_fetched);
        }
        else if( strcmp(attrName, "city")==0 && strcmp(record_fetched.city, (char*)value)==0){
          Print_Record(&record_fetched);
        }
      }
    }
    CALL_BF(BF_UnpinBlock(block));
  }
  BF_Block_Destroy(&block);
  printf("\n");
  return HP_OK;
}

HP_ErrorCode HP_GetEntry(int fileDesc, int rowId, Record *record) {
  int block_counter;
  char* data;
  int my_temp;
  int records_in_block=( BF_BLOCK_SIZE-sizeof(int) )/sizeof(Record);
  BF_Block *block;
  BF_Block_Init(&block);

  block_counter=((rowId-1)/records_in_block)+1;

  CALL_BF(BF_GetBlock(fileDesc, block_counter, block));
  data=BF_Block_GetData(block);

  my_temp=rowId%records_in_block-1;
  if(my_temp<0){
    my_temp=7;
  }

  memcpy(record, data + sizeof(int) + my_temp*sizeof(Record), sizeof(Record));

  CALL_BF(BF_UnpinBlock(block));
  BF_Block_Destroy(&block);
  return HP_OK;
}
