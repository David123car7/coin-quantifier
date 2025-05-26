/*****************************************************************//**
 * \file   colors.c
 * \brief  Functions for color space conversions and color-based segmentation.
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
/// Converts an image from GBR format to RGB format by swapping the Red and Blue channels.
/// </summary>
/// <param name="src">Pointer to the source IVC image in GBR format. The image must be color (3 channels).</param>
/// <returns>Returns 1 if the conversion is successful, or 0 if an error occurs</returns>
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

/// <summary>
/// Converts an image from RGB color space to HSV (Hue, Saturation, Value) color space.
/// The resulting HSV values are scaled to the range [0, 255].
/// </summary>
/// <param name="src">Pointer to the source IVC image in RGB format (must be 3 channels).</param>
/// <param name="dst">Pointer to the destination IVC image where HSV data will be stored (must also be 3 channels).</param>
/// <returns>Returns 1 if the conversion is successful, or 0 if an error occurs (e.g., invalid image or channel mismatch).</returns>
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
				h = 120 + 60 * (src->data[pos3] - src->data[pos1]) / (max - min);
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

/// <summary>
/// Segments an image in HSV color space by filtering pixels within a given range of H, S, and V.
/// The pixels within the range are set to white (255,255,255); others are set to black (0,0,0).
/// </summary>
/// <param name="src">Pointer to the HSV source image (3 channels, H[0–360] scaled to [0–255]).</param>
/// <param name="dst">Pointer to the destination image where segmentation result will be stored.</param>
/// <param name="hmin">Minimum hue value [0–360].</param>
/// <param name="hmax">Maximum hue value [0–360].</param>
/// <param name="smin">Minimum saturation value [0–100].</param>
/// <param name="smax">Maximum saturation value [0–100].</param>
/// <param name="vmin">Minimum value (brightness) [0–100].</param>
/// <param name="vmax">Maximum value (brightness) [0–100].</param>
/// <returns>1 if successful, 0 if an error occurs.</returns>
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
