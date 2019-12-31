#include <stdio.h>
#include <stdlib.h>
#include "bmp_header.h"
#include <math.h>
#include<string.h>

void move_pixel_array(unsigned char **bmp_pixel_array, int height_width_diff){
	//for(int i = )
}

void load_bmp_header_and_info(bmp_fileheader *img_header, bmp_infoheader *img_info, FILE *bmp_image){
	fread(img_header, sizeof(bmp_fileheader), 1, bmp_image);
	fread(img_info, sizeof(bmp_infoheader), 1, bmp_image);	
}

void black_and_white(unsigned char *bmp_pixel_array, int image_nr, bmp_fileheader img_header, bmp_infoheader img_info){
	char *out_filename_format = "test%d_black_white.bmp";
	char *out_filename = malloc(50 * sizeof(char));
	snprintf(out_filename, 50, out_filename_format, image_nr);
	FILE *out_file = fopen(out_filename, "wb");
	unsigned char avg, *padding = malloc(3 * sizeof(unsigned char));
	
	for(int i = 0; i < img_info.biSizeImage; i += 3){
		avg = (bmp_pixel_array[i] + bmp_pixel_array[i + 1] + bmp_pixel_array[i + 2]) / 3;
		bmp_pixel_array[i] = avg;
		bmp_pixel_array[i + 1] = avg;
		bmp_pixel_array[i + 2] = avg;
	}
	for(int i = 0; i < img_info.width % 4; i++){
		padding[i] = 0;
	}
		fwrite(&img_header, sizeof(bmp_fileheader), 1, out_file);
		fwrite(&img_info, sizeof(bmp_infoheader), 1, out_file);
		fwrite(bmp_pixel_array, sizeof(unsigned char*), img_info.biSizeImage, out_file);
	/*
	if(img_info.width % 4 == 0){
		fwrite(&img_header, sizeof(bmp_fileheader), 1, out_file);
		fwrite(&img_info, sizeof(bmp_infoheader), 1, out_file);
		fwrite(bmp_pixel_array, sizeof(unsigned char*), img_info.biSizeImage, out_file);
	} else{
		fwrite(&img_header, sizeof(bmp_fileheader), 1, out_file);
		fwrite(&img_info, sizeof(bmp_infoheader), 1, out_file);
		for(int j = 0; j < img_info.height; j++){
			fwrite(bmp_pixel_array, sizeof(unsigned char*), img_info.width * 3, out_file);
			fwrite(padding, sizeof(unsigned char*), img_info.width % 4, out_file);
		}
	}
	*/
	fclose(out_file);
	free(out_filename);
}

void no_crop(unsigned char *bmp_pixel_array, int image_nr, bmp_infoheader img_info){
	int height_width_diff, max_coord;
	
	if(img_info.width > img_info.height){
		height_width_diff = img_info.width - img_info.height;
		max_coord = img_info.width;
	}
	else if(img_info.width < img_info.height){
		height_width_diff = img_info.height - img_info.width;
		max_coord = img_info.height;
	}
	
	bmp_pixel_array = realloc(bmp_pixel_array, height_width_diff * max_coord * 3 * sizeof(unsigned char));
	move_pixel_array(&bmp_pixel_array, height_width_diff);
	
}

char *remove_padding(unsigned char *bmp_pixel_array, bmp_infoheader img_info){
	char *image_no_padding = malloc((img_info.biSizeImage + 1) * sizeof(unsigned char));
	int padding = 0;
	for(int j = 0; j < img_info.height; j++){
		for(int k = 0; k < img_info.width * 3; k++){
			if((j + 1) * img_info.width + k )
			image_no_padding[(j + 1) * img_info.width + k] = bmp_pixel_array[(j + 1) * img_info.width + k + padding];
		}
	}
	
	return image_no_padding;
	
}

int main(){
	char *filename_format = "input/images/test%d.bmp";
	char *filename = malloc(25 * sizeof(char));
	
	for(int img_nr = 0; img_nr < 10; img_nr++){
		bmp_fileheader img_header;
		bmp_infoheader img_info;
		unsigned char RGB_swap;
		snprintf(filename, 25, filename_format, img_nr);
		
		FILE *bmp_image = fopen(filename, "rb");
		
		load_bmp_header_and_info(&img_header, &img_info, bmp_image);
		if(img_info.width % 4 == 0){
			img_info.biSizeImage = img_info.height * img_info.width * 3;
		}
		
		unsigned char *bmp_pixel_array = malloc((img_info.biSizeImage + 1) * sizeof(unsigned char));
		fseek(bmp_image, img_header.imageDataOffset, SEEK_SET);
		fread(bmp_pixel_array, sizeof(unsigned char), img_info.biSizeImage, bmp_image);
	
		if(img_info.width % 4 != 0){
			printf("%u %u %u ---%u---\n", bmp_pixel_array[img_info.width * 3], bmp_pixel_array[img_info.width * 3 + 1], bmp_pixel_array[img_info.width * 3 + 2], img_info.width);
			int k = 0;
			for(int j = 0; j < img_info.height; j++){
				memmove(bmp_pixel_array + (j + 1) * img_info.width * 3,
						bmp_pixel_array + (j + 1) * img_info.width * 3 + (4 - img_info.height % 4) * 3,
						img_info.biSizeImage - ((j + 1) * img_info.width * 3));

				k += (4 - img_info.height % 4) * 3;
			}
			
			printf("%u %u %u\n", bmp_pixel_array[img_info.width * 3], bmp_pixel_array[img_info.width * 3 + 1], bmp_pixel_array[img_info.width * 3 + 2]);
		}
		
		black_and_white(bmp_pixel_array, img_nr, img_header, img_info);
		//no_crop(bmp_pixel_array, img_nr, img_header, img_info);
		
		fclose(bmp_image);
	}
	
	free(filename);
	
	return 0;
}