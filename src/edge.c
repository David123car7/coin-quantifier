#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <malloc.h>
#include "vc.h"

int vc_gray_edge_prewitt(IVC* src, IVC* dst, float th) {
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

int vc_gray_histogram_equalization(IVC* src, IVC* dst) {
	int width = src->width;
	int height = src->height;
	int bytesperline = src->bytesperline;
	int bpl = dst->bytesperline;
	int channels = src->channels;
	int x, y, c;
	int b = 0;
	long int pos1;

	if ((src->width <= 0) || (src->height <= 0) || (src->data == NULL)) return 0;
	if (channels != 1)return 0;

	int arr[256];
	float arr2[256];
	for (int i = 0; i < 256; i++) {
		arr[i] = 0;
		arr2[i] = 0;
	}
	float a = src->height * src->width;

	for (y = 1; y < height - 1; y++) {
		for (x = 1; x < width - 1; x++) {
			pos1 = y * bytesperline + x * channels;
			arr[src->data[pos1]]++;
		}
	}
	for (int i = 0; i < 256; i++) {
		arr2[i] = arr[i] / a;
		if (i != 0) {
			arr2[i] += arr2[i - 1];
		}
	}
	while (arr2[b]==0) {
		b++;
	}
	for (y = 1; y < height - 1; y++) {
		for (x = 1; x < width - 1; x++) {
			pos1 = y * bytesperline + x * channels;
			c = (arr2[src->data[pos1]] - arr2[b]) / (1 - arr2[b]) * 255;
			dst->data[pos1] = c;
		}
	}
	return 1;
}
