#include <qapi_fs.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <stdio.h>

extern int printf(const char *format, ...);

// change to FILE
FILE *fopen(const char *path, const char *mode){

	uint32_t flags;

	if(strcmp(mode,"r") == 0 || strcmp(mode,"rb") == 0)
		flags = QAPI_FS_O_RDONLY_E;
	else if(strcmp(mode,"r+") == 0)
		flags = QAPI_FS_O_RDWR_E;
	else if(strcmp(mode,"w") == 0)
		flags = QAPI_FS_O_WRONLY_E | QAPI_FS_O_TRUNC_E | QAPI_FS_O_CREAT_E;
	else if(strcmp(mode,"w+") == 0)
		flags = QAPI_FS_O_RDWR_E | QAPI_FS_O_TRUNC_E | QAPI_FS_O_CREAT_E;
	else if(strcmp(mode,"a") == 0)
		flags = QAPI_FS_O_WRONLY_E | QAPI_FS_O_CREAT_E;
	else if(strcmp(mode,"a+") == 0)
		flags = QAPI_FS_O_RDWR_E | QAPI_FS_O_CREAT_E;
	else
		return NULL;

	FILE *fp = malloc(sizeof(int));
	if(qapi_FS_Open ( path, flags, (int*)fp) == QAPI_OK){
		return fp;
	}
	else{
		free(fp);
		return NULL;
	}

}

// change to FILE
int fclose(FILE *fp){
	int *fd = (int*)fp;
	if(fd){
		if(qapi_FS_Close(*fd) == QAPI_OK){
			free(fd);
			return 0;
		}else
			return '\0';
	}else
		return '\0';
}

// change to FILE
int fseek(FILE *fp, long offset, int whence){
	int *fd = (int*)fp;
	int whence_flags;
	qapi_FS_Offset_t actual_offset;

	if(whence == 0)
		whence_flags = QAPI_FS_SEEK_SET_E;
	else if(whence == 1)
		whence_flags = QAPI_FS_SEEK_CUR_E; 
	else if(whence == 2)
		whence_flags = QAPI_FS_SEEK_END_E;

	if(qapi_FS_Seek ( *fd, (qapi_FS_Offset_t)offset, whence_flags, &actual_offset ) == QAPI_OK){
		return 0;
	}else{
		return -1;
	}
}

/*
*	Just return file size as theres no way knowing from current position with QAPI
*/
long ftell(FILE *fp){
	int *fd = (int*)fp;
	struct qapi_FS_Stat_Type_s finfo;
	if(qapi_FS_Stat_With_Handle(*fd,&finfo) == QAPI_OK){
		return finfo.st_size;
	}else{
		return -1;
	}
}

int fstat(int fd, struct stat *statbuf){
	struct qapi_FS_Stat_Type_s st;
	if(qapi_FS_Stat_With_Handle(fd, &st) == QAPI_OK){
		statbuf->st_dev = st.st_dev;
		statbuf->__st_dev_padding = 0;
		statbuf->__st_ino_truncated = 0;
		statbuf->st_mode = st.st_Mode;
		statbuf->st_nlink = st.st_nlink;
		statbuf->st_uid = st.st_uid;
		statbuf->st_gid = st.st_gid;
		statbuf->st_rdev = st.st_rdev;
		statbuf->__st_rdev_padding = 0;
		statbuf->st_size = st.st_size;
		statbuf->st_blksize = st.st_blksize;
		statbuf->st_blocks = st.st_blocks;
		//statbuf->st_atim.tv_sec = st.st_atime;
		//statbuf->st_mtim.tv_sec = st.st_mtime;
		//statbuf->st_ctim.tv_sec = st.ct_time;
		statbuf->st_ino = st.st_ino;
		return 0;
	}
	else{
		return -1;
	}
}
  
int lstat(const char *pathname, struct stat *statbuf){
	struct qapi_FS_Stat_Type_s st;
	if(qapi_FS_Stat (pathname, &st) == QAPI_OK){
		statbuf->st_dev = st.st_dev;
		statbuf->__st_dev_padding = 0;
		statbuf->__st_ino_truncated = 0;
		statbuf->st_mode = st.st_Mode;
		statbuf->st_nlink = st.st_nlink;
		statbuf->st_uid = st.st_uid;
		statbuf->st_gid = st.st_gid;
		statbuf->st_rdev = st.st_rdev;
		statbuf->__st_rdev_padding = 0;
		statbuf->st_size = st.st_size;
		statbuf->st_blksize = st.st_blksize;
		statbuf->st_blocks = st.st_blocks;
		//statbuf->st_atim.tv_sec = st.st_atime;
		//statbuf->st_mtim.tv_sec = st.st_mtime;
		//statbuf->st_ctim.tv_sec = st.ct_time;
		statbuf->st_ino = st.st_ino;
		return 0;
	}
	else{
		return -1;
	}
}

// change to FILE
size_t fread(void *ptr, size_t size, size_t nmemb, FILE *fp){
	int *fd = (int*)fp;
	struct qapi_FS_Stat_Type_s finfo;
	uint32_t read_bytes = 0;

	if(ptr && fp){	
		if( qapi_FS_Read (*fd, ptr, nmemb, &read_bytes) == QAPI_OK){			
			return read_bytes;
		}else{
			return 0;		
		}
	}else
		return 0;

}


size_t __wrap_fwrite(const void *ptr, size_t size, size_t nmemb, FILE *fp){
	int *fd = (int*)fp;
	uint32_t written_bytes = -1;
	if(ptr && fp){
		qapi_FS_Write (*fd, ptr,nmemb,&written_bytes);
	}

	return written_bytes;

}



DIR *opendir(const char *name){
 	qapi_FS_Iter_Handle_t dir_handle = malloc(sizeof(qapi_FS_Iter_Handle_t));

	if(qapi_FS_Iter_Open ( name, &dir_handle ) == QAPI_OK){
		return (DIR*)dir_handle;
	}else{
 		return NULL;
	}
 }

 struct dirent *readdir(DIR *dirp){
 	static struct dirent d;
 	struct qapi_FS_Iter_Entry_s iter_entry;
 	qapi_FS_Iter_Handle_t dir_handle;

 	if(dirp){
 		dir_handle = (qapi_FS_Iter_Handle_t)dirp;
 		if(qapi_FS_Iter_Next (dir_handle,&iter_entry) == QAPI_OK){
			if(iter_entry.file_Path[0]){
				strncpy(d.d_name,iter_entry.file_Path,256);
				return &d;
			}else
				return NULL;
 		}else{
 			return NULL;
 		}
 	}else
 		return NULL;

 }

int closedir(DIR *dirp){
	qapi_FS_Iter_Handle_t dir_handle;

	if(dirp){
		dir_handle = (qapi_FS_Iter_Handle_t)dirp;
		if(qapi_FS_Iter_Close (dir_handle) == QAPI_OK){
			free(&dir_handle);
			return 0;
		}
		else{
			return -1;
		}
	}else
		return -1;
}


int rmdir (const char *filename){
	if(filename == NULL) return -1;
	if(qapi_FS_Rm_Dir(filename) == QAPI_OK)
		return 0;
	else
		return -1;
}


int mkdir(const char *pathname, mode_t mode){
	if(pathname == NULL) return -1;
	if(qapi_FS_Mk_Dir (pathname, mode))
		return 0;
	else
		return -1;

}

int unlink (const char *filename){
	if(filename == NULL) return -1;
	if(qapi_FS_Unlink(filename) == QAPI_OK)
		return 0;
	else
		return -1;
}

int truncate(const char *path, off_t length){
	if(path == NULL) return -1;
	if(qapi_FS_Truncate (path, length ) == QAPI_OK)
		return 0;
	else
		return -1;
}

int rename(const char *old, const char *new){
	if(old == NULL && new == NULL) return -1;
	if(qapi_FS_Rename (old,new ) == QAPI_OK)
		return 0;
	else
		return -1;
}

/*

The return value is 0 if the access is permitted, and -1 otherwise. 
(In other words, treated as a predicate function, access returns true if the requested access is denied.)

These macros are defined in the header file unistd.h for use as the how argument to the access function. 
The values are integer constants.

Macro: int R_OK
Flag meaning test for read permission.

Macro: int W_OK
Flag meaning test for write permission.

Macro: int X_OK
Flag meaning test for execute/search permission.

Macro: int F_OK
Flag meaning test for existence of the file.

EACCES The access specified by how is denied.
ENOENT The file doesn’t exist.
EROFS Write permission was requested for a file on a read-only file system.

*/
int access (const char *filename, int how){

	return 0;

}



