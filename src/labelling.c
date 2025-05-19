//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//           INSTITUTO POLITÉCNICO DO CÁVADO E DO AVE
//                          2022/2023
//             ENGENHARIA DE SISTEMAS INFORMÁTICOS
//                    VISÃO POR COMPUTADOR
//
//             [  DUARTE DUQUE - dduque@ipca.pt  ]
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// Desabilita (no MSVC++) warnings de funções não seguras (fopen, sscanf, etc...)
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <malloc.h>
#include "vc.h"


/// <summary>
/// Etiquetagem de blobs
/// </summary>
/// <param name="src">Imagem binária de entrada</param>
/// <param name="dst">Imagem grayscale (irá conter as etiquetas)</param>
/// <param name="nlabels"> Endereço de memória de uma variável, onde será armazenado o número de etiquetas encontradas.</param>
/// <returns>Retorna um array de estruturas de blobs (objectos), com respectivas etiquetas. É necessário libertar posteriormente esta memória.</returns>
OVC* vc_binary_blob_labelling(IVC* src, IVC* dst, int* nlabels)
{
	unsigned char* datasrc = (unsigned char*)src->data;
	unsigned char* datadst = (unsigned char*)dst->data;
	int width = src->width;
	int height = src->height;
	int bytesperline = src->bytesperline;
	int channels = src->channels;
	int x, y, a, b;
	long int i, size;
	long int posX, posA, posB, posC, posD;
	int labeltable[255] = { 0 };
	int labelarea[255] = { 0 };
	int label = 1; // Etiqueta inicial.
	int num, tmplabel;
	OVC* blobs; // Apontador para array de blobs (objectos) que será retornado desta função.

	// Verificação de erros
	if ((src->width <= 0) || (src->height <= 0) || (src->data == NULL)) return 0;
	if ((src->width != dst->width) || (src->height != dst->height) || (src->channels != dst->channels)) return NULL;
	if (channels != 1) return NULL;

	// Copia dados da imagem binária para imagem grayscale
	memcpy(datadst, datasrc, bytesperline * height);

	// Todos os pixéis de plano de fundo devem obrigatóriamente ter valor 0
	// Todos os pixéis de primeiro plano devem obrigatóriamente ter valor 255
	// Serão atribuídas etiquetas no intervalo [1,254]
	// Este algoritmo está assim limitado a 254 labels
	for (i = 0, size = bytesperline * height; i < size; i++)
	{
		if (datadst[i] != 0) datadst[i] = 255;
	}

	// Limpa os rebordos da imagem binária
	for (y = 0; y < height; y++)
	{
		datadst[y * bytesperline + 0 * channels] = 0;
		datadst[y * bytesperline + (width - 1) * channels] = 0;
	}
	for (x = 0; x < width; x++)
	{
		datadst[0 * bytesperline + x * channels] = 0;
		datadst[(height - 1) * bytesperline + x * channels] = 0;
	}

	// Efectua a etiquetagem
	for (y = 1; y < height - 1; y++)
	{
		for (x = 1; x < width - 1; x++)
		{
			// Kernel:
			// A B C
			// D X

			posA = (y - 1) * bytesperline + (x - 1) * channels; // A
			posB = (y - 1) * bytesperline + x * channels; // B
			posC = (y - 1) * bytesperline + (x + 1) * channels; // C
			posD = y * bytesperline + (x - 1) * channels; // D
			posX = y * bytesperline + x * channels; // X

			// Se o pixel foi marcado
			if (datadst[posX] != 0)
			{
				if ((datadst[posA] == 0) && (datadst[posB] == 0) && (datadst[posC] == 0) && (datadst[posD] == 0))
				{
					datadst[posX] = label;
					labeltable[label] = label;
					label++;
				}
				else
				{
					num = 255;

					// Se A está marcado
					if (datadst[posA] != 0) num = labeltable[datadst[posA]];
					// Se B está marcado, e é menor que a etiqueta "num"
					if ((datadst[posB] != 0) && (labeltable[datadst[posB]] < num)) num = labeltable[datadst[posB]];
					// Se C está marcado, e é menor que a etiqueta "num"
					if ((datadst[posC] != 0) && (labeltable[datadst[posC]] < num)) num = labeltable[datadst[posC]];
					// Se D está marcado, e é menor que a etiqueta "num"
					if ((datadst[posD] != 0) && (labeltable[datadst[posD]] < num)) num = labeltable[datadst[posD]];

					// Atribui a etiqueta ao pixel
					datadst[posX] = num;
					labeltable[num] = num;

					// Actualiza a tabela de etiquetas
					if (datadst[posA] != 0)
					{
						if (labeltable[datadst[posA]] != num)
						{
							for (tmplabel = labeltable[datadst[posA]], a = 1; a < label; a++)
							{
								if (labeltable[a] == tmplabel)
								{
									labeltable[a] = num;
								}
							}
						}
					}
					if (datadst[posB] != 0)
					{
						if (labeltable[datadst[posB]] != num)
						{
							for (tmplabel = labeltable[datadst[posB]], a = 1; a < label; a++)
							{
								if (labeltable[a] == tmplabel)
								{
									labeltable[a] = num;
								}
							}
						}
					}
					if (datadst[posC] != 0)
					{
						if (labeltable[datadst[posC]] != num)
						{
							for (tmplabel = labeltable[datadst[posC]], a = 1; a < label; a++)
							{
								if (labeltable[a] == tmplabel)
								{
									labeltable[a] = num;
								}
							}
						}
					}
					if (datadst[posD] != 0)
					{
						if (labeltable[datadst[posD]] != num)
						{
							for (tmplabel = labeltable[datadst[posD]], a = 1; a < label; a++)
							{
								if (labeltable[a] == tmplabel)
								{
									labeltable[a] = num;
								}
							}
						}
					}
				}
			}
		}
	}

	// Volta a etiquetar a imagem
	for (y = 1; y < height - 1; y++)
	{
		for (x = 1; x < width - 1; x++)
		{
			posX = y * bytesperline + x * channels; // X

			if (datadst[posX] != 0)
			{
				datadst[posX] = labeltable[datadst[posX]];
			}
		}
	}

	//printf("\nMax Label = %d\n", label);

	// Contagem do número de blobs
	// Passo 1: Eliminar, da tabela, etiquetas repetidas
	for (a = 1; a < label - 1; a++)
	{
		for (b = a + 1; b < label; b++)
		{
			if (labeltable[a] == labeltable[b]) labeltable[b] = 0;
		}
	}
	// Passo 2: Conta etiquetas e organiza a tabela de etiquetas, para que não hajam valores vazios (zero) entre etiquetas
	*nlabels = 0;
	for (a = 1; a < label; a++)
	{
		if (labeltable[a] != 0)
		{
			labeltable[*nlabels] = labeltable[a]; // Organiza tabela de etiquetas
			(*nlabels)++; // Conta etiquetas
		}
	}

	// Se não há blobs
	if (*nlabels == 0) return NULL;

	// Cria lista de blobs (objectos) e preenche a etiqueta
	blobs = (OVC*)calloc((*nlabels), sizeof(OVC));
	if (blobs != NULL)
	{
		for (a = 0; a < (*nlabels); a++) blobs[a].label = labeltable[a];
	}
	else return NULL;

	return blobs;
}

/// <summary>
/// Calculates information about labeled binary blobs in an image:
/// - Area
/// - Centroid (center of gravity)
/// - Bounding box (x, y, width, height)
/// - Perimeter
/// </summary>
/// <param name="src">Pointer to the labeled binary image (each pixel has a blob label)</param>
/// <param name="blobs">Array of OVC structures (one for each labeled object)</param>
/// <param name="nlabels">Number of labeled objects (blobs)</param>
/// <returns>1 if successful</returns>
int vc_binary_blob_info(IVC* src, OVC* blobs, int nlabels) {
	int width = src->width;
	int height = src->height;
	int xMin, yMin, xMax, yMax, area;
	int sumX, sumY;
	int x, y;
	int pos;
	for (int i = 0; i < nlabels; i++) {
		xMin = width - 1;
		yMin = height - 1;
		xMax = 0;
		yMax = 0;
		sumX = 0;
		sumY = 0;
		area = 0;
		for (y = 0; y < height; y++) {
			for (x = 0; x < width; x++) {
				pos = y * src->bytesperline + x;
				if (src->data[pos] == blobs[i].label) {
					// Centro de Gravidade
					sumX += x;
					sumY += y;
					area++;

					// Bounding Box
					if (xMin > x) xMin = x;
					if (yMin > y) yMin = y;
					if (xMax < x) xMax = x;
					if (yMax < y) yMax = y;
				}
			}
		}

		//Area & Perimeter
		float raio = (xMax - xMin) / 2;
		/*blobs[i].area = 3.1416 * pow(raio, 2);*/
		blobs[i].area = area;
		blobs[i].perimeter = (3.1415 * raio) * 2;

		//Centro de gravidade
		if (blobs[i].area != 0) {
			blobs[i].xc = sumX / blobs[i].area;
			blobs[i].yc = sumY / blobs[i].area;
		}
		// Bounding Box
		blobs[i].x = xMin;
		blobs[i].y = yMin;
		blobs[i].xf = xMax;
		blobs[i].yf = yMax;
		blobs[i].width = (xMax - xMin) + 1;
		blobs[i].height = (yMax - yMin) + 1;
	}

	return 1;
}

int check_circle(OVC blob) {
	int box = blob.width * blob.height;
	if (blob.area == 0) return 0;
	float ratio = box / blob.area;
	if (ratio > 1.26f && ratio < 1.3) {
		return 0;
	}
	return 1;
}

int vc_delete_blob(IVC* img, OVC blob) {
	int x, y;
	long int pos;
	int bpl = img->bytesperline;
	int channels = img->channels;
	for (int a = 0; a < blob.height; a++) {
		for (int b = 0; b < blob.width; b++) {
			y = a + blob.y;
			x = b + blob.x;
			pos = y * img->bytesperline + x * channels;
			img->data[pos] = 0;
		}
	}
	//blob.area = 0;
	return 1;
}

/// <summary>
/// Draws the bounding box of a blob
/// </summary>
/// <param name="dest">Destination image (where boxes will be drawn)</param>
/// <param name="blobs">Array of blob structures with bounding box info</param>
/// <param name="nlabels">Number of blobs</param>
/// <returns>Returns 1 if successful</returns>
int vc_draw_bounding_box(IVC* dest, OVC* blobs, int nlabels) {
	int channels = dest->channels;
	int bpl = dest->bytesperline;

	for (int i = 0; i < nlabels; i++) {
		int shrink = 20; // Amount to shrink from each side (adjust as needed)

		int x = blobs[i].x + shrink;
		int y = blobs[i].y + shrink;
		int boxw = blobs[i].width - 2 * shrink;
		int boxh = blobs[i].height - 2 * shrink;

		// Ensure valid dimensions
		if (boxw <= 0 || boxh <= 0) continue;

		// Draw top and bottom lines
		for (int k = 0; k < boxw; k++) {
			int pos_top = (y * bpl) + ((x + k) * channels);
			int pos_bottom = ((y + boxh - 1) * bpl) + ((x + k) * channels);

			dest->data[pos_top] = 255;
			dest->data[pos_top + 1] = 0;
			dest->data[pos_top + 2] = 255;

			dest->data[pos_bottom] = 255;
			dest->data[pos_bottom + 1] = 0;
			dest->data[pos_bottom + 2] = 255;
		}

		// Draw left and right lines
		for (int k = 0; k < boxh; k++) {
			int pos_left = ((y + k) * bpl) + (x * channels);
			int pos_right = ((y + k) * bpl) + ((x + boxw - 1) * channels);

			dest->data[pos_left] = 255;
			dest->data[pos_left + 1] = 0;
			dest->data[pos_left + 2] = 255;

			dest->data[pos_right] = 255;
			dest->data[pos_right + 1] = 0;
			dest->data[pos_right + 2] = 255;
		}
	}

	return 1;
}

OVC* vc_check_if_circle(OVC* blobs, int* nLabels, IVC* src) { 
	float areaBoundingBox;
	float value;
	int validCount = 0;

	// First pass to count valid blobs
	for (int i = 0; i < *nLabels; i++) {
		areaBoundingBox = (float)blobs[i].width * (float)blobs[i].height;
		value = areaBoundingBox / (float)blobs[i].area;

		if (value > 1.2f && value < 1.351f && blobs[i].area > 1400) {
			validCount++;
		}
		else {
			blobs[i].area = 0; // Mark invalid
			vc_delete_blob(src, blobs[i]);
		}
	}

	if (validCount == 0) {
		*nLabels = validCount;
		free(blobs);
		return NULL;
	}

	// Allocate new array and copy valid blobs
	OVC* newBlobs = (OVC*)calloc(validCount, sizeof(OVC));
	if (newBlobs != NULL) {
		int j = 0;
		for (int i = 0; i < *nLabels; i++) {
			if (blobs[i].area != 0) {
				newBlobs[j++] = blobs[i];
			}
		}
	}

	*nLabels = validCount; // Update with new count
	free(blobs);
	return newBlobs;
}

/// <summary>
/// Checks for collisions betwen two blobs
/// </summary>
/// <param name="firstBlob">First blob</param>
/// <param name="secondBlob">Second blob</param>
/// <returns>Returns 1 if there is a collision, returns 0 if there is not a collision</returns>
int vc_check_collisions(OVC firstBlob, OVC secondBlob) {
	int x1 = firstBlob.x;
	int y1 = firstBlob.y;
	int x2 = secondBlob.x;
	int y2 = secondBlob.y;
	// Check if the bounding boxes overlap
	if (x1 < x2 + secondBlob.width && x1 + firstBlob.width > x2 && y1 < y2 + secondBlob.height && y1 + firstBlob.height > y2) {
		return 1;
	}
	return 0;
}

int vc_main_collisions(OVC* firstBlobs, OVC* secondBlobs, int firstBlob, int secondBlob) {
	if (firstBlobs == NULL || secondBlobs == NULL) return 0;
	int cont = 0;
	int res = 0;
	for (int i = 0; i < firstBlob; i++) {
		for (int j = 0; j < secondBlob; j++) {
			res = vc_check_collisions(firstBlobs[i], secondBlobs[i]);
			if (res == 1) break;
		}
		if (res == 0) cont++;
	}
	return cont;
}

