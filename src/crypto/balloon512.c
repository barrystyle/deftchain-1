#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "sha3/sph_shavite.h"
#include "sha3/sph_luffa.h"
#include "sha3/sph_jh.h"
#include "sha3/sph_echo.h"
#include "sha3/sph_fugue.h"

union block_t {
  uint32_t data[16];
};

// parameters
bool debug = false;
uint32_t target = 0x8fffffff;

void thrash(void* input, void* output, int length, int hashfunc, int8_t* modifier)
{
   int round;
   uint32_t hash[48];
   sph_jh512_context        ctx_jh;
   sph_luffa512_context     ctx_luffa;
   sph_shavite512_context   ctx_shavite;
   sph_echo512_context      ctx_echo;
   sph_fugue512_context     ctx_fugue;

   round++;
   memcpy(hash, input, length);

   // init outside of target loop
   sph_jh512_init(&ctx_jh);
   sph_luffa512_init(&ctx_luffa);
   sph_shavite512_init(&ctx_shavite);
   sph_echo512_init(&ctx_echo);
   sph_fugue512_init(&ctx_fugue);

   // hash observing length once for blockheader length
   if (hashfunc == 0) { sph_jh512(&ctx_jh, hash, length); sph_jh512_close(&ctx_jh, hash); }
   if (hashfunc == 1) { sph_luffa512(&ctx_luffa, hash, length); sph_luffa512_close(&ctx_luffa, hash); }
   if (hashfunc == 2) { sph_shavite512(&ctx_shavite, hash, length); sph_shavite512_close(&ctx_shavite, hash); }
   if (hashfunc == 3) { sph_echo512(&ctx_echo, hash, length); sph_echo512_close(&ctx_echo, hash); }
   if (hashfunc == 4) { sph_fugue512(&ctx_fugue, hash, length); sph_fugue512_close(&ctx_fugue, hash); }

   // hash until lower than target
   do {
     round++;
     if (hashfunc == 0) { sph_jh512(&ctx_jh, hash, 64); sph_jh512_close(&ctx_jh, hash); }
     if (hashfunc == 1) { sph_luffa512(&ctx_luffa, hash, 64); sph_luffa512_close(&ctx_luffa, hash); }
     if (hashfunc == 2) { sph_shavite512(&ctx_shavite, hash, 64); sph_shavite512_close(&ctx_shavite, hash); }
     if (hashfunc == 3) { sph_echo512(&ctx_echo, hash, 64); sph_echo512_close(&ctx_echo, hash); }
     if (hashfunc == 4) { sph_fugue512(&ctx_fugue, hash, 64); sph_fugue512_close(&ctx_fugue, hash); }
   } while (target < hash[0]);

   if (debug)
      printf("%d ", round);

   *modifier = *(uint8_t*)&hash;
   memcpy(output, hash, 64);
}

void balloon512(void* input, void* output, int s_cost, int t_cost, int neighbor, int hashfunc)
{
   int8_t modifier;
   bool alternate = false;
   uint32_t hash[32];
   uint32_t header[48];
   union block_t scratchpad[2];
   union block_t buffer[s_cost+1];

   // keep things separate
   memcpy(header,input,180);

   if (debug) {
      printf("buffer is %d bytes\n", sizeof(buffer));
      printf("block_t is %d bytes\n", sizeof(union block_t));
      printf("s_cost=%d, t_cost=%d, neighbor=%d\n", s_cost, t_cost, neighbor);
      printf("buffer expand rounds=");
   }

   // balloon expand
   thrash(header, hash, 180, hashfunc, &modifier);
   memcpy(&buffer[0], hash, 64);
   for (int expand = 0; expand < s_cost; expand++)
     thrash(&buffer[expand], &buffer[expand+1], 64, hashfunc, &modifier);

   if (debug) {
      printf("\n");
      printf("buffer mix rounds=");
   }

   // mix buffer content
   int z_cost = s_cost / (255-abs(modifier));
   int upper_bounds = ((1+t_cost)*(1+z_cost)*t_cost*z_cost*(1+neighbor))/(z_cost+t_cost);
   for (int mixbufa = 1; mixbufa < t_cost; mixbufa++) {
     for (int mixbufb = 1; mixbufb < z_cost; mixbufb++) {

       if (debug)
           printf("(%d+%d->%d) ", mixbufb-1, mixbufb, mixbufb);

       // hash prev and current blocks (into current)
       memcpy(&scratchpad[0],&buffer[mixbufb-1],64);                                      // prevblk
       memcpy(&scratchpad[1],&buffer[s_cost-mixbufb],64);                                 // currentblk
       thrash(&scratchpad[0],&buffer[s_cost-mixbufb],128,hashfunc,&modifier);             // hash into currentblk

       // hash in pseudorandomly chosen blocks
       for (int random = 0; random < neighbor; random++) {
         int block_select = abs((((1+mixbufa)*(1+mixbufb)*t_cost*s_cost*(1+random))/upper_bounds)%s_cost);

         if (debug) {
           if (block_select > s_cost)
           {
               printf("balloon error\n");
           } else {
               if (alternate == true)
                   printf("[%d+%d->%d] ", s_cost-block_select, mixbufb-1, mixbufb-1);
               else
                   printf("[%d+%d->%d] ", block_select, mixbufb-1, mixbufb-1);
           }
         }

         // hash current and block_select blocks together
         if (alternate == true)
             memcpy(&scratchpad[0],&buffer[s_cost-block_select],64);                      // block_select 0
         else
             memcpy(&scratchpad[0],&buffer[block_select],64);                             // block_select 1

         memcpy(&scratchpad[1],&buffer[mixbufb-1],64);                                    // currentblk

         thrash(&scratchpad[0],&buffer[mixbufb-1],128,hashfunc,&modifier);                // hash into currentblk
         alternate = !alternate;
       }

       if (debug) printf("\n");

     }
   }

   // extract output from buffer
   memcpy(output,&buffer[s_cost-1],32);
}
