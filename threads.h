#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include "yuv.h"

typedef struct {
	yuv_info * p_yuv_info;
	unsigned char * p_Y_space;
	FILE * fp_input_file;
	int thread_num;
} s_read_thread_data;

typedef struct {
	yuv_info * p_yuv_info;
	unsigned char * *p_Y_space;
	FILE * fp_output_file;
} s_write_thread_data;

sem_t *mutex_buffer_1, *mutex_buffer_2, *sem_empty_1, *sem_full_1, *sem_empty_2, *sem_full_2;

void SemaphoreInit();
void * ReadYUVFunction(void * read_thread_data);
void * WriteYUVFunction(void * write_thread_data);
void SetReadThreadData(s_read_thread_data * read_thread_data,
					   yuv_info * s_yuv_info,
					   unsigned char * p_Y_space,
					   FILE * fp_input_file,
					   int thread_num);
void SetWriteThreadData(s_write_thread_data * write_thread_data,
						yuv_info * s_yuv_info,
						unsigned char * *p_Y_space,
						FILE * fp_output_file);