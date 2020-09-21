/************************************************************************
  A light-weight collection of simple functions to handle files in the
  unix filesystem. The collection is small but is growing...

  Copyright 2014-2020 Ioannis Nompelis
  Ioannis Nompelis  <nompelis@nobelware.com>           Created: 20140925
  Ioannis Nompelis  <nompelis@nobelware.com>     Last modified: 20200806
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>

#include <sys/time.h>


#include "infs.h"


/*
 * Function to put escape characters when filenames have special characters
 * Make sure pointer efile is pointing to a large enough filename...
 * This function is DANGEROUS for so many reasons...
 * Ioannis Nompelis <nompelis#nobelware.com> Created:       20150616
 * Ioannis Nompelis <nompelis#nobelware.com> Last modified: 20150616
 */
int infs_Utils_FilenameEscape(char *file, char *efile)
{
   int n,m;

   if(file == NULL && efile == NULL) return(1);

   n = 0;
   m = 0;
   while(file[n] != '\0' && file[n] != '\n') {
      if(file[n] == ' ' ||
         file[n] == '\\' ||
         file[n] == '#' ||
         file[n] == '\'' ||
         file[n] == '&' ||
         file[n] == '$' ||
         file[n] == '(' ||
         file[n] == ')' ||
         file[n] == '[' ||
         file[n] == ']' ||
         file[n] == '{' ||
         file[n] == '}' 
        ) {
         efile[m] = '\\';
         ++m;
      }
      efile[m] = file[n];
      ++n;
      ++m;
   }
   efile[m] = file[n];

   return(0);
}

/*
 * Function to read the contents of a given directory into the info holding
 * structures.
 * Ioannis Nompelis <nompelis#nobelware.com> Created:       20120505
 * Ioannis Nompelis <nompelis#nobelware.com> Last modified: 20141002
 */
int infs_ReadDir(int *num_entries, struct my_file **files, char *dir)
{
   int n,num;
   DIR *dp;
   struct dirent *entryp;
   char ctemp[NAME_MAX]; // from manual page I found it is 256 or NAME_MAX=256
   int i,j,it;


   // make sure the user gave us a valid directory name
   if(dir == NULL) {
      fprintf(stdout," e Bad directory name or bad usage of schnauzer\n");
      return(1);
   }

   // open the directory
   //fprintf(stdout," i Opening directory \"%s\" \n",dir);
   dp = opendir(dir);
   if(dp == NULL) {
      fprintf(stdout," e Could not open directory!\n");
      return(2);
   }

   // count entries in directory
   num = 0;
   while( (entryp = readdir(dp)) ) {
   // printf("ENTRY: %s\n", entryp->d_name);
      ++num;
   }
   (void) closedir(dp);


   *files = (struct my_file *) malloc(num*sizeof(struct my_file));
   if(*files == NULL) {
      fprintf(stdout," e Could not allocate files list!\n");
      return(3);
   }


   dp = opendir(dir);
   for(n=0;n<num;++n) {
      struct stat fs;
      int type;

      entryp = readdir(dp);

      (void) stat(entryp->d_name, &fs);
      if(S_ISDIR(fs.st_mode) != 0) type = 1;
      if(S_ISCHR(fs.st_mode) != 0) type = 2;
      if(S_ISBLK(fs.st_mode) != 0) type = 3;
      if(S_ISREG(fs.st_mode) != 0) type = 4;
      if(S_ISLNK(fs.st_mode) != 0) type = 5;
      if(S_ISFIFO(fs.st_mode) != 0) type = 6;
      if(S_ISSOCK(fs.st_mode) != 0) type = 7;

      memset( (*files)[n].name , 0, (size_t) NAME_MAX);
      sprintf((*files)[n].name,"%s",entryp->d_name);
      (*files)[n].type = type;
      (*files)[n].filter = 0;
   // printf("ENTRY: %s       (%d) \n",(*files)[n].name, (*files)[n].type);
   }
   (void) closedir(dp);

   *num_entries = num;

   // sort the filenames
   for(i=0;i<num;++i) {
      for(j=0;j<num-1;++j) {
         if(strncmp((*files)[j].name,
                    (*files)[j+1].name, (size_t) NAME_MAX)> 0) {
            strncpy(ctemp, (*files)[j].name, (size_t) NAME_MAX);
            it = (*files)[j].type;

            strncpy((*files)[j].name, (*files)[j+1].name, (size_t) NAME_MAX);
            (*files)[j].type = (*files)[j+1].type;

            strncpy((*files)[j+1].name, ctemp, (size_t) NAME_MAX);
            (*files)[j+1].type = it;
         }
      }
//printf("----> %s %d \n",(*files)[i].name, num);
   }

   return(0);
}


/*
 * Function to filter files by extension
 * Returns the number of files encountered with given extension
 * Ioannis Nompelis <nompelis#nobelware.com> Created:       20140925
 * Ioannis Nompelis <nompelis#nobelware.com> Last modified: 20140925
 */
int infs_FilterExt(int num_entries, struct my_file *files, char *ext)
{
   int num,ns,nf;
   int n;


   if(ext == NULL) {
      fprintf(stdout," e  Extension is null! \n");
      return(1);
   }

   if(files == NULL) {
      fprintf(stdout," e  List of files is null! \n");
      return(1);
   }


   ns = 0;
   while(ext[ns] != '\0') ++ns;
//printf("SIZE OF EXT= %d \n",ns);

   num = 0;
   for(n=0;n<num_entries;++n) {

      // filter out directories immediatelly
      if(files[n].type != 4) files[n].filter = 1;

      nf = 0;
      while(files[n].name[nf] != '\0') ++nf;
//printf("LENGTH= %d  %s \n",nf,files[n].name);//HACK

      // filter out files of size smaller than the extension!
      if(nf >= ns) {
         char *p = &( files[n].name[nf-ns] );

         if(strncmp(p,ext, (size_t) ns) != 0) {
            files[n].filter = 1;
         }

      } else {
         files[n].filter = 1;
      }

//printf("FILTER IS= %d (%s) \n",files[n].filter, files[n].name);

      if(files[n].filter == 0) ++num;
   }

   return(num);
}


/*
 * Function to reset filter mask for a list of files
 * Ioannis Nompelis <nompelis#nobelware.com> Created:       20140925
 * Ioannis Nompelis <nompelis#nobelware.com> Last modified: 20140925
 */
void infs_FilterExtReset(int num_entries, struct my_file *files)
{
   int n;

   for(n=0;n<num_entries;++n) files[n].filter = 0;
}


/*
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
*/


/*
 * Function to traverse the entire depth of a directory tree and perform
 * certain operations to each of the regular files encountered in the
 * directory.
 * Ioannis Nompelis <nompelis#nobelware.com> Created:       20200806
 * Ioannis Nompelis <nompelis#nobelware.com> Last modified: 20200806
 */

int infs_TraverseDirTree( char *dir, int depth,
                 int (*func_dir_pre)( void *, char * ),  void *arg_dir_pre,
                 int (*func_dir_post)( void *, char * ), void *arg_dir_post,
                 int (*func_reg)( void *, char * ),      void *arg_reg )
{
#ifdef _DEBUG_
   char FUNC[] = "infs_TraverseDirTree";
#endif
   DIR *dp;
   struct dirent entry, *result;
   int ierr=0;


   // make sure the user gave us a valid directory name
   if( dir == NULL ) {
      fprintf( stdout," e  Bad directory name or bad usage of schnauzer\n" );
      return(1);
   }

   // open the directory
#ifdef _DEBUG_
   fprintf( stdout," [%s:%d]  Opening directory \"%s\" \n",FUNC,depth, dir);
#endif
   dp = opendir(dir);
   if( dp == NULL ) {
      fprintf (stdout," e   Could not open directory!\n" );
      return(2);
   }

   // change current directory
   if( depth > 0 ) chdir( dir );

   // sweep through the contents of the directory and use recersion
#ifdef _DEBUG_
   fprintf( stdout," [%s]  Traversing through directory's contents \n",FUNC);
#endif
   while( ierr == 0 ) {
      struct stat fs;
      int type, ient, istat=-100;

      // read the next entry; do error trapping and make termination decision
      ient = readdir_r( dp, &entry, &result );
      if( ient != 0 ) {
         fprintf( stdout, " e  Could not readdir() (%d) \n", ient );
         ierr = -1;
      } else {
         if( result != NULL ) {
#ifdef _DEBUG_
            fprintf( stdout," [%s]  About to stat() \"%s\" \n",FUNC,
                     entry.d_name );
#endif
            istat = stat( entry.d_name, &fs );
            if( istat != 0 ) {
               fprintf( stdout, " e  Could not stat() entry\n" );
               perror("INFS stat()");
               ierr = -2;
            }
         } else {
#ifdef _DEBUG_
            fprintf( stdout," [%s]  End of directory \"%s\" \n",FUNC, dir);
#endif
            ierr = 1;
         }
      }

      // continue if no errors and no termination
      if( istat == 0 ) {
         if(S_ISDIR( fs.st_mode) != 0 ) type = 1;
         if(S_ISCHR( fs.st_mode) != 0 ) type = 2;
         if(S_ISBLK( fs.st_mode) != 0 ) type = 3;
         if(S_ISREG( fs.st_mode) != 0 ) type = 4;
         if(S_ISLNK( fs.st_mode) != 0 ) type = 5;
         if(S_ISFIFO( fs.st_mode) != 0 ) type = 6;
         if(S_ISSOCK( fs.st_mode) != 0 ) type = 7;
#ifdef _DEBUG_
         fprintf( stdout, "  ENTRY: \"%s\"  (%d) \n", entry.d_name, type );
#endif

         // execute code for regular file
         // (NO ERROR TRAPPING)
         if( type == 4 ) {
            if( func_reg != NULL ) (*func_reg)( arg_reg, entry.d_name );
         }

         // execute code for directory, recurse, and execute code...
         // (NO ERROR TRAPPING)
         if( type == 1 ) {
            if( strcmp( entry.d_name, "." ) != 0 &&          // not in self
                strcmp( entry.d_name, ".." ) != 0 ) {        // not in parent

               if( func_dir_pre != NULL ) {
                  (*func_dir_pre)( arg_dir_pre, entry.d_name );
               }

               ient = infs_TraverseDirTree( entry.d_name, depth+1,
                                   func_dir_pre,  arg_dir_pre,
                                   func_dir_post, arg_dir_post,
                                   func_reg,      arg_reg );
               if( ient != 0 ) {
#ifdef _DEBUG_
                  fprintf( stdout," [%s]  Returned error! \"%d\" \n",FUNC,ient);
#endif
                  ierr = -3;
               }

               if( func_dir_post != NULL ) {
                  (*func_dir_post)( arg_dir_post, entry.d_name );
               }
#ifdef _DEBUG_
            } else {
               fprintf( stdout, " [%s]  Skipping \"%s\"\n",FUNC,entry.d_name);
#endif
            }
         }
      }
   }
#ifdef _DEBUG_
   fprintf( stdout," [%s:%d]  Closing directory \"%s\" \n",FUNC,depth, dir);
#endif
   (void) closedir(dp);

   // change current directory
   if( depth > 0 ) chdir( ".." );

   if( ierr != 1 ) {
      return ierr;
   } else {
      return 0;
   }
}

