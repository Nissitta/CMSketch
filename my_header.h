#include <stdio.h>
#include <stdlib.h>


//key generator
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

//Initialize sketch
int** create_sketch(int width, int depth){
    int** sketch = (int**)calloc(depth,sizeof(int*));
    for(int i=0 ; i < depth; i++){

        sketch[i] = (int*)calloc(width,sizeof(int));
    }
    return sketch;
}

int compare(char* s1, char* s2){
    int flag=0,i=0;  
    while(s1[i]!='\0' &&s2[i]!='\0')  
    {  
       if(s1[i]!=s2[i])  
       {  
           flag=1;  
           break;  
       }  
       i++;  
    }  
    if(flag==0) return 1;  
    else  return 0;  
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

// void MergeSketchCMS(int no_ele, int width, int depth){

//     //Array 1 of strings
//     char** Arr1 = (char**)calloc(no_ele, sizeof(char*));
//     for(int i = 0; i < no_ele; i++){
//         //Arr1[i] = randstring();
//     }

//     //Array 2 of strings
//     char** Arr2 = (char**)calloc(no_ele, sizeof(char*));
//     for(int i = 0; i < no_ele; i++){
//         //Arr2[i] = randstring();
//     }

//     //print the array 1
//     printf("\nArray 1\n");
//     for(int i = 0; i < no_ele; i++){
//         printf("%s\n", Arr1[i]);
//     }

//     //print the array 2
//     printf("\nArray 2\n");
//     for(int i = 0; i < no_ele; i++){
//         printf("%s\n", Arr2[i]);
//     }

//     char** Keys = (char**)calloc(depth, sizeof(char*));
//     for( int i = 0; i < depth; i++){
//         Keys[i] = randstring(8);
//         printf("%s\n",Keys[i]);
//     }

//     int** sketch_1 = create_sketch(width,depth);
//     int** sketch_2 = create_sketch(width,depth);

//     compute_sketch(sketch_1,width,depth,Arr1,Keys,no_ele);
//     compute_sketch(sketch_2,width,depth,Arr2,Keys,no_ele);

//     int** m_sketch = mergeSketch(sketch_1,sketch_2,width,depth);

//     printf("\nEnter the query string(5 chars max.) : ");
//     char* q = (char*)calloc(5,sizeof(char));
//     scanf("%s",q);    
    
//     query_element(m_sketch,width,depth,Keys,no_ele,q);
    
//     printf("\nIn Array 1 ");
//     actual_occurence(Arr1,q,no_ele);
//     printf("\n");

//     printf("\nIn Array 2 ");
//     actual_occurence(Arr2,q,no_ele);
//     printf("\n");

//     for(int i = 0; i < depth; i++){
//         free(Keys[i]);
//     }
//     free(Keys);

//     // //Free the sketches
//     // for(int i = 0; i<depth; i++){
//     //     free(sketch_1[i]);
//     // }
//     // free(sketch_1);

//     // for(int i = 0; i<depth; i++){
//     //     free(sketch_2[i]);
//     // }
//     // free(sketch_2);

//     // //free q_element
//     // free(q);
//     // //Free Array

//     // for(int i = 0; i<no_ele;i++){
//     //     free(Arr1[i]);
//     // }
//     // free(Arr1);

//     // for(int i = 0; i<no_ele;i++){
//     //     free(Arr2[i]);
//     // }
//     // free(Arr2);
// }







