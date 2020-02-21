#ifndef BITCOIN_H
#define BITCOIN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdint.h>
#include <ctype.h>
#include <time.h>



typedef struct transTD{
	int day;
	int month;
	int year;
	int time;
	int minutes;
}transTD;

typedef struct transaction{
	char* transactionID;
	char* senderID;
	char* receiverID;
	int amount;
	transTD td;
	struct transaction* next;
}transaction;
 
typedef struct bctNode{
	char* walletID;
	int bitcoinID;
	int amount;
	transaction* tran;
	struct bctNode* right; //sto deksi apothikevetai to ipolipomeno tou sender, ean iparxei ipoloipomeno
	struct bctNode* left; //sto aristero apothikevetai o neos receipient
}bctNode;


//o logos pou to hash den krataei olo to bitcoin alla mono ta dendera, einai epeidh otan tha arxisei to bitcoin na simmetexei se sinallages tha vrisketai se pollous xristes
typedef struct bcBuckEntry{
	bctNode* root; 
	int bitcoinID;
}bcBuckEntry;

typedef struct bcIdBuck{
	bcBuckEntry** arr;
	int numOfEntries;
	struct bcIdBuck* next;
}bcIdBuck;

typedef struct bitcoin{
	bcBuckEntry* rootPos;
	int bcBalance;
	struct bitcoin* next;

}bitcoin;

int bitcoinFound(bcIdBuck* hashed, int bitcoinID, bcIdBuck** toWrite);
void store_bitcoin(bitcoin* curr, char* walletID, char* token, bcBuckEntry* entry, int bc_val);
void free_bitcoin(bitcoin* bc);

void create_root(bctNode** root, int bitcoinID, char* walletID, int bc_val);
int height(bctNode* root);
bctNode* searchLastState(bctNode* root, char* walletID);
void searchGivenLevel(bctNode* root, int level, char* walletID, bctNode** lastState);
void preorderTraversal(bctNode* root);

void printTran(transaction* curr);
void printTree(bctNode* root);
void printGivenLevel(bctNode* root, int level);

void unspent(bctNode* root, int* amount);
int participatedTrans(bctNode* root);
int countNodes(bctNode* root);

void printLevelTrans(bctNode* root, int level, transaction** prev, int* hasTrans);
void tracecoin(bctNode* root);

void free_bcTree(bctNode* root);
void free_bcHashTable(bcIdBuck** hashTable);

#endif