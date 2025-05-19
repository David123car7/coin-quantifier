/*****************************************************************//**
 * \file   utils.c
 * \brief  General utility functions.
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
			dst->data[dPos1 + 1] = src->data[sPos1];
			dst->data[dPos1 + 2] = src->data[sPos1];
		}
	}
	return 1;
}

int vc_limit(IVC* src, IVC* dst, int y) {
	int width = src->width;
	int bpl = src->bytesperline;
	int channels = src->channels;
	long int pos1, pos2;
	int y1 = y;
	for (int y2 = y; y2 < y * 2; y2++) {
		for (int x = 0; x < width; x++) {
			pos1 = y2 * bpl + x * channels;
			pos2 = (y2 - y1) * bpl + x * channels;
			dst->data[pos2] = src->data[pos1];
			dst->data[pos2 + 1] = src->data[pos1 + 1];
			dst->data[pos2 + 2] = src->data[pos1 + 2];
		}
	}
	return 1;
}

int vc_limit2(IVC* src, IVC* dst, int y) {
	int width = src->width;
	int bpl = src->bytesperline;
	int channels = src->channels;
	long int pos1, pos2;
	int y1 = y;
	for (int y2 = y; y2 < y * 2; y2++) {
		for (int x = 0; x < width; x++) {
			pos1 = y2 * bpl + x * channels;
			pos2 = (y2 - y1) * bpl + x * channels;
			dst->data[pos1] = src->data[pos2];
			dst->data[pos1 + 1] = src->data[pos2 + 1];
			dst->data[pos1 + 2] = src->data[pos2 + 2];
		}
	}
	return 1;
}