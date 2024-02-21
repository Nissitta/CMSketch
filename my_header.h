#include <stdio.h>
#include <stdlib.h>

//Definitions

#define LONG_PRIME 4294967311l
#define HASH_LITTLE_ENDIAN 1
#define BUZZ_SIZE 100000
#define SHIFT_AMOUNT 51

//mix method for FastHash
#define mix_fh(h)                                                                 \
  ({                                                                           \
    (h) ^= (h) >> 23;                                                          \
    (h) *= 0x2127599bf4325c37ULL;                                              \
    (h) ^= (h) >> 47;                                                          \
  })

//rot method (FastHash)
#define rot(x,k) (((x)<<(k)) | ((x)>>(32-(k))))

//mix method with 3 parameters (for HashLittle)
#define mix(a,b,c) \
{ \
  a -= c;  a ^= rot(c, 4);  c += b; \
  b -= a;  b ^= rot(a, 6);  a += c; \
  c -= b;  c ^= rot(b, 8);  b += a; \
  a -= c;  a ^= rot(c,16);  c += b; \
  b -= a;  b ^= rot(a,19);  a += c; \
  c -= b;  c ^= rot(b, 4);  b += a; \
}

//definition for final method with 3 parameters (for HashLittle hf)
#define final(a,b,c) \
{ \
  c ^= b; c -= rot(b,14); \
  a ^= c; a -= rot(c,11); \
  b ^= a; b -= rot(a,25); \
  c ^= b; c -= rot(b,16); \
  a ^= c; a -= rot(c,4);  \
  b ^= a; b -= rot(a,14); \
  c ^= b; c -= rot(b,24); \
}

//Print contents of sketch
void print_sketch(int** sketch,int depth, int width);

//Read the data from mentioned file and store the elements in Array at ptr_Arr and return the count of elements in the Arr
int data_from_file(char* filename,int no_words,char** ptr_Arr);

//32 bit key generator (can be used for setting seed also)
uint32_t key_gen_32_bit();

//64 bit key generator
uint64_t key_gen_64_bit();

//Returns an array of 32bit keys
uint32_t* Arr_of_Keys_32bit(int no_of_keys);

//Returns an array of 64bit keys
uint64_t* Arr_of_Keys_64bit(int no_of_keys);

//Returns an array of random strings to be used as keys
char** Arr_of_random_string(size_t length, int no_of_keys);

//hmac with sha256
int HMAC_SHA256(char* key, char *message, int width);

//Fasthash for 64 bits
uint64_t fasthash64(const void *buf, size_t len, uint64_t seed) ;

//FastHash for 32 bits
uint32_t fasthash32(const void *buf, size_t len, uint32_t seed);

//Murmur Hash for 32 bits
uint32_t MurmurHash3_x86_32 ( const void * key, int len, uint32_t seed) ;

//Sub function of murmurhash
uint32_t getblock32 ( const uint32_t * p, int i );

//Sub function of murmurhash
uint32_t rotl32 ( uint32_t x, int8_t r ) ;

//Sub function of murmurhash
uint32_t fmix32 ( uint32_t h );

//FNV Hash Algorithm
uint64_t fnv_1a(char* element, int seed);

//Malloc create a new sketch
int** create_sketch(int width, int depth);

//Compute the values to be set inside sketch(64 bit hash val)
void compute_sketch_64_bit(int** sketch, int width, int depth, char** Arr, uint64_t* Keys, int no_ele);

//Compute the values to be set inside sketch(32 bit hash val)
void compute_sketch_32_bit(int** sketch, int width, int depth, char** Arr, uint32_t* Keys, int no_ele);

//Random string generator for key generation
char* randstring(size_t length);  

//Compares 2 strings
int compare(char* s1, char* s2);     

//Query any element's occurence from sketch FastHash 64 bit
void query_element_fastHash_64_bit(int** sketch, int width, int depth,uint64_t* Keys, int no_ele, char* q);

//Query any element's occurence from sketch FastHash 32 bit
void query_element_fastHash_32_bit(int** sketch, int width, int depth,uint32_t* Keys, int no_ele, char* q);

//Print the actual times of occurence of an element in a sketch
void actual_occurence(char** Arr, char* q, int no_ele);

//Merge 2 sketches
int** mergeSketch(int** sketch_1, int** aketch_2, int width, int depth); 

//Hash function for C++ implementation (Change sketch structure before using this)
void genaj(int** hashes, int ind);

//Just a not-so-special normal Hash function
int hashstr(char* str);

//Count Min Sketch Simulation
void CMS(char** Arr1, int no_ele,int width, int depth);

//Hashlittle hash function returning 32 bit hash value
uint32_t hashlittle( const void *key, size_t length, uint32_t initval);

//Scale down 32 bit hash to integer og range min_num to max_num
int number_from_32_bit_hash(uint32_t hash_value, int bucket_size);

//Scale down 64 bit hash to integer of range min_num to max_num
int number_from_64_bit_hash(uint64_t hash_value, int bucket_size);


//CMS using FastHash 64 bit
void CMS_fastHash64(char** Arr, int no_ele, int width, int depth);

//CMS using FastHash 32 bit
void CMS_fastHash32(char** Arr, int no_ele, int width, int depth);
