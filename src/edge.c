/*****************************************************************//**
 * \file   edge.c
 * \brief  Implementation of edge detection algorithms.
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

/// <summary>
/// Applies the Prewitt edge detection operator to a single-channel (grayscale) image.
/// Computes horizontal and vertical gradients, combines them to produce edge intensity,
/// and writes the result into the destination image buffer.
/// </summary>
/// <param name="src">
/// Pointer to the source IVC image struct.
/// </param>
/// <param name="dst">
/// Pointer to the destination IVC image struct.
/// </param>
/// <returns>
/// Returns 1 on success (edge image written to dst), or 0 if input validation fails.
/// </returns>
int vc_gray_edge_prewitt(IVC* src, IVC* dst) {
	int width = src->width;
	int height = src->height;
	int bytesperline = src->bytesperline;
	int bpl = dst->bytesperline;
	int channels = src->channels;
	int x, y, Gx, Gy, a, b;
	long int pos1;

	if ((src->width <= 0) || (src->height <= 0) || (src->data == NULL)) return 0;
	if (channels != 1)return 0;

	for (y = 1; y < height-1; y++) {
		for (x = 1; x < width-1; x++) {
			pos1 = y * bytesperline + x * channels;
			a = (src->data[pos1 - bytesperline + 1] + src->data[pos1 + 1] + src->data[pos1 + bytesperline + 1]) - (src->data[pos1 - bytesperline - 1] + src->data[pos1 - 1] + src->data[pos1 + bytesperline - 1]);
			Gx = a / 6;
			a = (src->data[pos1 + bytesperline + 1] + src->data[pos1 + bytesperline] + src->data[pos1 + bytesperline - 1]) - (src->data[pos1 - bytesperline + 1] + src->data[pos1 - bytesperline] + src->data[pos1 - bytesperline - 1]);
			Gy = a / 6;
			a = Gx * Gx;
			b = Gy * Gy;
			a = sqrt(a + b);
			dst->data[pos1] = a;
		}
	}
	return 1;
}

/// <summary>
/// Overlays detected edges from a single-channel edge image onto a color image.
/// </summary>
/// <param name="src">
/// Pointer to the source IVC image struct.
/// </param>
/// <param name="dst">
/// Pointer to the destination IVC image struct.
/// </param>
/// <returns>
/// Returns 1 on success (edges drawn on dst), or 0 on failure (e.g., invalid inputs).
/// </returns>
int vc_draw_edge(IVC* src, IVC* dst) {
	int width = src->width;
	int height = src->height;
	int bytesperline = src->bytesperline;
	int bpl = dst->bytesperline;
	int channels = src->channels;
	int channels2 = dst->channels;
	int x, y;
	long int pos1, pos2;

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			pos1 = y * bytesperline + x * channels;
			pos2 = y * bpl + x * channels2;
			if (src->data[pos1] != 0) {
				dst->data[pos2] = 0;
				dst->data[pos2 + 1] = 255;
				dst->data[pos2 + 2] = 0;
			}
		}
	}
	return 1;
}


