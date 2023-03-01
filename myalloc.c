#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>


#define ALIGNMENT 16
#define GET_PAD(x) ((ALIGNMENT - 1 ) - (((x) - 1) & (ALIGNMENT - 1)))
#define PADDED_SIZE(x) ((x) + GET_PAD(x))
#define PTR_OFFSET(p, offset) ((void*)((char *)(p) + (offset)))


struct block {
    struct block *next;
    int size;
    int in_use;
};

void print_data(void);
void *myalloc(int);
void myfree(void *);

struct block *head;


int main(void){

    head = NULL;
    void *p;

    print_data();

    p = myalloc(500);
    print_data();

    p = myalloc(16);
    printf("%p\n", p);

    p = myalloc(124);
    print_data();

return 0;
} // main


void print_data(void) {

    struct block *b = head;

    if(b == NULL){
        printf("[empty]\n");
        return;
    }

    while(b != NULL){

        // Uncomment the following line if you want to see the pointer values
        //printf("[%p:%d,%s]", b, b -> size, b -> in_use? "used": "free");

        printf("[%d,%s]", b -> size, b -> in_use? "used": "free");
        if(b -> next != NULL){
            printf(" -> ");
        }

	b = b -> next;
    }
    printf("\n");
}

void *myalloc(int size){

    printf("size before padding: %d\n", size);
    size = PADDED_SIZE(size);
    printf("padded size: %d\n", size);

    int padded_block_size = PADDED_SIZE(sizeof(struct block));
    int required_space = size + padded_block_size + 16; 

    if(head == NULL){
        head = mmap(NULL, 1024, PROT_READ | PROT_WRITE,
                    MAP_ANON | MAP_PRIVATE, -1, 0);
        head -> next = NULL;
        head -> size = 1024 - PADDED_SIZE(sizeof(struct block));
        head -> in_use = 0;
    }
    printf("size after 1st call to myalloc: %d\n", head -> size); //1008
    struct block *currNode = head;

    while(currNode != NULL){

        printf("padded size at start of loop: %d\n", size); //512

        if(currNode -> in_use == 0 && PADDED_SIZE(currNode -> size) >= size){
            
            currNode -> in_use = 1;
            currNode -> size = PADDED_SIZE(currNode -> size) - size;

            size -= padded_block_size;
            printf("currNode -> size: %d\n", currNode -> size);
     
            currNode -> next = currNode + (padded_block_size + currNode -> size);
            printf("head -> size: %d\n", head -> size);
          
            currNode -> next -> size = head -> size - currNode -> size;
            printf("size in if loop: %d\n", size);
  
            return PTR_OFFSET(currNode, padded_block_size);
        }
            else if(PADDED_SIZE(currNode -> size) < size && currNode -> size < required_space && currNode -> in_use == 1){

            currNode -> size = size - padded_block_size;
            printf("Split_Space currNode -> size: %d\n", currNode -> size);

            currNode -> in_use = 1;

            size -= currNode -> size;
            printf("size after calling 'size -= currNode -> size': %d\n", size);

            currNode -> next = currNode + (padded_block_size + currNode -> size);
            currNode -> next -> in_use = 0;
          
            return PTR_OFFSET(currNode, padded_block_size);
        }
        printf("exited IF block\n");
        currNode = currNode -> next;
    }            
    return NULL;
}

void myfree(void *p){

    p = (struct block*) (p - PADDED_SIZE(sizeof(struct block)));
    struct block *currNode = p;
    currNode -> in_use = 0;

}
