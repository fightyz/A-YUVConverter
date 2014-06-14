//test.exe -i in.yuv -r 2560x1600 -o out.yuv -b 8

#include "yuv.h"
#include <sys/stat.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <semaphore.h>

sem_t *mutex_buffer_1, *mutex_buffer_2, *empty_1, *full_1, *empty_2, *full_2;

// 1st read thread to get even frames
void * ReadYUVFunction_1(void * read_thread_data){
	struct YUV_Info * s_yuv_info = ((s_read_thread_data *)read_thread_data)->p_yuv_info;
	unsigned char * p_Y_space = ((s_read_thread_data *)read_thread_data)->p_Y_space;
	FILE * fp_input_file = ((s_read_thread_data *)read_thread_data)->fp_input_file;
	int thread_num = ((s_read_thread_data *)read_thread_data)->thread_num;
	long origin = 0;
	for(long j = 0; j < s_yuv_info->frame_num; j += 2){
		sem_wait(empty_1);
		sem_wait(mutex_buffer_1);
		printf("R 1: %ld\n", j);
		ReadImage(s_yuv_info, p_Y_space, fp_input_file, origin + j * s_yuv_info->frame_size);
		sem_post(mutex_buffer_1);
		sem_post(full_1);
	}
	return NULL;
}

// 2nd read thread to get odd frames
void * ReadYUVFunction_2(void * read_thread_data){
	struct YUV_Info * s_yuv_info = ((s_read_thread_data *)read_thread_data)->p_yuv_info;
	unsigned char * p_Y_space = ((s_read_thread_data *)read_thread_data)->p_Y_space;
	FILE * fp_input_file = ((s_read_thread_data *)read_thread_data)->fp_input_file;
	int thread_num = ((s_read_thread_data *)read_thread_data)->thread_num;
	long origin = 0;
	for(long j = 1; j < s_yuv_info->frame_num; j += 2){
		sem_wait(empty_2);
		sem_wait(mutex_buffer_2);
		printf("R 2: %ld\n", j);
		ReadImage(s_yuv_info, p_Y_space, fp_input_file, origin + j * s_yuv_info->frame_size);
		sem_post(mutex_buffer_2);
		sem_post(full_2);
	}
	return NULL;
}

void * WriteYUVFunction(void * write_thread_data){
	struct YUV_Info * s_yuv_info = ((s_write_thread_data *)write_thread_data)->p_yuv_info;
	unsigned char * p_Y_space_1 = ((s_write_thread_data *)write_thread_data)->p_Y_space[0];
	unsigned char * p_Y_space_2 = ((s_write_thread_data *)write_thread_data)->p_Y_space[1];
	FILE * fp_output_file = ((s_write_thread_data *)write_thread_data)->fp_output_file;

	for(long j = 0; j < s_yuv_info->frame_num - 1; j += 2){
		sem_wait(full_1);
		sem_wait(mutex_buffer_1);
		printf("W 0: %ld\n", j);
		WriteImage(s_yuv_info, p_Y_space_1, fp_output_file);
		sem_post(mutex_buffer_1);
		sem_post(empty_1);

		//pthread_mutex_unlock(&mutex_read_1);
		sem_wait(full_2);
		sem_wait(mutex_buffer_2);
		printf("W 1: %ld\n", j);
		WriteImage(s_yuv_info, p_Y_space_2, fp_output_file);
		sem_post(mutex_buffer_2);
		sem_post(empty_2);
		//pthread_mutex_unlock(&mutex_read_2);
	}
	if(s_yuv_info->frame_num / 2 != 0){
		sem_wait(full_1);
		sem_wait(mutex_buffer_1);
		WriteImage(s_yuv_info, p_Y_space_1, fp_output_file);
		sem_post(mutex_buffer_1);
		sem_post(empty_1);
		//pthread_mutex_unlock(&mutex_read_1);
	}
	return NULL;
}

void SetReadThreadData(s_read_thread_data * read_thread_data,
					   struct YUV_Info * s_yuv_info,
					   unsigned char * p_Y_space,
					   FILE * fp_input_file,
					   int thread_num){
	read_thread_data->p_yuv_info = s_yuv_info;
	read_thread_data->p_Y_space = p_Y_space;
	read_thread_data->fp_input_file = fp_input_file;
	read_thread_data->thread_num = thread_num;
}

void SetWriteThreadData(s_write_thread_data * write_thread_data,
						struct YUV_Info * s_yuv_info,
						unsigned char * *p_Y_space,
						FILE * fp_output_file){
	write_thread_data->p_yuv_info = s_yuv_info;
	write_thread_data->p_Y_space = p_Y_space;
	write_thread_data->fp_output_file = fp_output_file;
}



int
main(int argc, char * argv[]){
	time_t start, finish;
	start = time(NULL);

	struct YUV_Info * s_yuv_info = malloc(sizeof(struct YUV_Info));
	GetInfo(argc, argv, s_yuv_info);

	FILE *fp_input_file_1 = fopen(s_yuv_info->input_file_name, "rb");
	FILE *fp_input_file_2 = fopen(s_yuv_info->input_file_name, "rb");
	FILE *fp_output_file = fopen(s_yuv_info->output_file_name, "wb");
	if(!fp_input_file_1 || !fp_input_file_2){
		fprintf(stderr, "open file error: %s\n", s_yuv_info->input_file_name);
		return EXIT_FAILURE;
	}
	unsigned char * p_Y_space[2]; //<buff for convert per frame
	YUV_Init(s_yuv_info, fp_input_file_1, p_Y_space);
	printf("frame_num = %ld\n", s_yuv_info->frame_num);

	mutex_buffer_1 = sem_open("mutext_buffer_1", O_CREAT, 0644, 1);
	mutex_buffer_2 = sem_open("mutext_buffer_2", O_CREAT, 0644, 1);
	empty_1 = sem_open("empty_1", O_CREAT, 0644, 1);
	empty_2 = sem_open("empty_2", O_CREAT, 0644, 1);
	full_1 = sem_open("full_1", O_CREAT, 0644, 0);
	full_2 = sem_open("full_2", O_CREAT, 0644, 0);

	pthread_t t_read_thread[2], t_write_thread;

	s_read_thread_data read_thread_data[2];
	SetReadThreadData(&read_thread_data[0], s_yuv_info, p_Y_space[0], fp_input_file_1, 0);
	SetReadThreadData(&read_thread_data[1], s_yuv_info, p_Y_space[1], fp_input_file_2, 1);

	s_write_thread_data write_thread_data;
	SetWriteThreadData(&write_thread_data, s_yuv_info, p_Y_space, fp_output_file);

	int ret_val_read_1 = pthread_create(&t_read_thread[0], NULL, ReadYUVFunction_1, (void *)&read_thread_data[0]);
	int ret_val_read_2 = pthread_create(&t_read_thread[1], NULL, ReadYUVFunction_2, (void *)&read_thread_data[1]);
	int ret_val_write = pthread_create(&t_write_thread, NULL, WriteYUVFunction, (void *)&write_thread_data);

	pthread_join(t_read_thread[0], NULL);
	pthread_join(t_read_thread[1], NULL);
	pthread_join(t_write_thread, NULL);

	sem_close(mutex_buffer_1);
	sem_close(mutex_buffer_2);
	sem_close(empty_1);
	sem_close(empty_2);
	sem_close(full_1);
	sem_close(full_2);

	sem_unlink("mutex_buffer_1");
	sem_unlink("mutex_buffer_2");
	sem_unlink("empty_1");
	sem_unlink("empty_2");
	sem_unlink("full_1");
	sem_unlink("full_2");

	free(p_Y_space[0]);
	free(p_Y_space[1]);
	free(s_yuv_info);
	fclose(fp_input_file_1);
	fclose(fp_input_file_2);
	fclose(fp_output_file);

	finish = time(NULL);
	fprintf(stdout, "time: %f\n", difftime(finish, start));

	return 0;
}