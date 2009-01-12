#include <stdio.h>
#include <errno.h>
#include <fcntl.h>			  
#include <sys/param.h>
#include <stdarg.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <unistd.h>
#include <string.h>

//True libraries
static int   (*true_creat)   (const char*, mode_t);
static int   (*true_link)    (const char*, const char*);
static int   (*true_open)    (const char*, int, ...);
static int   (*true_rename)  (const char*, const char*);
static int   (*true_symlink) (const char*, const char*);
static int   (*true_mknod)   (const char*, mode_t, dev_t);
static FILE* (*true_fopen)   (const char*, const char*);


static void* xdlsym(void* handle, char* symbol){

	void* ret=dlsym(handle,symbol);
	return ret;
}



//Handles

void _init (void){
	char libc[16];
	sprintf (libc, "libc.so.%d",6);
	void *libc_handle=dlopen(libc,RTLD_LAZY);
	if (!libc_handle)
		lp_fail ("%s", dlerror ());

	
	true_creat   = xdlsym (libc_handle, "creat");
	true_fopen   = xdlsym (libc_handle, "fopen");
	true_link    = xdlsym (libc_handle, "link");
	true_open    = xdlsym (libc_handle, "open");
	true_rename  = xdlsym (libc_handle, "rename");
	true_symlink = xdlsym (libc_handle, "symlink");
	true_mknod   = xdlsym (libc_handle, "mknod");

}


//Overwrite them to extract calls!
int creat(const char* path,mode_t mode){
	int ret= true_open(path,O_CREAT | O_WRONLY | O_TRUNC,mode);
	printf(path);
	return ret;
}

int open(const char* path,int flags, ...){
	va_list ap;
	
	va_start (ap, flags);
	mode_t mode = va_arg (ap, mode_t);
	va_end (ap);
	int ret = true_open (path, flags, mode);
	if ((ret != -1) && (flags & (O_WRONLY | O_RDWR | O_CREAT))) 
	printf(path);
	
	return ret;
}

FILE* fopen (const char* path, const char* mode){
	FILE* ret=true_open(path,mode);
	if( (*mode=='w' || mode=='a' || *(mode+1)=='+') && ret)
		printf(path);
	return ret;
}

int link (const char* path1, const char* path2){
	int ret=true_link(path1,path2);
	if(!ret)
		printf(path2);
	return ret;
}

int rename (const char* path1, const char* path2){
	int ret=true_rename(path1,path2);
	if(!ret)
		printf(path2);
	return ret;
}

int symlink (const char* path1, const char* path2){
	int ret=true_symlink(path1,path2);
	if(!ret)
		printf(path2);
	return ret;
}

int mknod (const char* path1, const char* path2,dev_t devy){
	int ret=true_mknod(path1,path2,devy);
	if(!ret)
		printf(path2);
	return ret;
}
