#include <stdio.h>
#include <stdlib.h>
#include <math.h>

char* randstring(size_t length) {
    static char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";        
    char *randomString = NULL;

    if (length) {
        randomString = (char*)malloc(sizeof(char) * (length +1));

        if (randomString) {            
            for (int n = 0;n < length;n++) {            
                int key = rand() % (int)(sizeof(charset) -1);
                randomString[n] = charset[key];
            }

            randomString[length] = '\0';
        }
    }
    return randomString;
}

int** create_sketch(int width, int depth){
    int** sketch = (int**)calloc(depth,sizeof(int*));
    for(int i=0 ; i < depth; i++){

        sketch[i] = (int*)calloc(width,sizeof(int));
    }
    return sketch;
}

int hash1 (char* word, int width)
{
    unsigned int hash = 0;
    for (int i = 0 ; word[i] != '\0' ; i++)
    {
        hash = 31*hash + word[i];
    }
    return hash % width;
}

int hash2 (char *word, int width) {
    int sum,i;
    for(sum = 0, i=0 ; word[i] != '\0' ;i++ ){
        sum += word[i];
    }
    return sum%width;
}

int hash3 (char* word, int width){
    int len=5;
    int h = 5381;
    int i=0;
    while (word[i] != '\0') {
        /* h = 33 * h ^ s[i]; */
        h += (h << 5);  
        h ^= *word++;
        i++;
    }
    return h%width;
}

void compute_sketch(int** sketch, int width, int depth, char** Arr, int (*hf[])(char*,int), int no_ele, int count){

    for(int i = 0; i < no_ele; i++){
        for(int j = 0; j < depth; j++){
            int h = hf[j](Arr[i],width);
            sketch[j][h] += 1;
        }
    }
}

void query_element(int** sketch, int width, int depth, char** Arr, int (*hf[]) (char*, int), int no_ele, int count, char* q){
    int min = no_ele;
    for(int iter = 0; iter < depth; iter++){
        int h = hf[iter](q,width);
        if(min > sketch[iter][h]){
            min = sketch[iter][h];
        }
    }

    printf("\nThe Queried element occurs at least %d times\n", min);
}

int compare(char* s1, char* s2){
    int i = 0;
    
    while(s1[i] != '\0'){
        if(s1[i] != s2[i]){return 0;}
        i++;
    }
    return 1;
}


void actual_occurence(char** Arr, char* q, int no_ele,int str_len){
    int counter = 0;
    for( int i = 0; i < no_ele; i++){
        int flag = compare(Arr[i],q);
        if(flag == 1){ counter++; }
        
    }
    printf("\nThe actual occurence of the string is : %d\n",counter);

}

int main(){
    float ep = 0.1;
    float del = 0.001;
    int width = 27;
    int depth = 3;
    int no_ele = 100;
    int count = 100;//hash table values range
    int str_len = 5;

    //Array of strings
    char** Arr = (char**)calloc(no_ele, sizeof(char*));
    for(int i = 0; i < no_ele; i++){
        Arr[i] = randstring(str_len);
    }

    //print the array
    for(int i = 0; i < no_ele; i++){
        printf("%s\n", Arr[i]);
    }

    int** sketch = create_sketch(width,depth);

    int (*hf[])(char*, int) = {hash1,hash2, hash3}; 

    compute_sketch(sketch,width,depth,Arr,hf,no_ele,count);

    printf("\nEnter the query string(5 chars max.) : ");
    char* q = (char*)calloc(5,sizeof(char));
    scanf("%s",q);    
    
    query_element(sketch,width,depth,Arr,hf,no_ele,count,q);
    
    actual_occurence(Arr,q,no_ele,str_len);
    printf("\n");

    //Free the sketch
    for(int i = 0; i<depth; i++){
        free(sketch[i]);
    }
    free(sketch);

    //free q_element
    free(q);
    //Free Array
    for(int i = 0; i<no_ele;i++){
        free(Arr[i]);
    }
    free(Arr);
}
