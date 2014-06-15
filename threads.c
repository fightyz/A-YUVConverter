#include "threads.h"

void
SemaphoreInit(){
	mutex_buffer_1 	= sem_open("mutext_buffer_1", O_CREAT, 0644, 1);
	mutex_buffer_2 	= sem_open("mutext_buffer_2", O_CREAT, 0644, 1);
	sem_empty_1 	= sem_open("sem_empty_1", O_CREAT, 0644, 1);
	sem_empty_2 	= sem_open("sem_empty_2", O_CREAT, 0644, 1);
	sem_full_1 		= sem_open("sem_full_1", O_CREAT, 0644, 0);
	sem_full_2 		= sem_open("sem_full_2", O_CREAT, 0644, 0);
	if (sem_full_2 == SEM_FAILED)
    {
        printf("sem_open failed! (%s)\n", strerror(errno));
        exit(-1);
    }
	printf("sem successful!\n");
}

void
SemaphoreDestroy(){
	sem_close(mutex_buffer_1);
	sem_close(mutex_buffer_2);
	sem_close(sem_empty_1);
	sem_close(sem_empty_2);
	sem_close(sem_full_1);
	sem_close(sem_full_2);

	sem_unlink("mutex_buffer_1");
	sem_unlink("mutex_buffer_2");
	sem_unlink("sem_empty_1");
	sem_unlink("sem_empty_2");
	sem_unlink("sem_full_1");
	sem_unlink("sem_full_2");
}

// 1st read thread to get even frames
void * ReadYUVFunction(void * read_thread_data){
	yuv_info * s_yuv_info = ((s_read_thread_data *)read_thread_data)->p_yuv_info;
	unsigned char * p_Y_space = ((s_read_thread_data *)read_thread_data)->p_Y_space;
	FILE * fp_input_file = ((s_read_thread_data *)read_thread_data)->fp_input_file;
	int thread_num = ((s_read_thread_data *)read_thread_data)->thread_num;
	long origin = 0;
	if(thread_num == 0){
		for(long j = 0; j < s_yuv_info->frame_num; j += 2){
			sem_wait(sem_empty_1);
			sem_wait(mutex_buffer_1);
			printf("R 1: %ld\n", j);
			ReadImage(s_yuv_info, p_Y_space, fp_input_file, origin + j * s_yuv_info->frame_size);
			sem_post(mutex_buffer_1);
			sem_post(sem_full_1);
		}
		return NULL;
	} else {
		for(long j = 1; j < s_yuv_info->frame_num; j += 2){
			sem_wait(sem_empty_2);
			sem_wait(mutex_buffer_2);
			printf("R 2: %ld\n", j);
			ReadImage(s_yuv_info, p_Y_space, fp_input_file, origin + j * s_yuv_info->frame_size);
			sem_post(mutex_buffer_2);
			sem_post(sem_full_2);
		}
	return NULL;
	}
}

void * WriteYUVFunction(void * write_thread_data){
	yuv_info * s_yuv_info = ((s_write_thread_data *)write_thread_data)->p_yuv_info;
	unsigned char * p_Y_space_1 = ((s_write_thread_data *)write_thread_data)->p_Y_space[0];
	unsigned char * p_Y_space_2 = ((s_write_thread_data *)write_thread_data)->p_Y_space[1];
	FILE * fp_output_file = ((s_write_thread_data *)write_thread_data)->fp_output_file;

	for(long j = 0; j < s_yuv_info->frame_num - 1; j += 2){
		sem_wait(sem_full_1);
		sem_wait(mutex_buffer_1);
		printf("W 0: %ld\n", j);
		WriteImage(s_yuv_info, p_Y_space_1, fp_output_file);
		sem_post(mutex_buffer_1);
		sem_post(sem_empty_1);

		sem_wait(sem_full_2);
		sem_wait(mutex_buffer_2);
		printf("W 1: %ld\n", j);
		WriteImage(s_yuv_info, p_Y_space_2, fp_output_file);
		sem_post(mutex_buffer_2);
		sem_post(sem_empty_2);
	}
	if(s_yuv_info->frame_num / 2 != 0){
		sem_wait(sem_full_1);
		sem_wait(mutex_buffer_1);
		WriteImage(s_yuv_info, p_Y_space_1, fp_output_file);
		sem_post(mutex_buffer_1);
		sem_post(sem_empty_1);
	}
	return NULL;
}

void SetReadThreadData(s_read_thread_data * read_thread_data,
					   yuv_info * s_yuv_info,
					   unsigned char * p_Y_space,
					   FILE * fp_input_file,
					   int thread_num){
	read_thread_data->p_yuv_info = s_yuv_info;
	read_thread_data->p_Y_space = p_Y_space;
	read_thread_data->fp_input_file = fp_input_file;
	read_thread_data->thread_num = thread_num;
}

void SetWriteThreadData(s_write_thread_data * write_thread_data,
						yuv_info * s_yuv_info,
						unsigned char * *p_Y_space,
						FILE * fp_output_file){
	write_thread_data->p_yuv_info = s_yuv_info;
	write_thread_data->p_Y_space = p_Y_space;
	write_thread_data->fp_output_file = fp_output_file;
}