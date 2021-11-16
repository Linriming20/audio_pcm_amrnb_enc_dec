all: pcm2amrnb amrnb2pcm

# our static lib is compile by x86_64 gcc
CC := gcc
CFLAG := -I./include

pcm2amrnb: main_pcm2amrnb.c libs/libopencore-amrnb.a 
	$(CC) $^ $(CFLAG) -o $@

amrnb2pcm: main_amrnb2pcm.c libs/libopencore-amrnb.a 
	$(CC) $^ $(CFLAG) -o $@

clean :
	rm -rf pcm2amrnb amrnb2pcm out*
.PHONY := clean

