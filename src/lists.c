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
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <malloc.h>
#include "vc.h"

CoinsDict* CreateCoinsDictionaryEmpty() {
	CoinsDict* dict = (CoinsDict*)malloc(sizeof(CoinsDict));
	if (dict == NULL) return NULL;
	dict->coins = NULL;
	dict->key = -1;
	dict->next = NULL;
	return dict;
}

CoinsDict* CreateCoinsDictionary(Coins* coins, int key) {
	if (coins == NULL) return NULL;
	if (key < 0) return NULL;
	CoinsDict* dict = (CoinsDict*)malloc(sizeof(CoinsDict));
	if (dict == NULL) return NULL;
	dict->coins = coins;
	dict->key = key;
	dict->next = NULL;
	return dict;
}

CoinsDict* AddDictionary(CoinsDict* dict, CoinsDict* newDict) {
	if (newDict == NULL) return NULL;
	CoinsDict* aux = dict;
	
	if (aux == NULL) {
		dict = newDict;
	}
	else {
		while (aux->next != NULL) {
			aux = aux->next;
		}
		aux->next = newDict;
	}

	return dict;
}

Coins* CreateCoin(OVC coin) {
	Coins* coins = (Coins*)malloc(sizeof(Coins));
	if (coins == NULL) return NULL;
	coins->area = coin.area;
	coins->x = coin.x;
	coins->y = coin.y;
	coins->nextCoin = NULL;
	return coins;
}

Coins* AddCoin(Coins* coins, Coins* newCoin) {
	if (newCoin == NULL) return NULL;
	Coins* aux = coins;

	if (aux == NULL) {
		coins = newCoin;
	}
	else {
		while (aux->nextCoin != NULL) {
			aux = aux->nextCoin;
		}
		aux->nextCoin = newCoin;
	}

	return coins;
}

#pragma region Filter
CoinsDict* FilterCoinsDict(CoinsDict* coinsDict) {
	if (coinsDict == NULL) return NULL;
	CoinsDict* dictAux = coinsDict;

	while (dictAux->next != NULL) {
		Coins* coinsAux = dictAux->coins;
		while (coinsAux->nextCoin != NULL) {
			int 
		}
		dictAux = dictAux->next;
	}
}
#pragma endregion

