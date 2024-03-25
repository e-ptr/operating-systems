#include <stdio.h>

#define NUM_PAGES 10
#define NUM_FRAMES 4

int main(int argc, char **argv) {
    int frames[NUM_FRAMES], pages[NUM_PAGES], use[NUM_FRAMES];
    int counter = 0;
    int faults = 0;
    int addr, hit, frame;
    int framePos;

    //initialize frames and lru
    for(int i = 0; i < NUM_FRAMES; i++) {
        frames[i] = -1;
        use[i] = i;
    }

    //initialize pages
    for(int i = 0; i < NUM_PAGES; i++) {
        pages[i] = -1;
    }

    //print header
    printf("\n");
    printf(" address   page           page table entries          page frames   LRU order\n");
    printf("virt|phys  fault   | 0| 1| 2| 3| 4| 5| 6| 7| 8| 9|   | 0| 1| 2| 3|  LRU...MRU\n");
    printf("---- ----  -----   |--|--|--|--|--|--|--|--|--|--|   |--|--|--|--|  ---------\n");
    printf("                                                                     0,1,2,3\n");

    scanf("%d", &addr);
    while(!feof(stdin)) {
        counter++;
        hit = 0;

        //check for hit
        for(int i = 0; i < NUM_FRAMES; i++) {
            if(frames[i] == addr / 10) {
                hit = 1;
                frame = i;
                break;
            }
        }

        //no hit
        if(!hit) {
            faults++;
            frame = use[0];

            //update page entries to reflect replacement
            if(frames[frame] != -1) {
                pages[frames[frame]] = -1;
            }

            frames[frame] = addr / 10;
            pages[addr / 10] = frame;
        }

        //update use
        //find frame in use
        for(int i = 0; i < NUM_FRAMES; i++) {
            if(use[i] == frame) {
                framePos = i;
                break;
            }
        }

        //shift use
        for(int i = framePos; i < NUM_FRAMES - 1; i++) {
            use[i] = use[i + 1];
        }
        use[NUM_FRAMES - 1] = frame;

        //print line
        //phys virt addr
        printf(" %02d   %d%d   ", addr, frame, addr % 10);
        
        //hit fault
        if (!hit) {
            printf("fault");
        }
        else {
            printf("     ");
        }

        //page table
        printf("    ");
        for (int i = 0; i < NUM_PAGES; i++) {
            if (pages[i] != -1) {
                printf("%d", pages[i]);
                if (addr / 10 == i) {
                    if (hit) {
                        printf("+");
                    }
                    else {
                        printf("*");
                    }
                }
                else {
                    printf(" ");
                }
            }
            else {
                printf("  ");
            }
            printf(" ");
        }

        //frames
        printf("    ");
        for (int i = 0; i < NUM_FRAMES; i++) {
            if (frames[i] != -1) {
                printf("%d", frames[i]);
                if (use[NUM_FRAMES - 1] == i) {
                    if (hit) {
                        printf("+");
                    }
                    else {
                        printf("*");
                    }
                }
                else {
                    printf(" ");
                }
            }
            else {
                printf("  ");
            }
            printf(" ");
        }

        //LRU to MRU
        printf("   %d,%d,%d,%d\n", use[0], use[1], use[2], use[3]);

        scanf("%d", &addr);
    }

    //post stats
    printf("\n");
    printf("trace length = %d addresses, causing %d page faults\n", counter, faults);
    printf("\n");

    return 0;
}