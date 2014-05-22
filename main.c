//test.exe -i in.yuv -r 2560x1600 -o out.yuv -b 8

#include "yuv.h"

int main(int argc, char * argv[]){
	struct YUV_Info * s_yuv_info = malloc(sizeof(struct YUV_Info));
	GetInfo(argc, argv, s_yuv_info);

	YUV_Init(s_yuv_info);

	return 0;
}