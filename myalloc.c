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

struct block *head;


int main(void){

    head = NULL;
    void *p;

    print_data();

    p = myalloc(16);
    print_data();

    p = myalloc(16);
    printf("%p\n", p);

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

    size = PADDED_SIZE(size);

    if(head == NULL){
        head = mmap(NULL, 1024, PROT_READ | PROT_WRITE,
                    MAP_ANON | MAP_PRIVATE, -1, 0);
        head -> next = NULL;
        head -> size = 1024 - PADDED_SIZE(sizeof(struct block));
        head -> in_use = 0;
    }

    struct block *currNode = head;

    while(currNode != NULL){

        if(currNode -> in_use == 0 && PADDED_SIZE(currNode -> size) >= size){

            int padded_block_size = PADDED_SIZE(sizeof(struct block));
                       
            currNode -> in_use = 1;
            currNode -> size = PADDED_SIZE(currNode -> size);     
            currNode -> next = currNode + (padded_block_size + currNode -> size);          
            
            return PTR_OFFSET(currNode, padded_block_size);
        }
        currNode = currNode -> next;
    }            
    return NULL;
}
