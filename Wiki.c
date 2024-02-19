#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <openssl/hmac.h>
#include "fasthash.h"
#include "my_header.h"
#include "murmur3.h"
#include <assert.h>

#define BUZZ_SIZE 100000
#define mix(h)                                                                 \
  ({                                                                           \
    (h) ^= (h) >> 23;                                                          \
    (h) *= 0x2127599bf4325c37ULL;                                              \
    (h) ^= (h) >> 47;                                                          \
  })


//Random key generator
uint32_t key_gen(){
    uint32_t key = (uint32_t)rand();
    return key;
}

//Seed generator
uint32_t seed_gen(){
    uint32_t seed = (uint32_t)rand();
    return seed;
}

//HMAC_SHA256
int hash(char *key, char *message, int width) {
    unsigned char digest[EVP_MAX_MD_SIZE];
    unsigned int digest_len;

    // Compute HMAC-SHA256 hash
    HMAC(EVP_sha256(), key, strlen(key), (unsigned char *)message, strlen(message), digest, &digest_len);

    // Convert hash to integer
    unsigned int hashed_value = 0;
    for(int i = 0; i < digest_len && i < 4; i++) { // Assuming int is 4 bytes
        hashed_value = (hashed_value << 8) | digest[i];
    }

    return (int)(hashed_value%width);
}

//FastHash
uint64_t fasthash64(const void *buf, size_t len, uint64_t seed) {
  const uint64_t m = 0x880355f21e6d1965ULL;
  const uint64_t *pos = (const uint64_t *)buf;
  const uint64_t *end = pos + (len / 8);
  const unsigned char *pos2;
  uint64_t h = seed ^ (len * m);
  uint64_t v;

  while (pos != end) {
    v = *pos++;
    h ^= mix(v);
    h *= m;
  }

  pos2 = (const unsigned char *)pos;
  v = 0;

  switch (len & 7) {
  case 7:
    v ^= (uint64_t)pos2[6] << 48; /* fallthrough */
  case 6:
    v ^= (uint64_t)pos2[5] << 40; /* fallthrough */
  case 5:
    v ^= (uint64_t)pos2[4] << 32; /* fallthrough */
  case 4:
    v ^= (uint64_t)pos2[3] << 24; /* fallthrough */
  case 3:
    v ^= (uint64_t)pos2[2] << 16; /* fallthrough */
  case 2:
    v ^= (uint64_t)pos2[1] << 8;  /* fallthrough */
  case 1:
    v ^= (uint64_t)pos2[0];
    h ^= mix(v);
    h *= m;
  }

  return mix(h);
}

uint32_t fasthash32(const void *buf, size_t len, uint32_t seed) {
  // the following trick converts the 64-bit hashcode to Fermat
  // residue, which shall retain information from both the higher
  // and lower parts of hashcode.
  uint64_t h = fasthash64(buf, len, seed);
  return h - (h >> 32);
}


//Murmur hash
#ifdef __GNUC__
#define FORCE_INLINE __attribute__((always_inline)) inline
#else
#define FORCE_INLINE inline
#endif

static FORCE_INLINE uint32_t rotl32 ( uint32_t x, int8_t r )
{
  return (x << r) | (x >> (32 - r));
}

static FORCE_INLINE uint64_t rotl64 ( uint64_t x, int8_t r )
{
  return (x << r) | (x >> (64 - r));
}

#define	ROTL32(x,y)	rotl32(x,y)
#define ROTL64(x,y)	rotl64(x,y)

#define BIG_CONSTANT(x) (x##LLU)

//-----------------------------------------------------------------------------
// Block read - if your platform needs to do endian-swapping or can only
// handle aligned reads, do the conversion here

#define getblock(p, i) (p[i])

//-----------------------------------------------------------------------------
// Finalization mix - force all bits of a hash block to avalanche

static FORCE_INLINE uint32_t fmix32 ( uint32_t h )
{
  h ^= h >> 16;
  h *= 0x85ebca6b;
  h ^= h >> 13;
  h *= 0xc2b2ae35;
  h ^= h >> 16;

  return h;
}

//----------

static FORCE_INLINE uint64_t fmix64 ( uint64_t k )
{
  k ^= k >> 33;
  k *= BIG_CONSTANT(0xff51afd7ed558ccd);
  k ^= k >> 33;
  k *= BIG_CONSTANT(0xc4ceb9fe1a85ec53);
  k ^= k >> 33;

  return k;
}

void MurmurHash3_x86_32 ( const void * key, int len, uint32_t seed, void * out )
{
  const uint8_t * data = (const uint8_t*)key;
  const int nblocks = len / 4;
  int i;

  uint32_t h1 = seed;

  uint32_t c1 = 0xcc9e2d51;
  uint32_t c2 = 0x1b873593;

  //----------
  // body

  const uint32_t * blocks = (const uint32_t *)(data + nblocks*4);

  for(i = -nblocks; i; i++)
  {
    uint32_t k1 = getblock(blocks,i);

    k1 *= c1;
    k1 = ROTL32(k1,15);
    k1 *= c2;
    
    h1 ^= k1;
    h1 = ROTL32(h1,13); 
    h1 = h1*5+0xe6546b64;
  }

  //----------
  // tail

  const uint8_t * tail = (const uint8_t*)(data + nblocks*4);

  uint32_t k1 = 0;

  switch(len & 3)
  {
  case 3: k1 ^= tail[2] << 16;
  case 2: k1 ^= tail[1] << 8;
  case 1: k1 ^= tail[0];
          k1 *= c1; k1 = ROTL32(k1,15); k1 *= c2; h1 ^= k1;
  };

  //----------
  // finalization

  h1 ^= len;

  h1 = fmix32(h1);

  *(uint32_t*)out = h1;
} 

void MurmurHash3_x86_128 ( const void * key, const int len, uint32_t seed, void * out )
{
  const uint8_t * data = (const uint8_t*)key;
  const int nblocks = len / 16;
  int i;

  uint32_t h1 = seed;
  uint32_t h2 = seed;
  uint32_t h3 = seed;
  uint32_t h4 = seed;

  uint32_t c1 = 0x239b961b; 
  uint32_t c2 = 0xab0e9789;
  uint32_t c3 = 0x38b34ae5; 
  uint32_t c4 = 0xa1e38b93;

  //----------
  // body

  const uint32_t * blocks = (const uint32_t *)(data + nblocks*16);

  for(i = -nblocks; i; i++)
  {
    uint32_t k1 = getblock(blocks,i*4+0);
    uint32_t k2 = getblock(blocks,i*4+1);
    uint32_t k3 = getblock(blocks,i*4+2);
    uint32_t k4 = getblock(blocks,i*4+3);

    k1 *= c1; k1  = ROTL32(k1,15); k1 *= c2; h1 ^= k1;

    h1 = ROTL32(h1,19); h1 += h2; h1 = h1*5+0x561ccd1b;

    k2 *= c2; k2  = ROTL32(k2,16); k2 *= c3; h2 ^= k2;

    h2 = ROTL32(h2,17); h2 += h3; h2 = h2*5+0x0bcaa747;

    k3 *= c3; k3  = ROTL32(k3,17); k3 *= c4; h3 ^= k3;

    h3 = ROTL32(h3,15); h3 += h4; h3 = h3*5+0x96cd1c35;

    k4 *= c4; k4  = ROTL32(k4,18); k4 *= c1; h4 ^= k4;

    h4 = ROTL32(h4,13); h4 += h1; h4 = h4*5+0x32ac3b17;
  }

  //----------
  // tail

  const uint8_t * tail = (const uint8_t*)(data + nblocks*16);

  uint32_t k1 = 0;
  uint32_t k2 = 0;
  uint32_t k3 = 0;
  uint32_t k4 = 0;

  switch(len & 15)
  {
  case 15: k4 ^= tail[14] << 16;
  case 14: k4 ^= tail[13] << 8;
  case 13: k4 ^= tail[12] << 0;
           k4 *= c4; k4  = ROTL32(k4,18); k4 *= c1; h4 ^= k4;

  case 12: k3 ^= tail[11] << 24;
  case 11: k3 ^= tail[10] << 16;
  case 10: k3 ^= tail[ 9] << 8;
  case  9: k3 ^= tail[ 8] << 0;
           k3 *= c3; k3  = ROTL32(k3,17); k3 *= c4; h3 ^= k3;

  case  8: k2 ^= tail[ 7] << 24;
  case  7: k2 ^= tail[ 6] << 16;
  case  6: k2 ^= tail[ 5] << 8;
  case  5: k2 ^= tail[ 4] << 0;
           k2 *= c2; k2  = ROTL32(k2,16); k2 *= c3; h2 ^= k2;

  case  4: k1 ^= tail[ 3] << 24;
  case  3: k1 ^= tail[ 2] << 16;
  case  2: k1 ^= tail[ 1] << 8;
  case  1: k1 ^= tail[ 0] << 0;
           k1 *= c1; k1  = ROTL32(k1,15); k1 *= c2; h1 ^= k1;
  };

  //----------
  // finalization

  h1 ^= len; h2 ^= len; h3 ^= len; h4 ^= len;

  h1 += h2; h1 += h3; h1 += h4;
  h2 += h1; h3 += h1; h4 += h1;

  h1 = fmix32(h1);
  h2 = fmix32(h2);
  h3 = fmix32(h3);
  h4 = fmix32(h4);

  h1 += h2; h1 += h3; h1 += h4;
  h2 += h1; h3 += h1; h4 += h1;

  ((uint32_t*)out)[0] = h1;
  ((uint32_t*)out)[1] = h2;
  ((uint32_t*)out)[2] = h3;
  ((uint32_t*)out)[3] = h4;
}

void MurmurHash3_x64_128 ( const void * key, const int len, const uint32_t seed, void * out )
{
  const uint8_t * data = (const uint8_t*)key;
  const int nblocks = len / 16;
  int i;

  uint64_t h1 = seed;
  uint64_t h2 = seed;

  uint64_t c1 = BIG_CONSTANT(0x87c37b91114253d5);
  uint64_t c2 = BIG_CONSTANT(0x4cf5ad432745937f);

  //----------
  // body

  const uint64_t * blocks = (const uint64_t *)(data);

  for(i = 0; i < nblocks; i++)
  {
    uint64_t k1 = getblock(blocks,i*2+0);
    uint64_t k2 = getblock(blocks,i*2+1);

    k1 *= c1; k1  = ROTL64(k1,31); k1 *= c2; h1 ^= k1;

    h1 = ROTL64(h1,27); h1 += h2; h1 = h1*5+0x52dce729;

    k2 *= c2; k2  = ROTL64(k2,33); k2 *= c1; h2 ^= k2;

    h2 = ROTL64(h2,31); h2 += h1; h2 = h2*5+0x38495ab5;
  }

  //----------
  // tail

  const uint8_t * tail = (const uint8_t*)(data + nblocks*16);

  uint64_t k1 = 0;
  uint64_t k2 = 0;

  switch(len & 15)
  {
  case 15: k2 ^= (uint64_t)(tail[14]) << 48;
  case 14: k2 ^= (uint64_t)(tail[13]) << 40;
  case 13: k2 ^= (uint64_t)(tail[12]) << 32;
  case 12: k2 ^= (uint64_t)(tail[11]) << 24;
  case 11: k2 ^= (uint64_t)(tail[10]) << 16;
  case 10: k2 ^= (uint64_t)(tail[ 9]) << 8;
  case  9: k2 ^= (uint64_t)(tail[ 8]) << 0;
           k2 *= c2; k2  = ROTL64(k2,33); k2 *= c1; h2 ^= k2;

  case  8: k1 ^= (uint64_t)(tail[ 7]) << 56;
  case  7: k1 ^= (uint64_t)(tail[ 6]) << 48;
  case  6: k1 ^= (uint64_t)(tail[ 5]) << 40;
  case  5: k1 ^= (uint64_t)(tail[ 4]) << 32;
  case  4: k1 ^= (uint64_t)(tail[ 3]) << 24;
  case  3: k1 ^= (uint64_t)(tail[ 2]) << 16;
  case  2: k1 ^= (uint64_t)(tail[ 1]) << 8;
  case  1: k1 ^= (uint64_t)(tail[ 0]) << 0;
           k1 *= c1; k1  = ROTL64(k1,31); k1 *= c2; h1 ^= k1;
  };

  //----------
  // finalization

  h1 ^= len; h2 ^= len;

  h1 += h2;
  h2 += h1;

  h1 = fmix64(h1);
  h2 = fmix64(h2);

  h1 += h2;
  h2 += h1;

  ((uint64_t*)out)[0] = h1;
  ((uint64_t*)out)[1] = h2;
}

int** create_sketch(int width, int depth){
    int** sketch = (int**)calloc(depth,sizeof(int*));
    for(int i=0 ; i < depth; i++){

        sketch[i] = (int*)calloc(width,sizeof(int));
    }
    return sketch;
}

//Compute sketches
void compute_sketch(int** sketch, int width, int depth, char** Arr, uint32_t* Keys, int no_ele){
    
    for(int i = 0; i < no_ele; i++){
        
        for(int j = 0; j < depth; j++){
            //int h = hash(Keys[j],Arr[i],width);                                     //ordinary
            //int h = fasthash32(Arr[i],strlen(Arr[i]), Keys[j]) % width;           //fasthash
            int h;
            MurmurHash3_x86_32(Arr[i], strlen(Arr[i]),Keys[j], &h);  
            h = abs(h%width);                                                               //murmur3
            sketch[j][h] += 1;
        }
    }
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

void query_element(int** sketch, int width, int depth,uint32_t* Keys, int no_ele, char* q){
    int min = no_ele;
    for(int iter = 0; iter < depth; iter++){
        //int h = hash(Keys[iter],q,width);                           //ordinary
        //int h = fasthash32(q,strlen(q), Keys[iter]) % width;      //fasthash
        int h;
        MurmurHash3_x86_32(q,strlen(q), Keys[iter], &h);             //murmur3
        h = abs(h%width); 
        if(min > sketch[iter][h]){
            min = sketch[iter][h];
        }
    }

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

void normal_CMS(char** Arr1, int no_ele,int width, int depth, uint32_t* Keys){

    int** sketch_1 = create_sketch(width,depth);

    compute_sketch(sketch_1,width,depth,Arr1,Keys,no_ele);
    char* q = (char*)calloc(50,sizeof(char));
    printf("\nEnter the query string : ");
    
    scanf("%s",q);

    query_element(sketch_1,width,depth,Keys,no_ele,q);

    printf("\nIn Array 1 ");
    actual_occurence(Arr1,q,no_ele);
    printf("\n");

    // for(int depth_iter = 0; depth_iter < depth; depth_iter++){
    //   for(int width_iter = 0; width_iter < width;width_iter++){
    //       printf("%d ",sketch_1[depth_iter][width_iter]);
    //   }
    //   printf("\n");
    // }


    for(int i = 0; i < depth; i++){
        free(sketch_1[i]);
    }
    free(sketch_1);
    free(q);
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

int main(){

    static int mySeed = 25011984;
    srand(time(NULL) * 8 + ++mySeed);

    //--------INPUT PARAMETERS FOR SKETCH----------------
    //ep : Epsilon
    //del : Delta

    float ep = 0.01; //Error factor
    float del = 0.01; //Probability of getting error

    // int width = (int)(ceil(exp(1)/ep));   //28
    // int depth = (int)(ceil(log10(1/del)));  //3

    int width = 1500;
    int depth = 12;

    int no_words=2000000;
    int max_line_length = 40;

    printf("\nWidth : %d and depth :%d\n\n",width,depth);

    //--------------ARRAY OF KEYS----------------------

    uint32_t* Keys = (uint32_t*)calloc(depth, sizeof(uint32_t));
    for( int i = 0; i < depth; i++){
        Keys[i] = key_gen();
    }
    // char** Keys = (char**)calloc(depth, sizeof(char*));
    // for(int i = 0; i < depth; i++){
    //     Keys[i] = randstring(16);
    // }

    printf("\nThe keys are : \n");
    for(int i = 0; i < depth; i++){
        printf("%x\n",Keys[i]);
    }

    char** Arr = (char**)calloc(no_words,sizeof(char*));
    char* line = (char*)malloc(max_line_length * sizeof(char));


    // -----------GET INPUT FROM FILE------------------

    char buff[BUZZ_SIZE];
    FILE *f = fopen("final.txt", "r");
        int iter=0;
        while(fgets(line, max_line_length, f)){
            Arr[iter] = (char*)malloc(max_line_length* sizeof(line));
            strcpy(Arr[iter],line);
            iter++;
        }
    fclose(f);

    //------------------- COMPUTE CMS ----------------------

    normal_CMS(Arr,iter,width,depth, Keys); // --> NORMAL CMS

    //----------------- FREE MEMORY ------------------

    
    for(int i = 0; i<no_words; i++){
        free(Arr[i]);
    }
    free(Arr);
    free(Keys);

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

