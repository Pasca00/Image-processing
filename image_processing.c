#include <stdio.h>
#include <stdlib.h>
#include "bmp_header.h"

void load_bmp_header_and_info(bmp_fileheader *img_header, bmp_infoheader *img_info, FILE *bmp_image){
	fread(img_header, sizeof(bmp_fileheader), 1, bmp_image);
	fread(img_info, sizeof(bmp_infoheader), 1, bmp_image);	
}

void black_and_white(unsigned char *bmp_pixel_array, int image_nr, bmp_fileheader img_header, bmp_infoheader img_info){
	char *out_filename_format = "test%d_black_white.bmp";
	char *out_filename = malloc(50 * sizeof(char));
	snprintf(out_filename, 50, out_filename_format, image_nr);
	FILE *out_file = fopen(out_filename, "wb");
	
	for(int i = 0; i < img_header.bfSize; i += 3){
		int avg = (bmp_pixel_array[i] + bmp_pixel_array[i + 1] + bmp_pixel_array[i + 2]) / 3;
		bmp_pixel_array[i] = avg;
		bmp_pixel_array[i + 1] = avg;
		bmp_pixel_array[i + 2] = avg;
	}
	
	fwrite(&img_header, sizeof(bmp_fileheader), 1, out_file);
	fwrite(&img_info, sizeof(bmp_infoheader), 1, out_file);
	fwrite(bmp_pixel_array, sizeof(unsigned char*), img_header.bfSize, out_file);
	
	fclose(out_file);
	free(out_filename);
}

int main(){
	char *filename_format = "input/images/test%d.bmp";
	char *filename = malloc(25 * sizeof(char));
	
	for(int img_nr = 0; img_nr < 10; img_nr++){
		bmp_fileheader img_header;
		bmp_infoheader img_info;
		snprintf(filename, 25, filename_format, img_nr);
		
		FILE *bmp_image = fopen(filename, "rb");
		
		load_bmp_header_and_info(&img_header, &img_info, bmp_image);
		
		unsigned char *bmp_pixel_array = (unsigned char*)malloc(img_header.bfSize);
		fseek(bmp_image, img_header.imageDataOffset, SEEK_SET);
		fread(bmp_pixel_array, sizeof(unsigned char), img_header.bfSize, bmp_image);
		
		black_and_white(bmp_pixel_array, img_nr, img_header, img_info);
		
		fclose(bmp_image);
	}
	
	free(filename);
	
	return 0;
}