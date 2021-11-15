### 1. 编译

由于提供的.a静态库是在x86_64的机器上编译的，所以仅支持该架构的主机上编译运行：

```bash
make
```

### 2. 使用

本示例是使用amr(nb)与pcm（8KHz，16bits，单/双声道）音频数据进行相互转化（编解码），使用如下：

```bash
$ ./pcm2amrnb ./audio/test_8000_16_1.pcm test-out.amr 	# 不管输入的PCM是单声道还是双声道，这里输出的amr都是单声道的
$ ./amrnb2pcm ./audio/test.amr test-out.pcm
```

### 3. 参考文章

 - https://blog.csdn.net/hanzhen7541/article/details/100932834

 - https://blog.csdn.net/dinggo/article/details/1966444

 - "Amr supports only 8000Hz sample rate and 4.75k, 5.15k, ...": https://stackoverflow.com/questions/2559746/getting-error-while-converting-wav-to-amr-using-ffmpeg#

### 附录

```
.
├── audio
│   ├── test_8000_16_1.pcm
│   ├── test_8000_16_2.pcm
│   └── test.amr
├── docs
│   ├── AMR文件格式分析_dinggo的专栏-CSDN博客_amr格式.mhtml
│   ├── AMR编码文件解析_hanzhen7541的博客-CSDN博客.mhtml
│   └── audio - getting error while converting wav to amr using ffmpeg - Stack Overflow.mhtml
├── include
│   ├── interf_dec.h
│   └── interf_enc.h
├── libs
│   └── libopencore-amrnb.a
├── main_amrnb2pcm.c
├── main_pcm2amrnb.c
├── Makefile
└── README.md
```
