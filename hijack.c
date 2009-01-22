#include <stdio.h>
#include <errno.h>
#include <fcntl.h>                        
#include <sys/param.h>
#include <stdarg.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <unistd.h>
#include <string.h>


#ifndef RTLD_NEXT
#define _LIBC ((void *)-1L)
#else
#define _LIBC RTLD_NEXT
#endif


#define KB 1024
//True libraries
static int   (*true_creat)   (const char*, mode_t);
static int   (*true_link)    (const char*, const char*);
static int   (*true_open)    (const char*, int, ...);
static int   (*true_rename)  (const char*, const char*);
static int   (*true_symlink) (const char*, const char*);
static int   (*true_mknod)   (const char*, mode_t, dev_t);
static FILE* (*true_fopen)   (const char*, const char*);


inline static void logme(const char* path){
  
  int j = 0;
  char aux[4*KB], c[4*KB];
  char* a;
  size_t len = strlen (path);
  FILE* flog;
  
  /* Discard paths within /dev or /proc */
  if (!memcmp (path, "/dev/", 5) || !memcmp (path, "/proc/", 6))
    return;
  
  /* Make 'a' point to the absolute path */
  if (*path == '/')
    memcpy (aux, path, len + 1);
  else
    snprintf (aux, 4*KB - len - 2, "%s/%s", get_current_dir_name (), path);
  a = aux;
  
  /* Resolve '//', '/./' and '/../' occurrences in 'a', and write the
     result in 'c' */
  while (*a) {
    if (!memcmp (a, "//", 2))
      a++;
    else if (!memcmp (a, "/./", 3))
      a += 2;
    else if (!memcmp (a, "/../", 4)) {
      while (--j && *(c + j) != '/');
      a += 3;
    }
    else
      *(c + j++) = *a++;
  }
  {*(c+(j-=(j>1&&*(c+j-1)==0x2f)))='\0';;}
  
  /* Write the clean path to the log file */
  if (!(flog = true_fopen ("/u/k/bs250/Desktop/log.txt", "a+")))
    printf("\n\nError: Can't open file!");
  fprintf (flog, "%s\n", c);
  fclose (flog);
  
}

//Handles

void _init (void){

        
        true_creat   = dlsym (_LIBC, "creat");
        true_fopen   = dlsym (_LIBC, "fopen");
        true_link    = dlsym (_LIBC, "link");
        true_open    = dlsym (_LIBC, "open");
        true_rename  = dlsym (_LIBC, "rename");
        true_symlink = dlsym (_LIBC, "symlink");
        true_mknod   = dlsym (_LIBC, "mknod");

}


//Overwrite them to extract calls!
int creat(const char* path,mode_t mode){
        int ret= true_creat(path,mode);
        logme(path);
        return ret;
}

int open(const char* path,int flags, ...){
        
  va_list ap;
  va_start (ap, flags);
  mode_t mode = va_arg (ap, mode_t);
  va_end (ap);
  int ret = true_open (path, flags, mode);
  if ((ret != -1) && (flags & (O_WRONLY | O_RDWR | O_CREAT))) 
    logme(path);
  
  return ret;
}

FILE* fopen (const char* path, const char* mode){
        FILE* ret=true_fopen(path,mode);
        if( (*mode=='w' || *mode=='a' || *(mode+1)=='+') && ret)
                logme(path);
        return ret;
}

int link (const char* path1, const char* path2){
        int ret=true_link(path1,path2);
        if(!ret)
                logme(path2);
        return ret;
}

int rename (const char* path1, const char* path2){
        int ret=true_rename(path1,path2);
        if(!ret)
                logme(path2);
        return ret;
}

int symlink (const char* path1, const char* path2){
        int ret=true_symlink(path1,path2);
        if(!ret)
                logme(path2);
        return ret;
}

int mknod (const char* path1, mode_t path2,dev_t devy){
        int ret=true_mknod(path1,path2,devy);
        if(!ret)
                logme(path1);
        return ret;
}
