/*Designed and programmed by - Cem Gulec - 150117828
 * TODO: aynı node'u eklememe durumunu kontrol etmek gerek
 *
 * TODO: arayüz en son halinde tablolaştıracak şekilde print edilebilir
 * TODO: fonksiyon prototip ve alttaki yerlerdeki sıralamalar daha akıllıca olabilir (örneğin hesaplamalar alt alta)
 * TODO: fonksiyon isimleri, değişkenleri ve printleri falan değiştir
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <malloc.h>

#define MAX_LIMIT 50

struct node {
    int process;
    double priority;
    int firstTimeFlag;
    int n;
    int degree;
    struct node* parent;
    struct node* child;
    struct node* sibling;
};
typedef struct node node;

node * H = NULL;
node *Hr = NULL;
int process[MAX_LIMIT] = {0}, e[MAX_LIMIT], t_arrive[MAX_LIMIT], WaitTime[MAX_LIMIT] = {0}, firsTime[MAX_LIMIT] = {0};
int e_max, max_index, general_time = 0;
double least_priVal = 50;
int leastPriVal[MAX_LIMIT] = {0}, isSamePriCnt = 0, numberOfNodes;

//function prototypes
void increaseWT_SG(int);
void increaseWT_ALL(int);
int *changeInputProcess(int *, int, int);
int *deleteInputProcess(int *, int);
int isInputEmpty(int *);
int isSamePriVal(node *);
void calculatePriorityNodes(node *);
void printHeap(node *);
void Processor(int);
double calculatePriVal(double, int);
int whichHasTheLeastPri1(node *);
int *whichHasTheLeastPri(int *, node *);
double calculateCei(int);
void readFile();
node* MAKE_bin_HEAP();
void bin_LINK(node *, node *);
node* CREATE_NODE(int, int, int);
node* bin_HEAP_UNION(node *, node *);
node* bin_HEAP_INSERT(node *, node *);
node* bin_HEAP_MERGE(node *, node *);
node* bin_HEAP_EXTRACT_MIN(node *);
int REVERT_LIST(node *);
int DISPLAY(node *);
node* FIND_NODE(node *, int);
int bin_HEAP_DECREASE_KEY(node *, int, int);
int bin_HEAP_DELETE(node *, int);


int main() {

    readFile();

    Processor(1);

    DISPLAY(H);

}

void displayWT(){
    for(int i = 0; i<max_index; i++)
        printf("[%d]",WaitTime[i]);
}

void increaseWT_SG(int pr){
    for(int i = 0; i<max_index; i++){
        if(i == pr-1)
            WaitTime[i]++;
    }
}

void increaseWT_ALL(int k){
    for(int i = 0; i<max_index; i++){
        if(i != k-1)
            WaitTime[i]++;
    }
}

int *changeInputProcess(int *h, int process_num, int key) {

    for(int i = 0; i<MAX_LIMIT; i++)
        if(h[i] == process_num){
            h[i] = key;
            break;
        }

    return h;
}

int *deleteInputProcess(int *h, int process_num){

    for(int i = 0; i < MAX_LIMIT; i++){
        if(h[i] == process_num-1)
            h[i] = 0;
    }

    return h;
}

int isInputEmpty(int *h){
    for(int i = 0; i<MAX_LIMIT; i++)
        if(h[i] != 0){
            return 0;
        }

    return 1;
}

int isSamePriVal(node *head){

    while(head){
        if(head->priority == least_priVal)
            isSamePriCnt++;
        isSamePriVal(head->child);
        head = head->sibling;
    }

    //if all the nodes have the same priority value
    //and it is equal to the least_pri then we'll look for t_arrive
    if(isSamePriCnt == numberOfNodes)
        return 1;
    else
        return 0;
}

void calculatePriorityNodes(node *head){
    double c_ei, priorityVal;
    while(head){
        //if process enters for the first time
        if(head->firstTimeFlag == 0)
            c_ei = 1;
        //for further insertions
        else
            c_ei = calculateCei(e[head->process - 1]);

        double rounded = calculatePriVal(c_ei, e[head->process - 1]);
        rounded = (int)(rounded * 1000.0)/1000.0;
        priorityVal = rounded;
        head->priority = priorityVal;
        if(priorityVal < least_priVal)
            least_priVal = priorityVal;

        calculatePriorityNodes(head->child);
        head = head->sibling;
    }
}

void printHeap(node *head){
    while(head){
        printf("n: %d p: %.3lf P%d | ", head->n, head->priority, head->process);
        printHeap(head->child);
        head = head->sibling;
    }
}

void Processor(int quantum_time){

    node *np;

    //after reading file add all the processes
    //while there exist processes in the input list
    //while (!isInputEmpty(process))
    //{
    for(int j=0; j<6; j++){
        int num_nodes = 0;
        //check for the input arrive time, t_arr <= general_time
        //means that element can enter the system from BH
        for(int i = 0; i < max_index; i++){
            if(t_arrive[i] <= general_time && e[i] != 0){
                np = CREATE_NODE(e[i] ,process[i], firsTime[i]);
                H = bin_HEAP_INSERT(H, np);
                num_nodes++;
            }
        }
        numberOfNodes = num_nodes;
        //calculate priority values for all the elements in BH
        calculatePriorityNodes(H);
        printf("\nGeneral time: %d", general_time);
        printf("\nLeast pri val: %.3lf", least_priVal);
        printf("\nmax index: %d", max_index);
        printf("\nNumber of nodes: %d",numberOfNodes);
        printf("\nBH nodes' priorities are same?: %d",isSamePriVal(H));
        printf("\nWhich has the least pri val: P%d\n",whichHasTheLeastPri1(H));
        printf("-------------------------------\n");
    printf("//  "); printHeap(H); printf("  //");
    printf("\n//  "); displayWT(); printf("  //");
        int least_process_index = whichHasTheLeastPri1(H)-1;
        bin_HEAP_DELETE(H, whichHasTheLeastPri1(H));
    printf("\n//  "); printHeap(H); printf("  //");

        //if the process is not completed
        if( e[least_process_index] > quantum_time ){
            e[least_process_index] -= quantum_time;
            general_time += quantum_time;
            firsTime[least_process_index] = 1;
        }

        //if completed but it may be shorter than quantum_time
        //so that it can shift the time domain
        else if ( e[least_process_index] <= quantum_time ){
            if(e[least_process_index < quantum_time])
                increaseWT_ALL(least_process_index);
            general_time += e[least_process_index];
            deleteInputProcess(process, least_process_index);
            e[least_process_index] = 0;
        }

        printf("\n//  "); displayWT(); printf("  //\n\n\n");
        //diğer işleme geçmeden önce node sayısını resetle
        numberOfNodes = 0;
        least_priVal = 50;
        }
    //}
}

double calculatePriVal(double c_ei, int e_i){
    return c_ei * e_i;
}

int whichHasTheLeastPri1(node *head){

    while(head){
        printf("\n[[%lf %lf]]",head->priority, least_priVal);
        if(head->priority == least_priVal){
            return (head->process);
        }
        int recursive_return = whichHasTheLeastPri1(head->child);
        if(recursive_return != -1) return recursive_return;
        head = head->sibling;
    }

    return -1;
}

int *whichHasTheLeastPri(int *h, node *head){

    while(head){
        if(head->priority == least_priVal){
            return h;
        }
        whichHasTheLeastPri(h, head->child);
        head = head->sibling;
    }

    return h;
}

double calculateCei(int e_i){
    double val1 = 2 * e_i;
    double val2 = 3 * e_max;
    double temp = pow(val1/val2, 3);
    double result = 1 / ( exp(-temp) );

    return result;
}

void readFile(){
    char data[150][4];
    char counter = 0;
    FILE *ptr = fopen("C:/Users/cemgg/Documents/GitHub/Preemptive-Scheduling-System-with-BH/Project3 2019/deneme.txt","r");
    if (ptr){
        while(!feof(ptr)){ //inserting the text document
            fscanf(ptr,"%s",data[counter]);
            counter++;
        }
    }
    fclose(ptr);

    //seperate input file into process, e and t_arr values
    int process_index = 0, e_index = 0, t_arrive_index = 0;
    for(int i=0; i<counter; i++){
        if(i % 3 == 0){
            process[process_index] = atoi(&data[i][1]);
            process_index++;
        }
        if(i % 3 == 1){
            e[e_index] = atoi(data[i]);
            e_index++;
        }
        if(i % 3 == 2){
            t_arrive[t_arrive_index] = atoi(data[i]);
            t_arrive_index++;
        }
    }

    //to check what value is the e_max
    int local_max = 1;
    for(int i=0; i<e_index; i++){
        if(e[i] > local_max)
            local_max = e[i];
    }
    e_max = local_max;
    max_index = counter / 3;
}

node* MAKE_bin_HEAP(){
    node* np;
    np = NULL;

    return np;
}

void bin_LINK(node *y, node *z){
    y->parent = z;
    y->sibling = z->child;
    z->child = y;
    z->degree = z->degree + 1;
}

node* CREATE_NODE(int k, int pr, int FTFlag){
    node* p; //new node;
    p = (node*) malloc(sizeof(node));
    p->firstTimeFlag = FTFlag;
    p->process = pr;
    p->n = k;

    return p;
}

node* bin_HEAP_UNION(node *H1, node *H2){
    node* prev_x;
    node* next_x;
    node* x;
    node* H = MAKE_bin_HEAP();
    H = bin_HEAP_MERGE(H1, H2);
    if (H == NULL)
        return H;
    prev_x = NULL;
    x = H;
    next_x = x->sibling;
    while (next_x != NULL) {
        if ((x->degree != next_x->degree) || ((next_x->sibling != NULL)
                                              && (next_x->sibling)->degree == x->degree)) {
            prev_x = x;
            x = next_x;
        } else {
            if (x->n <= next_x->n) {
                x->sibling = next_x->sibling;
                bin_LINK(next_x, x);
            } else {
                if (prev_x == NULL)
                    H = next_x;
                else
                    prev_x->sibling = next_x;
                bin_LINK(x, next_x);
                x = next_x;
            }
        }
        next_x = x->sibling;
    }

    return H;
}

node* bin_HEAP_INSERT(node *H, node *x){
    node* H1 = MAKE_bin_HEAP();
    x->parent = NULL;
    x->child = NULL;
    x->sibling = NULL;
    x->degree = 0;
    H1 = x;
    H = bin_HEAP_UNION(H, H1);

    return H;
}

node* bin_HEAP_MERGE(node *H1, node *H2){
    node* H = MAKE_bin_HEAP();
    node* y;
    node* z;
    node* a;
    node* b;
    y = H1;
    z = H2;
    if (y != NULL) {
        if (z != NULL && y->degree <= z->degree)
            H = y;
        else if (z != NULL && y->degree > z->degree)
            /* need some modifications here;the first and the else conditions can be merged together!!!! */
            H = z;
        else
            H = y;
    } else
        H = z;
    while (y != NULL && z != NULL) {
        if (y->degree < z->degree) {
            y = y->sibling;
        } else if (y->degree == z->degree) {
            a = y->sibling;
            y->sibling = z;
            y = a;
        } else {
            b = z->sibling;
            z->sibling = y;
            z = b;
        }
    }

    return H;
}

int DISPLAY(node *H){
    node* p;
    if (H == NULL) {
        printf("\nHEAP EMPTY");
        return 0;
    }
    printf("\nTHE ROOT NODES ARE:-\n");
    p = H;
    while (p != NULL) {
        printf("n: %d, d: %d, P%d", p->n, p->degree, p->process);
        if (p->sibling != NULL)
            printf("-->");
        p = p->sibling;
    }
    printf("\n");
}

node* bin_HEAP_EXTRACT_MIN(node *H1){
    int min;
    node* t = NULL;
    node* x = H1;
    node *Hr;
    node* p;
    Hr = NULL;
    if (x == NULL) {
        printf("\nNOTHING TO EXTRACT");
        return x;
    }
    //    int min=x->n;
    p = x;
    while (p->sibling != NULL) {
        if ((p->sibling)->n < min) {
            min = (p->sibling)->n;
            t = p;
            x = p->sibling;
        }
        p = p->sibling;
    }
    if (t == NULL && x->sibling == NULL)
        H1 = NULL;
    else if (t == NULL)
        H1 = x->sibling;
    else if (t->sibling == NULL)
        t = NULL;
    else
        t->sibling = x->sibling;
    if (x->child != NULL) {
        REVERT_LIST(x->child);
        (x->child)->sibling = NULL;
    }
    H = bin_HEAP_UNION(H1, Hr);

    return x;
}

int REVERT_LIST(node *y){
    if (y->sibling != NULL) {
        REVERT_LIST(y->sibling);
        (y->sibling)->sibling = y;
    } else {
        Hr = y;
    }
}

node* FIND_NODE(node *H, int k){
    node* x = H;
    node* p = NULL;
    if (x->process == k) {
        p = x;
        return p;
    }
    if (x->child != NULL && p == NULL) {
        p = FIND_NODE(x->child, k);
    }

    if (x->sibling != NULL && p == NULL) {
        p = FIND_NODE(x->sibling, k);
    }

    return p;
}

int bin_HEAP_DECREASE_KEY(node *H, int i, int k){
    int temp;
    node* p;
    node* y;
    node* z;
    p = FIND_NODE(H, i);
    if (p == NULL) {
        printf("\nINVALID CHOICE OF KEY TO BE REDUCED");
        return 0;
    }
    if (k > p->n) {
        printf("\nSORY!THE NEW KEY IS GREATER THAN CURRENT ONE");
        return 0;
    }
    p->n = k;
    y = p;
    z = p->parent;
    while (z != NULL && y->n < z->n) {
        temp = y->n;
        y->n = z->n;
        z->n = temp;
        y = z;
        z = z->parent;
    }
    printf("\nKEY REDUCED SUCCESSFULLY!");
}

int bin_HEAP_DELETE(node *H, int k){
    node* np;
    if (H == NULL) {
        printf("\nHEAP EMPTY");
        return 0;
    }

    bin_HEAP_DECREASE_KEY(H, k, -1000);
    np = bin_HEAP_EXTRACT_MIN(H);
    if (np != NULL)
        printf("\nNODE DELETED SUCCESSFULLY");
}