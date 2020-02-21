#include "transaction.h"

int main(int argc,char** argv){

    char bcb_file[30], buff[500], *command=NULL;
    char tran_file[30], *maxTransID=NULL, transID[50], *token=NULL, *trans, *newTransID,*user=NULL, *subtok1=NULL, *subtok2=NULL, *subtok3=NULL, *subtok4=NULL;
    int bc_val, h1, h2, buckSize, i, numOfWallets, maxB_entries, earnings=0, hash_index, position, amount, numOfTrans;
    wallet* walletArr=NULL;
    transaction *currTran=NULL;
    bucket** senderHashTable=NULL, **receiverHashTable=NULL, *currBuck=NULL, *startBuck=NULL;    
    tranIdBuck** transIDHashTable=NULL;
    bcIdBuck** bcIDHashTable=NULL, *currBcBuck=NULL;
    FILE* fp;
    transTD latestTD;

    //arxikopoiw to latestTD
    latestTD.year=-1;
    latestTD.month=-1;
    latestTD.day=-1;
    latestTD.time=-1;
    latestTD.minutes=-1;
    

    //argument parsing

    if(argc<13){
        fprintf(stderr,"Invalid number of arguments.Exiting the program.\n");
        exit(1);
    }
    if(strcmp(argv[0],"./bitcoin")!=0){
        fprintf(stderr,"Invalid name of program.Exiting the program.\n");
        exit(1);
    }
    
    if(argc==13){ //se auth th periptwsh exw ola ta arguments gia ta files 
        for(i=0;i<argc;i++){
            if((i!=0)&&(i<argc-1)){
                if(strcmp(argv[i], "-a")==0) strcpy(bcb_file,argv[i+1]);
                else if(strcmp(argv[i], "-t")==0) strcpy(tran_file,argv[i+1]); 
                else if(strcmp(argv[i], "-v")==0) bc_val=atoi(argv[i+1]);
                else if(strcmp(argv[i], "-h1")==0) h1=atoi(argv[i+1]); 
                else if(strcmp(argv[i], "-h2")==0) h2=atoi(argv[i+1]);    
                else if(strcmp(argv[i], "-b")==0) buckSize=atoi(argv[i+1]);   
                else{
                    //fprintf(stderr, "Invalid type of argument  %s\n", argv[i]);
                    //exit(1);
                }     
            }
        }
    }              
    else{
        fprintf(stderr, "Invalid number of arguments.Exiting the program.\n");
        exit(1);
    }


    maxB_entries=(buckSize-sizeof(int)-sizeof(bucket*))/sizeof(buckEntry); //arithmos entries pou xwraei ena bucket
    
    //desmefseis mnimia gia hashtables 
    senderHashTable=malloc(h1*sizeof(bucket*));
    if(senderHashTable==NULL){
        fprintf(stderr, "Malloc failed.\n");
        exit(1);
    }
    receiverHashTable=malloc(h2*sizeof(bucket*));
    if(receiverHashTable==NULL){
        fprintf(stderr, "Malloc failed.\n");
        exit(1);
    }


    //arxikopoihsh twn hashtables
    for(i=0;i<h1;i++){
        senderHashTable[i]=NULL;
    }
    for(i=0;i<h2;i++){
        receiverHashTable[i]=NULL;
    }


    //dimiourgia twn hashtables apo ta dosmena arxeia
    createHash(bcb_file,&walletArr, &senderHashTable, &receiverHashTable, &bcIDHashTable, h1, h2, bc_val, maxB_entries, &numOfWallets);
    

    //ektelesi twn transactions apo to transaction file pou exei dothei
    fp=fopen(tran_file,"r");
    if(fp==NULL) fprintf(stderr, "Invalid command.The file named %s does not exist.\n",token );
    else{
        while(fgets(buff,300,fp)){
            makeTransaction(buff, &senderHashTable, &receiverHashTable, h1, h2, &walletArr, &transIDHashTable, &latestTD, &maxTransID, 0);
        }
        fclose(fp);

    }
    
    printf("maxTransID %s\n",maxTransID );


    printf("%d/%d/%d at ",latestTD.day, latestTD.month, latestTD.year);
    if(latestTD.time/10==0) printf("0%d:", latestTD.time);
    else printf("%d:", latestTD.time);
    if(latestTD.minutes/10==0) printf("0%d\n", latestTD.minutes);
    else printf("%d\n", latestTD.minutes);

    //twra o xristis tha valei tis entoles pou thelei na treksei
    while(1){
        fflush(stdout);//katharizw to stdout
        //printf("Input command: \n");

        if(fgets(buff, 500, stdin)==NULL) break;
        

        buff[strlen(buff)-1]='\0';
        token=strtok(buff," \n;");
        if(token!=NULL){
            if(strcmp(token,"/requestTransaction")==0){ //request Transaction
                token=strtok(NULL, ";"); //pairnw oti mou apemeine afou afairwsa tin entolh, diladi to transaction
                if(token!=NULL){
                    sprintf(transID, "%d", atoi(maxTransID)+1);
                    newTransID=concat(transID, " ");
                    trans=concat(newTransID, token);
                    printf("%s\n", trans);
                    makeTransaction(trans, &senderHashTable, &receiverHashTable, h1, h2, &walletArr, &transIDHashTable, &latestTD, &maxTransID, 1);
                    free(trans);
                    free(newTransID);
                }
                   
            }
            else if(strcmp(token,"/requestTransactions")==0){
                token=strtok(NULL, "\n");
                if(token[strlen(token)-1]==';'){ //ean exei erwtimatiko de prokeitai gia arxeio alla gia omada sinallagwn
                    token[strlen(token)-1]='\0';
                    //arxika tsekarw an to request Transactions dothike me kapoia sigekrimeni entolh
                    while(token!=NULL){
                        sprintf(transID, "%d", atoi(maxTransID)+1);
                        newTransID=concat(transID, " ");
                        trans=concat(newTransID, token);
                        printf("%s\n", trans);
                        makeTransaction(trans, &senderHashTable, &receiverHashTable, h1, h2, &walletArr, &transIDHashTable, &latestTD, &maxTransID, 1);
                        free(trans);
                        free(newTransID);
                        printf("\n");
                        token=strtok(NULL, ";"); //pairnw oti mou apemeine afou afairwsa tin entolh, diladi to transaction
                    }
                    while(1){                    

                        //eksakolouthw kai pairnw transactions pros ektelesh
                        fgets(buff, 500, stdin);
                        buff[strlen(buff)-1]='\0';
                        //ean o xristis dwsei done vgainei apo to requestTransactions
                        if(strcmp(buff,"done")==0) { 
                            printf("Exiting /requestTransactions\n");
                            break;
                        }
                        
                        token=strtok(buff, "\n"); //pairnw oti mou apemeine afou afairwsa tin entolh, diladi to transaction
                        if(token[strlen(token)-1]==';'){
                            token[strlen(token)-1]='\0';
                            while(token!=NULL){                        
                                sprintf(transID, "%d", atoi(maxTransID)+1);
                                newTransID=concat(transID, " ");
                                trans=concat(newTransID, token);
                                printf("%s\n", trans);
                                makeTransaction(trans, &senderHashTable, &receiverHashTable, h1, h2, &walletArr, &transIDHashTable, &latestTD, &maxTransID, 1);
                                free(trans);
                                free(newTransID);
                                printf("\n");
                                token=strtok(NULL, ";"); //pairnw oti mou apemeine afou afairwsa tin entolh, diladi to transaction
                            }
                        }
                        else fprintf(stderr, "Invalid command.Wrong format of requested transaction.\n");
                        
                    }

                }
                else{ //prokeitai gia arxeio
                    printf("Reading from %s...\n\n", token);
                    fp=fopen(token,"r");
                    if(fp==NULL) fprintf(stderr, "Invalid command.The file named %s does not exist.\n",token );
                    else{
                        while(fgets(buff,500,fp)){
                            token=strtok(buff, "\n"); //pairnw oti mou apemeine afou afairwsa tin entolh, diladi to transaction
                            if(token[strlen(token)-1]==';'){
                                buff[strlen(buff)-1]='\0';
                                sprintf(transID, "%d", atoi(maxTransID)+1);
                                newTransID=concat(transID, " ");
                                trans=concat(newTransID, buff);
                                printf("%s\n", trans);
                                makeTransaction(trans, &senderHashTable, &receiverHashTable, h1, h2, &walletArr, &transIDHashTable, &latestTD, &maxTransID, 1);
                                free(trans);
                                free(newTransID);
                                printf("\n");
                            }
                            else fprintf(stderr, "Invalid command.Wrong format of requested transaction.\n\n");
                            
                        }
                        fclose(fp); 
                    }
                    
                }
                
            }
            else if((strcmp(token,"/findEarnings")==0)||(strcmp(token,"/findPayments")==0)){
                i=0;
                earnings=0;
                command=malloc((strlen(token)+1)*sizeof(char));
                if(command==NULL){
                    fprintf(stderr, "Malloc failed.\n");
                    exit(1);
                }
                strcpy(command, token);
                token=strtok(NULL, " \n"); //o user gia ton opoio tha psaksoume
                
                if(token!=NULL){
                    user=malloc((strlen(token)+1)*sizeof(char));
                    if(user==NULL){
                        fprintf(stderr, "Malloc failed.\n");
                        exit(1);
                    }
                    strcpy(user, token);
                    token=strtok(NULL, " \n"); //time1 ean iparxei
                    while(token!=NULL){
                        if(i==0){
                            subtok1=malloc((strlen(token)+1)*sizeof(char));
                            if(subtok1==NULL){
                                fprintf(stderr, "Malloc failed.\n");
                                exit(1);
                            }
                            strcpy(subtok1, token);
                        }
                        if(i==1){
                            subtok2=malloc((strlen(token)+1)*sizeof(char));
                            if(subtok2==NULL){
                                fprintf(stderr, "Malloc failed.\n");
                                exit(1);
                            }
                            strcpy(subtok2, token);
                        }
                        if(i==2){
                            subtok3=malloc((strlen(token)+1)*sizeof(char));
                            if(subtok3==NULL){
                                fprintf(stderr, "Malloc failed.\n");
                                exit(1);
                            }
                            strcpy(subtok3, token);
                        }
                        if(i==3){
                            subtok4=malloc((strlen(token)+1)*sizeof(char));
                            if(subtok4==NULL){
                                fprintf(stderr, "Malloc failed.\n");
                                exit(1);
                            }
                            strcpy(subtok4, token);
                        }
                        i++;
                        token=strtok(NULL," \n");
                    }

                    if(strcmp(command,"/findPayments")==0){
                        hash_index=hash_func(h1, user); //xrisimopoiw to h1 afou prokeitai gia to sender hash table
                        startBuck=senderHashTable[hash_index];
                    }
                    else{
                        hash_index=hash_func(h2, user); //xrisimopoiw to h2 afou prokeitai gia to receiver hash table
                        startBuck=receiverHashTable[hash_index];
                    }
                    //se auth th periptwsh den exei dothei xroniko diasthma kai ektipwnw oles tis sinallages
                    if(i==0){                        
                        position=userFound(startBuck, user, &currBuck, walletArr);
                        if(position==-1) fprintf(stderr, "Requested receiver %s is not a user.\n", user);
                        else{
                            currTran=currBuck->arr[position].tranList;
                            while(currTran!=NULL){
                                printTran(currTran);
                                earnings+=currTran->amount;
                                currTran=currTran->next;
                            }                       
                        }
                        if(strcmp(command,"/findPayments")==0) printf("User %s's payments: %d\n", user, earnings);
                        else printf("User %s's earnings: %d\n", user, earnings);
                    }
                    else if((i==1)||(i==3)) fprintf(stderr, "Wrong number of arguments. [time2] and/or [date2] are missing");
                    else{
                        position=userFound(startBuck, user, &currBuck, walletArr);
                        if(position==-1) fprintf(stderr, "Requested receiver %s is not a user.\n", user);
                        else{
                            earnings=find_Earns_Pays(currBuck->arr[position].tranList, subtok1, subtok2, subtok3, subtok4);
                            if(earnings!=-1){
                                if(strcmp(command,"/findPayments")==0) printf("User %s's payments: %d\n", user, earnings);
                                else printf("User %s's earnings: %d\n", user, earnings);
                            } 
                        }
                    }
                    free(user);
                    
                    
                }
                else fprintf(stderr, "Invalid command.User not given.\n");
                free(command);

            }
            else if(strcmp(token,"/walletStatus")==0){
                token=strtok(NULL, "\n");
                if(token!=NULL){
                    hash_index=hash_func(h1,token);
                    position=userFound(senderHashTable[hash_index], token, &currBuck, walletArr);
                    if(position!=-1){
                        printf("User %s's current balance: %d\n",token, walletArr[currBuck->arr[position].walletInd].balance);
                    }
                    else fprintf(stderr, "User %s not found.\n",token);
                }
                else fprintf(stderr, "Invalid command.User not given.\n");

            }
            else if(strcmp(token,"/bitCoinStatus")==0){
                token=strtok(NULL, "\n");
                if(token!=NULL){
                    hash_index=hash_func(11,token);
                    position=bitcoinFound(bcIDHashTable[hash_index], atoi(token), &currBcBuck);
                    if(position!=-1){
                        unspent(currBcBuck->arr[position]->root,&amount);
                        numOfTrans=participatedTrans(currBcBuck->arr[position]->root);
                        printf("%s %d %d\n",token,numOfTrans,amount);
                    }
                    else fprintf(stderr, "The bitcoin with ID %s does not exist.\n", token );
                    
                    
                }
                else fprintf(stderr, "Invalid command.BitcoinID not given.\n");
                
            }
            else if(strcmp(token,"/tracecoin")==0){
                token=strtok(NULL, "\n");
                if(token!=NULL){
                    hash_index=hash_func(11,token);
                    position=bitcoinFound(bcIDHashTable[hash_index], atoi(token), &currBcBuck);
                    if(position!=-1) tracecoin(currBcBuck->arr[position]->root);
                    else fprintf(stderr, "The bitcoin with ID %s does not exist.\n", token );                    
                    
                }
                else fprintf(stderr, "Invalid command.BitcoinID not given.\n");

            }
            else if(strcmp(token,"/exit")==0){ //apeleutherwnei oli ti desmevemeni mnimi kai termatizei to programma                 

                free_hashTable(receiverHashTable, h2);
                free_hashTable(senderHashTable, h1);
                free_tranIDhashTable(transIDHashTable);
                free_bcHashTable(bcIDHashTable);
                for(i=0;i<numOfWallets;i++) free_wallet(&(walletArr[i]));
                free(walletArr);
                free(maxTransID);
                if(subtok1!=NULL) free(subtok1);
                if(subtok2!=NULL) free(subtok2);
                if(subtok3!=NULL) free(subtok3);
                if(subtok4!=NULL) free(subtok4);

                return 0;
            }

            else fprintf(stderr, "Requested command is invalid.Please try again.\n");
        }
            
    }




}

    