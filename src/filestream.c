#include <errno.h>
#include <malloc.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include "filestream.h"
#ifdef MEM_LEAK_CHECK
#include "CMemLeak.h"
#endif



#define IFSTREAM_BLOCK_SIZE 8192

/**************** Private Function Declarations *******************/
static void util_appendChunk(void *arg, const uint8_t *pChunk, uint32_t chunkLen);

/**************** Private Variable Declarations *******************/


/****************** Public Function Definitions *******************/
void ifstream_create(ifstream_t *self, ifstream_handler_t *handler){
   if(self != 0){
      memcpy(&self->handler,handler,sizeof(ifstream_handler_t));
   }
}

void ifstream_destroy(ifstream_t *self){
   (void) self;
}

ifstream_t *ifstream_new(ifstream_handler_t *handler){
   ifstream_t *self = (ifstream_t*) malloc(sizeof(ifstream_t));
   if(self != 0){
      ifstream_create(self,handler);
   }
   else{
      errno = ENOMEM;
   }
   return self;
}

void ifstream_delete(ifstream_t *self){
   if(self != 0){
      ifstream_destroy(self);
      free(self);
   }
}

void ifstream_open(ifstream_t *self){
   if( (self != 0) && (self->handler.open != 0)){
      self->handler.open(self->handler.arg);
   }
}

void ifstream_write(ifstream_t *self, const uint8_t *pChunk, uint32_t chunkLen){
   if( (self != 0) && (self->handler.write != 0)){
      self->handler.write(self->handler.arg,pChunk,chunkLen);
   }
}

void ifstream_close(ifstream_t *self){
   if( (self != 0) && (self->handler.close != 0)){
      self->handler.close(self->handler.arg);
   }
}

int ifstream_readBinaryFile(ifstream_t *self,const char *filename)
{
   if( (self != 0) && (filename != 0) ){
      uint32_t chunkLen = 0;
      char *chunk;
      chunk = (char*) malloc(IFSTREAM_BLOCK_SIZE);
      if( chunk != 0 ){
         FILE* fh = fopen(filename,"r");
         if (fh != 0){
            if(self->handler.open != 0){
               self->handler.open(self->handler.arg);
            }
            while(1){
               size_t len = fread(&chunk[0], 1, IFSTREAM_BLOCK_SIZE, fh);
               if (len > 0)
               {
                  chunkLen = (uint32_t) len;
                  if(self->handler.write != 0){
                     self->handler.write(self->handler.arg,(uint8_t*)chunk, chunkLen);
                  }
                  if (len < IFSTREAM_BLOCK_SIZE)
                  {
                     break; //done
                  }
               }
            }
            if(self->handler.close != 0){
               self->handler.close(self->handler.arg);
            }
            if (chunk != 0) free(chunk);
            fclose(fh);
            return 0;
         }
      }
      if (chunk != 0) free(chunk);
   }
   else{
      errno = EINVAL;
   }
   return -1;
}

int ifstream_readTextFile(ifstream_t *self,const char *filename)
{
   int retval = 0;
   if( (self != 0) && (filename != 0) )
   {
      FILE* fh = fopen(filename,"r");
      if (fh != 0)
      {
         retval =  ifstream_readTextFileFromHandle(self, fh);
         fclose(fh);
      }
   }
   else
   {
      retval = -1;
      errno = EINVAL;
   }
   return retval;
}

int ifstream_readTextFileFromHandle(ifstream_t *self, FILE *fh)
{
   int retval = 0;
   if( (self != 0) && (fh != 0) ){
      uint32_t chunkLen = 0;
      char *buf;
      char *chunk;
      buf = (char*) malloc(IFSTREAM_BLOCK_SIZE);
      chunk = (char*) malloc(IFSTREAM_BLOCK_SIZE);
      if( (buf != 0) && (chunk != 0) )
      {
         if(self->handler.open != 0)
         {
            self->handler.open(self->handler.arg);
         }

         while(fgets(buf, IFSTREAM_BLOCK_SIZE, fh) != NULL)
         {
            size_t len = strlen(buf);
            assert(len>0);
            //dos2unix file ending
            if( (len>=2) && buf[len-2] == '\r')
            {
               len--;
               buf[len-1]='\n';
            }
            if(len+chunkLen < IFSTREAM_BLOCK_SIZE)
            {
               memcpy(&chunk[chunkLen],buf,len);
               chunkLen += (uint32_t) len;
            }
            else
            {
               if(self->handler.write != 0)
               {
                  self->handler.write(self->handler.arg,(uint8_t*)chunk,chunkLen);
               }
               memcpy(&chunk[0],buf,len);
               chunkLen = (uint32_t) len;
            }
         }
         if( (chunkLen > 0) && (self->handler.write != 0))
         {
            self->handler.write(self->handler.arg,(uint8_t*)chunk,chunkLen);
         }
         if(self->handler.close != 0)
         {
            self->handler.close(self->handler.arg);
         }
      }
      if (buf != 0) free(buf);
      if (chunk != 0) free(chunk);
   }
   else
   {
      errno = EINVAL;
      retval = -1;
   }
   return retval;
}

adt_bytearray_t *ifstream_util_readTextFile(const char *filename)
{
   adt_bytearray_t *buf = adt_bytearray_new(0);
   if (buf != 0)
   {
      ifstream_handler_t handler;
      ifstream_t ifstream;
      memset(&handler, 0, sizeof(handler));
      handler.arg = buf;
      handler.write = util_appendChunk;
      ifstream_create(&ifstream, &handler);
      if (ifstream_readTextFile(&ifstream, filename) != 0)
      {
         adt_bytearray_delete(buf);
         buf = (adt_bytearray_t*) 0;
      }
      return buf;
   }
   return (adt_bytearray_t*) 0;
}


/***************** Private Function Definitions *******************/
static void util_appendChunk(void *arg, const uint8_t *pChunk, uint32_t chunkLen)
{
   adt_bytearray_t *buf = (adt_bytearray_t*) arg;
   if (buf != 0 && pChunk != 0 && chunkLen > 0)
   {
      adt_bytearray_append(buf, pChunk, chunkLen);
   }
}
