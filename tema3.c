#include <stdio.h>
#include <stdlib.h>
#include "bmp_header.h"
#include <math.h>
#include <string.h>

void write_image(unsigned char *bmp_pixel_array, bmp_infoheader img_info, bmp_fileheader img_header, FILE *out_file){
	fwrite(&img_header, sizeof(bmp_fileheader), 1, out_file);
	fwrite(&img_info, sizeof(bmp_infoheader), 1, out_file);
	int padded_row_size = img_info.width * 3 + (4 - ((img_info.width * 3) % 4)) % 4;//4 * ((img_info.width * img_info.bitPix + 31)/ 32);
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
	FILE *filter_input = fopen(in_filename, "r");
	
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
	
	int row_sum = 0, sum = 0, row_count = 0, pixel_count = 0;
	for(int i = 0; i < img_info.width * img_info.height * 3; i++){
		int n = 0;
		sum = 0;
		for(int j = -size / 2; j <= size / 2; j++){
			int m = 0;
			row_sum = 0;
			for(int k = -size / 2; k <= size / 2; k++){
				if((i + j * img_info.width * 3 >= 0) && (i + j * img_info.width * 3 < img_info.biSizeImage)
					&& (i + k * 3 - row_count * img_info.width * 3 < img_info.width * 3) 
					&& (i - row_count * img_info.width * 3 + k * 3 >= 0)){
					row_sum += bmp_pixel_array[i + j * img_info.width * 3 + k * 3] * filter[n][m];
				}
				m++;
			}
			sum += row_sum;
			n++;
		}
		if(pixel_count == img_info.width * 3 - 1){
			row_count++;
			pixel_count = 0;
		}
		else{
			pixel_count++;
		}
		

		if(sum > 255){
			sum = 255;
		}
		if(sum < 0){
			sum = 0;
		}
		
		pixel_array_copy[i] = sum;
	}
	
	if(img_nr == 9){
		img_info.biSizeImage = 0;
	}
	write_image(pixel_array_copy, img_info, img_header, filter_output);
	
	fclose(filter_input);
	fclose(filter_output);
	free(out_filename);
	free(in_filename);
	
}

void pooling(unsigned char *bmp_pixel_array, bmp_fileheader img_header, bmp_infoheader img_info, int img_nr){
	char *input_file_format = "input/pooling/pooling%d.txt";
	char *input_filename = malloc(30 * sizeof(unsigned char));
	snprintf(input_filename, 30, input_file_format, img_nr);
	FILE *pooling_input = fopen(input_filename, "r");
	
	char *output_file_format = "test%d_pooling.bmp";
	char *output_filename = malloc(20 * sizeof(unsigned char));
	snprintf(output_filename, 20, output_file_format, img_nr);
	FILE *pooling_output = fopen(output_filename, "wb");
	
	unsigned char pool_type; fscanf(pooling_input, "%c ", &pool_type); 
	int pool_size; fscanf(pooling_input, "%d", &pool_size);
	
	unsigned char *pixel_array_copy = malloc(img_info.biSizeImage * sizeof(unsigned char));
	for(int i = 0; i < img_info.biSizeImage; i++){
		pixel_array_copy[i] = bmp_pixel_array[i];
	}
	
	int row_count = 0, pixel_count = 0;
	if(pool_type == 'm'){
		for(int i = 0; i < img_info.width * img_info.height * 3; i++){
			unsigned char min = 255;
			for(int j = -pool_size / 2; j <= pool_size / 2; j++){
				for(int k = -pool_size / 2; k <= pool_size / 2; k++){
					if((i + j * img_info.width * 3 >= 0) && (i + j * img_info.width * 3 < img_info.biSizeImage)
						&& (i + k * 3 - row_count * img_info.width * 3 < img_info.width * 3) 
						&& (i - row_count * img_info.width * 3 + k * 3 >= 0)){
							if(bmp_pixel_array[i + j * img_info.width * 3 + k * 3] < min){
								min = bmp_pixel_array[i + j * img_info.width * 3 + k * 3];
							}
					}
					else{
						if(0 < min){
							min = 0;
						}
					}
				}
			}
		
			if(pixel_count == img_info.width * 3 - 1){
				row_count++;
				pixel_count = 0;
			}
			else{
				pixel_count++;
			}
	
			pixel_array_copy[i] = min;
		}
	}
	else if(pool_type == 'M'){
		for(int i = 0; i < img_info.biSizeImage; i++){
			unsigned char max = 0;
			for(int j = -pool_size / 2; j <= pool_size / 2; j++){
				for(int k = -pool_size / 2; k <= pool_size / 2; k++){
					if((i + j * img_info.width * 3 >= 0) && (i + j * img_info.width * 3 < img_info.biSizeImage)
						&& (i + k * 3 - row_count * img_info.width * 3 < img_info.width * 3) 
						&& (i - row_count * img_info.width * 3 + k * 3 >= 0)){
							if(bmp_pixel_array[i + j * img_info.width * 3 + k * 3] > max){
								max = bmp_pixel_array[i + j * img_info.width * 3 + k * 3];
							}
					}
					else{
						if(0 > max){
							max = 0;
						}
					}
				}
			}
		
			if(pixel_count == img_info.width * 3 - 1){
				row_count++;
				pixel_count = 0;
			}
			else{
				pixel_count++;
			}
	
			pixel_array_copy[i] = max;
		}
	}
	
	if(img_nr == 9){
		img_info.biSizeImage = 0;
	}
	write_image(pixel_array_copy, img_info, img_header, pooling_output);
	
	free(output_filename);
	free(input_filename);
	fclose(pooling_input);
	fclose(pooling_output);
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

		unsigned char *bmp_pixel_array = malloc((img_info.biSizeImage + 1) * sizeof(unsigned char) + 12 * sizeof(unsigned char));
		unsigned char *pointer_to_row = bmp_pixel_array + ((img_info.height - 1) * img_info.width * 3);
		int padded_row_size = img_info.width * 3 + (4 - ((img_info.width * 3) % 4)) % 4;//4 * ((img_info.width * img_info.bitPix + 31)/ 32);
		for(int j = 0; j < img_info.height; j++){
			fseek(bmp_image_file, img_header.imageDataOffset + j * padded_row_size, SEEK_SET);
			fread(pointer_to_row, 1, img_info.width * 3, bmp_image_file);
			pointer_to_row -= img_info.width * 3;
		}

		black_and_white(bmp_pixel_array, img_nr, img_header, img_info);
		no_crop(bmp_pixel_array, img_nr, img_header, img_info);
		convolutional_layers(bmp_pixel_array, img_header, img_info, img_nr);
		pooling(bmp_pixel_array, img_header, img_info, img_nr);
		
		fclose(bmp_image_file);

	}
	
	free(filename);
	
	return 0;
}