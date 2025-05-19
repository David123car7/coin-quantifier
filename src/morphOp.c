/*****************************************************************//**
 * \file   morphOp.c
 * \brief  Implementation of  morphological operations such as erosion, dilation.
 *
 * \author David Carvalho & Gonçalo Vidal & Diogo Marques & Gabriel Fortes
 * \date   May 2025
 *********************************************************************/

#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <malloc.h>
#include "vc.h"

int vc_binary_dilate(IVC* src, IVC* dst, int kernel)
{
	if (!src || !dst || kernel < 1 || kernel % 2 == 0) return 0; // Sanity checks

	int width = src->width;
	int height = src->height;
	int bytesperline = src->bytesperline;
	int channels = src->channels;

	int bound = (kernel - 1) / 2;

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {

			int found = 0;

			// Check kernel neighborhood
			for (int y1 = -bound; y1 <= bound && !found; y1++) {
				int y2 = y + y1;
				if (y2 < 0 || y2 >= height) continue;

				for (int x1 = -bound; x1 <= bound && !found; x1++) {
					int x2 = x + x1;
					if (x2 < 0 || x2 >= width) continue;

					long int pos = y2 * bytesperline + x2 * channels;

					if (src->data[pos] == 255) {
						found = 1;
					}
				}
			}

			long int dst_pos = y * bytesperline + x * channels;

			if (channels == 1) {
				dst->data[dst_pos] = found ? 255 : 0;
			}
			else if (channels == 3) {
				dst->data[dst_pos] = found ? 255 : 0;
				dst->data[dst_pos + 1] = found ? 255 : 0;
				dst->data[dst_pos + 2] = found ? 255 : 0;
			}
		}
	}

	return 1;
}

int vc_binary_erode(IVC* src, IVC* dst, int kernel)
{
	if (!src || !dst || kernel < 1 || kernel % 2 == 0) return 0; // Sanity checks

	int width = src->width;
	int height = src->height;
	int bytesperline = src->bytesperline;
	int channels = src->channels;

	int bound = (kernel - 1) / 2;

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {

			int erode = 1;

			for (int y1 = -bound; y1 <= bound && erode; y1++) {
				int y2 = y + y1;
				if (y2 < 0 || y2 >= height) continue;

				for (int x1 = -bound; x1 <= bound && erode; x1++) {
					int x2 = x + x1;
					if (x2 < 0 || x2 >= width) continue;

					long int pos = y2 * bytesperline + x2 * channels;

					if (src->data[pos] != 255) {
						erode = 0;
					}
				}
			}

			long int pos2 = y * bytesperline + x * channels;

			if (channels == 1) {
				dst->data[pos2] = erode ? 255 : 0;
			}
			else if (channels == 3) {
				dst->data[pos2] = erode ? 255 : 0;
				dst->data[pos2 + 1] = erode ? 255 : 0;
				dst->data[pos2 + 2] = erode ? 255 : 0;
			}
		}
	}

	return 1;
}
