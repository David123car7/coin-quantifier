 //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//           INSTITUTO POLIT�CNICO DO C�VADO E DO AVE
//                          2022/2023
//             ENGENHARIA DE SISTEMAS INFORM�TICOS
//                    VIS�O POR COMPUTADOR
//
//             [  DUARTE DUQUE - dduque@ipca.pt  ]
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// Desabilita (no MSVC++) warnings de fun��es n�o seguras (fopen, sscanf, etc...)
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <malloc.h>
#include "vc.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//            FUN��ES: ALOCAR E LIBERTAR UMA IMAGEM
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


// Alocar mem�ria para uma imagem
IVC *vc_image_new(int width, int height, int channels, int levels)
{
	IVC *image = (IVC *) malloc(sizeof(IVC));

	if(image == NULL) return NULL;
	if((levels <= 0) || (levels > 255)) return NULL;

	image->width = width;
	image->height = height;
	image->channels = channels;
	image->levels = levels;
	image->bytesperline = image->width * image->channels;
	image->data = (unsigned char *) malloc(image->width * image->height * image->channels * sizeof(char));

	if(image->data == NULL)
	{
		return vc_image_free(image);
	}

	return image;
}


// Libertar mem�ria de uma imagem
IVC *vc_image_free(IVC *image)
{
	if(image != NULL)
	{
		if(image->data != NULL)
		{
			free(image->data);
			image->data = NULL;
		}

		free(image);
		image = NULL;
	}

	return image;
}


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//    FUN��ES: LEITURA E ESCRITA DE IMAGENS (PBM, PGM E PPM)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


char *netpbm_get_token(FILE *file, char *tok, int len)
{
	char *t;
	int c;
	
	for(;;)
	{
		while(isspace(c = getc(file)));
		if(c != '#') break;
		do c = getc(file);
		while((c != '\n') && (c != EOF));
		if(c == EOF) break;
	}
	
	t = tok;
	
	if(c != EOF)
	{
		do
		{
			*t++ = c;
			c = getc(file);
		} while((!isspace(c)) && (c != '#') && (c != EOF) && (t - tok < len - 1));
		
		if(c == '#') ungetc(c, file);
	}
	
	*t = 0;
	
	return tok;
}


long int unsigned_char_to_bit(unsigned char *datauchar, unsigned char *databit, int width, int height)
{
	int x, y;
	int countbits;
	long int pos, counttotalbytes;
	unsigned char *p = databit;

	*p = 0;
	countbits = 1;
	counttotalbytes = 0;

	for(y=0; y<height; y++)
	{
		for(x=0; x<width; x++)
		{
			pos = width * y + x;

			if(countbits <= 8)
			{
				// Numa imagem PBM:
				// 1 = Preto
				// 0 = Branco
				//*p |= (datauchar[pos] != 0) << (8 - countbits);
				
				// Na nossa imagem:
				// 1 = Branco
				// 0 = Preto
				*p |= (datauchar[pos] == 0) << (8 - countbits);

				countbits++;
			}
			if((countbits > 8) || (x == width - 1))
			{
				p++;
				*p = 0;
				countbits = 1;
				counttotalbytes++;
			}
		}
	}

	return counttotalbytes;
}


void bit_to_unsigned_char(unsigned char *databit, unsigned char *datauchar, int width, int height)
{
	int x, y;
	int countbits;
	long int pos;
	unsigned char *p = databit;

	countbits = 1;

	for(y=0; y<height; y++)
	{
		for(x=0; x<width; x++)
		{
			pos = width * y + x;

			if(countbits <= 8)
			{
				// Numa imagem PBM:
				// 1 = Preto
				// 0 = Branco
				//datauchar[pos] = (*p & (1 << (8 - countbits))) ? 1 : 0;

				// Na nossa imagem:
				// 1 = Branco
				// 0 = Preto
				datauchar[pos] = (*p & (1 << (8 - countbits))) ? 0 : 1;
				
				countbits++;
			}
			if((countbits > 8) || (x == width - 1))
			{
				p++;
				countbits = 1;
			}
		}
	}
}


IVC *vc_read_image(char *filename)
{
	FILE *file = NULL;
	IVC *image = NULL;
	unsigned char *tmp;
	char tok[20];
	long int size, sizeofbinarydata;
	int width, height, channels;
	int levels = 255;
	int v;
	
	// Abre o ficheiro
	if((file = fopen(filename, "rb")) != NULL)
	{
		// Efectua a leitura do header
		netpbm_get_token(file, tok, sizeof(tok));

		if(strcmp(tok, "P4") == 0) { channels = 1; levels = 1; }	// Se PBM (Binary [0,1])
		else if(strcmp(tok, "P5") == 0) channels = 1;				// Se PGM (Gray [0,MAX(level,255)])
		else if(strcmp(tok, "P6") == 0) channels = 3;				// Se PPM (RGB [0,MAX(level,255)])
		else
		{
			#ifdef VC_DEBUG
			printf("ERROR -> vc_read_image():\n\tFile is not a valid PBM, PGM or PPM file.\n\tBad magic number!\n");
			#endif

			fclose(file);
			return NULL;
		}
		
		if(levels == 1) // PBM
		{
			if(sscanf(netpbm_get_token(file, tok, sizeof(tok)), "%d", &width) != 1 || 
			   sscanf(netpbm_get_token(file, tok, sizeof(tok)), "%d", &height) != 1)
			{
				#ifdef VC_DEBUG
				printf("ERROR -> vc_read_image():\n\tFile is not a valid PBM file.\n\tBad size!\n");
				#endif

				fclose(file);
				return NULL;
			}

			// Aloca mem�ria para imagem
			image = vc_image_new(width, height, channels, levels);
			if(image == NULL) return NULL;

			sizeofbinarydata = (image->width / 8 + ((image->width % 8) ? 1 : 0)) * image->height;
			tmp = (unsigned char *) malloc(sizeofbinarydata);
			if(tmp == NULL) return 0;

			#ifdef VC_DEBUG
			printf("\nchannels=%d w=%d h=%d levels=%d\n", image->channels, image->width, image->height, levels);
			#endif

			if((v = fread(tmp, sizeof(unsigned char), sizeofbinarydata, file)) != sizeofbinarydata)
			{
				#ifdef VC_DEBUG
				printf("ERROR -> vc_read_image():\n\tPremature EOF on file.\n");
				#endif

				vc_image_free(image);
				fclose(file);
				free(tmp);
				return NULL;
			}

			bit_to_unsigned_char(tmp, image->data, image->width, image->height);

			free(tmp);
		}
		else // PGM ou PPM
		{
			if(sscanf(netpbm_get_token(file, tok, sizeof(tok)), "%d", &width) != 1 || 
			   sscanf(netpbm_get_token(file, tok, sizeof(tok)), "%d", &height) != 1 || 
			   sscanf(netpbm_get_token(file, tok, sizeof(tok)), "%d", &levels) != 1 || levels <= 0 || levels > 255)
			{
				#ifdef VC_DEBUG
				printf("ERROR -> vc_read_image():\n\tFile is not a valid PGM or PPM file.\n\tBad size!\n");
				#endif

				fclose(file);
				return NULL;
			}

			// Aloca mem�ria para imagem
			image = vc_image_new(width, height, channels, levels);
			if(image == NULL) return NULL;

			#ifdef VC_DEBUG
			printf("\nchannels=%d w=%d h=%d levels=%d\n", image->channels, image->width, image->height, levels);
			#endif

			size = image->width * image->height * image->channels;

			if((v = fread(image->data, sizeof(unsigned char), size, file)) != size)
			{
				#ifdef VC_DEBUG
				printf("ERROR -> vc_read_image():\n\tPremature EOF on file.\n");
				#endif

				vc_image_free(image);
				fclose(file);
				return NULL;
			}
		}
		
		fclose(file);
	}
	else
	{
		#ifdef VC_DEBUG
		printf("ERROR -> vc_read_image():\n\tFile not found.\n");
		#endif
	}
	
	return image;
}


int vc_write_image(char *filename, IVC *image)
{
	FILE *file = NULL;
	unsigned char *tmp;
	long int totalbytes, sizeofbinarydata;
	
	if(image == NULL) return 0;

	if((file = fopen(filename, "wb")) != NULL)
	{
		if(image->levels == 1)
		{
			sizeofbinarydata = (image->width / 8 + ((image->width % 8) ? 1 : 0)) * image->height + 1;
			tmp = (unsigned char *) malloc(sizeofbinarydata);
			if(tmp == NULL) return 0;
			
			fprintf(file, "%s %d %d\n", "P4", image->width, image->height);
			
			totalbytes = unsigned_char_to_bit(image->data, tmp, image->width, image->height);
			printf("Total = %ld\n", totalbytes);
			if(fwrite(tmp, sizeof(unsigned char), totalbytes, file) != totalbytes)
			{
				#ifdef VC_DEBUG
				fprintf(stderr, "ERROR -> vc_read_image():\n\tError writing PBM, PGM or PPM file.\n");
				#endif

				fclose(file);
				free(tmp);
				return 0;
			}

			free(tmp);
		}
		else
		{
			fprintf(file, "%s %d %d 255\n", (image->channels == 1) ? "P5" : "P6", image->width, image->height);
		
			if(fwrite(image->data, image->bytesperline, image->height, file) != image->height)
			{
				#ifdef VC_DEBUG
				fprintf(stderr, "ERROR -> vc_read_image():\n\tError writing PBM, PGM or PPM file.\n");
				#endif

				fclose(file);
				return 0;
			}
		}
		
		fclose(file);

		return 1;
	}
	
	return 0;
}

///////////////////////////////////////////////////////////////////
int vc_gray_negative(IVC* srcdst) {
    int width = srcdst->width;
    int height = srcdst->height;
    int bytesperline = srcdst->bytesperline;
    int channels = srcdst->channels;
    int x, y;
    long int pos;

    if ((srcdst->width <= 0) || (srcdst->height <= 0) || (srcdst->data == NULL)) return 0;
    if (channels != 1)return 0;

    for (y = 0; y < height; y++) {
        for (x = 0; x < width; x++) {
            pos = y * bytesperline + x * channels;
            srcdst->data[pos] = 255 - srcdst->data[pos];
        }
    }
    return 1;
}

int vc_gbr_rgb(IVC* src) {
	int width = src->width;
	int height = src->height;
	int bytesperline = src->bytesperline;
	int channels = src->channels;
	int x, y, a;
	long int pos1, pos2;
	if ((src->width <= 0) || (src->height <= 0) || (src->data == NULL)) return 0;
	if (channels != 3)return 0;

	for (y = 0; y < height; y++) {
		for (x = 0; x < width; x++) {
			pos1 = y * bytesperline + x * channels;
			pos2 = pos1 + 2;
			a = src->data[pos1];
			src->data[pos1] = src->data[pos2];
			src->data[pos2] = a;
		}
	}
	return 1;
}

int vc_rgb_to_hsv(IVC* src, IVC* dst) {
	int width = src->width;
	int height = src->height;
	int bytesperline = src->bytesperline;
	int channels = src->channels;
	int x, y, a, b, c;
	long int pos1, pos2, pos3;
	float max, min, h, s, v;

	if ((src->width <= 0) || (src->height <= 0) || (src->data == NULL)) return 0;
	if (channels != 3)return 0;

	for (y = 0; y < height; y++) {
		for (x = 0; x < width; x++) {
			pos1 = y * bytesperline + x * channels;
			pos2 = pos1 + 1;
			pos3 = pos1 + 2;
			a = src->data[pos1];
			b = src->data[pos2];
			c = src->data[pos3];
			max = MAX3(src->data[pos1], src->data[pos2], src->data[pos3]);
			min = MIN3(src->data[pos1], src->data[pos2], src->data[pos3]);
			if (max == 0) {
				dst->data[pos1] = 0;
				dst->data[pos2] = 0;
				dst->data[pos3] = 0;
				continue;
			}
			v = max;
			if (max == min) {
				s = 0;
				h = 0;
				dst->data[pos1] = h;
				dst->data[pos2] = s;
				dst->data[pos3] = v;
				continue;
			}
			s = (max - min) / v;
			if (src->data[pos1] == max) {
				if (src->data[pos2] >= src->data[pos3]) {
					h = 60 * (src->data[pos2] - src->data[pos3]) / (max - min);
				}
				else {
					h = 360 + 60 * (src->data[pos2] - src->data[pos3]) / (max - min);
				}
			}
			else if (src->data[pos2] == max) {
				h= 120 + 60 * (src->data[pos3] - src->data[pos1]) / (max - min);
			}
			else if (src->data[pos3] == max) {
				h = 240 + 60 * (src->data[pos1] - src->data[pos2]) / (max - min);
			}
			h = (h / 360) * 255;
			dst->data[pos1] = h;
			dst->data[pos2] = s * 255;
			dst->data[pos3] = v;
		}
	}
	return 1;

}

int vc_scale_gray_tocolor_pallete(IVC* src, IVC* dst) {
	int width = src->width;
	int height = src->height;
	int bytesperline = src->bytesperline;
	int bpl = dst->bytesperline;
	int channels = src->channels;
	int x, y;
	long int pos1, pos2;

	if ((src->width <= 0) || (src->height <= 0) || (src->data == NULL)) return 0;
	if (channels != 1)return 0;

	for (y = 0; y < height; y++) {
		for (x = 0; x < width; x++) {
			pos1 = y * bytesperline + x * channels;
			pos2 = y * bpl + x * 3;
			if (src->data[pos1] < 64) {
				dst->data[pos2] = 0;
				dst->data[pos2 + 1] = src->data[pos1] * 4;
				dst->data[pos2 + 2] = 255;
				continue;
			}
			if (src->data[pos1] >= 64 && src->data[pos1] < 128) {
				dst->data[pos2] = 0;
				dst->data[pos2 + 1] = 255;
				dst->data[pos2 + 2] = (src->data[pos1] - 128) * -4;
				continue;
			}
			if (src->data[pos1] >= 128 && src->data[pos1] < 192) {
				dst->data[pos2] = (src->data[pos1] - 128) * 4;
				dst->data[pos2 + 1] = 255;
				dst->data[pos2 + 2] = 0;
				continue;
			}
			if (src->data[pos1] >=192 && src->data[pos1] < 256) {
				dst->data[pos2] = 255;
				dst->data[pos2 + 1] = (src->data[pos1] - 255) * -4;
				dst->data[pos2 + 2] = 0;
				continue;
			}
		}
	}
	return 1;
}

int vc_hsv_segmentation(IVC* src, IVC* dst, int hmin, int hmax, int smin, int smax, int vmin, int vmax) {
	int width = src->width;
	int height = src->height;
	int bytesperline = src->bytesperline;
	int bpl = dst->bytesperline;
	int channels = src->channels;
	int x, y;
	long int pos1, pos2;


	if ((src->width <= 0) || (src->height <= 0) || (src->data == NULL)) return 0;
	if (channels != 3)return 0;

	for (y = 0; y < height; y++) {
		for (x = 0; x < width; x++) {
			pos1 = y * bytesperline + x * channels;
			//pos2 = y * bpl + x * 1;

			float fhmin = src->data[pos1] / 255.0f * 360;
			float fhmax = src->data[pos1] / 255.0f * 360;
			float fsmin = src->data[pos1 + 1] / 255.0f * 100;
			float fsmax = src->data[pos1 + 1] / 255.0f * 100;
			float fvmin = src->data[pos1 + 2] / 255.0f * 100;
			float fvmax = src->data[pos1 + 2] / 255.0f * 100;

			if (fhmin >= hmin && fhmax <= hmax && fsmin >= smin && fsmax <= smax && fvmin >= vmin && fvmax <= vmax) {
				dst->data[pos1] = 255;
				dst->data[pos1 + 1] = 255;
				dst->data[pos1 + 2] = 255;
				continue;
			}
			dst->data[pos1] = 0;
			dst->data[pos1 + 1] = 0;
			dst->data[pos1 + 2] = 0;
		}
	}
	return 1;
}

int vc_hsv_segmentation2(IVC* src, IVC* dst, int hmin, int hmax, int smin, int smax, int vmin, int vmax) {
	int width = src->width;
	int height = src->height;
	int bytesperline = src->bytesperline;
	int bpl = dst->bytesperline;
	int channels = src->channels;
	int x, y;
	long int pos1, pos2;


	if ((src->width <= 0) || (src->height <= 0) || (src->data == NULL)) return 0;
	if (channels != 3)return 0;

	for (y = 0; y < height; y++) {
		for (x = 0; x < width; x++) {
			pos1 = y * bytesperline + x * channels;
			//pos2 = y * bpl + x * 1;

			float fhmin = src->data[pos1] / 255.0f * 360;
			float fhmax = src->data[pos1] / 255.0f * 360;
			float fsmin = src->data[pos1 + 1] / 255.0f * 100;
			float fsmax = src->data[pos1 + 1] / 255.0f * 100;
			float fvmin = src->data[pos1 + 2] / 255.0f * 100;
			float fvmax = src->data[pos1 + 2] / 255.0f * 100;

			if (fhmin >= hmin && fhmax <= hmax && fsmin >= smin && fsmax <= smax && fvmin >= vmin && fvmax <= vmax) {
				dst->data[pos1] = 255;
				dst->data[pos1 + 1] = 255;
				dst->data[pos1 + 2] = 255;
				continue;
			}
			dst->data[pos1] = 0;
			dst->data[pos1 + 1] = 0;
			dst->data[pos1 + 2] = 0;
		}
	}
	return 1;
}

int vc_gray_to_binary_midpoint(IVC* src, IVC* dst, int kernel) {
	int width = src->width;
	int height = src->height;
	int bytesperline = src->bytesperline;
	int channels = src->channels;
	int x, y;
	long int pos1, pos2;

	int y1, x1, xmin, xmax, t;
	int bound = (kernel - 1) / 2;
	int vmax = 0;
	int vmin = 255;

	if ((src->width <= 0) || (src->height <= 0) || (src->data == NULL)) return 0;
	if (channels != 1)return 0;

	for (y = 0; y < height; y++) {
		for (x = 0; x < width; x++) {
			pos1 = y * bytesperline + x * channels;
			//d� o numero da coluna do pixel central
			y1 = y;
			//o for anda com as colunas para cima a partir do pixel central
			for (y1; y1 > y - bound; y1--) {
				if (y < 0) break;
				//quanto pode andar para tras e para a frente
				xmin = y1 * src->bytesperline;
				xmax = y1 * src->bytesperline + src->bytesperline - 1;
				//da a posicao central da linha a verificar os lados
				pos2 = y1 * bytesperline + x * channels;
				x1 = pos2;
				//anda para tras a partir da nova posicao central
				for (x1; x1 > x - bound; x1--) {
					//se ja tiver passado para a linha anterior, mata
					if (x1 < xmin) break;
					if (src->data[x1] < vmin) vmin = src->data[x1];
					if (src->data[x1] > vmax) vmax = src->data[x1];
				}
				//anda para a frente a partir da nova posicao central
				x1 = pos2 + 1;
				for (x1; x1 < x + bound; x1++) {
					//se ja tiver passado para a proxima linha, mata
					if (x1 > xmax) break;
					if (src->data[x1] < vmin) vmin = src->data[x1];
					if (src->data[x1] > vmax) vmax = src->data[x1];
				}
			}
			y1 = y;
			for (y1; y1 < y + bound; y1++) {
				if (y > src->height) break;
				xmin = y1 * src->bytesperline;
				xmax = y1 * src->bytesperline + src->bytesperline - 1;
				pos2 = y1 * bytesperline + x * channels;
				x1 = pos2;
				for (x1; x1 > x - bound; x1--) {
					if (x1 < xmin) break;
					if (src->data[x1] < vmin) vmin = src->data[x1];
					else if (src->data[x1] > vmax) vmax = src->data[x1];
				}
				x1 = pos2 + 1;
				for (x1; x1 < x + bound; x1++) {
					if (x1 > xmax) break;
					if (src->data[x1] < vmin) vmin = src->data[x1];
					else if (src->data[x1] > vmax) vmax = src->data[x1];
				}
			}
			t = (vmin + vmax) / 2;
			dst->data[pos1] = 1;
			if (src->data[pos1] < t)dst->data[pos1] = 0;
		}
	}
}

int vc_binary_dilate(IVC* src, IVC* dst, int kernel){
	int width = src->width;
	int height = src->height;
	int bytesperline = src->bytesperline;
	int channels = src->channels;
	int x, y;
	long int pos1, pos2;

	int y1, x1, xmin, xmax;
	int bound = (kernel - 1) / 2;


	for (y = 0; y < height; y++) {
		for (x = 0; x < width; x++) {
			pos1 = y * bytesperline + x * channels;
			dst->data[pos1] = 0;
			//d� o numero da linha do pixel central
			y1 = y;
			//o for anda com as colunas para cima a partir do pixel central
			for (y1; y1 >= y - bound; y1--) {
				if (y1 < 0) break;
				//quanto pode andar para tras e para a frente
				xmin = y1 * src->bytesperline;
				xmax = y1 * src->bytesperline + src->bytesperline - 1;
				//da a posicao central da linha a verificar os lados
				pos2 = y1 * bytesperline + x * channels;
				x1 = pos2;
				//anda para tras a partir da nova posicao central
				for (x1; x1 >= x - bound; x1--) {
					//se ja tiver passado para a linha anterior, mata
					if (x1 < xmin) break;
					if (src->data[x1] == 1) {
						dst->data[pos1] = 1;
						break;
					}
				}
				if (dst->data[pos1] == 1) break;
				//anda para a frente a partir da nova posicao central
				x1 = pos2 + 1;
				for (x1; x1 <= x + bound; x1++) {
					//se ja tiver passado para a proxima linha, mata
					if (x1 > xmax) break;
					if (src->data[x1] == 1) {
						dst->data[pos1] = 1;
						break;
					}
				}
				if (dst->data[pos1] == 1) break;
			}
			y1 = y;
			for (y1; y1 <= y + bound; y1++) {
				if (y > src->height) break;
				xmin = y1 * src->bytesperline;
				xmax = y1 * src->bytesperline + src->bytesperline - 1;
				pos2 = y1 * bytesperline + x * channels;
				x1 = pos2;
				if (dst->data[pos1] == 1) break;
				for (x1; x1 >= x - bound; x1--) {
					if (x1 < xmin) break;
					if (src->data[x1] == 1) {
						dst->data[pos1] = 1;
						break;
					}
				}
				if (dst->data[pos1] == 1) break;
				x1 = pos2 + 1;
				for (x1; x1 <= x + bound; x1++) {
					if (x1 > xmax) break;
					if (src->data[x1] == 1) {
						dst->data[pos1] = 1;
						break;
					}
				}
				if (dst->data[pos1] == 1) break;
			}
		}
	}
	return 1;
}

/// @brief Converts a hsv image with 3 channels to a image with only 1 channel
/// @param src Hsv Image
/// @param dst New image
int vc_three_to_one_channel(IVC* src, IVC* dst) {
	int width = src->width;
	int height = src->height;
	int bytesperline = src->bytesperline;
	int destBytesperline = dst->bytesperline;
	int channels = src->channels;
	int sPos1, dPos1 = 0;
	int x, y, k = 0;

	for (y = 0; y < height; y++) {
		for (x = 0; x < width; x++) {
			sPos1 = y * bytesperline + x * channels;
			dPos1 = y * destBytesperline + x;
			dst->data[dPos1] = src->data[sPos1];
		}
	}
	return 1;
}

int vc_one_to_three_channel(IVC* src, IVC* dst) {
	int width = src->width;
	int height = src->height;
	int sPos1, dPos1 = 0;
	int x, y, k = 0;

	for (y = 0; y < height; y++) {
		for (x = 0; x < width; x++) {
			sPos1 = y * src->bytesperline + x;
			dPos1 = y * dst->bytesperline + x * dst->channels;
			dst->data[dPos1] = src->data[sPos1];
			dst->data[dPos1 +1] = src->data[sPos1];
			dst->data[dPos1+2] = src->data[sPos1];
		}
	}
	return 1;
}
