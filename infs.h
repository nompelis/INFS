
// this (extensible) structure holds info for any entry found in a directory
struct my_file {
   int filter;         // filtering flag(s) for my use
   int type;           // type based on the filesystem
   char name[256];     // "infinity is set to 256" (I read that somewhere)
};


int infs_Utils_FilenameEscape(char *file, char *efile);

int infs_ReadDir(int *num_entries, struct my_file **files, char *dir);

int infs_FilterExt(int num_entries, struct my_file *files, char *ext);

void infs_FilterExtReset(int num_entries, struct my_file *files);


