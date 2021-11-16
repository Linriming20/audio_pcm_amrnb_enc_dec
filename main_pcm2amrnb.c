#include <stdio.h>
#include <stdlib.h>

#include "interf_enc.h"


/* PCM参数 */
#define PCM_SAMPLERATE 	(8000) 	/* 只能编码 8 khz */
#define PCM_SAMPLEBITS 	(16) 	/* 只支持16位 */
#define PCM_CHANNELS 	(1) 	/* 不管PCM输入是单声道还是双声道，这里输出的amr都是单声道的 */

/* amr一帧数据是20ms，一秒50帧。8000,16,1 ==> 320 Bytes */
#define PCM_ONE_FRAME_SIZE  (PCM_SAMPLERATE/50 * PCM_SAMPLEBITS/8 * PCM_CHANNELS)

/* AMR参数 */
#define AMR_ENCODE_MODE MR122
#define AMR_ONE_FRAME_SIZE (32) /* MR122格式是32字节一帧 */

/* 是否使能背景噪声编码模式 */
#define DTX_DECODE_ENABLE 	1
#define DTX_DECODE_DISABLE 	0


int main(int argc, char *argv[])
{
	int dtx = DTX_DECODE_ENABLE;
	void *vpAmr = NULL;
	FILE *fpAmr = NULL;
	FILE *fpPcm = NULL;

	/* 检查参数 */
	if(argc != 3)
	{
		printf("Usage: \n"
			   "\t %s ./audio/test_8000_16_1.pcm out.amr\n", argv[0]);
		return -1;
	}
	printf("It will encode a PCM file as [sample rate: %d] - [sample bits: %d] - [channels: %d] !\n", 
			PCM_SAMPLERATE, PCM_SAMPLEBITS, PCM_CHANNELS);

	/* 初始化编码器 */
	vpAmr = Encoder_Interface_init(dtx);
	if(vpAmr == NULL)
	{
		printf("Encoder_Interface_init error!\n");
		return -1;
	}

	/* 打开pcm文件 */
	fpPcm = fopen(argv[1], "rb");
	if(fpPcm == NULL)   
	{   
		perror("argv[1]");
		return -1;
	}

	/* 打开amr文件 */
	fpAmr = fopen(argv[2], "wb");
	if(fpAmr == NULL)
	{
		perror("argv[2]");
		return -1;
	}
	/* 先写入amr头部 */
	fwrite("#!AMR\n", 1, 6, fpAmr);

	/* 循环编码 */
	while(1)
	{
		unsigned char acPcmBuf[PCM_ONE_FRAME_SIZE] = {0}; 	/* 保存在文件中一帧（20ms）PCM数据，8bit为单位，这里是unsigned */
		short asEncInBuf[PCM_ONE_FRAME_SIZE/2] = {0}; 		/* 编码需要的一帧（20ms）PCM数据，16bit为单位 */
		char acEncOutBuf[AMR_ONE_FRAME_SIZE] = {0};			/* 编码出来的一帧（20ms）AMR数据 */
		int iReadPcmBytes = 0; 								/* 从PCM文件中读取出的数据大小，单位：字节 */
		int iEncAmrBytes = 0; 								/* 编码出的AMR数据大小，单位：字节 */

		/* 读出一帧PCM数据 */
		iReadPcmBytes = fread(acPcmBuf, 1, PCM_ONE_FRAME_SIZE, fpPcm);
		if(iReadPcmBytes <= 0)
		{
			break;
		}
		//printf("iReadPcmBytes = %d\n", iReadPcmBytes);

#if 0
		/* 编码方式 1：像官方测试程序一样转换为short类型再进行编码 */
		for(int i = 0; i < PCM_ONE_FRAME_SIZE/2; i++)
		{
			unsigned char *p = &acPcmBuf[2*PCM_CHANNELS*i];
			asEncInBuf[i] = (p[1] << 8) | p[0];
		}

		/* 编码 */
		iEncAmrBytes = Encoder_Interface_Encode(vpAmr, AMR_ENCODE_MODE, asEncInBuf, acEncOutBuf, 0/* 参数未使用 */);
#else
		/* 编码方式 2：传参时直接类型强制转换即可 */
		/* 编码 */
		iEncAmrBytes = Encoder_Interface_Encode(vpAmr, AMR_ENCODE_MODE, (short *)acPcmBuf, acEncOutBuf, 0/* 参数未使用 */);
#endif
		//printf("iEncAmrBytes = %d\n", iEncAmrBytes);

		/* 写入到AMR文件中 */
		fwrite(acEncOutBuf, 1, iEncAmrBytes, fpAmr);
	}

	/* 关闭文件 */
	fclose(fpAmr);
	fclose(fpPcm);
	
	/* 关闭编码器 */
	Encoder_Interface_exit(vpAmr);

	printf("%s -> %s: Success!\n", argv[1], argv[2]);

	return 0;
}
