#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdint.h>
#include <ctype.h>
#include <time.h>
#include "hash.h"
#include "bitcoin.h"


typedef struct tranIdBuck{
	char** arr;
	int numOfEntries;
	struct tranIdBuck* next;
}tranIdBuck;

char* concat(char *transactionID, char *transaction);

bitcoin* chooseBitcoin(wallet user,int amount, bitcoin** prev);
int transIDFound(tranIdBuck* hashed, char* transactionID, tranIdBuck** toWrite);
void makeTransaction(char* tranRec, bucket*** senderHashTable, bucket*** receiverHashTable, int h1, int h2, wallet** walletArr, tranIdBuck*** transIDHashTable, transTD* latestTD, char** maxTransID, int mode);

void storeMaxDateTime(int day, int month, int year, int time, int minutes, transTD* latestTD);
int dateIsLater(int day, int month, int year, int time, int minutes, transTD latestTD);
int argDigitsOnly(char *arg);

int find_Earns_Pays(transaction* head, char* arg1, char* arg2, char* arg3, char* arg4);

void print_transactions(wallet w, wallet* walletArr, bucket** senderHashTable, bucket** receiverHashTable, int h1, int h2);
void printTran(transaction* curr);

void free_tranIDhashTable(tranIdBuck** hashTable);
void free_transaction(transaction* t);



#endif