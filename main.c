/*Designed and programmed by - Cem Gulec - 150117828
 * TODO: P2 P1'den önce bitebilir dolayısıyla last_index'e bağımlı bir döngü ile BH'e kaydetme yapamam
 * TODO: aynı node'u eklememe durumunu kontrol etmek gerek
 * TODO: BH'den silip tekrar ekleyeceğin nodun gerekli bilgileri girilmesi gerek (firstTimeFlag = 0 gibi)
 * TODO: processor node'una trace edecek kodlama
 * TODO: şayet processor'a ulaştım bunu nasıl delete ederim
 * TODO: arayüz en son halinde tablolaştıracak şekilde print edilebilir
 * TODO: fonksiyon prototip ve alttaki yerlerdeki sıralamalar daha akıllıca olabilir (örneğin hesaplamalar alt alta)
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
int e_max, max_index, general_time = 26;

//function prototypes
int *changeInputProcess(int *, int, int);
int *deleteInputProcess(int *, int);
int isInputEmpty(int *);
void calculatePriorityNodes(node *);
void printHeap(node*);
void Processor(int);
double calculatePriVal(double, int);
double calculateCei(int);
void readFile();
node* MAKE_bin_HEAP();
void bin_LINK(node*, node*);
node* CREATE_NODE(int, int, int);
node* bin_HEAP_UNION(node*, node*);
node* bin_HEAP_INSERT(node*, node*);
node* bin_HEAP_MERGE(node*, node*);
node* bin_HEAP_EXTRACT_MIN(node*);
int REVERT_LIST(node*);
int DISPLAY(node*);
node* FIND_NODE(node*, int);
int bin_HEAP_DECREASE_KEY(node*, int, int);
int bin_HEAP_DELETE(node*, int);


int main() {

    readFile();

    Processor(1);
    printHeap(H);

    DISPLAY(H);

}

int *changeInputProcess(int *arr, int process_num, int key) {

    for(int i = 0; i<MAX_LIMIT; i++)
        if(arr[i] == process_num){
            arr[i] = key;
            break;
        }

    return arr;
}

int *deleteInputProcess(int *arr, int process_num){

    for(int i = 0; i < MAX_LIMIT; i++){
        if(arr[i] == process_num)
            arr[i] = 0;
    }

    return arr;
}

int isInputEmpty(int *arr){
    for(int i = 0; i<MAX_LIMIT; i++)
        if(arr[i] != 0){
            return 0;
        }

    return 1;
}

void calculatePriorityNodes(node * head){
    double c_ei, priorityVal;
    while(head){
        if(head->firstTimeFlag == 0)
            c_ei = 1;
        else
            c_ei = calculateCei(e[head->process - 1]);

        priorityVal = calculatePriVal(c_ei, e[head->process - 1]);
        head->priority = priorityVal;
        calculatePriorityNodes(head->child);
        head = head->sibling;
    }
}

void printHeap(node* head){
    while(head){
        printf("%d:%.3lf ", head->process, head->priority);
        printHeap(head->child);
        head = head->sibling;
    }
}

void Processor(int quantum_time){

    int q = quantum_time;
    node *np;

    //after reading file add all the processes
    //while there exist processes in the input list

    //while (!isInputEmpty(process))
    //{
        //check for the input arrive time, t_arr <= general_time
        //means that element can enter the system from BH
        for(int i = 0; i < max_index; i++){
            if(t_arrive[i] <= general_time && process[i] != 0){
                np = CREATE_NODE(e[i] ,process[i], firsTime[i]);
                H = bin_HEAP_INSERT(H, np);
                firsTime[i] = 1;
            }
        }
        //calculate priority values for all the elements in BH
        calculatePriorityNodes(H);
    //}
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
    max_index = process_index - 1;
}

node* MAKE_bin_HEAP(){
    node* np;
    np = NULL;

    return np;
}

void bin_LINK(node* y, node* z){
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

node* bin_HEAP_UNION(node* H1, node* H2){
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

node* bin_HEAP_INSERT(node* H, node* x){
    node* H1 = MAKE_bin_HEAP();
    x->parent = NULL;
    x->child = NULL;
    x->sibling = NULL;
    x->degree = 0;
    H1 = x;
    H = bin_HEAP_UNION(H, H1);

    return H;
}

node* bin_HEAP_MERGE(node* H1, node* H2){
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

int DISPLAY(node* H){
    node* p;
    if (H == NULL) {
        printf("\nHEAP EMPTY");
        return 0;
    }
    printf("\nTHE ROOT NODES ARE:-\n");
    p = H;
    while (p != NULL) {
        printf("%d, %d, %d", p->n, p->degree, p->process);
        if (p->sibling != NULL)
            printf("-->");
        p = p->sibling;
    }
    printf("\n");
}

node* bin_HEAP_EXTRACT_MIN(node* H1){
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

int REVERT_LIST(node* y){
    if (y->sibling != NULL) {
        REVERT_LIST(y->sibling);
        (y->sibling)->sibling = y;
    } else {
        Hr = y;
    }
}

node* FIND_NODE(node* H, int k){
    node* x = H;
    node* p = NULL;
    if (x->n == k) {
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

int bin_HEAP_DECREASE_KEY(node* H, int i, int k){
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

int bin_HEAP_DELETE(node* H, int k){
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