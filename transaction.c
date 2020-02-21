#include "transaction.h"


//synarthsh pou mou xreiazetai otan ftiaxnw to transaction pou tha dothei sto meta apo to /requestTransaction
char* concat(char *transactionID, char *transaction){
    char *result = malloc(strlen(transactionID) + strlen(transaction) + 1); // +1 for the null-terminator
    // in real code you would check for errors in malloc here
    strcpy(result, transactionID);
    strcat(result, transaction);
    return result;
}

//psaxnei epilegei kai epistrefei to prwto bitcoin ths listas
bitcoin* chooseBitcoin(wallet user,int amount,bitcoin** prev){ 
    bitcoin* head=user.bcList;
    bitcoin* curr=head;

    if(curr==NULL){
        fprintf(stderr, "Sender doesn't own any bitcoins\n");
        return NULL;
    }
    (*prev)=NULL;
    return curr;
}

//elegxei ena iparxei idi to dothen transaction ID
int transIDFound(tranIdBuck* hashed, char* transactionID, tranIdBuck** toWrite){
	tranIdBuck* curr=hashed;
	*toWrite=hashed;
	int i;

	while(curr!=NULL){
		for(i=0;i<curr->numOfEntries;i++){
			if(strcmp(curr->arr[i], transactionID)==0) return i;
		}
		curr=curr->next;
		if(curr!=NULL) *toWrite=curr;
	}
	return -1;
}

//elegxei ean to dothen transactionID einai mono apo ppsifia
int argDigitsOnly(char *arg){
    while(*arg){
        if (isdigit(*arg++) == 0) return 0;
    }
    return 1;
}


//pragmatopoioei to transaction pou dinetai sto dothen string
void makeTransaction(char* tranRec, bucket*** senderHashTable, bucket*** receiverHashTable, int h1, int h2, wallet** walletArr, tranIdBuck*** transIDHashTable, transTD* latestTD, char** maxTransID, int mode){

	char *saveptr, *datesaveptr, *timesaveptr, *token, *sender=NULL, *receiver=NULL, *transactionID=NULL, *trID=NULL, *subtoken;
	int hash_index, sender_index, receiver_index, amount, maxBuckEnts, numOfEntries, i=0, day, month, year, hour, minutes, isLater;
	bucket* senderb=NULL, *receiverb=NULL;
	transaction *currTran, *senderTran, *headTran, *prevTran, *receiverTran;
	bitcoin* chosen_rbc, *chosen_sbc, *curr, *prevR=NULL, *temp, *head, *prevS=NULL;
	bctNode* lastState=NULL;
	tranIdBuck* currBuck=NULL;

	//desmevw xwro gia to hashtable twn transaction IDs ean einai adeio. epilegw 11 hashvals giati enas prime dinei kalitero hashing kai ligotera colissions
	if((*transIDHashTable)==NULL){
		(*transIDHashTable)=malloc(11*sizeof(tranIdBuck*));
		if((*transIDHashTable)==NULL){
			fprintf(stderr, "Malloc failed.\n");
			exit(1);
		}
		for(i=0;i<11;i++){
			(*transIDHashTable)[i]=NULL;
		}
	}

	//prwta tha kanw parse to transaction record
	saveptr=tranRec;

	//apothikevw to transaction ID
	token=strtok_r(tranRec," \n",&saveptr);
	if(token!=NULL){
		if(transIDFound((*transIDHashTable)[hash_func(11, token)], token, &currBuck)!=-1){
			fprintf(stderr, "Transaction ID %s already exists. The transaction record is invalid.\n", token);
			return;
		}		
		trID=malloc((strlen(token)+1)*sizeof(token));
		if(trID==NULL){
			fprintf(stderr, "Malloc failed.\n");
			exit(1);
		}
		strcpy(trID, token);

	}
	else{
		fprintf(stderr, "The transaction record is empty.\n");
		return;
	}
		
	//sender
	token=strtok_r(NULL," \n",&saveptr);
	if(token!=NULL){
		sender=malloc((strlen(token)+1)*sizeof(char));
		if(sender==NULL){
			fprintf(stderr, "Malloc failed.\n");
			exit(1);
		}
		strcpy(sender, token);
	}
	else{
		fprintf(stderr, "Transaction record %s is invalid. Wrong type of transaction record.\n", trID);
		free(trID);
		return;
	}

	hash_index=hash_func(h1, token);
	sender_index=userFound((*senderHashTable)[hash_index], token, &senderb, *walletArr);

	//receiver
	token=strtok_r(NULL," \n",&saveptr);
	if(token!=NULL){
		receiver=malloc((strlen(token)+1)*sizeof(char));
		if(receiver==NULL){
			fprintf(stderr, "Malloc failed.\n");
			exit(1);
		}
		strcpy(receiver, token);
	}
	else{
		fprintf(stderr, "Transaction record %s is invalid. Wrong type of transaction record.\n", trID);
		free(trID);
		free(sender);
		return;
	}

	if(strcmp(sender, receiver)==0){
		fprintf(stderr, "Transaction record %s is invalid, sender and receiver cannot be the same user.\n", trID);
		free(trID);
		free(sender);
		free(receiver);
		return;
	}

	hash_index=hash_func(h2, token);
	receiver_index=userFound((*receiverHashTable)[hash_index], token, &receiverb, *walletArr);

	if((receiver_index!=-1)&&(sender_index!=-1)){
		token=strtok_r(NULL," \n", &saveptr);
		if(token!=NULL){
			if(!argDigitsOnly(token)){
				fprintf(stderr, "Transaction record %s is invalid. Amount is missing.\n", trID);
				free(trID);
				free(sender);
				free(receiver);
				return;
			}
			amount=atoi(token);
			if((*walletArr)[senderb->arr[sender_index].walletInd].balance < amount){
				fprintf(stderr, "The sender does not have enough balance to complete transaction %s.\n", trID);
				free(trID);
				free(sender);
				free(receiver);
				return;
			}
			//date
			token=strtok_r(NULL," \n",&saveptr);
			if(token!=NULL){
				datesaveptr=token;

				subtoken=strtok_r(token,"-/", &datesaveptr);
				if(subtoken!=NULL){
					day=atoi(subtoken);
				}
				subtoken=strtok_r(NULL,"-/", &datesaveptr);
				if(subtoken!=NULL){
					month=atoi(subtoken);
				}
				subtoken=strtok_r(NULL,"-/", &datesaveptr);
				if(subtoken!=NULL){
					year=atoi(subtoken);
				}
			}
			else{
				if(mode==1){
					time_t t = time(NULL);
					struct tm tm = *localtime(&t);
					year=tm.tm_year+1900;
					month=tm.tm_mon + 1;
					day=tm.tm_mday;
					
					//fprintf(stderr, "%d-%d-%d , %d:%d", day,month,year,hour,minutes);
				}
				else{
					fprintf(stderr, "Transaction record %s is invalid. Wrong type of transaction record.\n", trID);
					free(trID);
					free(sender);
					free(receiver);
					return;
				}
			}			

			//time
			token=strtok_r(NULL," \n",&saveptr);
			if(token!=NULL){
				timesaveptr=token;
				subtoken=strtok_r(token,":\n", &timesaveptr);
				if(subtoken!=NULL){
					hour=atoi(subtoken);
				}
				subtoken=strtok_r(NULL,":\n", &timesaveptr);
				if(subtoken!=NULL){
					minutes=atoi(subtoken);
				}
			}
			else{
				if(mode==1){
					time_t t = time(NULL);
					struct tm tm = *localtime(&t);
					hour=tm.tm_hour;
					minutes=tm.tm_min;
				}
				else{
					fprintf(stderr, "Transaction record %s is invalid. Wrong type of transaction record.\n", trID);
					free(trID);
					free(sender);
					free(receiver);
					return;
				}				
			}



			//twra tha elegksw ean i imerominia einai argotera apo tin argoterh apothikevmeni
			isLater=dateIsLater(day, month, year, hour, minutes, *latestTD);
			if(mode==1){ //ean to transaction exei dothei apo grammi entolwn 
				if(!isLater){ //kai einaiprogenesteri i imerominia, to transaction de pragamtopoieitai
					fprintf(stderr, "Given transaction time and date is not right.Transaction is invalid.\n");
					free(trID);
					free(sender);
					free(receiver);
					return;
				}
			}

			//elegxw ean iparxei to transactionID, twra pou kserw oti to dothen transaction einai valid, gia na apothikefsw to transactionID
			hash_index=hash_func(11, trID);
		
			maxBuckEnts=(150-sizeof(int)-sizeof(tranIdBuck*))/sizeof(char*);
			if((*transIDHashTable)[hash_index]==NULL){
				(*transIDHashTable)[hash_index]=malloc(sizeof(tranIdBuck));
				if((*transIDHashTable)[hash_index]==NULL){
			        fprintf(stderr, "Malloc failed.\n");
			        exit(1);
			    }
			    (*transIDHashTable)[hash_index]->arr=malloc(maxBuckEnts*sizeof(char*));
			    if((*transIDHashTable)[hash_index]->arr==NULL){
			        fprintf(stderr, "Malloc failed.\n");
			        exit(1);
			    }
			    (*transIDHashTable)[hash_index]->numOfEntries=0;
			    (*transIDHashTable)[hash_index]->next=NULL;

			    numOfEntries=(*transIDHashTable)[hash_index]->numOfEntries;
				
			    (*transIDHashTable)[hash_index]->arr[numOfEntries]=malloc((strlen(trID)+1)*sizeof(char));
				if((*transIDHashTable)[hash_index]->arr[numOfEntries]==NULL){
					fprintf(stderr, "Malloc failed.\n");
					exit(1);
				}
				strcpy((*transIDHashTable)[hash_index]->arr[numOfEntries], trID);
				transactionID=(*transIDHashTable)[hash_index]->arr[numOfEntries];
			    (*transIDHashTable)[hash_index]->numOfEntries++;

			}
			else{ //an auto to hash value exei kai alla tranIds mesa	
				currBuck=NULL;

				//arxika tha elegksw an iparxei to tranID sto hash
				if(transIDFound((*transIDHashTable)[hash_index], trID, &currBuck)==-1){
					if(currBuck->numOfEntries<maxBuckEnts){ //an o user xwraei sto bucket

						numOfEntries=currBuck->numOfEntries;

						currBuck->arr[numOfEntries]=malloc((strlen(trID)+1)*sizeof(char));
						if(currBuck->arr[numOfEntries]==NULL){
							fprintf(stderr, "Malloc failed.\n");
							exit(1);
						}
						strcpy(currBuck->arr[numOfEntries], trID);
						transactionID=currBuck->arr[numOfEntries];
					    currBuck->numOfEntries++;
					}
					else if(currBuck->numOfEntries==maxBuckEnts){ //ean o user den xwraei so bucket

						//ftiaxnw kainourgio bucket
						currBuck->next=malloc(sizeof(tranIdBuck));
						if(currBuck->next==NULL){
					        fprintf(stderr, "Malloc failed.\n");
					        exit(1);
					    }
					    currBuck=currBuck->next;
					    currBuck->arr=malloc(maxBuckEnts*sizeof(char*));
					    if(currBuck->arr==NULL){
					        fprintf(stderr, "Malloc failed.\n");
					        exit(1);
					    }

					    currBuck->numOfEntries=0;
					    currBuck->next=NULL;

					    numOfEntries=currBuck->numOfEntries;

					    currBuck->arr[numOfEntries]=malloc((strlen(trID)+1)*sizeof(char));
						if(currBuck->arr[numOfEntries]==NULL){
							fprintf(stderr, "Malloc failed.\n");
							exit(1);
						}
					    
						strcpy(currBuck->arr[numOfEntries], trID);
						transactionID=currBuck->arr[numOfEntries];
					    currBuck->numOfEntries++;
					}
					else{
						fprintf(stderr, "Something's wrong\n");
					}
				}
				else{
					fprintf(stderr, "Transaction ID %s already exists. The transaction record is invalid.\n", token);
					free(trID);
					free(sender);
					free(receiver);
					return;
				}
			}


			//ean i imerominia einai metagenesterh ginetai i max imerominia
			if(isLater) storeMaxDateTime(day, month, year, hour, minutes, latestTD);


			//gia ton sender
			headTran=senderb->arr[sender_index].tranList;
			currTran=headTran;			
			prevTran=currTran;

			while(currTran!=NULL) {
				prevTran=currTran;
				currTran=currTran->next;
			}

			currTran=malloc(sizeof(transaction));
			if(currTran==NULL){
				fprintf(stderr, "Malloc failed.\n");
				exit(1);
			}

			currTran->transactionID=transactionID;

			currTran->senderID=(*walletArr)[senderb->arr[sender_index].walletInd].walletID;
			
			currTran->receiverID=(*walletArr)[receiverb->arr[receiver_index].walletInd].walletID;

			currTran->amount=amount;

			//date
			currTran->td.day=day;
			currTran->td.month=month;
			currTran->td.year=year;

			//time
			currTran->td.time=hour;
			currTran->td.minutes=minutes;


			if(prevTran!=NULL) {
				prevTran->next=currTran;
				senderb->arr[sender_index].tranList=headTran;
			}
			else senderb->arr[sender_index].tranList=currTran;
			currTran->next=NULL;

			senderTran=currTran;

			//gia ton receiver
			headTran=receiverb->arr[receiver_index].tranList;
			currTran=headTran;
			prevTran=currTran;

			while(currTran!=NULL){
				prevTran=currTran;
				currTran=currTran->next;
			}

			currTran=malloc(sizeof(transaction));
			if(currTran==NULL){
				fprintf(stderr, "Malloc failed.\n");
				exit(1);
			}

			currTran->transactionID=transactionID;

			currTran->amount=amount;

			currTran->senderID=(*walletArr)[senderb->arr[sender_index].walletInd].walletID;

			currTran->receiverID=(*walletArr)[receiverb->arr[receiver_index].walletInd].walletID;
			
			//date
			currTran->td.day=senderTran->td.day;
			currTran->td.month=senderTran->td.month;
			currTran->td.year=senderTran->td.year;

			//time
			currTran->td.time=senderTran->td.time;
			currTran->td.minutes=senderTran->td.minutes;

			if(prevTran!=NULL) {
				prevTran->next=currTran;
				receiverb->arr[receiver_index].tranList=headTran;
			}
			else receiverb->arr[receiver_index].tranList=currTran;
			currTran->next=NULL;

			receiverTran=currTran;
		}
		else{
			fprintf(stderr, "Transaction record %s is invalid. Wrong type of transaction record.\n", trID);
			free(trID);
			free(sender);
			free(receiver);
			return;
		}

		//prepei na epilegei to bitcoin h ta bitcoin tis sinallagis kai na ginei i sinallagi
		while(amount>0){ //oso iparxei akoma poso pou prepei na metaferthei
			prevS=NULL;			
			chosen_sbc=chooseBitcoin((*walletArr)[senderb->arr[sender_index].walletInd], amount, &prevS);
			if(chosen_sbc!=NULL){
				head=(*walletArr)[receiverb->arr[receiver_index].walletInd].bcList;
				if(chosen_sbc->bcBalance<amount){ //tha xreiastei kai allo bitcoin gia na oloklirwthei kai i sinallagi				
					curr=head;
					prevR=NULL;
					while(curr!=NULL){
						if(curr->rootPos->bitcoinID==chosen_sbc->rootPos->bitcoinID) break; //o receiver exei hdh ipo tin katoxi tou meros autou tou bitcoin
						prevR=curr;
						curr=curr->next;
					}
					chosen_rbc=curr;

					if(chosen_rbc!=NULL){ //o receiver exei hdh ipo tin katoxi tou meros autou tou bitcoin
						chosen_rbc->bcBalance+=chosen_sbc->bcBalance;		
						(*walletArr)[receiverb->arr[receiver_index].walletInd].bcList=head;				
					}
					else{
						chosen_rbc=malloc(sizeof(bitcoin));
						if(chosen_rbc==NULL){
							fprintf(stderr, "Malloc failed.\n");
							exit(1);
						}
						chosen_rbc->bcBalance=chosen_sbc->bcBalance;
						chosen_rbc->rootPos=chosen_sbc->rootPos;

						(*walletArr)[receiverb->arr[receiver_index].walletInd].bcList=head;
						if(prevR!=NULL) prevR->next=chosen_rbc;
						else (*walletArr)[receiverb->arr[receiver_index].walletInd].bcList=chosen_rbc;
						
						chosen_rbc->next=NULL;
						
					}
					//tha psaksw sto bitcoin tree to node pou apeikonizei tin teleutaia fora pou to wallet id xrisimopoihse to sigekrimeno bc
					lastState=searchLastState(chosen_sbc->rootPos->root, (*walletArr)[senderb->arr[sender_index].walletInd].walletID); 
					if(lastState==NULL) {
						printf("Something's very wrong.User not found in the bitcoin's transaction history.\n");
						free(trID);
						free(sender);
						free(receiver);
						return;
					}
					if(lastState->right==NULL){
						create_root(&(lastState->right), lastState->bitcoinID, (*walletArr)[senderb->arr[sender_index].walletInd].walletID, 0);
						lastState->right->tran=senderTran;
					}
					else{
						printf("The search of the state was unsuccesful.\n");
					}
					if(lastState->left==NULL){
						create_root(&(lastState->left), lastState->bitcoinID, (*walletArr)[receiverb->arr[receiver_index].walletInd].walletID, chosen_sbc->bcBalance);
						lastState->left->tran=receiverTran;
					}
					else{
						printf("The search of the state was unsuccesful.\n");
					}

					amount-=chosen_sbc->bcBalance;
					
					(*walletArr)[senderb->arr[sender_index].walletInd].balance-=chosen_sbc->bcBalance;
					(*walletArr)[receiverb->arr[receiver_index].walletInd].balance+=chosen_sbc->bcBalance;	
					chosen_sbc->bcBalance=0;	

				}
				else{ //to balance autou tou bitcoin arkei gia to transaction
					curr=head;
					prevR=NULL;
					while(curr!=NULL){
						if(curr->rootPos->bitcoinID==chosen_sbc->rootPos->bitcoinID) break; //o receiver exei hdh ipo tin katoxi tou meros autou tou bitcoin
						prevR=curr;
						curr=curr->next;
					}
					chosen_rbc=curr;
					if(chosen_rbc!=NULL){ //o receiver exei hdh ipo tin katoxi tou meros autou tou bitcoin
						chosen_rbc->bcBalance+=amount;					
					}
					else{
						chosen_rbc=malloc(sizeof(bitcoin));
						if(chosen_rbc==NULL){
							fprintf(stderr, "Malloc failed.\n");
							exit(1);
						}
						chosen_rbc->bcBalance=amount;
						chosen_rbc->rootPos=chosen_sbc->rootPos;

						(*walletArr)[receiverb->arr[receiver_index].walletInd].bcList=head;
						if(prevR!=NULL) prevR->next=chosen_rbc;
						else (*walletArr)[receiverb->arr[receiver_index].walletInd].bcList=chosen_rbc;
						chosen_rbc->next=NULL;						
					}

					//tha psaksw sto bitcoin tree to node pou apeikonizei tin teleutaia fora pou to wallet id xrisimopoihse to sigekrimeno bc
					lastState=searchLastState(chosen_sbc->rootPos->root, (*walletArr)[senderb->arr[sender_index].walletInd].walletID); 
					if(lastState==NULL) {
						printf("Something's very wrong.User not found in the bitcoin's transaction history.\n");
						free(trID);
						free(sender);
						free(receiver);
						return;
					}
					if(lastState->right==NULL){
						create_root(&(lastState->right), lastState->bitcoinID, (*walletArr)[senderb->arr[sender_index].walletInd].walletID, chosen_sbc->bcBalance-amount);
						lastState->right->tran=senderTran;
					}
					else{
						printf("The search of the state was unsuccesful.\n");
					}
					if(lastState->left==NULL){
						create_root(&(lastState->left), lastState->bitcoinID, (*walletArr)[receiverb->arr[receiver_index].walletInd].walletID, amount);
						lastState->left->tran=receiverTran;
					}
					else{
						printf("The search of the state was unsuccesful.\n");
					}
					
					chosen_sbc->bcBalance-=amount;
					(*walletArr)[senderb->arr[sender_index].walletInd].balance-=amount;
					(*walletArr)[receiverb->arr[receiver_index].walletInd].balance+=amount;
					amount=0;
				}

				if(chosen_sbc->bcBalance==0){ //afairw to bitcoin apo ti dikaiodosia tou sender

					if(prevS!=NULL){
						temp=chosen_sbc->next;
						prevS->next=temp;
						free_bitcoin(chosen_sbc);
					}
					else{		
						(*walletArr)[senderb->arr[sender_index].walletInd].bcList=chosen_sbc->next;
						free_bitcoin(chosen_sbc);
					}
				}
			}
			else{
				fprintf(stderr, "Something's so wrong.\n");
				exit(1);
			}
		}		
	}
	else{
		if(sender_index==-1) fprintf(stderr, "Requested sender %s is not a user.\n", sender );
		if(receiver_index==-1) fprintf(stderr, "Requested receiver %s is not a user.\n", receiver );
		fprintf(stderr, "Transaction invalid.\n");
		free(trID);
		free(sender);
		free(receiver);
		return;
	}

	//tha checkarw an to transaction ID einai mono apo psifia
	if(argDigitsOnly(currTran->transactionID)){
		if((*maxTransID)!=NULL){
			if(atoi(*maxTransID)<atoi(transactionID)){
				free(*maxTransID);
				(*maxTransID)=malloc((strlen(transactionID)+1)*sizeof(char));
				if(*maxTransID==NULL){
					fprintf(stderr, "Malloc failed.\n");
					exit(1);
				}
				strcpy((*maxTransID),transactionID);
			}
		}
		else{
			(*maxTransID)=malloc((strlen(transactionID)+1)*sizeof(char));
			if(*maxTransID==NULL){
				fprintf(stderr, "Malloc failed.\n");
				exit(1);
			}
			strcpy((*maxTransID),transactionID);
		}		
	}

	if(mode==1) printf("Transaction %s was successfully completed.\n",trID);
	free(sender);
	free(receiver);
	free(trID);

}

//synarthsh pou elegxei ean i dosmeni imerominia einai metagenesterh ths pio prosfaths
int dateIsLater(int day, int month, int year, int time, int minutes, transTD latestTD){
	if(latestTD.year<year) return 1;
	else if(latestTD.year==year){
		if(latestTD.month<month) return 1;
		else if(latestTD.month==month){
			if(latestTD.day<day) return 1;
			else if(latestTD.day==day){
				if(latestTD.time<time) return 1;
				else if(latestTD.time==time){
					if(latestTD.minutes<=minutes) return 1;
					else return 0;
				}
				else return 0;
			}
			else return 0;
		}
		else return 0;
	}
	else return 0;
	return 0;
}

//apothikevei tin dothisa imerominia ws max
void storeMaxDateTime(int day, int month, int year, int time, int minutes, transTD* latestTD){
	latestTD->day=day;
	latestTD->month=month;
	latestTD->year=year;
	latestTD->time=time;
	latestTD->minutes=minutes;
}


//ektipwnei ta transactiosn sta opoia simmetixe o xristis tou dosmenou wallet
void print_transactions(wallet w, wallet* walletArr, bucket** senderHashTable, bucket** receiverHashTable, int h1, int h2){
	print_walletInfo(w);
	transaction* curr=NULL;
	bucket* currBuck;
	int i, index;
	printf("Transactions %s partcipated as a sender: \n",w.walletID);
	
	index=hash_func(h1,w.walletID);
	currBuck=senderHashTable[index];
	while(currBuck!=NULL){
		for(i=0;i<currBuck->numOfEntries;i++){
			if(strcmp(w.walletID, walletArr[currBuck->arr[i].walletInd].walletID)==0){
				curr=currBuck->arr[i].tranList;
				break;
			}
		}
		currBuck=currBuck->next;
	}
	if(curr==NULL) printf("		User has never sent money.\n");
	else{
		while(curr!=NULL){
			printTran(curr);
			curr=curr->next;
		}
	}
	printf("\n\n");
	printf("Transactions %s partcipated as a receiver: \n",w.walletID);;
	index=hash_func(h2,w.walletID);
	currBuck=receiverHashTable[index];
	curr=NULL;
	while(currBuck!=NULL){
		for(i=0;i<currBuck->numOfEntries;i++){
			if(strcmp(w.walletID, walletArr[currBuck->arr[i].walletInd].walletID)==0){
				curr=currBuck->arr[i].tranList;
				break;
			}
		}
		currBuck=currBuck->next;
	}
	if(curr==NULL) printf("		User has never received money.\n");
	else{
		while(curr!=NULL){
			printTran(curr);
			curr=curr->next;
		}
	}

	printf("***************************************************************************************\n");
}

//synarthsh poy ektipwnei ta transactions enos xristi sto dosmeno xroniko diasthma kai epistrefei to poso pou esteile/elave 
int find_Earns_Pays(transaction* head, char* arg1, char* arg2, char* arg3, char* arg4){
	char *str1=NULL, *str2=NULL, *str3=NULL, *str4=NULL, *token=NULL;
	int hour1, hour2, minutes1, minutes2, day1, day2, month1, month2, year1, year2, amount=0;
	transaction* currTran=NULL;

	//se auth th periptwsh o xristis exei dwsei mono 2 orismata, gia na einai egkiri i entoli prepei na einai i kai ta dio time i kai ta dio date
	if((arg1!=NULL)&&(arg2!=NULL)&&(arg3==NULL)&&(arg4==NULL)){
		//arxika antigrafw ta orismata giati isws na ksanaxreiastei na ta kanw tokenize
        str1=malloc((strlen(arg1)+1)*sizeof(char));
        if(str1==NULL){
            fprintf(stderr, "Malloc failed.\n");
            exit(1);
        }
        strcpy(str1, arg1);

        str2=malloc((strlen(arg2)+1)*sizeof(char));
        if(str2==NULL){
            fprintf(stderr, "Malloc failed.\n");
            exit(1);
        }
        strcpy(str2, arg2);

        //elegxw to zeugos apo arguments
		token=strtok(arg1,":");
		if((token!=NULL)&&(strcmp(token,str1)!=0)){
			hour1=atoi(token);
			token=strtok(NULL,":\n");
			if(token!=NULL){
				minutes1=atoi(token);
			}
			else{
				fprintf(stderr, "Invalid type of argument.Exiting function.\n");
				if(str1!=NULL) free(str1);
				if(str2!=NULL) free(str2);
				return -1;
			}

			//gia to argument 2 tsekarw an einai time h date. gia na einai to command egkuro prepei na einai oti einai kai to arg1 , diladi time
			token=strtok(arg2,":");
			if((token!=NULL)&&(strcmp(token,str2)!=0)){
				hour2=atoi(token);
				token=strtok(NULL,":\n");
				if(token!=NULL){
					minutes2=atoi(token);
					//thewrhtika epiviwsa apo olous tous elegxous
					currTran=head;
					while(currTran!=NULL){
						if((currTran->td.time>=hour1)&&(currTran->td.minutes>=minutes1)){
							if(currTran->td.time<hour2){
								amount+=currTran->amount;
								printTran(currTran);

							}
							else if((currTran->td.time==hour2)&&(currTran->td.minutes<=minutes2)){
								amount+=currTran->amount;
								printTran(currTran);
							}
						}			            
			            currTran=currTran->next;
			        }

				}
				else{
					fprintf(stderr, "Invalid type of argument.Exiting function.\n");
					return -1;
				}
			}
			else{
				fprintf(stderr, "arg1 is a time argument, but arg2 isn't.Exiting function.\n");
				if(str1!=NULL) free(str1);
				if(str2!=NULL) free(str2);
				return -1;
			}
		}
		else{
			fprintf(stderr, "arg1 isn't a time argument.\n");
			token=strtok(str1,"-/");
			if(token!=NULL){
				day1=atoi(token);
				token=strtok(NULL,"-/");
				if(token!=NULL){
					month1=atoi(token);
					token=strtok(NULL,"-/\n");
					if(token!=NULL){
						year1=atoi(token);
						
					}
				}
				else{
					fprintf(stderr, "Invalid type of argument.Exiting function.3\n");
					if(str1!=NULL) free(str1);
					if(str2!=NULL) free(str2);
					return -1;
				}				

				//gia to argument 2 tsekarw an einai time h date. gia na einai to command egkuro prepei na einai oti einai kai to arg1 , diladi date
				token=strtok(arg2,"-/");
				if((token!=NULL)&&(strcmp(token,str2)!=0)){
					day2=atoi(token);
					token=strtok(NULL,"-/");
					if(token!=NULL){
						month2=atoi(token);
						token=strtok(NULL,"-/\n");
						if(token!=NULL){
							year2=atoi(token);
							//thewrhtika epiviwsa apo olous tous elegxous
							currTran=head;
							while(currTran!=NULL){
								if((currTran->td.year>=year1)&&(currTran->td.year>=month1)){
									if(currTran->td.year<year2){
										amount+=currTran->amount;
										printTran(currTran);

									}
									else if((currTran->td.year==year2)&&(currTran->td.month<=month2)){
										amount+=currTran->amount;
										printTran(currTran);
									}
								}			            
					            currTran=currTran->next;
					        }
						}						
					}					
					else{
						fprintf(stderr, "Invalid type of argument.Exiting function.\n");
						if(str1!=NULL) free(str1);
						if(str2!=NULL) free(str2);
						return -1;
					}
				}
				else{
					fprintf(stderr, "arg1 is a date argument, but arg2 isn't.Exiting function.\n");
					if(str1!=NULL) free(str1);
					if(str2!=NULL) free(str2);
					return -1;
				}
			}
			else{
				fprintf(stderr, "Invalid type of argument.Exiting function.\n");
				if(str1!=NULL) free(str1);
				if(str2!=NULL) free(str2);
				return -1;
			}
		}

		
	}
	else if((arg1!=NULL)&&(arg2!=NULL)&&(arg3!=NULL)&&(arg4!=NULL)){  //[time1] [year1] [time2] [year2]
		//arxika antigrafw ta orismata giati isws na ksanaxreiastei na ta kanw tokenize
        //arxika antigrafw ta orismata giati isws na ksanaxreiastei na ta kanw tokenize
        str1=malloc((strlen(arg1)+1)*sizeof(char));
        if(str1==NULL){
            fprintf(stderr, "Malloc failed.\n");
            exit(1);
        }
        strcpy(str1, arg1);

        str2=malloc((strlen(arg2)+1)*sizeof(char));
        if(str2==NULL){
            fprintf(stderr, "Malloc failed.\n");
            exit(1);
        }
        strcpy(str2, arg2);

        str3=malloc((strlen(arg3)+1)*sizeof(char));
        if(str3==NULL){
            fprintf(stderr, "Malloc failed.\n");
            exit(1);
        }
        strcpy(str3, arg3);

        str4=malloc((strlen(arg4)+1)*sizeof(char));
        if(str4==NULL){
            fprintf(stderr, "Malloc failed.\n");
            exit(1);
        }
        strcpy(str4, arg4);

        //twra ksekinaw tin diadikasia filtrarismatos

		token=strtok(arg1,":\n");
		if((token!=NULL)&&(strcmp(token,str1)!=0)){
			hour1=atoi(token);
			token=strtok(NULL,":\n");
			if(token!=NULL){
				minutes1=atoi(token);
			}
			else{
				fprintf(stderr, "Invalid type of argument.Exiting function.1\n");
				if(str1!=NULL) free(str1);
				if(str2!=NULL) free(str2);
				if(str3!=NULL) free(str3);
				if(str4!=NULL) free(str4);
				return -1;
			}

			//gia to argument 2 tsekarw an einai time h date. gia na einai to command egkuro prepei na einai date
			token=strtok(arg2,"-/");
			if((token!=NULL)&&(strcmp(token,str2)!=0)){
				day1=atoi(token);
				token=strtok(NULL,"-/");
				if(token!=NULL){
					month1=atoi(token);
					token=strtok(NULL,"-/\n");
					if(token!=NULL){
						year1=atoi(token);
					}
				}
				else{
					fprintf(stderr, "Invalid type of argument.Exiting function.2\n");
					if(str1!=NULL) free(str1);
					if(str2!=NULL) free(str2);
					if(str3!=NULL) free(str3);
					if(str4!=NULL) free(str4);
					return -1;
				}
			}
			else{
				fprintf(stderr, "arg1 is a time argument, but arg2 isn't a date argument.Exiting function.\n");
				if(str1!=NULL) free(str1);
				if(str2!=NULL) free(str2);
				if(str3!=NULL) free(str3);
				if(str4!=NULL) free(str4);
				return -1;
			}
		}
		else{
			fprintf(stderr, "Invalid type of argument.Exiting function.3\n");
			if(str1!=NULL) free(str1);
			if(str2!=NULL) free(str2);
			if(str3!=NULL) free(str3);
			if(str4!=NULL) free(str4);
			return -1;
		}

		//gia to deutero zeugari arguments 
		token=strtok(arg3,":\n");
		if((token!=NULL)&&(strcmp(token,str3)!=0)){
			hour2=atoi(token);
			token=strtok(NULL,":\n");
			if(token!=NULL){
				minutes2=atoi(token);
			}
			else{
				fprintf(stderr, "Invalid type of argument.Exiting function.4\n");
				if(str1!=NULL) free(str1);
				if(str2!=NULL) free(str2);
				if(str3!=NULL) free(str3);
				if(str4!=NULL) free(str4);
				return -1;
			}

			//gia to argument 4 tsekarw an einai time h date. gia na einai to command egkuro prepei na einai date 
			token=strtok(arg4,"-/");
			if((token!=NULL)&&(strcmp(token,str4)!=0)){
				day2=atoi(token);
				token=strtok(NULL,"-/");
				if(token!=NULL){
					month2=atoi(token);
					token=strtok(NULL,"-/\n");
					if(token!=NULL){
						year2=atoi(token);
						//thewrhtika epiviwsa apo olous tous elegxous kai exw parsed
						currTran=head;
						while(currTran!=NULL){
							if((currTran->td.year>year1)&&(currTran->td.year<year2)){
								amount+=currTran->amount;
								printTran(currTran);
							}
							else if((currTran->td.year==year1)&&(currTran->td.year<year2)){
								if(currTran->td.month>month1){
									amount+=currTran->amount;
									printTran(currTran);									
								}
								else if(currTran->td.month==month1){
									if(currTran->td.day>day1){
										amount+=currTran->amount;
										printTran(currTran);
									}
									else if(currTran->td.day==day1){
										if(currTran->td.time>hour1){
											amount+=currTran->amount;
											printTran(currTran);
										}
										else if(currTran->td.time==hour1){
											if(currTran->td.minutes>=minutes1){
												amount+=currTran->amount;
												printTran(currTran);
											}
										}
									}
								}
							}
							else if((currTran->td.year>year1)&&(currTran->td.year==year2)){
								if(currTran->td.month<month2){
									amount+=currTran->amount;
									printTran(currTran);
								}
								else if(currTran->td.month==month2){
									if(currTran->td.day<day2){
										amount+=currTran->amount;
										printTran(currTran);
									}
									else if(currTran->td.day==day2){
										if(currTran->td.time<hour2){
											amount+=currTran->amount;
											printTran(currTran);
										}
										else if(currTran->td.time==hour2){
											if(currTran->td.minutes<=minutes1){
												amount+=currTran->amount;
												printTran(currTran);
											}
										}
									}
								}									
							}
							else if((currTran->td.year==year1)&&(currTran->td.year==year2)){
								int flag=0;
								if(currTran->td.month>month1){
									if(currTran->td.month<month2){
										amount+=currTran->amount;
										printTran(currTran);
									}
								}								
								
								else if(currTran->td.month==month1){
									if(currTran->td.day>day1) flag=1;
									else if(currTran->td.day==day1){
										if(currTran->td.time>hour1) flag=1;
										else if(currTran->td.time==hour1){
											if(currTran->td.minutes>=minutes1) flag=1;
										}
									}
								}

								if(flag==1){ //sauth th periptwsh exei noima na psaksw kai gia to an aniksei sto panw orio
									flag=0;
									if(currTran->td.month<month2) flag=1;								
									
									else if(currTran->td.month==month2){
										if(currTran->td.day<day2) flag=1;
										else if(currTran->td.day==day2){
											if(currTran->td.time<hour2) flag=1;
											else if(currTran->td.time==hour2){
												if(currTran->td.minutes<=minutes2) flag=1;
											}
										}
									}
								}

								if(flag==1){
									amount+=currTran->amount;
									printTran(currTran);
								}

							}	            
				            currTran=currTran->next;
				        }
					}
				}
				else{
					fprintf(stderr, "Invalid type of argument.Exiting function.5\n");
					if(str1!=NULL) free(str1);
					if(str2!=NULL) free(str2);
					if(str3!=NULL) free(str3);
					if(str4!=NULL) free(str4);
					return -1;
				}
			}
			else{
				fprintf(stderr, "arg3 is a time argument, but arg4 isn't a date argument.Exiting function.\n");
				if(str1!=NULL) free(str1);
				if(str2!=NULL) free(str2);
				if(str3!=NULL) free(str3);
				if(str4!=NULL) free(str4);
				return -1;
			}
		}
		else{
			fprintf(stderr, "Invalid type of argument.Exiting function.6\n");
			if(str1!=NULL) free(str1);
			if(str2!=NULL) free(str2);
			if(str3!=NULL) free(str3);
			if(str4!=NULL) free(str4);
			return -1;
		}
	}
	else{
		fprintf(stderr, "Invalid combination of arguments.Exiting function.7\n");
		return -1;
	}
	if(str1!=NULL) free(str1);
	if(str2!=NULL) free(str2);
	if(str3!=NULL) free(str3);
	if(str4!=NULL) free(str4);
	return amount;
} 

//diagrafw kai apeleutherwnw th mnhmh pou krataei ena transaction
void free_transaction(transaction* t){
	t->transactionID=NULL;
	t->senderID=NULL;
	t->receiverID=NULL;
	t->next=NULL;
	free(t);
	t=NULL;
}

//diagrafw to hashtable twn transaction IDs
void free_tranIDhashTable(tranIdBuck** hashTable){
	int i,j;
	tranIdBuck* currBuck=NULL, *tempBuck=NULL;

	for(i=0;i<11;i++){
		if(hashTable[i]!=NULL){
			currBuck=hashTable[i];
			while(currBuck!=NULL){
				for(j=0;j<currBuck->numOfEntries;j++){
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