#ifndef HASH_H
#define HASH_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdint.h>
#include <ctype.h>
#include<time.h>
#include "bitcoin.h"

typedef struct wallet{
	char* walletID;
	int balance;
	int numOfbitcoins;
	bitcoin* bcList;
}wallet;

typedef struct buckEntry{
	int walletInd;
	transaction* tranList;
}buckEntry;

typedef struct bucket{
	buckEntry* arr;
	int numOfEntries;
	struct bucket* next;
}bucket;

int hash_func(int h,char* walletID);
int userFound(bucket* hashed, char* username, bucket** toWrite, wallet* walletArr);
void print_hashTable(bucket** hashTable, int hash_size, wallet* walletArr);
void print_walletInfo(wallet w);
void store_wallet(wallet* w, char* walletID, char* saveptr, int bc_val, bcIdBuck*** bcIDHashTable);
void createHash(char* balanceFile,wallet** walletArr, bucket*** senderHashTable, bucket*** receiverHashTable, bcIdBuck*** bcIDHashTable, int h1, int h2, int bc_val, int maxBuckEnts,int* numOfWallets);
void free_transaction(transaction* t);
void free_hashTable(bucket** hashTable, int size);
void free_wallet(wallet* w);
#endif