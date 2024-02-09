#include <stdio.h>
#include <stdlib.h>

int hash1(int element){
    return element%10;
}

int hash2(int element){
    return element/10;
}

int main(){
    //declaring and initializing variables
    int count = 100;
	int lower = 10;
	int upper = 99;
    int w,d;
    int no_hash_fn = 2;
    int size_hash_table = 10;
    w=no_hash_fn;
    d=size_hash_table;

    //pointer array of functions
    int (*hf[])(int)= {hash1,hash2};
    
    //Array of elements
    int* Arr = (int*)malloc(sizeof(int)*count);
    for(int i = 0 ; i < count ; i++){
        Arr[i] = (rand() % (upper-lower+1))+lower;
    }

    //Dynamic mem. allocation for sketch
    int** sketch = (int**)malloc(sizeof(int*) * w);
    for(int i= 0 ; i < w ; i++){
        *(sketch+i) = (int*)malloc(sizeof(int)*d);
    }

    //Computing the sketch values
    for(int iter = 0; iter < count ; iter++){
        for(int j=0; j < no_hash_fn ; j++ ){
            int h = hf[j](Arr[iter]);
            sketch[j][h] += 1;
        }
    }

    //Print the array
    for(int i = 0 ; i < count ; i++ ){
        printf("%d ",Arr[i]);
    }

    //caclulating the minimum occurence for point query
    int query_ele;
    int minim = count; 

    printf("\nEnter element to query : ");
    scanf("%d",&query_ele);

    for(int iter = 0; iter < no_hash_fn ; iter++){
        int hash_q_ele = hf[iter](query_ele);
        if(sketch[iter][hash_q_ele] < minim) {minim = sketch[iter][hash_q_ele];}
    }

    //Printing the frequency of occurence
    printf("\nThe minimum occurence is %d\n",minim);

    //Check for actual frequency
    int freq = 0;
    for(int i = 0; i < count; i++){
        if(Arr[i] == query_ele){ freq++; }
    }
    printf("\nThe element to be queried actually occurs %d times\n",freq);

    //Free up the acquired memory back to heap
    for (int i = 0; i < w ;i++){
        free(sketch[i]);
    }
    free(sketch);

    // //Release the free mem. back to OS
    // malloc_trim();
    
}