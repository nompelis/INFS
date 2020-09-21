/************************************************************************
  Copyright 2020 Ioannis Nompelis
  Ioannis Nompelis  <nompelis@nobelware.com>           Created: 20200807
  Ioannis Nompelis  <nompelis@nobelware.com>     Last modified: 20200921
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>

int inUtils_File_HashXOR( const char *key, int key_size, const char *name )
{
   int fd, num_byte, send_byte;
   off_t offset, got_offset;
   char *buffer;

   buffer = (char *) malloc( ((size_t) key_size+1) * sizeof(char) );
   if( buffer == NULL ) return -1;
   memset( buffer, '\0', (size_t) key_size );

   fd = open( name, O_RDWR );
   if( fd == -1 ) return 1;

   num_byte = key_size;
   while( num_byte > 0 ) {
      int n=0;
      num_byte = (int) read( fd, buffer, (size_t) key_size );
#ifdef _DEBUG_
printf("num_byte= %d (bytes read into the buffer)\n",num_byte);
#endif

      while( n < num_byte ) {
         buffer[ n ] = buffer[ n ] ^ key[ n ];
         ++n;
      }

      offset = (off_t) num_byte;
      offset = -offset;
      got_offset = lseek( fd, offset, SEEK_CUR );
#ifdef _DEBUG_
printf("got_offset= %ld (offset after lseek()ing) \n",(long) got_offset);
#endif

      send_byte = (int) write( fd, buffer, (size_t) num_byte );
#ifdef _DEBUG_
printf("send_byte= %d (number of bytes over-written) END CYLCE\n", send_byte);
#endif
   }

   close( fd );

   free( buffer );

   return 0;
}


#ifdef _DRIVER_
int main( int argc, char *argv[] )
{
   (void) inUtils_File_HashXOR( argv[1], strlen(argv[1]), argv[2] );

   return(0);
}
#endif

