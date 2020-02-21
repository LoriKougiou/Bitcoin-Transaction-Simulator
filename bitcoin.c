#include "bitcoin.h"

//synarthseis gia to tree

//dhmiourgei kai desmevei xwro gia ti riza tou dendrou enos bitcoin
void create_root(bctNode** root, int bitcoinID, char* walletID, int bc_val){
	*root=NULL;
	(*root)=malloc(sizeof(bctNode));
	if(*root==NULL){
		fprintf(stderr, "Malloc failed \n"); 
	    exit(-1);
	}

	(*root)->bitcoinID=bitcoinID;

	(*root)->walletID=walletID;
	(*root)->tran=NULL;
	(*root)->amount=bc_val;
	(*root)->right=NULL;
	(*root)->left=NULL;
}


//ektipwnei to dendro se proerder traversal
void preorderTraversal(bctNode* root){
   if(root != NULL) {
   	  printf("BITCOIN: %d\n",root->bitcoinID);
      printf("WalletID: %s\n",root->walletID);

      printf("Balance left: %d\n",root->amount);
      preorderTraversal(root->left);
      preorderTraversal(root->right);
   }
}


//psaxnei ean to dothen bitcoinID aniki hdh se kapoion, ean nai, tote epistrefei to bucket kai th thesh sto bucket, alliws epistrefei -1
int bitcoinFound(bcIdBuck* hashed, int bitcoinID, bcIdBuck** toWrite){
	bcIdBuck* curr=hashed;
	*toWrite=hashed;
	int i;

	while(curr!=NULL){
		for(i=0;i<curr->numOfEntries;i++){
			if(curr->arr[i]->bitcoinID==bitcoinID) return i;
		}
		curr=curr->next;
		if(curr!=NULL) *toWrite=curr;
	}
	return -1;
}

//apothikevei to neo bitcoin, kai dimiourgei ti riza tou dedrou gia auto
void store_bitcoin(bitcoin* curr, char* walletID, char* token, bcBuckEntry* entry, int bc_val){

	curr->bcBalance=bc_val;
	curr->next=NULL;
	if(entry->root==NULL){
		create_root(&(entry->root), atoi(token), walletID, bc_val);
		entry->bitcoinID=atoi(token);
	}
	curr->rootPos=entry;

}

//epistrefei to ipsos tou dendrou
int height(bctNode* root){ 
    if (root==NULL) return 0; 
    else{ 
        //ipologizw to ipsos kathe ipodendrou
        int lh=height(root->left); 
        int rh=height(root->right); 
  
        //to ipsos tou dendrou einai to max ipsos ipodendrou
        if (lh>rh) return(lh+1); 
        else return(rh+1); 
    } 
} 

//psaxnei epipedo epipedo to dendro gia na vrei ton komvo me tin teleutaia xrhsh tou walletID sto sigekrimeno bitcoin
bctNode* searchLastState(bctNode* root, char* walletID){ 
    int i, h=height(root); 
    bctNode* lastState=NULL;
 
    for(i=1;i<=h;i++){ 
        searchGivenLevel(root,i, walletID, &lastState); 
    } 
    return lastState;
} 
  
//psaxnei to dentro epipedo ana epipedo gia na vrei ton komvo pou exei krathsei tin teleutaia fora pou xrisimopoihse to dothen wallet to bitcoin
void searchGivenLevel(bctNode* root, int level, char* walletID, bctNode** lastState){ 
    if (root==NULL) return;
    if (level==1){
    	if((strcmp(walletID,root->walletID)==0)&&(root->amount!=0)){
    		(*lastState)=root;
    	}
    } 
    else if (level>1){ 
        searchGivenLevel(root->left, level-1, walletID, lastState); 
        searchGivenLevel(root->right, level-1, walletID, lastState); 
    } 
}


//synarthsh pou ektipwnei ena transaction
void printTran(transaction* curr){
	printf("%s %s %s %d %d/%d/%d ",curr->transactionID,curr->senderID, curr->receiverID, curr->amount, curr->td.day, curr->td.month, curr->td.year);
	if(curr->td.time/10==0) printf("0%d:", curr->td.time);
	else printf("%d:", curr->td.time);
	if(curr->td.minutes/10==0) printf("0%d\n", curr->td.minutes);
	else printf("%d\n", curr->td.minutes);
}

//ektipwnei to dedndro ana epipeda
void printTree(bctNode* root){ 
    int i, h=height(root); 
 
    for(i=1;i<=h;i++){ 
        printGivenLevel(root,i); 
    } 
}  

//ektipwnei to dosmeno epipedo tou dendorou
void printGivenLevel(bctNode* root, int level){ 
    if (root==NULL) return;
    if (level==1){
    	printf("%d\n",root->bitcoinID);
    	printf("%s\n", root->walletID);
    	printf("%d\n", root->amount);
    	if(root->tran!=NULL) printTran(root->tran);
    	
    } 
    else if (level>1){ 
        printGivenLevel(root->left, level-1); 
        printGivenLevel(root->right, level-1); 
    } 
}

//synarthsh pou pairnei tin riza enos dendrou kai vriskei poso apo to arxiko tou balance den exei xrisimopoiithei se sinallagi akoma
void unspent(bctNode* root, int* amount){
	if(root==NULL) return;
	(*amount)=root->amount;
	unspent(root->right, amount);
}

//borei na min einai o pio orthodoksos tropos, alla genika paratirisa to pattern oti ean n o # twn komvwn tou tree, tote #transactions=(n-1)/2
//opote ousiastika metraw anadromika tous komvous tou tree kai epistrefw to (n-1)/2 (ousiastika  ws mia sinallagi kathe komvo me ta left 
//kai right paidia tou)
int participatedTrans(bctNode* root){
	int numOfNodes=countNodes(root);
	return ((numOfNodes-1)/2);
}

//metraei tous komvous tou dendrou
int countNodes(bctNode* root){
	int numOfNodes=1;
	if(root==NULL) return 0;
	else{
		if(root->left!= NULL) numOfNodes+=countNodes(root->left);
	    if (root->right!=NULL) numOfNodes+= countNodes(root->right);
	    return (numOfNodes);
	}    
}

//ektipwnei ta transactions tou epipedou, alla elegxontas ta diplotipa kai paraleipontas ta
void printLevelTrans(bctNode* root, int level, transaction** prev,int* hasTrans){
	if (root==NULL) return;
    if (level==1){
    	if((root->tran!=NULL)&&((*prev)==NULL)){
    		printTran(root->tran);  
    		(*prev)=root->tran;
    		(*hasTrans)=1;
    	}
    	else if((root->tran!=NULL)&&((strcmp(root->tran->transactionID,(*prev)->transactionID)!=0))){
    		printTran(root->tran);  
    		(*prev)=root->tran;
    		(*hasTrans)=1;
    	}   	
    } 
    else if (level>1){ 
        printLevelTrans(root->left, level-1, prev, hasTrans); 
        printLevelTrans(root->right, level-1, prev, hasTrans); 
    }
}

//ektipwnei tis sinallages stis opoioes simmeteixe to bitcoin
void tracecoin(bctNode* root){
	int i, h=height(root), flag=0; 
	transaction* prev=NULL;
 
    for(i=1;i<=h;i++){ 
        printLevelTrans(root, i, &prev,&flag);	   
    }
    if(!flag) fprintf(stderr, "Bitcoin %d has not participated in any transactions.\n", root->bitcoinID );
}


//synarthsh pou diatrexei to dendro kai ipologizei se posa transactions exei simmetasxei to bitcoin
void free_bitcoin(bitcoin* bc){
	bc->rootPos=NULL;
	bc->next=NULL;
	free(bc);
	bc=NULL;
}

//anadromikh synarthsh pou diagrafei to dentro
void free_bcTree(bctNode* root){
    
    if (root == NULL) return;   	
  	//diagrafw ta paidia
    free_bcTree(root->left); 
    free_bcTree(root->right); 
    
    //diagrafw ton komvo    
    root->walletID=NULL;
    root->tran=NULL;
    free(root);
    root=NULL;
}

//diagrafei to bcIdhashtable
void free_bcHashTable(bcIdBuck** hashTable){
	int i,j;
	bcIdBuck* currBuck=NULL, *tempBuck=NULL;

	for(i=0;i<11;i++){
		if(hashTable[i]!=NULL){
			currBuck=hashTable[i];
			while(currBuck!=NULL){
				for(j=0;j<currBuck->numOfEntries;j++){
					free_bcTree(currBuck->arr[j]->root);
					free(currBuck->arr[j]);
				}
				tempBuck=currBuck->next;
				free(currBuck->arr);
				free(currBuck);
				currBuck=tempBuck;
			}
		}
	}
	free(hashTable);
}

