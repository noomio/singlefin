#include <qapi_fs.h>
#include  <sys/stat.h>

int *fopen(const char *path, const char *mode){

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

	int *fd = malloc(1);
	if(qapi_FS_Open ( path, flags, fd) == QAPI_OK)
		return fd;
	else{
		free(fd);
		return NULL;
	}

}

int fclose(int *fd){
	if(fd){
		if(qapi_FS_Close(fd) == QAPI_OK){
			free(fd);
			return 0;
		}else
			return '\0';
	}else
		return '\0';
}

int fseek(int *fd, long offset, int whence){
	int whence_flags;
	qapi_FS_Offset_t actual_offset;

	if(whence == 0)
		whence_flags = QAPI_FS_SEEK_SET_E;
	else if(whence == 1)
		whence_flags = QAPI_FS_SEEK_CUR_E; 
	else if(whence == 2)
		whence_flags = QAPI_FS_SEEK_END_E;

	if(qapi_FS_Seek ( fd, (qapi_FS_Offset_t)offset, whence_flags, &actual_offset ) == QAPI_OK){
		return 0;
	}else
		return -1;
}

/*
*	Just return file size as theres no way knowing from current position with QAPI
*/
long ftell(int *fd){
	struct qapi_FS_Stat_Type_s finfo;
	if(qapi_FS_Stat_With_Handle(*fd,&finfo) == QAPI_OK){
		return finfo.st_size;
	}else
		return -1;
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
	else
		return -1;
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
	else
		return -1;
}

size_t fread(void *ptr, size_t size, size_t nmemb, int *fd){

	struct qapi_FS_Stat_Type_s finfo;
	uint32_t read_bytes = 0;

	if(ptr && fd){	
		if( qapi_FS_Read (*fd, ptr, nmemb, &read_bytes) == QAPI_OK){			
			return read_bytes;
		}else
			return 0;		
	}else
		return 0;

}