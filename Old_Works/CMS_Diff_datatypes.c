#include <stdio.h>
#include <stdlib.h>

//SELF DEFINED HASH FUNCTIONS
int hash1_int(int element){
    return element%10;
}
int hash2_int(int element){
    return element/10;
}

int hash1_float(float element){
    return (element*10)/10;
}
int hash2_float(float element){
    return (int)(element*10)%10;
}

int hash1_char(char ch){
    return (rand() % (9-0+1));
}
int hash2_char(char ch){
    return (rand() % (9-0+1));
}

//CREATE SKETCH
int** create_sketch(int w, int d,int count,int** sketch){
    sketch = (int**)calloc(w,sizeof(int*));
    for(int i= 0 ; i < w ; i++){
        *(sketch+i) = (int*)calloc(d,sizeof(int));
    }
    return sketch;
}

//COMPUTE THE SKETCHES FOR DIFFERENT DATATYPES
void compute_sketch_int(int** sketch, int no_hash_fn, int* Arr,int (*hf[])(int),int count){
    for(int iter = 0; iter < count ; iter++){
        for(int j=0; j < no_hash_fn ; j++ ){
            int h = hf[j](Arr[iter]);
            sketch[j][h] += 1;
        }
    }
}

void compute_sketch_float(int** sketch, int no_hash_fn, float* Arr,int(*hf[])(float),int count){
    for(int iter = 0; iter < count ; iter++){
        for(int j=0; j < no_hash_fn ; j++ ){
            int h = hf[j](Arr[iter]);
            sketch[j][h] += 1;
        }
    }
}

void compute_sketch_char(int** sketch, int no_hash_fn, char* Arr,int(*hf[])(char),int count){
    for(int iter = 0; iter < count ; iter++){
        for(int j=0; j < no_hash_fn ; j++ ){
            int h = hf[j](Arr[iter]);
            sketch[j][h] += 1;
        }
    }
}

//QUERY FOR DIFFERENT DATATYPES
void query_element_int(int* Arr,int no_hash_fn,int(*hf[])(int),int** sketch, int count){
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
}

void query_element_float(float* Arr, int no_hash_fn,int(*hf[])(float),int** sketch, int count){
    float query_ele;
    int minim = count; 
    printf("\nEnter Element to query : ");
    scanf("%f",&query_ele);

    for(int iter = 0; iter < no_hash_fn ; iter++){
        int hash_q_ele = hf[iter](query_ele);
        if(sketch[iter][hash_q_ele] < minim) {minim = sketch[iter][hash_q_ele];}
    }

    printf("\nThe minimum occurence is %d\n",minim);

    int freq = 0;
    for(int i = 0; i < count; i++){
        if(Arr[i] == query_ele){ freq++; }
    }
    printf("\nThe element to be queried actually occurs %d times\n",freq);
}

void query_element_char(char* Arr, int no_hash_fn,int(*hf[])(char),int** sketch, int count){
    char query_ele;
    int minim = count; 
    printf("\nEnter String to query : ");
    scanf("%c",&query_ele);

    for(int iter = 0; iter < no_hash_fn ; iter++){
        int hash_q_ele = hf[iter](query_ele);
        if(sketch[iter][hash_q_ele] < minim) {minim = sketch[iter][hash_q_ele];}
    }
    printf("\nThe minimum occurence is %d\n",minim);

    int freq = 0;
    for(int i = 0; i < count; i++){
        if(Arr[i] == query_ele){ freq++; }
    }
    printf("\nThe element to be queried actually occurs %d times\n",freq);
}

void my_free(int** sketch,int w){
    for (int i = 0; i < w ;i++){
        free(sketch[i]);
    }
    free(sketch);
    // //Release the free mem. back to OS
    // malloc_trim();
}

//PRINT DIFFERENT DATATYPE ARRAYS
void print_array_int(int* Arr,int count){
    for(int i = 0 ; i < count ; i++ ){
        printf("%d ",Arr[i]);
    }
}

void print_array_float(float* Arr, int count){
    for(int i = 0 ; i < count ; i++ ){
        printf("%f ",Arr[i]);
    }
}

void print_array_char(char* Arr, int count){
    for(int i = 0 ; i < count ; i++ ){
        printf("%c ",Arr[i]);
    }
}


//CMS FUNCTIONS
void cms_int(int w, int d, int count, int** sketch ,int no_hash_fn,int(*hf[])(int), int*Arr){
    create_sketch(w,d,count,sketch);
    compute_sketch_int(sketch,no_hash_fn,Arr,hf,count); 
    print_array_int(Arr,count);
    query_element_int(Arr,no_hash_fn,hf,sketch,count);
    my_free(sketch,w);
}

void cms_float(int w, int d, int count, int** sketch, int no_hash_fn, int(*hf[])(float), float* Arr){
    create_sketch(w,d,count,sketch);
    compute_sketch_float(sketch,no_hash_fn,Arr,hf,count); 
    print_array_float(Arr,count);
    query_element_float(Arr,no_hash_fn,hf,sketch,count);
    my_free(sketch,w);
}

void cms_char(int w, int d, int count, int** sketch, int no_hash_fn, int(*hf[])(char), char* Arr){
    create_sketch(w,d,count,sketch);
    compute_sketch_char(sketch,no_hash_fn,Arr,hf,count); 
    print_array_char(Arr,count);
    query_element_char(Arr,no_hash_fn,hf,sketch,count);
    my_free(sketch,w);
}

//MAIN
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
    int** sketch = create_sketch(w,d,count,sketch);

    //(to change data types, change the hash functions, data type of input array, cms function)

    //pointer array of functions
    int (*hf[])(char)= {hash1_char,hash2_char};
    
    //Generating array of elements
    char* Arr = (char*)calloc(count,sizeof(char));
    for(int i = 0 ; i < count ; i++){
        // Arr[i] = (rand() % (upper-lower+1))+lower; --> for an integer array
        //Arr[i] = ((float)rand()/RAND_MAX)*(float)(10.0);-->for float array
        Arr[i] = (char)(26 * (rand() / (RAND_MAX +  1.0))+97);
    }
    
    //cms_int(w,d,count,sketch,no_hash_fn,hf,Arr);
    //cms_float(w,d,count,sketch,no_hash_fn,hf,Arr);
    cms_char(w,d,count,sketch,no_hash_fn,hf,Arr);
}
