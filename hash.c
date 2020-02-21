#include "hash.h"

//hash function gia ola ta hashtables
int hash_func(int h,char* walletID){
	if(walletID!=NULL){
		char c=walletID[0];
		int key=c-'0';
		return (key%h);
	}
	else return -1;

}

//ftiaxnei kai apothikevei to wallet me ta dosmena dedomena, enimerwnontas kai to bitcoinHash
void store_wallet(wallet* w, char* walletID, char* saveptr, int bc_val, bcIdBuck*** bcIDHashTable){
	//ftiaxnw to wallet tou user kai to apothikevw ston array me wallets
	int numOfbitcoins=0, hash_index, maxBuckEnts, numOfEntries;
	char* token=walletID;
	bcIdBuck* currBuck;
	bitcoin* head, *curr, *prev;

	w->walletID=malloc((strlen(walletID)+1)*sizeof(char));
	if(w->walletID==NULL){
        fprintf(stderr, "Malloc failed.\n");
        exit(1);
    }
    strcpy(w->walletID, walletID);
    

    w->bcList=NULL;
    w->bcList=malloc(sizeof(bitcoin));
    if (w->bcList== NULL){
	    fprintf(stderr, "Malloc failed \n"); 
	    exit(-1);
	}

    head=w->bcList;
    curr=w->bcList;
    prev=curr;

    //twra tha apothikefsw ta valid bitcoins, kai ftiaxnw tis rizes twn dendrwn tous
    while(token!=NULL){
    	token=strtok_r(NULL," \n",&saveptr);
    	if(token!=NULL){
    		hash_index=hash_func(11, token);
    		maxBuckEnts=(150-sizeof(int)-sizeof(bcIdBuck*))/sizeof(bcBuckEntry*);

    		//ean to hashtable den exei kamia eggrafi gia auto to hash value
    		if((*bcIDHashTable)[hash_index]==NULL){
    			(*bcIDHashTable)[hash_index]=malloc(sizeof(bcIdBuck));
				if((*bcIDHashTable)[hash_index]==NULL){
			        fprintf(stderr, "Malloc failed.\n");
			        exit(1);
			    }
			    (*bcIDHashTable)[hash_index]->arr=malloc(maxBuckEnts*sizeof(bcBuckEntry*));
			    if((*bcIDHashTable)[hash_index]->arr==NULL){
			        fprintf(stderr, "Malloc failed.\n");
			        exit(1);
			    }
			    (*bcIDHashTable)[hash_index]->numOfEntries=0;
			    (*bcIDHashTable)[hash_index]->next=NULL;

			    numOfEntries=(*bcIDHashTable)[hash_index]->numOfEntries;
			    (*bcIDHashTable)[hash_index]->arr[numOfEntries]=malloc(sizeof(bcBuckEntry));
			    if((*bcIDHashTable)[hash_index]->arr[numOfEntries]==NULL){
			        fprintf(stderr, "Malloc failed.\n");
			        exit(1);
			    }

			    (*bcIDHashTable)[hash_index]->arr[numOfEntries]->root=NULL;

			    
			    if(numOfbitcoins==0){
    				prev=curr;
    				store_bitcoin(curr, w->walletID, token, (*bcIDHashTable)[hash_index]->arr[numOfEntries], bc_val);
					curr=curr->next;
    			}
    			else{
    				curr=malloc(sizeof(bitcoin)); //desmevw xwro gia to kathgorhma sth lista
					if (curr== NULL) {
					    fprintf(stderr, "Malloc failed \n"); 
					    exit(-1);
					}
					store_bitcoin(curr, w->walletID, token, (*bcIDHashTable)[hash_index]->arr[numOfEntries], bc_val);	   			
					prev->next=curr;
					prev=curr;
					curr=curr->next;
    			}		    			
    			numOfbitcoins+=1;
    			(*bcIDHashTable)[hash_index]->numOfEntries++;
    		}
    		else{ //ean exei hdh eggrafes me auto to hash value
    			if(bitcoinFound((*bcIDHashTable)[hash_index], atoi(token), &currBuck)==-1){
	    			if(currBuck->numOfEntries<maxBuckEnts){ //an o user xwraei sto bucket

	    				numOfEntries=currBuck->numOfEntries;
	    				currBuck->arr[numOfEntries]=malloc(sizeof(bcBuckEntry));
					    if(currBuck->arr[numOfEntries]==NULL){
					        fprintf(stderr, "Malloc failed.\n");
					        exit(1);
					    }
	    				currBuck->arr[numOfEntries]->root=NULL;

						if(numOfbitcoins==0){
		    				prev=curr;
		    				store_bitcoin(curr, w->walletID, token, currBuck->arr[numOfEntries], bc_val);
							curr=curr->next;
		    			}
		    			else{
		    				curr=malloc(sizeof(bitcoin)); //desmevw xwro gia to kathgorhma sth lista
							if (curr== NULL) {
							    fprintf(stderr, "Malloc failed \n"); 
							    exit(-1);
							}
							store_bitcoin(curr, w->walletID, token, currBuck->arr[numOfEntries], bc_val);	   			
							prev->next=curr;
							prev=curr;
							curr=curr->next;
		    			}		    			
		    			numOfbitcoins+=1;
		    			currBuck->numOfEntries++;
					}
					else if(currBuck->numOfEntries==maxBuckEnts){ //ean o user den xwraei so bucket

						//ftiaxnw kainourgio bucket
						currBuck->next=malloc(sizeof(bcIdBuck));
						if(currBuck->next==NULL){
					        fprintf(stderr, "Malloc failed.\n");
					        exit(1);
					    }
					    currBuck=currBuck->next;
					    currBuck->arr=malloc(maxBuckEnts*sizeof(bcBuckEntry*));
					    if(currBuck->arr==NULL){
					        fprintf(stderr, "Malloc failed.\n");
					        exit(1);
					    }

					    currBuck->numOfEntries=0;
					    currBuck->next=NULL;

					    numOfEntries=currBuck->numOfEntries;
	    				currBuck->arr[numOfEntries]=malloc(sizeof(bcBuckEntry));
					    if(currBuck->arr[numOfEntries]==NULL){
					        fprintf(stderr, "Malloc failed.\n");
					        exit(1);
					    }
					    currBuck->arr[numOfEntries]->root=NULL;

						if(numOfbitcoins==0){
		    				prev=curr;
		    				store_bitcoin(curr, w->walletID, token, currBuck->arr[numOfEntries], bc_val);
							curr=curr->next;
		    			}
		    			else{
		    				curr=malloc(sizeof(bitcoin)); //desmevw xwro gia to kathgorhma sth lista
							if (curr== NULL) {
							    fprintf(stderr, "Malloc failed \n"); 
							    exit(-1);
							}
							store_bitcoin(curr, w->walletID, token, currBuck->arr[numOfEntries], bc_val);	   			
							prev->next=curr;
							prev=curr;
							curr=curr->next;
		    			}		    			
		    			numOfbitcoins+=1;
		    			currBuck->numOfEntries++;
					}
					else{
						fprintf(stderr, "Something's wrong\n");
					}    			
	    		}
	    		else fprintf(stderr,"Bitcoin with ID %d is already owned by someone else.\n", atoi(token));	
    		}
    			    		
    	}
    }

    if(numOfbitcoins==0){ //se auti ti periptwsh den exei kanena bitcoin 
    	free(w->bcList);
    	w->bcList=NULL;
    }
    else w->bcList=head;

    w->balance=numOfbitcoins*bc_val;

}


//synarthsh pou elegxei sto hashtable an to walletid iparxei idi, an nai epistrefei ti thesi tou alliws epistrefei -1
int userFound(bucket* hashed, char* username, bucket** toWrite, wallet* walletArr){
	bucket* curr=hashed;
	*toWrite=hashed;
	int i;

	while(curr!=NULL){
		for(i=0;i<curr->numOfEntries;i++){
			if(strcmp(walletArr[curr->arr[i].walletInd].walletID, username)==0) return i;
		}
		curr=curr->next;
		if(curr!=NULL) *toWrite=curr;
	}
	return -1;
}

//synarthsh pou ektipwnei ola ta dedomena pou einai apothikevmena sto hashtable
void print_hashTable(bucket** hashTable, int hash_size, wallet* walletArr){
	int i, j, k;
	bucket* curr;

	for(i=0;i<hash_size;i++){
        curr=hashTable[i];
        k=0;
        printf("########### HASH VALUE: %d ############\n",i );
        while(curr!=NULL){
            printf("        Bucket %d:\n\n", k);
            for(j=0;j<curr->numOfEntries;j++){
                print_walletInfo(walletArr[curr->arr[j].walletInd]);
            }
            curr=curr->next;
            k++;
        }
    }
}

//ftiaxnei to hashtable me vasei to balance file pou exei dothei
void createHash(char* balanceFile,wallet** walletArr, bucket*** senderHashTable, bucket*** receiverHashTable, bcIdBuck*** bcIDHashTable, int h1, int h2, int bc_val, int maxBuckEnts, int* numOfWallets){
	char buff[200], *token, *line, *saveptr;
	int sender_index, receiver_index, i, numOfwallets=0, numOfEntries, walletInd;
	bucket* currBuck=NULL, *toWrite=NULL;
	FILE* fp;

	//desmevw ton array me ta wallets
	if((*walletArr)==NULL){
		fp=fopen(balanceFile,"r");
		if(fp==NULL) {
			fprintf(stderr, "Invalid command.The file named %s does not exist.\n",balanceFile);
			exit(1);
		}

		while(fgets(buff,200,fp)) numOfwallets++;
		rewind(fp);

		*numOfWallets=numOfwallets;
		(*walletArr)=malloc(numOfwallets*sizeof(wallet));
		if((*walletArr)==NULL){
			fprintf(stderr, "Malloc failed \n"); 
	    	exit(-1);
		}
	}
	//prwta desmevw xwro gia to hash twn bitcoinIDs ean einai adeio. epilegw 11 hashvals giati enas prime dinei ligotera collisions
	if((*bcIDHashTable)==NULL){
		(*bcIDHashTable)=malloc(11*sizeof(bcIdBuck*));
		if((*bcIDHashTable)==NULL){
			fprintf(stderr, "Malloc failed.\n");
			exit(1);
		}
		for(i=0;i<11;i++){
			(*bcIDHashTable)[i]=NULL;
		}
	}

	//prwta tha apothikefsw tous xristes
	walletInd=0;

	while(fgets(buff,200,fp)){

		//kanw copy to string giati tha to xreiastw gia na metrhsw ta bitcoins argotera
		line=malloc((strlen(buff)+1)*sizeof(char));
		if (line== NULL) {
		    fprintf(stderr, "Malloc failed \n"); 
		    exit(-1);
		}
		strcpy(line, buff);

		saveptr=buff;


		token=strtok_r(buff, " \n", &saveptr);
		
		sender_index=hash_func(h1, token);

		//####################### SENDER HASH ##################################
		//an einai o prwtos user pou anhkei se auto to hashval
		if((*senderHashTable)[sender_index]==NULL){
			(*senderHashTable)[sender_index]=malloc(sizeof(bucket));
			if((*senderHashTable)[sender_index]==NULL){
		        fprintf(stderr, "Malloc failed.\n");
		        exit(1);
		    }
		    (*senderHashTable)[sender_index]->arr=malloc(maxBuckEnts*sizeof(buckEntry));
		    if((*senderHashTable)[sender_index]->arr==NULL){
		        fprintf(stderr, "Malloc failed.\n");
		        exit(1);
		    }
		    (*senderHashTable)[sender_index]->numOfEntries=0;
		    (*senderHashTable)[sender_index]->next=NULL;

		    numOfEntries=(*senderHashTable)[sender_index]->numOfEntries;
			
			store_wallet(&((*walletArr)[walletInd]), token, saveptr, bc_val, bcIDHashTable);

			(*senderHashTable)[sender_index]->arr[numOfEntries].walletInd=walletInd;
			(*senderHashTable)[sender_index]->arr[numOfEntries].tranList=NULL;
			walletInd++;
		    (*senderHashTable)[sender_index]->numOfEntries++;

		}
		else{ //an auto to hash value exei kai allous user mesa			
			currBuck=NULL;

			//arxika tha elegksw an iparxei o user sto hash
			if(userFound((*senderHashTable)[sender_index], token, &currBuck, *walletArr)==-1){
				if(currBuck->numOfEntries<maxBuckEnts){ //an o user xwraei sto bucket

					numOfEntries=currBuck->numOfEntries;
					//ftiaxnw to wallet tou user
					store_wallet(&((*walletArr)[walletInd]), token, saveptr, bc_val, bcIDHashTable);

					currBuck->arr[numOfEntries].walletInd=walletInd;
					currBuck->arr[numOfEntries].tranList=NULL;
					walletInd++;
				    currBuck->numOfEntries++;
				}
				else if(currBuck->numOfEntries==maxBuckEnts){ //ean o user den xwraei so bucket

					//ftiaxnw kainourgio bucket
					currBuck->next=malloc(sizeof(bucket));
					if(currBuck->next==NULL){
				        fprintf(stderr, "Malloc failed.\n");
				        exit(1);
				    }
				    currBuck=currBuck->next;
				    currBuck->arr=malloc(maxBuckEnts*sizeof(buckEntry));
				    if(currBuck->arr==NULL){
				        fprintf(stderr, "Malloc failed.\n");
				        exit(1);
				    }
				    currBuck->numOfEntries=0;
				    currBuck->next=NULL;

				    numOfEntries=currBuck->numOfEntries;
					
					//ftiaxnw to wallet tou user
					store_wallet(&((*walletArr)[walletInd]), token, saveptr, bc_val, bcIDHashTable);
				    
				    currBuck->arr[numOfEntries].walletInd=walletInd;
					currBuck->arr[numOfEntries].tranList=NULL;
					walletInd++;
				    currBuck->numOfEntries++;
				}
				else{
					fprintf(stderr, "Something's wrong\n");
				}
			}
			else{
				fprintf(stderr, "User already in the table\n");
				(*numOfWallets)-=1;

			}
		}
		free(line);
	}

	//####################### RECEIVER HASH ##################################
	//se auth th fasi exei dimiourgithei idi to hash tou sender kai to wallet array me monadikous users
	for(i=0;i<walletInd;i++){
		receiver_index=hash_func(h2, (*walletArr)[i].walletID);

		if((*receiverHashTable)[receiver_index]==NULL){
			(*receiverHashTable)[receiver_index]=malloc(sizeof(bucket));
			if((*receiverHashTable)[receiver_index]==NULL){
		        fprintf(stderr, "Malloc failed.\n");
		        exit(1);
		    }
		    (*receiverHashTable)[receiver_index]->arr=malloc(maxBuckEnts*sizeof(buckEntry));
		    if((*receiverHashTable)[receiver_index]->arr==NULL){
		        fprintf(stderr, "Malloc failed.\n");
		        exit(1);
		    }
		    (*receiverHashTable)[receiver_index]->numOfEntries=0;
		    (*receiverHashTable)[receiver_index]->next=NULL;

		    numOfEntries=(*receiverHashTable)[receiver_index]->numOfEntries;
			
			(*receiverHashTable)[receiver_index]->arr[numOfEntries].walletInd=i;
			(*receiverHashTable)[receiver_index]->arr[numOfEntries].tranList=NULL;
		    (*receiverHashTable)[receiver_index]->numOfEntries++;

		}
		else{
			currBuck=(*receiverHashTable)[receiver_index];
			toWrite=currBuck;
			while(currBuck!=NULL){
				toWrite=currBuck;
				currBuck=currBuck->next;
			}
			currBuck=toWrite;
			if(currBuck->numOfEntries<maxBuckEnts){ //an o user xwraei sto bucket

				numOfEntries=currBuck->numOfEntries;
				currBuck->arr[numOfEntries].walletInd=i;
				currBuck->arr[numOfEntries].tranList=NULL;
			    currBuck->numOfEntries++;
			}
			else if(currBuck->numOfEntries==maxBuckEnts){ //ean o user den xwraei so bucket

				//ftiaxnw kainourgio bucket
				currBuck->next=malloc(sizeof(bucket));
				if(currBuck->next==NULL){
			        fprintf(stderr, "Malloc failed.\n");
			        exit(1);
			    }
			    currBuck=currBuck->next;
			    currBuck->arr=malloc(maxBuckEnts*sizeof(buckEntry));
			    if(currBuck->arr==NULL){
			        fprintf(stderr, "Malloc failed.\n");
			        exit(1);
			    }
			    currBuck->numOfEntries=0;
			    currBuck->next=NULL;

			    numOfEntries=currBuck->numOfEntries;		    
			    currBuck->arr[numOfEntries].walletInd=i;
				currBuck->arr[numOfEntries].tranList=NULL;
			    currBuck->numOfEntries++;
			}
			else{
				fprintf(stderr, "Something's wrong\n");
			}
		}
	}	
	fclose(fp);
}


//synarthsh pou ektipwnei ta dedomena enos wallet
void print_walletInfo(wallet w){

	bitcoin* head=w.bcList;
	bitcoin* curr=head;

	printf("WalletID: %s\n",w.walletID);
	printf("Total balance: %d\n",w.balance);
	printf("Bitcoins owned by %s:\n",w.walletID);
	while(curr!=NULL){
		printf("	%d with balance %d\n",curr->rootPos->bitcoinID,curr->bcBalance);
		printf("	TRANSACTION TREE of bitcoin: \n");
		printTree(curr->rootPos->root);
		printf("\n");
		curr=curr->next;
	}
	printf("*********************************************\n\n");
}

//apeleutherwnei thn mnhmh pou exei desmeutei ggia ena wallet
void free_wallet(wallet* w){
	bitcoin* currBc=NULL, *temp=NULL;

	free(w->walletID);
	currBc=w->bcList;
	while(currBc!=NULL){
		temp=currBc->next;
		free_bitcoin(currBc);
		currBc=temp;
	}
}

//apeleutherwnei ti mnhmh th mnhmh pou exei desmeftei gia ena hashtable
void free_hashTable(bucket** hashTable, int size){
	int i,j;
	bucket* currBuck=NULL, *tempBuck=NULL;
	transaction* currTran=NULL, *tempTran=NULL;

	for(i=0;i<size;i++){
		if(hashTable[i]!=NULL){
			currBuck=hashTable[i];
			while(currBuck!=NULL){
				for(j=0;j<currBuck->numOfEntries;j++){
					currTran=currBuck->arr[j].tranList;
					while(currTran!=NULL) {
						tempTran=currTran->next;
						free_transaction(currTran);
						currTran=tempTran;
					}
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