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

#include <sys/time.h>

#include "infs.h"

// a dummy function that prints a string
int dummy_func( void *arg )
{
   printf("Hello: \"%s\"\n", (char *) arg );
   return 0;
}


int main() {
   int num;
   struct my_file *files;
   int iret;
   int (*func_p)( void * );

   (void) infs_ReadDir(&num, &files, ".");
   iret = infs_FilterExt(num, files, ".tif");
printf("Found %d files matching \n",iret);
   (void) infs_FilterExtReset(num, files);

   free(files);

func_p = &dummy_func;
(void) infs_TraverseDirTree( ".", 0,
             func_p, "DIR_pre World!!",
             func_p, "DIR_post World!",
             func_p, "REG World!" );

   return(0);
}

