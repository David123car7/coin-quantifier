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
	int labeltable[256] = { 0 };
	int labelarea[256] = { 0 };
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
	int xMin, yMin, xMax, yMax;
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
		for (y = 0; y < height; y++) {
			for (x = 0; x < width; x++) {
				pos = y * src->bytesperline + x;
				if (src->data[pos] == blobs[i].label) {
					// Centro de Gravidade
					sumX += x;
					sumY += y;

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
		blobs[i].area = 3.1416 * pow(raio, 2);
		blobs[i].perimeter = (3.1415 * raio) * 2;

		//Centro de gravidade
		blobs[i].xc = sumX / blobs[i].area;
		blobs[i].yc = sumY / blobs[i].area;

		// Bounding Box
		blobs[i].x = xMin;
		blobs[i].y = yMin;
		blobs[i].width = (xMax - xMin) + 1;
		blobs[i].height = (yMax - yMin) + 1;
	}

	return 1;
}

/// <summary>
/// Draws the bounding box of a blob
/// </summary>
/// <param name="src">Source Image</param>
/// <param name="dest">Destination image (where boxes will be drawn)</param>
/// <param name="blobs">Array of blob structures with bounding box info</param>
/// <param name="nlabels">Number of blobs</param>
/// <returns>Returns 1 if successful</returns>
int vc_draw_bounding_box(IVC* src, IVC* dest, OVC* blobs, int nlabels) {
	int width = src->width;
	int height = src->height;
	int pos, pos2;
	for (int i = 0; i < nlabels; i++) {
		pos = blobs[i].y * src->bytesperline + blobs[i].x;
		pos2 = pos + src->bytesperline * blobs[i].height;
		dest->data[pos] = 255;

		//Top & Bottom
		for (int k = 0; k < blobs[i].width; k++) {
			pos++;
			pos2++;
			dest->data[pos] = 255;
			dest->data[pos2] = 255;
		}

		//Right & Left
		pos2 = pos2 - blobs[i].width;
		for (int x = 0; x < blobs[i].height; x++) {
			pos = pos + src->bytesperline;
			pos2 = pos2 - src->bytesperline;
			dest->data[pos] = 255;
			dest->data[pos2] = 255;
		}
	}
	return 1;
}






