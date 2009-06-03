/*
 * 		Copyright 2009 (c) Bpack- Bati Sengul <batieatworld@gmail.com>
 * 							      Mike Bush <mpbush@gmail.com>
 * 
 * 					http://www.bpack.co.uk
 * 
 *      This program is free software; you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation; either version 2 of the License, or
 *      (at your option) any later version.
 *      
 *      This program is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *      
 *      You should have received a copy of the GNU General Public License
 *      along with this program; if not, write to the Free Software
 *      Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 *      MA 02110-1301, USA.
 */


#include <stdio.h>
#include <errno.h>
#include <fcntl.h>                        
#include <sys/param.h>
#include <stdarg.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <unistd.h>
#include <string.h>
#include <aio.h>
#include "../config.h"
#define _HAVE_64_ (HAVE_OPEN64 && HAVE_CREAT64 && HAVE_TRUNCATE64 && HAVE_FOPEN64 && HAVE_FREOPEN64)


#ifndef RTLD_NEXT
#define _LIBC ((void *)-1L)
#else
#define _LIBC RTLD_NEXT
#endif


//True libraries
static int   (*true_creat)   (const char*, mode_t);
static int   (*true_link)    (const char*, const char*);
static int   (*true_open)    (const char*, int, ...);
static int   (*true_rename)  (const char*, const char*);
static int   (*true_symlink) (const char*, const char*);
static int   (*true_mknod)   (const char*, mode_t, dev_t);
static FILE* (*true_fopen)   (const char*, const char*);
static FILE* (*true_freopen) (const char*, const char* ,FILE*);
static int   (*true_truncate)(const char*, off_t);

#if _HAVE_64_
static int   (*true_creat64)   (const char*, mode_t);
static int   (*true_open64)    (const char*, int, ...);
static FILE* (*true_fopen64)   (const char*, const char*);
static FILE* (*true_freopen64) (const char*, const char* ,FILE*);
//static int   (*true_truncate64)(const char*, off64_t);
#endif

inline static void logme(const char* path){
  
  int j = 0;
  char aux[4*1024], c[4*1024];
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
    snprintf (aux, 4*1024 - len - 2, "%s/%s", get_current_dir_name (), path);
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
  if (!(flog = true_fopen ("/tmp/hijack_log.txt", "a+")))
    printf("\n\nError: Can't open file!");
  fprintf (flog, "%s\n", c);
  fclose (flog);
  
}

//Handles

void hiinit (){
	
        true_creat   = dlsym (_LIBC, "creat");
        true_fopen   = dlsym (_LIBC, "fopen");
        true_link    = dlsym (_LIBC, "link");
        true_open    = dlsym (_LIBC, "open");
        true_rename  = dlsym (_LIBC, "rename");
        true_symlink = dlsym (_LIBC, "symlink");
        true_mknod   = dlsym (_LIBC, "mknod");
	true_freopen = dlsym (_LIBC, "freopen");
	true_truncate= dlsym (_LIBC, "truncate");
	
	#if _HAVE_64_
	true_open64  = dlsym (_LIBC, "open64");
	true_freopen64=dlsym (_LIBC, "freopen64");
	//true_truncate64=dlsym(_LIBC, "truncate64");
	true_fopen64 = dlsym (_LIBC, "fopen64");
	true_creat64 = dlsym (_LIBC, "creat64");
	#endif

}


//Overwrite them to extract calls!
int creat(const char* path,mode_t mode){
	hiinit();
        int ret= true_creat(path,mode);
	if(ret!=-1)
        	logme(path);
        return ret;
}

int open(const char* path,int flags, ...){
        
	hiinit();
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
	hiinit();
        FILE* ret=true_fopen(path,mode);
        if( (*mode=='w' || *mode=='a' || *(mode+1)=='+') && ret)
                logme(path);
        return ret;
}

int link (const char* path1, const char* path2){
	hiinit();
        int ret=true_link(path1,path2);
        if(!ret)
                logme(path2);
        return ret;
}

int rename (const char* path1, const char* path2){
	hiinit();
        int ret=true_rename(path1,path2);
        if(!ret)
                logme(path2);
        return ret;
}

int symlink (const char* path1, const char* path2){
	hiinit();
        int ret=true_symlink(path1,path2);
        if(!ret)
                logme(path2);
        return ret;
}

int mknod (const char* path1, mode_t path2,dev_t devy){
	hiinit();
        int ret=true_mknod(path1,path2,devy);
        if(!ret)
                logme(path1);
        return ret;
}

FILE* freopen (const char* path, const char* mode, FILE* file){
	hiinit();

	FILE* ret;
	ret=true_freopen(path,mode,file);
	
        if( (*mode=='w' || *mode=='a' || *(mode+1)=='+') && ret)
		logme (path);
	
	return ret;
}

int truncate (const char* path, off_t len){
	hiinit();

	int ret=true_truncate(path, len);

	if ( ret!=-1)
		logme(path);

	return ret;
}

#if _HAVE_64_

FILE* freopen64 (const char* path, const char* mode, FILE* file){
	hiinit();

	FILE* ret;
	ret=true_freopen64(path,mode,file);
	if( (*mode=='w' || *mode=='a' || *(mode+1)=='+') && ret)
		logme(path);

	return ret;
}

FILE* fopen64(const char* path, const char* mode){
	hiinit();
	FILE* ret;
	ret=true_fopen64(path,mode);
	
        if( (*mode=='w' || *mode=='a' || *(mode+1)=='+') && ret)
		logme(path);

	return ret;
}

/*int truncate64 (const char* path, off64_t len){
	printf("\n\nTRUNCATE64:%s\n\n",path);
	hiinit();

	int ret=true_truncate64(path, len);

	if ( ret!=-1)
		logme(path);

	return ret;
}*/

int open64 (const char* path,int flags, ...){
	hiinit();
        
  	va_list ap;
 	 va_start (ap, flags);
  mode_t mode = va_arg (ap, mode_t);
  va_end (ap);
  int ret = true_open64 (path, flags, mode);
  if ((ret != -1) && (flags & (O_WRONLY | O_RDWR | O_CREAT))) 
    logme(path);
  
  return ret;
}

int creat64 (const char* path,mode_t mode){
        int ret= true_creat64(path,mode);
	if(ret!=-1)
        	logme(path);
        return ret;
}
#endif

