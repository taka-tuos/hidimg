#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"

#ifdef _WIN32
#define STDIN  0
#define STDOUT 1
#define STDERR 2
#include <io.h>
#include <fcntl.h>
#define SET_BINARY_MODE(handle) setmode(handle, O_BINARY)
#else
#define SET_BINARY_MODE(handle) ((void)0)
#endif

stbi_uc *in;
int w,h,bpp;

int encode_byte(int inp)
{
	static int ip=0;
	
	for(int j=0;j<8;j++) {
		in[ip] = (in[ip] & 0xfe) | ((inp >> j) & 1);
		ip++;
		if(ip >= w*h*bpp) {
			printf("wrapped in %d bytes",ip);
			return -1;
		}
	}
	
	return 0;
}

int decode_byte()
{
	static int p=0;
	int bd=0;
	
	for(int i=0;i<8;i++) {
		bd |= (in[p] & 1) << i;
		p++;
	}
	
	return bd;
}

void decode(char *pic)
{
	in = stbi_load(pic,&w,&h,&bpp,4);
	
	int bd=0,bp=0;
	
	int b0 = decode_byte();
	int b1 = decode_byte();
	int b2 = decode_byte();
	int b3 = decode_byte();
	
	int len = b0 | (b1 << 8) | (b2 << 16) | (b3 << 24);
	
	for(int i=0;i<len;i++) {
		if(i >= w*h*bpp) break;
		int d = decode_byte();
		putchar(d);
	}
}

void encode(char *pic, char *out)
{
	in = stbi_load(pic,&w,&h,&bpp,4);
	
	int ip=0;
	int bf=0;
	
	unsigned char *buf=NULL;
	int bsiz=0;
	
	while(1) {
		unsigned char blk[512];
		int red = fread(blk, 1, 512, stdin);
		
		if(red <= 0) break;
		
		buf = realloc(buf,bsiz+red);
		
		memcpy(buf+bsiz,blk,red);
		
		bsiz += red;
	}
	
	encode_byte((bsiz >>  0) & 255);
	encode_byte((bsiz >>  8) & 255);
	encode_byte((bsiz >> 16) & 255);
	encode_byte((bsiz >> 24) & 255);
	
	for(int i=0;i<bsiz;i++) {
		encode_byte(buf[i]);
	}
	
	stbi_write_png(out,w,h,bpp,in,0);
}

int main(int argc, char *argv[])
{
	if(argc < 2) {
		printf("%s: not specified command e or d.\n",argv[0]);
		return 1;
	}
	
	if(*argv[1] == 'e') {
		if(argc < 4) {
			printf("%s: encode usage: %s e <picture> <output> (text from stdin)\n",argv[0],argv[0]);
			return 2;
		}
		SET_BINARY_MODE(STDIN);
		encode(argv[2],argv[3]);
	}
	
	if(*argv[1] == 'd') {
		if(argc < 3) {
			printf("%s: decode usage: %s d <picture>\n",argv[0],argv[0]);
			return 3;
		}
		SET_BINARY_MODE(STDOUT);
		decode(argv[2]);
	}
	
	return 0;
}
