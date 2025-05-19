/*****************************************************************//**
 * \file   vc.h
 * \brief  Declarations of all functions in the program.
 * 
 * \author David Carvalho & Gonçalo Vidal & Diogo Marques & Gabriel Fortes
 * \date   May 2025
 *********************************************************************/

#define VC_DEBUG

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>

typedef struct {
	unsigned char *data;
	int width, height;
	int channels;		
	int levels;				
	int bytesperline;		
} IVC;

#define MAX3(a, b, c) (a > b ? (a > c ? a : c) : (b > c ? b : c))
#define MIN3(a, b, c) (a < b ? (a < c ? a : c) : (b < c ? b : c))

IVC *vc_image_new(int width, int height, int channels, int levels);
IVC *vc_image_free(IVC *image);
IVC *vc_read_image(char *filename);
int vc_write_image(char *filename, IVC *image);
int vc_add_image(IVC* src, IVC* dst);

#pragma region Colors
int vc_rgb_to_hsv(IVC* src, IVC* dst);
int vc_gbr_rgb(IVC* src);
int vc_hsv_segmentation(IVC* src, IVC* dst, int hmin, int hmax, int smin, int smax, int vmin, int vmax);
#pragma endregion

#pragma region MorphologicalOperators
int vc_binary_dilate(IVC* src, IVC* dst, int kernel);
int vc_binary_erode(IVC* src, IVC* dst, int kernel);
#pragma endregion

#pragma region Edges
int vc_gray_edge_prewitt(IVC* src, IVC* dst);
int vc_draw_edge(IVC* src, IVC* dst);
#pragma endregion

#pragma region Utils
int vc_three_to_one_channel(IVC* src, IVC* dst);
int vc_one_to_three_channel(IVC* src, IVC* dst);
int vc_limit(IVC* src, IVC* dst, int y);
int vc_limit2(IVC* src, IVC* dst, int y);
#pragma endregion

#pragma region Labelling
typedef struct {
	int x, y, xf, yf, width, height;	
	int area;					
	int xc, yc;					
	int perimeter;				
	int label;					
} OVC;

OVC* vc_binary_blob_labelling(IVC* src, IVC* dst, int* nlabels);
int vc_binary_blob_info(IVC* src, OVC* blobs, int nlabels);
int vc_draw_bounding_box(IVC* dest, OVC* blobs, int nlabels);
OVC* vc_check_if_circle(OVC* blobs, int* nLabels, IVC* src);
int vc_check_collisions(OVC firstBlob, OVC secondBlob);
int vc_main_collisions(OVC* firstBlobs, OVC* secondBlobs, int firstBlob, int secondBlob);
int vc_delete_blob(IVC* img, OVC blob);
int idCoin(int area, int perimeter);
int vc_center(OVC* blobs, IVC* dst, int nlabels);
#pragma endregion

