#include <stdio.h>
#include <stdlib.h>

#define LONG_PRIME 4294967311l
#define HASH_LITTLE_ENDIAN 1
#define BUZZ_SIZE 100000

#define rot(x,k) (((x)<<(k)) | ((x)>>(32-(k))))

#define mix(a,b,c) \
{ \
  a -= c;  a ^= rot(c, 4);  c += b; \
  b -= a;  b ^= rot(a, 6);  a += c; \
  c -= b;  c ^= rot(b, 8);  b += a; \
  a -= c;  a ^= rot(c,16);  c += b; \
  b -= a;  b ^= rot(a,19);  a += c; \
  c -= b;  c ^= rot(b, 4);  b += a; \
}

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

char* randstring(size_t length);                    //key generator

int** create_sketch(int width, int depth);          //Initialize sketch

int compare(char* s1, char* s2);                    //comparing 2 strings

int** mergeSketch(int** sketch_1, int** aketch_2, int width, int depth);        //Merging 2 sketches

void genaj(int** hashes, int ind);

int hashstr(char* str);

uint32_t hashlittle( const void *key, size_t length, uint32_t initval);

#ifdef __GNUC__
#define FORCE_INLINE __attribute__((always_inline)) inline
#else
#define FORCE_INLINE inline
#endif

// #ifndef VALGRIND
//     switch(length)
//     {
//     case 12: c+=k[2]; b+=k[1]; a+=k[0]; break;
//     case 11: c+=k[2]&0xffffff; b+=k[1]; a+=k[0]; break;
//     case 10: c+=k[2]&0xffff; b+=k[1]; a+=k[0]; break;
//     case 9 : c+=k[2]&0xff; b+=k[1]; a+=k[0]; break;
//     case 8 : b+=k[1]; a+=k[0]; break;
//     case 7 : b+=k[1]&0xffffff; a+=k[0]; break;
//     case 6 : b+=k[1]&0xffff; a+=k[0]; break;
//     case 5 : b+=k[1]&0xff; a+=k[0]; break;
//     case 4 : a+=k[0]; break;
//     case 3 : a+=k[0]&0xffffff; break;
//     case 2 : a+=k[0]&0xffff; break;
//     case 1 : a+=k[0]&0xff; break;
//     case 0 : return c;              /* zero length strings require no mixing */
//     }
// #else /* make valgrind happy */
//     const uint8_t  *k8;
//     k8 = (const uint8_t *)k;
//     switch(length)
//     {
//     case 12: c+=k[2]; b+=k[1]; a+=k[0]; break;
//     case 11: c+=((uint32_t)k8[10])<<16;  /* fall through */
//     case 10: c+=((uint32_t)k8[9])<<8;    /* fall through */
//     case 9 : c+=k8[8];                   /* fall through */
//     case 8 : b+=k[1]; a+=k[0]; break;
//     case 7 : b+=((uint32_t)k8[6])<<16;   /* fall through */
//     case 6 : b+=((uint32_t)k8[5])<<8;    /* fall through */
//     case 5 : b+=k8[4];                   /* fall through */
//     case 4 : a+=k[0]; break;
//     case 3 : a+=((uint32_t)k8[2])<<16;   /* fall through */
//     case 2 : a+=((uint32_t)k8[1])<<8;    /* fall through */
//     case 1 : a+=k8[0]; break;
//     case 0 : return c;
//     }
// #endif /* !valgrind */
//   } else if (HASH_LITTLE_ENDIAN && ((u.i & 0x1) == 0)) {
//     const uint16_t *k = (const uint16_t *)key;         /* read 16-bit chunks */
//     const uint8_t  *k8;
//     /*--------------- all but last block: aligned reads and different mixing */
//     while (length > 12)
//     {
//       a += k[0] + (((uint32_t)k[1])<<16);
//       b += k[2] + (((uint32_t)k[3])<<16);
//       c += k[4] + (((uint32_t)k[5])<<16);
//       mix(a,b,c);
//       length -= 12;
//       k += 6;
//     }
//     /*----------------------------- handle the last (probably partial) block */
//     k8 = (const uint8_t *)k;
//     switch(length)
//     {
//     case 12: c+=k[4]+(((uint32_t)k[5])<<16);
//              b+=k[2]+(((uint32_t)k[3])<<16);
//              a+=k[0]+(((uint32_t)k[1])<<16);
//              break;
//     case 11: c+=((uint32_t)k8[10])<<16;     /* fall through */
//     case 10: c+=k[4];
//              b+=k[2]+(((uint32_t)k[3])<<16);
//              a+=k[0]+(((uint32_t)k[1])<<16);
//              break;
//     case 9 : c+=k8[8];                      /* fall through */
//     case 8 : b+=k[2]+(((uint32_t)k[3])<<16);
//              a+=k[0]+(((uint32_t)k[1])<<16);
//              break;
//     case 7 : b+=((uint32_t)k8[6])<<16;      /* fall through */
//     case 6 : b+=k[2];
//              a+=k[0]+(((uint32_t)k[1])<<16);
//              break;
//     case 5 : b+=k8[4];                      /* fall through */
//     case 4 : a+=k[0]+(((uint32_t)k[1])<<16);
//              break;
//     case 3 : a+=((uint32_t)k8[2])<<16;      /* fall through */
//     case 2 : a+=k[0];
//              break;
//     case 1 : a+=k8[0];
//              break;
//     case 0 : return c;                     /* zero length requires no mixing */
//     }
//   } else {                        /* need to read the key one byte at a time */
//     const uint8_t *k = (const uint8_t *)key;
//     /*--------------- all but the last block: affect some 32 bits of (a,b,c) */
//     while (length > 12)
//     {
//       a += k[0];
//       a += ((uint32_t)k[1])<<8;
//       a += ((uint32_t)k[2])<<16;
//       a += ((uint32_t)k[3])<<24;
//       b += k[4];
//       b += ((uint32_t)k[5])<<8;
//       b += ((uint32_t)k[6])<<16;
//       b += ((uint32_t)k[7])<<24;
//       c += k[8];
//       c += ((uint32_t)k[9])<<8;
//       c += ((uint32_t)k[10])<<16;
//       c += ((uint32_t)k[11])<<24;
//       mix(a,b,c);
//       length -= 12;
//       k += 12;
//     }
//     /*-------------------------------- last block: affect all 32 bits of (c) */
//     switch(length)                   /* all the case statements fall through */
//     {
//     case 12: c+=((uint32_t)k[11])<<24;
//     case 11: c+=((uint32_t)k[10])<<16;
//     case 10: c+=((uint32_t)k[9])<<8;
//     case 9 : c+=k[8];
//     case 8 : b+=((uint32_t)k[7])<<24;
//     case 7 : b+=((uint32_t)k[6])<<16;
//     case 6 : b+=((uint32_t)k[5])<<8;
//     case 5 : b+=k[4];
//     case 4 : a+=((uint32_t)k[3])<<24;
//     case 3 : a+=((uint32_t)k[2])<<16;
//     case 2 : a+=((uint32_t)k[1])<<8;
//     case 1 : a+=k[0];
//              break;
//     case 0 : return c;
//     }
//   }


// #define mix(h)                                                                 \
//   ({                                                                           \
//     (h) ^= (h) >> 23;                                                          \
//     (h) *= 0x2127599bf4325c37ULL;                                              \
//     (h) ^= (h) >> 47;                                                          \
//   })


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
