#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "interf_dec.h"

/* amrnb解码出来的PCM就是这个参数 */
#define PCM_SAMPLERATE  (8000)
#define PCM_SAMPLEBITS  (16)
#define PCM_CHANNELS    (1)

/* amr一帧数据是20ms，一秒50帧。8000,16,1 ==> 320 Bytes */
#define PCM_ONE_FRAME_SIZE  (PCM_SAMPLERATE/50 * PCM_SAMPLEBITS/8 * PCM_CHANNELS)

/* AMR参数 */
#define AMR_ONE_FRAME_SIZE (32) /* 对于NB，一般占用字节最大的MR122格式是32字节一帧 */


int main(int argc, char *argv[])
{
	void *vpDecoder = NULL;
	FILE *fpAmr = NULL;
	FILE *fpPcm = NULL;
	char acAmrHeader[6] = {0};
	int iReadBytes = 0;
	int iFrameSizes[] = {12, 13, 15, 17, 19, 20, 26, 31, 5, 6, 5, 5, 0, 0, 0, 0};


	/* 检查参数 */
	if(argc != 3)
	{
		printf("Usage: \n"
			   "\t %s ./audio/test.amr out_8000_16_1.pcm\n", argv[0]);
		return -1;
	}

	/* 初始化解码器 */
	vpDecoder = Decoder_Interface_init();
	if(vpDecoder == NULL)
	{
		printf("Decoder_Interface_init() error!\n");
		return -1;
	}

	/* 打开文件 */
	fpPcm = fopen(argv[2], "wb");
	if(fpPcm == NULL)
	{
		perror("test_enc.amr");
		return -1;
	}
	fpAmr = fopen(argv[1], "rb");
	if(fpAmr == NULL)   
	{   
		perror("argv[1]");
		return -1;
	}

	/* 判断是否为AMR文件 */
	iReadBytes = fread(acAmrHeader, 1, 6, fpAmr);
	if (iReadBytes != 6 || memcmp(acAmrHeader, "#!AMR\n", 6)) {
		printf("%s is not a amr file!\n", argv[1]);
		return -1;
	}
	
	/* 循环解码 */
	while(1)
	{
		unsigned char acAmrBuf[AMR_ONE_FRAME_SIZE] = {0}; 	// 对于NB，一般最大是32字节，从amr文件读出一帧最大是32（31）字节
		unsigned char acPcmBuf[PCM_ONE_FRAME_SIZE] = {0}; 	// 解码出来的是以8bit为单位
		short asDecBuf[PCM_ONE_FRAME_SIZE/2] = {0}; 		// 解码出来的是以16bit为单位
		int iFrameSize = 0; 		// 根据AMR文件每帧的头部获取该帧数据大小
		
		/* 获取AMR规格 */
		iReadBytes = fread(acAmrBuf, 1, 1, fpAmr);
		if(iReadBytes <= 0)
			break;

		/* 获取一帧的大小, 对于 12.2 kbps 是 31 bytes */
		iFrameSize = iFrameSizes[(acAmrBuf[0] >> 3) & 0x0F];

		/* 读取一帧AMR数据，需要注意的是记得偏移一个地址存入31字节，而解码时需要32字节一起解码 */
		iReadBytes = fread(acAmrBuf + 1, 1, iFrameSize, fpAmr);
		if(iFrameSize != iReadBytes)
			break;

#if 0
		/* 解码方式 1：像官方测试程序一样解码出来存到short类型的缓存里 */
		/* 将AMR数据解码 */
		Decoder_Interface_Decode(vpDecoder, acAmrBuf, asDecBuf, 0/* 参数未使用 */);

		char *p = acPcmBuf;
		for(int i = 0; i < 160; i++)
		{
			*p++ = (asDecBuf[i] >> 0) & 0xff;
			*p++ = (asDecBuf[i] >> 8) & 0xff;		
		}
#else
		/* 解码方式2：传参时直接强制类型转换即可 */
		/* 将AMR数据解码 */
		Decoder_Interface_Decode(vpDecoder, acAmrBuf, (short *)acPcmBuf, 0/* 参数未使用 */);
#endif

		fwrite(acPcmBuf, 1, 320, fpPcm);
	}

	/* 关闭文件 */
	fclose(fpAmr);
	fclose(fpPcm);

	/* 关闭解码器 */
	Decoder_Interface_exit(vpDecoder);

	printf("%s -> %s : Success!\n", argv[1], argv[2]);

	return 0;
}
