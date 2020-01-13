/*Designed and programmed by - Cem Gulec - 150117828
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <malloc.h>

#define MAX_LIMIT 25

struct node {
    int process;
    double priority;
    int t_arrive;
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
double least_priVal = 50, least_TArrive = 50;
int leastPriVal[MAX_LIMIT] = {0}, isSamePriCnt = 0, isSamePriFlag = 0, numberOfNodes, heapEmptyFlag = 0;

//function prototypes
void AWT_result();
void clearWT();
void displayWT();
void increaseWT_SG(int);
void increaseWT_ALL_shifted(int, int);
int *changeInputProcess(int *, int, int);
void deleteInputProcess(int);
int isInputEmpty();
int isSamePriVal(node *);
void calculatePriorityNodes(node *);
void printHeap(node *);
void Processor(int);
double calculatePriVal(double, int);
int whichHasTheLeastPri1(node *);
int whichHasTheLeastPri2(node *);
double calculateCei(int);
void readFile();
node* MAKE_bin_HEAP();
void bin_LINK(node *, node *);
node* CREATE_NODE(int, int, int, int);
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

    int q = 1;
    readFile();
    Processor(q);
    printf("\nquantum_value: %d  |  Waiting Time of processes: \n", q);
    printf("-----------------------------------------------\n");
    displayWT();
    AWT_result();

    for(q = 2; q <= 10; q++){
        clearWT();
        readFile();
        Processor(q);
        printf("\nquantum_value: %d  |  Waiting Time of processes: \n", q);
        printf("-----------------------------------------------\n");
        displayWT();
        AWT_result();
    }

}

void AWT_result(){
    double result = 0;
    for(int i = 0; i<MAX_LIMIT; i++)
        result += WaitTime[i];


    printf("\tAverage Time Waited in queue: %.0lf/%d = %.6lf\n\n", result, max_index, result/max_index);

}

void clearWT(){
    for(int i = 0; i<MAX_LIMIT; i++){
        process[i] = 0;
        e[i] = 0;
        t_arrive[i] = 0;
        WaitTime[i] = 0;
        firsTime[i] = 0;
    }

    general_time = 0;
    least_priVal = 50;
    isSamePriCnt = 0;
    heapEmptyFlag = 0;
}

void displayWT(){
    for(int i = 0; i<max_index; i++)
        printf("P%d, waited time in queue: %d\n", i+1, WaitTime[i]);
}

void increaseWT_SG(int pr){
    for(int i = 0; i<max_index; i++){
        if(i == pr-1)
            WaitTime[i]++;
    }
}

void increaseWT_ALL_shifted(int index_num, int difference){
    for(int i = 0; i<max_index; i++){
        if(i != index_num-1)
            WaitTime[i] += difference;
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

void deleteInputProcess(int process_num){

    for(int i = 0; i < MAX_LIMIT; i++){
        if(process[i] == process_num+1)
            process[i] = 0;
    }

}

int isInputEmpty(){
    int counter = 0;

    for(int i=0; i<max_index; i++)
        if(e[i] != 0)
            counter++;

    if(counter == 0)
        return 1;
    else
        return 0;
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
    if(isSamePriCnt == numberOfNodes && numberOfNodes != 1){
        isSamePriCnt = 0;
        return 1;
    }
    else{
        isSamePriCnt = 0;
        return 0;
    }

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
        if(head->t_arrive < least_TArrive)
            least_TArrive = t_arrive[head->process -1];

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

    int processes[20] = {0};
    node *np;

    //after reading file add all the processes
    //while there exist processes in the input list
    while(heapEmptyFlag == 0)
    {
        int num_nodes = 0, processes_index = 0;
        //check for the input arrive time, t_arr <= general_time
        //means that element can enter the system from BH
        for(int i = 0; i < max_index; i++){
            if(t_arrive[i] <= general_time && e[i] != 0 && process[i] != 0){
                np = CREATE_NODE(e[i] ,process[i], firsTime[i], t_arrive[i]);
                H = bin_HEAP_INSERT(H, np);
                num_nodes++;
                processes[processes_index++] = process[i];
            }
        }
        numberOfNodes = num_nodes;
        //calculate priority values for all the elements in BH
        calculatePriorityNodes(H);

        /*printf("\nGeneral time: %d", general_time);
        printf("\nLeast pri val: %.3lf", least_priVal);
        printf("\nLeast t_arrive val: %.3lf", least_TArrive);
        printf("\nmax index: %d", max_index);
        printf("\nNumber of nodes: %d",numberOfNodes);
        printf("\nWhich has the least pri val: P%d\n",whichHasTheLeastPri1(H));
        printf("-------------------------------\n");
        printf("//  ");
        for(int x = 0; x < num_nodes; x++)
            printf("n:%d  P%d | ", e[processes[x]-1], processes[x]);
        printf("  //");*/

        int least_process_index;
        if(!isSamePriVal(H)){
            least_process_index = whichHasTheLeastPri1(H)-1;
            bin_HEAP_DELETE(H, whichHasTheLeastPri1(H));
        }
        else{
            least_process_index = whichHasTheLeastPri2(H)-1;
            bin_HEAP_DELETE(H, whichHasTheLeastPri2(H));
        }

        for(int i = 0; i<max_index; i++){
            if(processes[i] != 0 && firsTime[processes[i] - 1] == 0 && WaitTime[processes[i] - 1] == 0)
                WaitTime[processes[i] - 1] += general_time - t_arrive[processes[i] - 1];
            if(e[processes[i]-1] != 0 && processes[i] != least_process_index+1){
                WaitTime[processes[i] - 1] +=
                        e[least_process_index] > quantum_time ? quantum_time : e[least_process_index];

            }

        }
        //reset deployed
        for(int l = 0; l<max_index; l++)
            processes[l] = 0;

        //if the process is not completed
        if( e[least_process_index] > quantum_time ){
            e[least_process_index] -= quantum_time;
            general_time += quantum_time;
            firsTime[least_process_index] = 1;
        }

            //if completed
        else if ( e[least_process_index] <= quantum_time ){
            //it may be shorter than quantum_time
            //so that it can shift the time domain
            general_time += e[least_process_index];
            deleteInputProcess(least_process_index);
            e[least_process_index] = 0;
        }

        //printf("\n//  "); displayWT(); printf("  //\n\n");
        //update before attempting to the other process
        numberOfNodes = 0;
        isSamePriCnt = 0;
        isSamePriFlag = 0;
        least_priVal = 50;
        least_TArrive = 50;
    }

}

int whichHasTheLeastPri1(node *head){
    while(head){
        //return the one with smaller priority val
        if(head->priority == least_priVal){
            return (head->process);
        }
        int recursive_return = whichHasTheLeastPri1(head->child);
        if(recursive_return != -1) return recursive_return;
        head = head->sibling;
    }

    return -1;
}

int whichHasTheLeastPri2(node *head){
    while(head){
        //return the one with smaller priority val
        if(head->t_arrive == least_TArrive){
            return (head->process);
        }
        int recursive_return = whichHasTheLeastPri2(head->child);
        if(recursive_return != -1) return recursive_return;
        head = head->sibling;
    }

    return -1;
}

double calculatePriVal(double c_ei, int e_i){
    return c_ei * e_i;
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
    FILE *ptr = fopen("C:/Users/cemgg/Documents/GitHub/Preemptive-Scheduling-System-with-BH/Project3 2019/input.txt","r");
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

node* CREATE_NODE(int k, int pr, int FTFlag, int t){
    node* p; //new node;
    p = (node*) malloc(sizeof(node));
    p->firstTimeFlag = FTFlag;
    p->process = pr;
    p->n = k;
    p->t_arrive = t;

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
        //printf("\nHEAP EMPTY");
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
    //printf("\nKEY REDUCED SUCCESSFULLY!");
}

int bin_HEAP_DELETE(node *H, int k){
    node* np;
    if (H == NULL) {
        //printf("\nHEAP EMPTY");
        heapEmptyFlag = 1;
        return 0;
    }

    bin_HEAP_DECREASE_KEY(H, k, -1000);
    np = bin_HEAP_EXTRACT_MIN(H);
    /*if (np != NULL)
        printf("\nNODE DELETED SUCCESSFULLY");*/
}