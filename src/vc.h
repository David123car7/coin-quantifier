//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//           INSTITUTO POLIT�CNICO DO C�VADO E DO AVE
//                          2022/2023
//             ENGENHARIA DE SISTEMAS INFORM�TICOS
//                    VIS�O POR COMPUTADOR
//
//             [  DUARTE DUQUE - dduque@ipca.pt  ]
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


#define VC_DEBUG
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//                   ESTRUTURA DE UMA IMAGEM
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


typedef struct {
	unsigned char *data;
	int width, height;
	int channels;			// Bin�rio/Cinzentos=1; RGB=3
	int levels;				// Bin�rio=1; Cinzentos [1,255]; RGB [1,255]
	int bytesperline;		// width * channels
} IVC;

#define MAX3(a, b, c) (a > b ? (a > c ? a : c) : (b > c ? b : c))
#define MIN3(a, b, c) (a < b ? (a < c ? a : c) : (b < c ? b : c))

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//                    PROT�TIPOS DE FUN��ES
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// FUN��ES: ALOCAR E LIBERTAR UMA IMAGEM
IVC *vc_image_new(int width, int height, int channels, int levels);
IVC *vc_image_free(IVC *image);

// FUN��ES: LEITURA E ESCRITA DE IMAGENS (PBM, PGM E PPM)
IVC *vc_read_image(char *filename);
int vc_write_image(char *filename, IVC *image);
int vc_gray_negative(IVC* srcdst);

int vc_rgb_to_hsv(IVC* src, IVC* dst);
int vc_gbr_rgb(IVC* src);
int vc_scale_gray_tocolor_pallete(IVC* src, IVC* dst);
int vc_hsv_segmentation(IVC* src, IVC* dst, int hmin, int hmax, int smin, int smax, int vmin, int vmax);
int vc_hsv_segmentation2(IVC* src, IVC* dst, int hmin, int hmax, int smin, int smax, int vmin, int vmax);
int vc_gray_to_binary_midpoint(IVC* src, IVC* dst, int kernel);
int vc_binary_dilate(IVC* src, IVC* dst, int kernel);
int vc_binary_erode(IVC* src, IVC* dst, int kernel);
int vc_gray_histogram_equalization(IVC* src, IVC* dst);
int vc_gray_edge_prewitt(IVC* src, IVC* dst, float th);
int vc_draw_edge(IVC* src, IVC* dst);


int vc_three_to_one_channel(IVC* src, IVC* dst);
int vc_one_to_three_channel(IVC* src, IVC* dst);
int vc_add_image(IVC* src, IVC* dst);

#pragma region Labels
typedef struct {
	int x, y, xf, yf, width, height;	// Caixa Delimitadora (Bounding Box)
	int area;					// �rea
	int xc, yc;					// Centro-de-massa
	int perimeter;				// Per�metro
	int label;					// Etiqueta
} OVC;

typedef struct Coins {
	int x, y;	
	int area;									
	struct Coins* nextCoin;        
} Coins;

typedef struct CoinsDict {
	int key;
	Coins* coins;
	struct CoinsDict* next;
} CoinsDict;

OVC* vc_binary_blob_labelling(IVC* src, IVC* dst, int* nlabels);
int vc_binary_blob_info(IVC* src, OVC* blobs, int nlabels);
int vc_draw_bounding_box(IVC* src, IVC* dest, OVC* blobs, int nlabels);
int vc_draw_bounding_box2(IVC* dest, OVC* blobs, int nlabels);
OVC* vc_check_if_circle(OVC* blobs, int* nLabels);
int vc_check_collisions(OVC firstBlob, OVC secondBlob, int imageWidth);
#pragma endregion

#pragma region Dictionary
CoinsDict* CreateCoinsDictionaryEmpty();
CoinsDict* CreateCoinsDictionary(Coins* coins, int key);
CoinsDict* AddDictionary(CoinsDict* dict, CoinsDict* newDict);
#pragma endregion

#pragma region List
Coins* CreateCoin(OVC coin);
Coins* AddCoin(Coins* coins, Coins* newCoin);
#pragma endregion
