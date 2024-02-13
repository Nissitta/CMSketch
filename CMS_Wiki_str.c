#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define BUZZ_SIZE 100000

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

void compute_sketch(int** sketch, int width, int depth, char** Arr, int (*hf[])(char*,int), int no_ele){
    
    for(int i = 0; i < no_ele; i++){
        
        for(int j = 0; j < depth; j++){
            int h = hf[j](Arr[i],width);
            sketch[j][h] += 1;
        }
    }
}

void query_element(int** sketch, int width, int depth, int (*hf[]) (char*, int), int no_ele, char* q){
    int min = no_ele;
    for(int iter = 0; iter < depth; iter++){
        int h = hf[iter](q,width);
        if(min > sketch[iter][h]){
            min = sketch[iter][h];
        }
    }

    printf("\nThe Queried element occurs less than %d times\n", min);
}

int compare(char* s1, char* s2){
    int i = 0;
    
    while(s1[i] != '\0'){
        if(s1[i] != s2[i]){return 0;}
        i++;
    }
    return 1;
}

void actual_occurence(char** Arr, char* q, int no_ele){
    int counter = 0;
    for( int i = 0; i < no_ele; i++){
        int flag = compare(Arr[i],q);
        if(flag == 1){ counter++; }
        
    }
    printf("\nThe actual occurence of the string is : %d\n",counter);
}

int** mergeSketch(int** sketch_1, int** sketch_2, int width, int depth){
    int **m_sketch = (int**)calloc(depth,sizeof(int*));
    for(int i = 0; i < depth; i++){
        m_sketch[i] = (int*)calloc(width, sizeof(int));
    }

    for(int i = 0; i<depth; i++){
        for(int j = 0; j < width ; j++){
            m_sketch[i][j] = sketch_1[i][j] + sketch_2[i][j];
        }
    }
    return m_sketch;
}

void normal_CMS(char** Arr1, int no_ele,int width, int depth, int (*hf[])(char*,int)){

    int** sketch_1 = create_sketch(width,depth);

    compute_sketch(sketch_1,width,depth,Arr1,hf,no_ele);

    printf("\nEnter the query string : ");
    char* q = (char*)calloc(50,sizeof(char));
    scanf("%s",q);

    query_element(sketch_1,width,depth,hf,no_ele,q);

    printf("\nIn Array 1 ");
    actual_occurence(Arr1,q,no_ele);

    printf("\n");

    //Free the sketches
    for(int i = 0; i<depth; i++){
        free(sketch_1[i]);
    }
    free(sketch_1);

    //free q_element
    free(q);

    //Free Array
    for(int i = 0; Arr1[i] != NULL;i++){
        free(Arr1[i]);
    }
    free(Arr1);
}

void MergeSketchCMS(int no_ele, int width, int depth){

    //Array 1 of strings
    char** Arr1 = (char**)calloc(no_ele, sizeof(char*));
    for(int i = 0; i < no_ele; i++){
        //Arr1[i] = randstring();
    }

    //Array 2 of strings
    char** Arr2 = (char**)calloc(no_ele, sizeof(char*));
    for(int i = 0; i < no_ele; i++){
        //Arr2[i] = randstring();
    }

    //print the array 1
    printf("\nArray 1\n");
    for(int i = 0; i < no_ele; i++){
        printf("%s\n", Arr1[i]);
    }

    //print the array 2
    printf("\nArray 2\n");
    for(int i = 0; i < no_ele; i++){
        printf("%s\n", Arr2[i]);
    }

    int** sketch_1 = create_sketch(width,depth);
    int** sketch_2 = create_sketch(width,depth);

    int (*hf[])(char*, int) = {hash1,hash2, hash3}; 

    compute_sketch(sketch_1,width,depth,Arr1,hf,no_ele);
    compute_sketch(sketch_2,width,depth,Arr2,hf,no_ele);

    int** m_sketch = mergeSketch(sketch_1,sketch_2,width,depth);

    printf("\nEnter the query string(5 chars max.) : ");
    char* q = (char*)calloc(5,sizeof(char));
    scanf("%s",q);    
    
    query_element(m_sketch,width,depth,hf,no_ele,q);
    
    printf("\nIn Array 1 ");
    actual_occurence(Arr1,q,no_ele);
    printf("\n");

    printf("\nIn Array 2 ");
    actual_occurence(Arr2,q,no_ele);
    printf("\n");

    //Free the sketches
    for(int i = 0; i<depth; i++){
        free(sketch_1[i]);
    }
    free(sketch_1);

    for(int i = 0; i<depth; i++){
        free(sketch_2[i]);
    }
    free(sketch_2);

    //free q_element
    free(q);
    //Free Array

    for(int i = 0; i<no_ele;i++){
        free(Arr1[i]);
    }
    free(Arr1);

    for(int i = 0; i<no_ele;i++){
        free(Arr2[i]);
    }
    free(Arr2);
}


int main(){
    //input parameters for the sketch
    float ep = 0.1;
    float del = 0.001;

    int width = (int)(ceil(exp(1)/ep));
    int depth = (int)(ceil(log10(1/del)));

    printf("\nWidth : %d and depth :%d\n",width,depth);

    //Read input from file
    
    char buff[BUZZ_SIZE];

    FILE *f = fopen("text.txt", "r");
    fgets(buff, BUZZ_SIZE, f);
    printf("String read: %s\n", buff);

    char* token = strtok(buff, " - ");

    char** Arr = (char**)malloc(10000*sizeof(char*));
    int iter=0;
    while (token != NULL) {
        Arr[iter] = token;
        // printf(" %s\n", token);
        token = strtok(NULL, " - ");
        iter++;
    }

    printf("\nNo of Words in paragraph : %d",iter);
    fclose(f);

    int (*hf[])(char*, int) = {hash1,hash2, hash3}; 
    
    normal_CMS(Arr,iter,width,depth, hf);

    //MergeSketchCMS(no_ele,str_len,width,depth);
    
}
