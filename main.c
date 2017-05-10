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


int main() {
   int num;
   struct my_file *files;
   int iret;


   (void) infs_ReadDir(&num, &files, ".");
   iret = infs_FilterExt(num, files, ".tif");
printf("Found %d files matching \n",iret);
   (void) infs_FilterExtReset(num, files);

   free(files);

   return(0);
}

