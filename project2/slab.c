#include "slab.h"


unsigned char *slab_allocate(){
  unsigned int slab_index = 15;
  unsigned int obj_index = 14;
  unsigned char *addr;

  if(full_mask==0xffff) return NULL;

  // fill in
  short unsigned int slab_mask, obj_mask;

  //partial slab to fill
  if(partial_mask != 0) {
    slab_mask = partial_mask;
  }
  //no partial slab, start new slab
  else {
    slab_mask = empty_mask;
  }

  //find slab_index from most sig bit
  while(slab_mask > 1) {
    slab_mask = slab_mask >> 1;
    slab_index--;
  }
  slab_mask = 1 << (15 - slab_index);

  //find obj_index from most sig bit
  obj_mask = s[slab_index].free_mask;
  while(obj_mask > 1) {
    obj_mask = obj_mask >> 1;
    obj_index--;
  }
  obj_mask = 1 << (14 - obj_index);

  //if slab was empty, update empty_mask and partial_mask
  if(s[slab_index].free_count == 15) {
    empty_mask = empty_mask & (~slab_mask);
    partial_mask = partial_mask | slab_mask;
  }

  //update free_count, free_mask, and calculate addr
  s[slab_index].free_count = s[slab_index].free_count - 1;
  s[slab_index].free_mask = s[slab_index].free_mask & (~obj_mask);
  addr = start + (4096 * slab_index) + (256 * (obj_index + 1));

  //if slab is now full, update partial_mask and full_mask
  if(s[slab_index].free_count == 0) {
    partial_mask = partial_mask & (~slab_mask);
    full_mask = full_mask | slab_mask;
  }

  return(addr);
}

int slab_release( unsigned char *addr ){
  unsigned int sanity_check = ((int)(addr - start))>>16;  /* should be 0 */
  unsigned int slab_index = ((int)(addr - start))>>12;
  unsigned int obj_index = (((int)(addr - start))>>8)&15;

  /* bad address */
  if(sanity_check) return(1);
  if(s[slab_index].signature!=0x51ab51ab) return(1);
  /* slab is already marked as free */
  if(s[slab_index].free_mask & (short)(1<<(15-obj_index))) return(2);
  /* bad free count */
  if(s[slab_index].free_count>=15) printf("srel: high error in slab count!\n");

  s[slab_index].free_mask = s[slab_index].free_mask | (short)(1<<(15-obj_index));
  s[slab_index].free_count++;
  if(s[slab_index].free_count==1){
    /* move from full to partial */
    full_mask = full_mask ^ (short)(1<<(15-slab_index));
    partial_mask = partial_mask | (short)(1<<(15-slab_index));
  }else if(s[slab_index].free_count==15){
    /* move from partial to empty */
    partial_mask = partial_mask ^ (short)(1<<(15-slab_index));
    empty_mask = empty_mask | (short)(1<<(15-slab_index));
  }
  return(0);
}
