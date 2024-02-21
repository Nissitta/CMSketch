#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <openssl/hmac.h>
#include "my_header.h"
#include <assert.h>

// Functions

void print_sketch(int** sketch,int depth, int width){
    int count_zeros = 0;
    for(int depth_iter = 0; depth_iter < depth; depth_iter++){
        for(int width_iter = 0; width_iter < width; width_iter++){
            if(sketch[depth_iter][width_iter] == 0){count_zeros++;}
            printf("%d", sketch[depth_iter][width_iter]);
        }
        printf("\n");
    }
    printf("\n No. of Zeros : %d",count_zeros);
}

int data_from_file(char* filename,int no_words,char** Arr){

    int max_line_length = 40;

    char* line = (char*)malloc(max_line_length * sizeof(char));

    FILE *f = fopen(filename, "r");
        int iter=0;
        while(fgets(line, max_line_length, f)){
            Arr[iter] = (char*)malloc(max_line_length* sizeof(line));
            line[strcspn(line, "\r\n")] = 0;
            strcpy(Arr[iter],line);
            iter++;
        }
        printf("No of Words : %d",iter);
    fclose(f);
    
    return iter;
}

uint32_t key_gen_32_bit(){
    uint32_t key = (uint32_t)rand();
    return key;
}

uint64_t key_gen_64_bit(){
    uint64_t key = (uint64_t)rand();
    return key;
}

uint32_t* Arr_of_Keys_32bit(int no_of_keys){
    uint32_t* Keys = (uint32_t*)calloc(no_of_keys, sizeof(uint32_t));
    for( int i = 0; i < no_of_keys; i++){
        Keys[i] = key_gen_32_bit();
    }
    return Keys;
}

uint64_t* Arr_of_Keys_64bit(int no_of_keys){
    uint64_t* Keys = (uint64_t*)calloc(no_of_keys, sizeof(uint64_t));
    for( int i = 0; i < no_of_keys; i++){
        Keys[i] = key_gen_64_bit();
    }
    return Keys;
}

char** Arr_of_random_string(size_t length, int no_of_keys){
    char** Keys = (char**)calloc(no_of_keys, sizeof(char*));
    for(int i = 0; i < no_of_keys; i++){
        Keys[i] = randstring(16);
    }
    return Keys;
}

int HMAC_SHA256(char *key, char *message, int width) {
    unsigned char digest[EVP_MAX_MD_SIZE];
    unsigned int digest_len;

    // Compute HMAC-SHA256 hash
    HMAC(EVP_sha256(), key, strlen(key), (unsigned char *)message, strlen(message), digest, &digest_len);

    // Convert hash to integer
    unsigned int hashed_value = 0;
    for(int i = 0; i < digest_len && i < 4; i++) { // Assuming int is 4 bytes
        hashed_value = (hashed_value << 8) | digest[i];
    }
    return (unsigned int)(hashed_value%width);
}

uint64_t fasthash64(const void *buf, size_t len, uint64_t seed) {
    const uint64_t m = 0x880355f21e6d1965ULL;
    const uint64_t *pos = (const uint64_t *)buf;
    const uint64_t *end = pos + (len / 8);
    const unsigned char *pos2;
    uint64_t h = seed ^ (len * m);
    uint64_t v;

    while (pos != end) {
        v = *pos++;
        h ^= mix_fh(v);
        h *= m;
    }

    pos2 = (const unsigned char *)pos;
    v = 0;

    switch (len & 7) {
    case 7:
        v ^= (uint64_t)pos2[6] << 48; 
    case 6:
        v ^= (uint64_t)pos2[5] << 40; 
    case 5:
        v ^= (uint64_t)pos2[4] << 32; 
    case 4:
        v ^= (uint64_t)pos2[3] << 24; 
    case 3:
        v ^= (uint64_t)pos2[2] << 16; 
    case 2:
        v ^= (uint64_t)pos2[1] << 8; 
    case 1:
        v ^= (uint64_t)pos2[0];
        h ^= mix_fh(v);
        h *= m;
    }

    return mix_fh(h);
}

uint32_t fasthash32(const void *buf, size_t len, uint32_t seed) {
    uint64_t h = fasthash64(buf, len, seed);
    return h - (h >> 32);
}

uint32_t MurmurHash3_x86_32 ( const void * key, int len, uint32_t seed) {
    const uint8_t * data = (const uint8_t*)key;
    const int nblocks = len / 4;
    uint32_t h1 = seed;
    const uint32_t c1 = 0xcc9e2d51;
    const uint32_t c2 = 0x1b873593;

    const uint32_t * blocks = (const uint32_t *)(data + nblocks*4);

    for(int i = -nblocks; i; i++)  {
        uint32_t k1 = getblock32(blocks,i);
        k1 *= c1;
        k1 = rotl32(k1,15);
        k1 *= c2;
        h1 ^= k1;
        h1 = rotl32(h1,13); 
        h1 = h1*5+0xe6546b64;
    }

    const uint8_t * tail = (const uint8_t*)(data + nblocks*4);
    uint32_t k1 = 0;
    switch(len & 3)  {
        case 3: k1 ^= tail[2] << 16;
        case 2: k1 ^= tail[1] << 8;
        case 1: k1 ^= tail[0];
            k1 *= c1; k1 = rotl32(k1,15); k1 *= c2; h1 ^= k1;
    };

    h1 ^= len;
    h1 = fmix32(h1);
    return h1;
} 

uint32_t getblock32 ( const uint32_t * p, int i ) { return p[i]; }

uint32_t rotl32 ( uint32_t x, int8_t r ) { return (x << r) | (x >> (32 - r)); }

uint32_t fmix32 ( uint32_t h ) {
    h ^= h >> 16;
    h *= 0x85ebca6b;
    h ^= h >> 13;
    h *= 0xc2b2ae35;
    h ^= h >> 16;
    return h;
}

uint64_t fnv_1a(char* element, int seed){
    int i, len = strlen(element);
    uint64_t h = 14695981039346656037ULL + (31 * seed); 
    for (i = 0; i < len; ++i){
        h = h ^ (unsigned char) element[i];
        h = h * 1099511628211ULL; 
    }
    return h;
}

int** create_sketch(int width, int depth){
    int** sketch = (int**)calloc(depth,sizeof(int*));
    for(int i=0 ; i < depth; i++){
        sketch[i] = (int*)calloc(width,sizeof(int));
    }
    return sketch;
}

uint32_t hashlittle( const void *key, size_t length, uint32_t initval) {
    uint32_t a,b,c;                                         
    union { const void *ptr; size_t i; } u;     
    a = b = c = 0xdeadbeef + ((uint32_t)length) + initval;
    u.ptr = key;
    if (HASH_LITTLE_ENDIAN && ((u.i & 0x3) == 0)) {
        const uint32_t *k = (const uint32_t *)key;        
    
        while (length > 12) {
            a += k[0];
            b += k[1];
            c += k[2];
            mix(a,b,c);
            length -= 12;
            k += 3;
        }

        #ifndef VALGRIND
        switch(length) {
            case 12: c+=k[2]; b+=k[1]; a+=k[0]; break;
            case 11: c+=k[2]&0xffffff; b+=k[1]; a+=k[0]; break;
            case 10: c+=k[2]&0xffff; b+=k[1]; a+=k[0]; break;
            case 9 : c+=k[2]&0xff; b+=k[1]; a+=k[0]; break;
            case 8 : b+=k[1]; a+=k[0]; break;
            case 7 : b+=k[1]&0xffffff; a+=k[0]; break;
            case 6 : b+=k[1]&0xffff; a+=k[0]; break;
            case 5 : b+=k[1]&0xff; a+=k[0]; break;
            case 4 : a+=k[0]; break;
            case 3 : a+=k[0]&0xffffff; break;
            case 2 : a+=k[0]&0xffff; break;
            case 1 : a+=k[0]&0xff; break;
            case 0 : return c;
        }
        #else // make valgrind happy 
            const uint8_t  *k8;
            k8 = (const uint8_t *)k;
            switch(length) {
                case 12: c+=k[2]; b+=k[1]; a+=k[0]; break;
                case 11: c+=((uint32_t)k8[10])<<16; 
                case 10: c+=((uint32_t)k8[9])<<8;   
                case 9 : c+=k8[8];                 
                case 8 : b+=k[1]; a+=k[0]; break;
                case 7 : b+=((uint32_t)k8[6])<<16;  
                case 6 : b+=((uint32_t)k8[5])<<8;  
                case 5 : b+=k8[4];                  
                case 4 : a+=k[0]; break;
                case 3 : a+=((uint32_t)k8[2])<<16;  
                case 2 : a+=((uint32_t)k8[1])<<8;  
                case 1 : a+=k8[0]; break;
                case 0 : return c;
            }
        #endif 
    } else if (HASH_LITTLE_ENDIAN && ((u.i & 0x1) == 0)) {
        const uint16_t *k = (const uint16_t *)key;        
        const uint8_t  *k8;
        while (length > 12) {
            a += k[0] + (((uint32_t)k[1])<<16);
            b += k[2] + (((uint32_t)k[3])<<16);
            c += k[4] + (((uint32_t)k[5])<<16);
            mix(a,b,c);
            length -= 12;
            k += 6;
        }
    
        k8 = (const uint8_t *)k;
        switch(length) {
            case 12: c+=k[4]+(((uint32_t)k[5])<<16);
                b+=k[2]+(((uint32_t)k[3])<<16);
                a+=k[0]+(((uint32_t)k[1])<<16);
                break;
            case 11: c+=((uint32_t)k8[10])<<16;     
            case 10: c+=k[4];
                b+=k[2]+(((uint32_t)k[3])<<16);
                a+=k[0]+(((uint32_t)k[1])<<16);
                break;
            case 9 : c+=k8[8];                      
            case 8 : b+=k[2]+(((uint32_t)k[3])<<16);
                a+=k[0]+(((uint32_t)k[1])<<16);
                break;
            case 7 : b+=((uint32_t)k8[6])<<16;      
            case 6 : b+=k[2];
                a+=k[0]+(((uint32_t)k[1])<<16);
                break;
            case 5 : b+=k8[4];                      
            case 4 : a+=k[0]+(((uint32_t)k[1])<<16);
                break;
            case 3 : a+=((uint32_t)k8[2])<<16;      
            case 2 : a+=k[0];
                break;
            case 1 : a+=k8[0];
                break;
            case 0 : return c;                     
        }
    } else {                        
        const uint8_t *k = (const uint8_t *)key;
        while (length > 12) {
            a += k[0];
            a += ((uint32_t)k[1])<<8;
            a += ((uint32_t)k[2])<<16;
            a += ((uint32_t)k[3])<<24;
            b += k[4];
            b += ((uint32_t)k[5])<<8;
            b += ((uint32_t)k[6])<<16;
            b += ((uint32_t)k[7])<<24;
            c += k[8];
            c += ((uint32_t)k[9])<<8;
            c += ((uint32_t)k[10])<<16;
            c += ((uint32_t)k[11])<<24;
            mix(a,b,c);
            length -= 12;
            k += 12;
        }
    
        switch(length) {
            case 12: c+=((uint32_t)k[11])<<24;
            case 11: c+=((uint32_t)k[10])<<16;
            case 10: c+=((uint32_t)k[9])<<8;
            case 9 : c+=k[8];
            case 8 : b+=((uint32_t)k[7])<<24;
            case 7 : b+=((uint32_t)k[6])<<16;
            case 6 : b+=((uint32_t)k[5])<<8;
            case 5 : b+=k[4];
            case 4 : a+=((uint32_t)k[3])<<24;
            case 3 : a+=((uint32_t)k[2])<<16;
            case 2 : a+=((uint32_t)k[1])<<8;
            case 1 : a+=k[0];
            break;
            case 0 : return c;
        }
    }
    final(a,b,c);
    return c;
}

void compute_sketch_64_bit(int** sketch, int width, int depth, char** Arr, uint64_t* Keys, int no_ele){
    int count = 0;
    for(int i = 0; i < no_ele; i++){
        for(int j = 0; j < depth; j++){
            int h=0;

            //Include proper necessary hash functions
            
            // h = fasthash64(Arr[i], strlen(Arr[i]),Keys[j]);           //FastHash64 bits
            // uint64_t shiftedHash = h >> SHIFT_AMOUNT;      
            // uint64_t maskedBits = shiftedHash & 0x3FFF;
            // h = (int)maskedBits;
            h = number_from_64_bit_hash(fasthash64(Arr[i], strlen(Arr[i]), Keys[j]), width);
            sketch[j][h] += 1;
        }
    }
}

void compute_sketch_32_bit(int** sketch, int width, int depth, char** Arr, uint32_t* Keys, int no_ele){
    int count = 0;
    for(int i = 0; i < no_ele; i++){
        for(int j = 0; j < depth; j++){
            int h=0;
            h = number_from_32_bit_hash(fasthash32(Arr[i],strlen(Arr[i]), Keys[j]),width);       //FastHash32 bits             
            sketch[j][h] += 1;
        }
    }
}

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

int compare(char* s1, char* s2){
    int flag=0,i=0;  
    while(s1[i]!='\0' &&s2[i]!='\0')  
    {  
       if(s1[i]!=s2[i]) { flag=1; break; }  
       i++;  
    }  
    if(flag==0) return 1;  
    else  return 0;  
}

void query_element_fastHash_64_bit(int** sketch, int width, int depth,uint64_t* Keys, int no_ele, char* q){
    
    int min = no_ele;
    for(int iter = 0; iter < depth; iter++){
        int h = 0;

        //Include proper required hash functions

        //h = HMAC_SHA256(Keys[iter], q,width);                 //HMAC_SHA256
        //h = fasthash32(q,strlen(q), Keys[iter]) % width;      //FastHash32 bits
        // h = fasthash64(q,strlen(q), Keys[iter]) % width;        //FastHash64 bits
        // uint64_t shiftedHash = h >> SHIFT_AMOUNT;      
        // uint64_t maskedBits = shiftedHash & 0x3FFF;
        // h = (int)maskedBits;

        h = number_from_64_bit_hash(fasthash64(q, strlen(q), Keys[iter]), width);
        if(min > sketch[iter][h]){
            min = sketch[iter][h];
        }
    }
    //hashes = &hf;
    printf("\nThe Estimate of occurence of queried element : %d times\n", min);
}

void query_element_fastHash_32_bit(int** sketch, int width, int depth,uint32_t* Keys, int no_ele, char* q){
    
    int min = no_ele;
    for(int iter = 0; iter < depth; iter++){
        int h = 0;

        //Include proper required hash functions

        //h = HMAC_SHA256(Keys[iter], q,width);                 //HMAC_SHA256
        //h = fasthash32(q,strlen(q), Keys[iter]) % width;      //FastHash32 bits
        // h = fasthash64(q,strlen(q), Keys[iter]) % width;        //FastHash64 bits

        h = number_from_32_bit_hash(fasthash32(q, strlen(q), Keys[iter]), width);
        int deter = sketch[iter][h];
        if(min > sketch[iter][h]){
            min = sketch[iter][h];
        }
    }
    //hashes = &hf;
    printf("\nThe Estimate of occurence of queried element : %d times\n", min);
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

void genaj(int** hashes, int ind){
    hashes[ind][0] = (int)((float)(rand())*(float)(LONG_PRIME)/(float)(RAND_MAX) + 1);
    hashes[ind][1] = (int)((float)(rand())*(float)(LONG_PRIME)/(float)(RAND_MAX) + 1);
}

int hashstr(char* str){
    unsigned long hash = 5381;
    int c;
    while (c = *str++) { hash = ((hash << 5) + hash) + c; }
    return hash;
}

void CMS(char** Arr, int no_ele,int width, int depth){

    int** sketch = create_sketch(width,depth);

    //----included only for changing sketch structure (C++ implemetation) ----

    // srand(time(NULL));
    // int** hashes = (int**)malloc(depth * sizeof(int*));
    // for(int iter_ind = 0; iter_ind < depth; iter_ind++){
    //     hashes[iter_ind] = (int*)malloc(sizeof(int)*2);
    //     genaj(hashes, iter_ind);
    // }
    //------------------------------------------------------------------
    //uint32_t* Keys = Arr_of_Keys_32bit(depth);

    //char** Keys = Arr_of_random_string(16, depth); //-----> use this for HMAC_SHA256

    uint64_t* Keys = Arr_of_Keys_64bit(depth);
    compute_sketch_64_bit(sketch,width,depth,Arr,Keys,no_ele);
    char q[50];

    int ans = 1;
    do{
        printf("\nEnter the query string : ");
        scanf("%s",q);
        query_element_fastHash_64_bit(sketch,width,depth,Keys,no_ele,q);

        printf("\nIn given Array,  ");
        actual_occurence(Arr,q,no_ele);
        printf("\n");

        printf("Do you want to repeat ? (1/0)");
        scanf("%d", &ans);

    }while(ans == 1);

    for(int i = 0; i < depth; i++){ free(sketch[i]); }
    free(sketch);
    free(Keys);
}


int number_from_32_bit_hash(uint32_t hash_value, int bucket_size) {
    int num_bits = (int)ceil(log2(bucket_size));
    int shift_amount = 32 - num_bits;
    uint32_t shifted_hash = hash_value >> shift_amount;
    uint32_t masked_bits = shifted_hash & ((1 << num_bits) - 1); 
    int bucket_index = (int)masked_bits;
    return bucket_index;
}

int number_from_64_bit_hash(uint64_t hash_value, int bucket_size) {
    int num_bits = (int)ceil(log2(bucket_size));
    int shift_amount = 64 - num_bits + 1;
    uint64_t shifted_hash = hash_value >> shift_amount;
    uint64_t masked_bits = shifted_hash & ((1 << num_bits) - 1);
    int bucket_index = (int)masked_bits;
    return bucket_index;
}

void CMS_fastHash64(char** Arr, int no_ele, int width, int depth){
    uint64_t* Keys = Arr_of_Keys_64bit(depth);
    int** sketch = create_sketch(width,depth);
    compute_sketch_64_bit(sketch,width,depth,Arr,Keys,no_ele);
    char q[50];

    int ans = 1;
    do{
        printf("\nEnter the query string : ");
        scanf("%s",q);
        query_element_fastHash_64_bit(sketch,width,depth,Keys,no_ele,q);

        //actual_occurence(Arr,q,no_ele);
        printf("\n");

        printf("Do you want to repeat ? (1/0)");
        scanf("%d", &ans);

    }while(ans == 1);

    for(int i = 0; i < depth; i++){ free(sketch[i]); }
    free(sketch);
    free(Keys);


}

void CMS_fastHash32(char** Arr, int no_ele, int width, int depth){
    uint32_t* Keys = Arr_of_Keys_32bit(depth);
    int** sketch = create_sketch(width,depth);
    compute_sketch_32_bit(sketch,width,depth,Arr,Keys,no_ele);
    char q[50];

    int ans = 1;
    do{
        printf("\nEnter the query string : ");
        scanf("%s",q);
        query_element_fastHash_32_bit(sketch,width,depth,Keys,no_ele,q);

        // actual_occurence(Arr,q,no_ele);
        printf("\n");

        printf("Do you want to repeat ? (1/0)");
        scanf("%d", &ans);

    }while(ans == 1);

    for(int i = 0; i < depth; i++){ free(sketch[i]); }
    free(sketch);
    free(Keys);
}

int main(){

    float ep = 0.01;    //Error factor
    float del = 0.01;   //Probability of getting error

    // int width = (int)(ceil(exp(1)/ep));   
    // int depth = (int)(ceil(log10(1/del))); 

    int width = 100;
    int depth = 10;
    int no_words=20000000;
    
    printf("Initializing Count Min Sketch..... ");
    printf("\nWidth : %d and depth :%d\n\n",width,depth);

    char** Arr = (char**)calloc(no_words,sizeof(char*));
    int count_of_elements = data_from_file("unique_words_500.txt",no_words,Arr);
    
    //count min sketch using FastHash64 bit hash function
    CMS_fastHash64(Arr,count_of_elements,width,depth);

    //free the malloc'd memory
    for(int i = 0; i<no_words; i++){
        free(Arr[i]);
    }
    free(Arr);

}

