#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <malloc.h>
#include <assert.h>
#include <pthread.h>

//create linked list
typedef struct node{
  int num;
  struct node *next;
  int vacant;
}number;

typedef struct node2{
  int num;
  struct node2 *next;
  int vacant;
}number2;

//allow for multi-arg
struct argument{
  number *head;
  number2 *head2;
  int count;
  int upperlimit;
};

//create list of numbers
void* createList(void* argpoint){
  struct argument args =  *((struct argument*) argpoint);
  number *current= args.head;
  number *adjacent= args.head->next;
  number2 *current2= args.head2;
  number2 *adjacent2= args.head2->next;
  //generate random numbers
  int i;
  for(i=0;i<args.count;i++){
    //rand()%args.upperlimit;
    current->num=rand()%args.upperlimit;
    current2->num=current->num;
    adjacent->vacant=0;
    adjacent2->vacant=0;
    current=adjacent;
    current2=adjacent2;
    current->next=malloc(sizeof(number));
    current2->next=malloc(sizeof(number2));
    adjacent=current->next;
    adjacent2=current2->next;
  }
  current->vacant=1;
  current2->vacant=1;
  // printf("List created");
  return (void*) args.head;
  
}

void* checkList(void* args){
  number2 *head2=(number2*)args;
  number2*current2=head2;
  number2*adjacent2=head2->next;
  //handle numbers within 8 of each other
  while(!(current2->vacant)){
    // if(abs(adjacent->num - current->num) <=8 && (adjacent->vacant==0)){
    if((current2->num==adjacent2->num) && (adjacent2->vacant==0)){
      if(adjacent2->next->vacant){
	adjacent2->num=0;
	adjacent2->vacant=1;
	break;
      }else{
	number2 *temp=adjacent2;
	current2->next=adjacent2->next;
	adjacent2=current2->next;
	free(temp);
	continue;
      }
    }
    current2=current2->next;
    adjacent2=current2->next;
  }
  return (void*) head2;
}

void* FIFO(void* args){
  int PageFaultNorm;
  int PageFaultCull;
  int CounterNorm;
  int CounterCull;
  PageFaultNorm=0;
  PageFaultCull=0;
  CounterNorm=0;
  CounterCull=0;
  number *head=(number*)args;
  number*current=head;
  number*adjacent=head->next;
  number2 *head2=(number2*)args;
  number2*current2=head2;
  number2*adjacent2=head2->next;
  //store first 16 in array
  int i;
  int j;
  int k;
  
  //This Section calculates the Page Fault Rate of the Normal List
  while(current != NULL){
    CounterNorm++;
    current=current->next;
  }
  int Pages2[CounterNorm];
  current=head;
  for(i=0;i<CounterNorm;i++){
    Pages2[i]=current->num;
    current=current->next;
  }
  //store numbers inside frames
  int temp2[16];
  for(i=0;i<16;i++){
    temp2[i]=-1;
  }
  for(i=0;i<CounterNorm;i++){
    j=0;
    for(k=0;k<16;k++){
      if(Pages2[i]==temp2[k]){
        j++;
        PageFaultNorm--;
      }
    }
    PageFaultNorm++;
    if((PageFaultNorm<=16) && (j==0)){
      temp2[i]=Pages2[i];
    }
    else if(j==0){
      temp2[(PageFaultNorm-1)%16]=Pages2[i];
    }
    // for(k=0;k<16;k++){
    //printf("%d\t",temp[k]);
    // }
  }

  //This Section calculates the Page Fault Rate of the Culled List
  while(current2 != NULL){
    CounterCull++;
    current2=current2->next;
  }
  int Pages[CounterCull];
  current2=head2;
  for(i=0;i<CounterCull;i++){
    Pages[i]=current2->num;
    current2=current2->next;
  }
  int temp[16];
  for(i=0;i<16;i++){
    temp[i]=-1;
  }
  for(i=0;i<CounterCull;i++){
    j=0;
    for(k=0;k<16;k++){
      if(Pages[i]==temp[k]){
	j++;
	PageFaultCull--;
      }
    }
    PageFaultCull++;
    if((PageFaultCull<=16) && (j==0)){
      temp[i]=Pages[i];
    }
    else if(j==0){
      temp[(PageFaultCull-1)%16]=Pages[i];
    }
    // for(k=0;k<16;k++){
    //printf("%d\t",temp[k]);
    // }
  }
  float PFN;
  PFN=(float)PageFaultNorm/(float)CounterNorm;
    
  float PFC;
  PFC=(float)PageFaultCull/(float)CounterCull;
      
  //Displays the Rates
  printf("FIFO Page Replacement\n");
  printf("Page Fault Rate for Normal List:\t");
  printf("%f\n",PFN);

  printf("Page Fault Rate for Culled List:\t");
  printf("%f\n",PFC);
   
  return (void*) head2;
}
	   
 
void* Optimal(void* args){
  int PFNorm;
  int PFCull;
  int CountNorm;
  int CountCull;
  PFNorm=0;
  PFCull=0;
  CountNorm=0;
  CountCull=0;
  number *head=(number*)args;
  number*current=head;
  number*adjacent=head->next;
  number2 *head2=(number2*)args;
  number2*current2=head2;
  number2*adjacent2=head2->next;
  int i,j,k;
  int flag1, flag2, flag3;
  int posi, max;
  int temp3[16], temp4[16];
  //This section is for the normal list
  while(current != NULL){
    CountNorm++;
    current=current->next;
  }
  int Pages3[CountNorm];
  current=head;
  for(i=0;i<CountNorm;i++){
    Pages3[i]=current->num;
    current=current->next;
  }
  int frames[16];
  for(i=0;i<16;i++){
    frames[i]=-1;
  }
  //Count how far away each number in frame is, and choose the fartest to be replaced
  for(i=0;i<CountNorm;i++){
    flag1=flag2=0;
    for(j=0;j<16;j++){
      if(frames[j]==Pages3[i]){
	flag1=flag2=1;
	break;
      }
    }
    if(flag1==0){
      for(j=0;j<16;j++){
	if(frames[j]==-1){
	  PFNorm++;
	  frames[j]=Pages3[i];
	  flag2=1;
	  break;
	}
      }
    }
    if(flag2==0){
      flag3=0;
      for(j=0;j<16;j++){
	temp3[j]=-1;
	for(k=i+1;k<16;k++){
	  if(frames[j]==Pages3[k]){
	    temp3[j]=k;
	    break;
	  }
	}
      }
      for(j=0;j<16;j++){
	if(temp3[j]==-1){
	  posi=j;
	  flag3=1;
	  break;
	}
      }
      if(flag3==0){
	max=temp3[0];
	posi=0;
	for(j=1;j<16;j++){
	  if(temp3[j]>max){
	    max=temp3[j];
	    posi=j;
	  }
	}
      }
      frames[posi]=Pages3[i];
      PFNorm++;
    }
    // for(j=0;j<16;j++){
      // printf("%d\t", frames[j]);
      // }
  }

  //Optimal on the Culled List
  while(current2 != NULL){
    CountCull++;
    current2=current2->next;
  }
  int Pages4[CountCull];
  current2=head2;
  for(i=0;i<CountCull;i++){
    Pages4[i]=current2->num;
    current2=current2->next;
  }
  int frames2[16];
  for(i=0;i<16;i++){
    frames2[i]=-1;
  }
  for(i=0;i<CountCull;i++){
    flag1=flag2=0;
    for(j=0;j<16;j++){
      if(frames2[j]==Pages4[i]){
        flag1=flag2=1;
        break;
      }
    }
    if(flag1==0){
      for(j=0;j<16;j++){
        if(frames2[j]==-1){
          PFCull++;
          frames2[j]=Pages4[i];
          flag2=1;
          break;
        }
      }
    }
    if(flag2==0){
      flag3=0;
      for(j=0;j<16;j++){
        temp4[j]=-1;
        for(k=i+1;k<16;k++){
          if(frames2[j]==Pages4[k]){
            temp4[j]=k;
            break;
          }
        }
      }
      for(j=0;j<16;j++){
        if(temp4[j]==-1){
          posi=j;
          flag3=1;
          break;
        }
      }
      if(flag3==0){
        max=temp4[0];
        posi=0;
        for(j=1;j<16;j++){
          if(temp4[j]>max){
            max=temp4[j];
            posi=j;
          }
        }
      }
      frames2[posi]=Pages4[i];
      PFCull++;
    }
    // for(j=0;j<16;j++){
    // printf("%d\t", frames[j]);
    // }
  }

  
  float PFN;
  PFN=(float)PFNorm/(float)CountNorm;

  float PFC;
  PFC=(float)PFCull/(float)CountCull;

  //Displays the Rates
  printf("Optimal Page Replacement\n");
  printf("Page Fault Rate for Normal List:\t");
  printf("%f\n",PFN);

  printf("Page Fault Rate for Culled List:\t");
  printf("%f\n",PFC);
 
 return (void*) head2;
} 



main(argc,argv)
int argc;
char *argv[]; {


    int count,upperlimit,x;

/*	check to make sure we pass in arguments  */
    if(argc <= 2) {
	printf("Usage:  %s Count UpperLimit\n",argv[0]);
	exit(0);
    }
/*	we've checked to make sure arguments are there,  */
/*	it is now save to access argv[1] and argv[2]     */
    count = atoi(argv[1]);
    upperlimit = atoi(argv[2]);
    srand((int)time(NULL));
    number*head=malloc(sizeof(number));
    number2*head2=malloc(sizeof(number2));
    head->next=malloc(sizeof(number));
    head2->next=malloc(sizeof(number2));
    
    //create first thread
    pthread_t T1ID;
    struct argument T1args;
    T1args.count=count;
    T1args.head=head;
    T1args.head2=head2;
    T1args.upperlimit=upperlimit;
    void *T1argsPointer = &T1args;
    pthread_attr_t T1attr;
    pthread_attr_init(&T1attr);
    pthread_attr_setdetachstate(&T1attr, PTHREAD_CREATE_JOINABLE);

    //create second thread
    pthread_t T2ID;
    pthread_attr_t T2attr;
    pthread_attr_init(&T2attr);
    pthread_attr_setdetachstate(&T2attr, PTHREAD_CREATE_JOINABLE);

    //create thrird thread
    pthread_t T3ID;
    pthread_attr_t T3attr;
    pthread_attr_init(&T3attr);
    pthread_attr_setdetachstate(&T3attr, PTHREAD_CREATE_JOINABLE);

    //create forth thread
    pthread_t T4ID;
    pthread_attr_t T4attr;
    pthread_attr_init(&T4attr);
    pthread_attr_setdetachstate(&T4attr, PTHREAD_CREATE_JOINABLE);

    //cretae list
    pthread_create(&T1ID,NULL,createList,T1argsPointer);
    pthread_join(T1ID, (void**)&head);

    //check list
    pthread_create(&T2ID,NULL,checkList, (void*) head2);
    pthread_join(T2ID, (void**)&head2);

    //run FIFO
    pthread_create(&T3ID,NULL,FIFO, (void*) head2);
    pthread_join(T2ID, (void**)&head2);

    //run Optimal
    pthread_create(&T4ID,NULL,Optimal, (void*) head2);
    pthread_join(T4ID, (void**)&head2);


    number*current=head;
    number*adjacent=head->next;
    number2*current2=head2;
    number2*adjacent2=head2->next;

    //print list
    /*  while(!(current->vacant)){
      printf("%d ",current->num);
      current=adjacent;
      adjacent=current->next;
    }
    printf("\n--------------------\n");
    while(!(current2->vacant)){
      printf("%d ",current2->num);
      current2=adjacent2;
      adjacent2=current2->next;
      } */
    

    return 0;
}
