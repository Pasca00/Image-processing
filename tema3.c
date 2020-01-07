#include <stdio.h>
#include <stdlib.h>
#include "bmp_header.h"
#include <math.h>
#include<string.h>

void write_image(unsigned char *bmp_pixel_array, bmp_infoheader img_info, bmp_fileheader img_header, FILE *out_file){
	fwrite(&img_header, sizeof(bmp_fileheader), 1, out_file);
	fwrite(&img_info, sizeof(bmp_infoheader), 1, out_file);
	int padded_row_size = (int)(4 * ceil((float)img_info.width/4.0f)) * 3;
	for(int i = 0; i < img_info.height; i++){
		int pixel_offset = ((img_info.height - i) - 1) * img_info.width * 3;
		fwrite(&bmp_pixel_array[pixel_offset], 1, padded_row_size, out_file);
	}
}

void load_bmp_header_and_info(bmp_fileheader *img_header, bmp_infoheader *img_info, FILE *bmp_image_file){
	fread(img_header, sizeof(bmp_fileheader), 1, bmp_image_file);
	fread(img_info, sizeof(bmp_infoheader), 1, bmp_image_file);	
}

void black_and_white(unsigned char *bmp_pixel_array, int image_nr, bmp_fileheader img_header, bmp_infoheader img_info){
	char *out_filename_format = "test%d_black_white.bmp";
	char *out_filename = malloc(50 * sizeof(char));
	snprintf(out_filename, 50, out_filename_format, image_nr);
	FILE *out_file = fopen(out_filename, "wb");
	unsigned char avg;
	unsigned char *pixel_array_copy = malloc(img_info.biSizeImage * sizeof(unsigned char));
	for(int i = 0; i < img_info.biSizeImage; i++){
		pixel_array_copy[i] = bmp_pixel_array[i];
	}
	
	for(int i = 0; i < img_info.biSizeImage; i += 3){
		avg = (pixel_array_copy[i] + pixel_array_copy[i + 1] + pixel_array_copy[i + 2]) / 3;
		pixel_array_copy[i] = avg;
		pixel_array_copy[i + 1] = avg;
		pixel_array_copy[i + 2] = avg;
	}
	
	if(image_nr == 9){
		img_info.biSizeImage = 0;
	}
	
	write_image(pixel_array_copy, img_info, img_header, out_file);
	
	//free(pixel_array_copy);
	fclose(out_file);
	free(out_filename);
}

void no_crop(unsigned char *bmp_pixel_array, int image_nr, bmp_fileheader img_header, bmp_infoheader img_info){
	int diff = 0, max_coord;
	char *out_filename_format = "test%d_nocrop.bmp", *out_filename = malloc(20 * sizeof(unsigned char));
	
	snprintf(out_filename, 20, out_filename_format, image_nr);
	FILE *out_file = fopen(out_filename, "wb");
	if(img_info.width > img_info.height){
		diff = img_info.width - img_info.height;
		max_coord = img_info.width;
	}
	else if(img_info.width <= img_info.height){
		diff = img_info.height - img_info.width;
		max_coord = img_info.height;
	}
	printf("===%u===\n", img_info.biSizeImage);
	
	unsigned char *pixel_array_copy = malloc(img_info.width * img_info.height * 3 + diff * max_coord * 3);
	for(int i = 0; i < max_coord * max_coord * 3; i++){
		pixel_array_copy[i] = 255;
	}
	
	if(max_coord == img_info.width){
		for(int i = 0; i < img_info.height; i++){
			memcpy(pixel_array_copy + i * img_info.width * 3 + diff / 2 * img_info.width * 3, bmp_pixel_array + i * img_info.width * 3, img_info.width * 3);
		}
	}
	
	if(max_coord == img_info.height){
		//memcpy(pixel_array_copy + diff / 2 * 3, bmp_pixel_array, img_info.width * 3);
		//memcpy(pixel_array_copy + img_info.width * 3 + diff / 2 * 3, bmp_pixel_array + img_info.width * 3, img_info.width * 3);
		
		for(int i = 0; i < img_info.height; i++){
			memcpy(pixel_array_copy + i * img_info.height * 3 + diff / 2 * 3, bmp_pixel_array + i * img_info.width * 3, img_info.width * 3);
		}
		
	}
	
	if(max_coord == img_info.width){
		img_info.height = img_info.width;
	}
	if(max_coord == img_info.height){
		img_info.width = img_info.height;
	}
	if(image_nr == 9){
		img_info.biSizeImage = 0;
	}
	printf("---%u---\n", img_info.biSizeImage);
	
	write_image(pixel_array_copy, img_info, img_header, out_file);

	free(out_filename);
	fclose(out_file);
}

void convolutional_layers(unsigned char *bmp_pixel_array, bmp_fileheader img_header, bmp_infoheader img_info, int img_nr){
	char *out_filename_format = "test%d_filter.bmp";
	char *out_filename = malloc(20 * sizeof(char));
	snprintf(out_filename, 20, out_filename_format, img_nr);
	FILE *filter_output = fopen(out_filename, "wb");
	
	char *in_filename_format = "input/filters/filter%d.txt";
	char *in_filename = malloc(50 * sizeof(char));
	snprintf(in_filename, 50, in_filename_format, img_nr);
	FILE *filter_input = fopen(in_filename, "rb");
	
	unsigned char *pixel_array_copy = malloc(img_info.biSizeImage * sizeof(unsigned char));
	for(int i = 0; i < img_info.biSizeImage; i++){
		pixel_array_copy[i] = 0;
	}
	
	int size;
	fscanf(filter_input, "%d\n", &size);
	int *filter[size];
	
	
	for(int i = 0; i < size; i++){
		filter[i] = malloc((size + 5) * sizeof(int));
	}
	
	for(int i = 0; i < size; i++){
		for(int j = 0; j < size; j++){
			if(j != size - 1){
				fscanf(filter_input, "%d ", &filter[i][j]);
			}
			else{
				fscanf(filter_input, "%d\n", &filter[i][j]);
			}
		}
	}
	
	for(int i = 0; i < img_info.biSizeImage; i++){
		for(int j = 0; j < size; j++){
			//pixel_array_copy[i] = 
		}
	}
	
	write_image(pixel_array_copy, img_info, img_header, filter_output);
	
	fclose(filter_input);
	fclose(filter_output);
	free(out_filename);
	free(in_filename);
	
}

int main(){
	char *filename_format = "input/images/test%d.bmp";
	char *filename = malloc(25 * sizeof(char));
	
	for(int img_nr = 0; img_nr < 10; img_nr++){
		bmp_fileheader img_header;
		bmp_infoheader img_info;
		snprintf(filename, 25, filename_format, img_nr);
		
		FILE *bmp_image_file = fopen(filename, "rb");
		
		load_bmp_header_and_info(&img_header, &img_info, bmp_image_file);
		if(img_info.width % 4 == 0){
			img_info.biSizeImage = img_info.height * img_info.width * 3;
		}
		printf("///%u///\n", img_header.bfSize);
		unsigned char *bmp_pixel_array = malloc((img_info.biSizeImage + 1) * sizeof(unsigned char) + 12 * sizeof(unsigned char));
		unsigned char *pointer_to_row = bmp_pixel_array + ((img_info.height - 1) * img_info.width * 3);
		int padded_row_size = (int)(4 * ceil((float)img_info.width/4.0f)) * 3;
		for(int j = 0; j < img_info.height; j++){
			fseek(bmp_image_file, img_header.imageDataOffset + j * padded_row_size, SEEK_SET);
			fread(pointer_to_row, 1, img_info.width * 3, bmp_image_file);
			pointer_to_row -= img_info.width * 3;
		}
		//printf("%u %u %u\n", bmp_pixel_array[img_info.width * 3 - 3], bmp_pixel_array[img_info.width * 3 - 2], bmp_pixel_array[img_info.width * 3 - 1]);
		black_and_white(bmp_pixel_array, img_nr, img_header, img_info);
		printf("%d, %d\n", (int)(4 * ceil((float)img_info.width/4.0f)), (4 - (img_info.width % 4)) + img_info.width);
		printf("%u %u %u\n\n", bmp_pixel_array[img_info.width * 3 - 3], bmp_pixel_array[img_info.width * 3 - 2], bmp_pixel_array[img_info.width * 3 - 1]);
		no_crop(bmp_pixel_array, img_nr, img_header, img_info);
		convolutional_layers(bmp_pixel_array, img_header, img_info, img_nr);
		
		fclose(bmp_image_file);

	}
	
	free(filename);
	
	return 0;
}