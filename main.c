/************************************************************************
  Copyright 2014 Ioannis Nompelis
  Ioannis Nompelis  <nompelis@nobelware.com>           Created: 20140925
  Ioannis Nompelis  <nompelis@nobelware.com>     Last modified: 20140925
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>

#include <sys/time.h>

#include "infs.h"

// a dummy function
int dummy_func0( void *arg, char *name )
{
   FILE *fp;
// printf("Hello: \"%s\" (%s)\n", (char *) arg, name );
   char cmd[512];
   sprintf( cmd, "%s > KEY", (char *) arg) ;
// system( cmd );
   printf("CMD: \"%s\"\n", cmd );
   sprintf( cmd, "%s >> KEY", (char *) arg) ;
// system( cmd );
   printf("CMD: \"%s\"\n", cmd );
   sprintf( cmd, "openssl enc -aes-256-cbc -a -in %s > %s.enc < KEY",
             name, name );
   fp = popen( cmd, "w" );
   fprintf( fp, "%s\n", (char *) arg );
   usleep( 200000 );
   fprintf( fp, "%s\n", (char *) arg );
   pclose( fp );
   printf("CMD: \"%s\"\n", cmd );
   sprintf( cmd, "rm -f KEY" );
// system( cmd );
   printf("CMD: \"%s\"\n", cmd );
   sprintf( cmd, "rm -f %s; mv %s.enc %s", name, name, name );
// system( cmd );
   printf("CMD: \"%s\"\n", cmd );
   return 0;
}

// a function taken from INutils
#include "xorhash.c"

int dummy_func( void *arg, char *name )
{
   char *key = (char *) arg;
   int num = strlen( key );

#ifdef _DEBUG_
   fprintf( stdout, "Dummy func: \"%s\" (string was: %s)\n",name,(char *) arg);
#else
   (void) inUtils_File_HashXOR( key, num, name );
#endif

   return 0;
}


int main( int argc, char *argv[] ) {
   int num;
   struct my_file *files;
   int iret;
   int (*func_p)( void *, char * );

// (void) infs_ReadDir(&num, &files, ".");
// iret = infs_FilterExt(num, files, ".tif");
//printf("Found %d files matching \n",iret);
// (void) infs_FilterExtReset(num, files);
// free(files);

// the TESTER
//inUtils_File_HashXOR( argv[1], strlen( argv[1] ), argv[2] );//HACK

func_p = &dummy_func;
(void) infs_TraverseDirTree( ".", 0,
             NULL, "DIR_pre World!!",
             NULL, "DIR_post World!",
          // func_p, "DUMMY_STRING" );
             func_p, (char *) (argv[1]));

   return(0);
}

