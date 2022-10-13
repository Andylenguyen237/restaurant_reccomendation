#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <math.h>
#include "listops.c"


/* stage heading */
#define STAGE_NUM_ONE 1						  /* stage numbers */ 
#define STAGE_NUM_TWO 2
#define STAGE_NUM_THREE 3
#define STAGE_NUM_FOUR 4
#define HEADING "==========================Stage %d==========================\n"

#define MAX_CUSINE_LENGTH 20 
#define MAX_RES_NAME_LENGTH 100
#define MAX_RES_INPUT 99
#define MAX_IDENTIFIER_LINE_LENGTH 5
#define MAX_CUSTOMER_ID_LENGTH 6

#define FOUND 1 
#define NOT_FOUND 0

#define POW_TWO 2

#define VISIT 1
#define NOT_EXIST 0

#define PRINT_NEGATIVE_ONE "  -"
#define PRINT_NEGATIVE_TWO "  +"
#define PRINT_NEGATIVE_THREE "  *"

#define SIMILAR_VISIT -1 

#define AVG_PRICE_BOUND 20
#define DISTANCE_BOUND 30

#define SIMILAR_BOUND 0 


/* typedefs */
typedef char cusine_t [MAX_CUSINE_LENGTH+1];
typedef char rest_name_t [MAX_RES_NAME_LENGTH+1];
typedef char identifier_t [MAX_IDENTIFIER_LINE_LENGTH +1];
typedef char customer_id_string [MAX_CUSTOMER_ID_LENGTH+1];

typedef struct{

    int restaurant_id;
    double x_coor;
    double y_coor;
    int avg_per_head;
    cusine_t cusine;
    rest_name_t restaurant_name;


} restaurant_t;

typedef struct {

    int year,month,day,hour,minute,second;

} date_t ;

typedef struct {

    date_t transaction_time;
    customer_id_string customer_id;
    int restaurant_id;
    int transaction_amount;

} customer_t; 

/****************************************************************/
/* function prototypes */
void print_stage_header(int stage);

void stage_one(restaurant_t restaurant[], int *num_reads);
void stage_two(restaurant_t restaurant[], list_t *customer_list, int num_reads);
void stage_three(restaurant_t restaurant[], list_t *customer_list, int num_reads);
void stage_four(list_t *customer_list, int num_reads);

// return number of reads 
int take_restaurant_reads(restaurant_t res_info[]);
int take_customer_reads(customer_t *customer_info);
int get_restaurant_id_index(restaurant_t restaurant[], int restaurant_id, int num_reads);
void print_visisting_array(data_t customer_list, int num_reads);
void print_customer_list_array(list_t *customer_list, int num_reads);
void create_new_node(list_t *customer_list, customer_t customer_transaction, int restaurant_index, int num_reads);
int is_exist_customer_id(list_t *customer_list, customer_t customer, int restaurant_index);
double distance_calculation(double x1, double y1, double x2, double y2);
int similarity_calculation(node_t *main_tmp, node_t *tmp, int num_reads);
node_t *find_top_2(node_t *main_tmp, node_t *tmp, node_t *prev_tmp, int num_reads);

/****************************************************************/

/* main program controls all the action */
int
main(int argc, char *argv[]) {
    /* TODO: Write your solution! */

    restaurant_t restaurant_info[MAX_RES_INPUT];
    int num_reads;

    /* STAGE 1 */
    stage_one(restaurant_info, &num_reads);

    /* STAGE 2 */
    list_t *customer_list = make_empty_list();
    stage_two(restaurant_info, customer_list, num_reads);

    /* STAGE 3 */
    stage_three(restaurant_info, customer_list, num_reads);

    /* STAGE 4 */ 
    stage_four(customer_list, num_reads);

    /* FREE LIST */
    free_list(customer_list);
    customer_list = NULL;
    
    return 0;
}

/* print stage header given stage number */
void 
print_stage_header(int stage_num) {
	printf(HEADING, stage_num);
}

/* TODO: Define more functions! */

int take_restaurant_reads(restaurant_t *res_info){

    int values_read;

    values_read = scanf("%d %lf %lf %d %s %s",
    &res_info->restaurant_id,
    &res_info->x_coor,
    &res_info->y_coor,
    &res_info->avg_per_head,
    res_info->cusine,
    res_info->restaurant_name);

    if (values_read != 6){
        return EOF;
    }
    else{
        return 0;
    }

}

int take_customer_reads(customer_t *customer_info){
    
    int values_read;

    values_read = scanf("%d:%d:%d:%d:%d:%d %s %d %d",
    &customer_info->transaction_time.year,
    &customer_info->transaction_time.month,
    &customer_info->transaction_time.day,
    &customer_info->transaction_time.hour,
    &customer_info->transaction_time.minute,
    &customer_info->transaction_time.second,
    customer_info->customer_id,
    &customer_info->restaurant_id,
    &customer_info->transaction_amount);

    if (values_read != 9){
        return EOF;
    }
    else{
        return 0;
    }
}

void print_customer_list_array(list_t *customer_list, int num_reads){

    node_t *tmp = customer_list->head;
    while(!is_empty_list(customer_list) && tmp != NULL){
        printf("%s:",tmp->data.id);
        print_visisting_array(tmp->data,num_reads);
        tmp = tmp->next;
        printf("\n");
    }

}

void stage_one(restaurant_t restaurant[], int *num_reads){

    print_stage_header(STAGE_NUM_ONE);

    int index = 0;
    int restaurant_reads;
    identifier_t separator_line;

    while (take_restaurant_reads(&restaurant[index]) != EOF)
    {
        index++;  
    }

    scanf("%s", separator_line);

    // take the number of reads which = number of restaurants (if unique)
    restaurant_reads = index;
    *num_reads = index; // pointer send back to num_reads in main function 

    // find restaurant with smallest price per head 
    int min_avg_price = restaurant[0].avg_per_head;
    char min_avg_price_restaurant[MAX_RES_NAME_LENGTH];

    for (int i = 0; i < restaurant_reads; i++){
        if (restaurant[i].avg_per_head < min_avg_price){
            strcpy(min_avg_price_restaurant,restaurant[i].restaurant_name);
            min_avg_price = restaurant[i].avg_per_head;
        }
    }

    printf("Number of restaurants: %d\n", restaurant_reads);
    printf("Restaurant with the smallest average price: %s\n", min_avg_price_restaurant);
    printf("\n");

}

void stage_two(restaurant_t restaurant[], list_t *customer_list, int num_reads){

    print_stage_header(STAGE_NUM_TWO);
    customer_t customer;
    int restaurant_index;

    while (take_customer_reads(&customer)!= EOF){
    
        restaurant_index = get_restaurant_id_index(restaurant, customer.restaurant_id, num_reads);

        if (is_empty_list(customer_list)){
            create_new_node(customer_list, customer, restaurant_index, num_reads);
        }
        else{
            if (is_exist_customer_id(customer_list,customer,restaurant_index) == 0){
                create_new_node(customer_list, customer, restaurant_index, num_reads);
            }
        }

    }

    print_customer_list_array(customer_list, num_reads);

    printf("\n");

}


// FUNCTION TO UPDATE THE EXISTENT ID and return 1 if the identitcal id was found, ELSE return 0 CREATE NEW NODE IF HAVENT EXIST ALREADY in stage2 function 
int is_exist_customer_id(list_t *customer_list, customer_t customer, int restaurant_index){

    node_t *tmp = customer_list->head;

    while (tmp != NULL){

        if (strcmp(tmp->data.id,customer.customer_id) == 0){
            tmp->data.visiting[restaurant_index] += VISIT;
            return FOUND;
        }

        tmp = tmp->next;
        
    }

    return NOT_FOUND;
    
}

void create_new_node(list_t *customer_list, customer_t customer_transaction, int restaurant_index, int num_reads){

    data_t customer;
    strcpy(customer.id, customer_transaction.customer_id);
    for (int i = 0; i < num_reads; i++){
        customer.visiting[i] = 0;
    }
    customer.visiting[restaurant_index] += VISIT;
    customer_list = insert_at_foot(customer_list, customer);
    
}

int get_restaurant_id_index(restaurant_t restaurant[], int restaurant_id, int num_reads){

    for (int i = 0; i < num_reads; i++){

        if (restaurant[i].restaurant_id == restaurant_id){
            return i;
        }

    } 

    return NOT_EXIST; 

}

void print_visisting_array(data_t customer_list, int num_reads){

    for (int i = 0; i < num_reads; i++){

        if (customer_list.visiting[i] == -1){
            printf(PRINT_NEGATIVE_ONE);
        } 
        else if (customer_list.visiting[i] == -2){
            printf(PRINT_NEGATIVE_TWO);
        }
        else if (customer_list.visiting[i] == -3){
            printf(PRINT_NEGATIVE_THREE);
        }
        else{
            printf("  %d", customer_list.visiting[i]);
        }

    }

}

void stage_three(restaurant_t restaurant[], list_t *customer_list, int num_reads){
    
    print_stage_header(STAGE_NUM_THREE);

    node_t *tmp = customer_list->head;

    double main_x_coor, main_y_coor;
    int avg_price;
    cusine_t cusine;

    while (tmp != NULL){

        for (int i = 0; i < num_reads; i++){
            if (tmp->data.visiting[i] > 0){
              
                main_x_coor = restaurant[i].x_coor;
                main_y_coor = restaurant[i].y_coor;
                avg_price = restaurant[i].avg_per_head;
                strcpy(cusine,restaurant[i].cusine);
                
                for (int j = 0; j < num_reads; j++){
                    if(tmp->data.visiting[j] <= 0){
            
                        if (strcmp(cusine, restaurant[j].cusine) == 0){
                            tmp->data.visiting[j] = SIMILAR_VISIT; 
                        }else if (restaurant[j].avg_per_head <= AVG_PRICE_BOUND+avg_price && restaurant[j].avg_per_head >= avg_price - AVG_PRICE_BOUND){
                            tmp->data.visiting[j] = SIMILAR_VISIT;
                        }else if (distance_calculation(main_x_coor, main_y_coor, restaurant[j].x_coor, restaurant[j].y_coor) <= DISTANCE_BOUND){
                            tmp->data.visiting[j] = SIMILAR_VISIT;
                        }
                    }
                }
            }
        }

        tmp = tmp->next;

    }

    print_customer_list_array(customer_list, num_reads);

    printf("\n");

}

double distance_calculation(double x1, double y1, double x2, double y2){

    double distance = sqrt(pow((x1-x2),POW_TWO) + pow((y1-y2),POW_TWO));

    return distance;

}

void stage_four(list_t *customer_list, int num_reads){

    print_stage_header(STAGE_NUM_FOUR);

    node_t *main_tmp = customer_list->head;
    node_t *tmp, *tmp1, *tmp2;

    while (main_tmp != NULL){

        tmp = customer_list->head;

        tmp1 = find_top_2(main_tmp, tmp, NULL, num_reads);
        tmp2 = find_top_2(main_tmp, tmp, tmp1, num_reads);

        if (tmp1 != NULL && tmp2 != NULL){

            for (int i = 0; i < num_reads; i++){
            
                if (main_tmp->data.visiting[i] <= SIMILAR_BOUND  && tmp1->data.visiting[i] > SIMILAR_BOUND && tmp2->data.visiting[i] > SIMILAR_BOUND){
                    main_tmp->data.visiting[i] += (SIMILAR_VISIT*2);
                }
                else if ( (main_tmp->data.visiting[i] <= SIMILAR_BOUND && tmp1->data.visiting[i] > SIMILAR_BOUND) || (main_tmp->data.visiting[i] <= SIMILAR_BOUND && tmp2->data.visiting[i] > SIMILAR_BOUND) ){
                    main_tmp->data.visiting[i] += SIMILAR_VISIT;
                }

            }

        }

        /* case where only 1 similar customer */ 
        if (tmp1 != NULL && tmp2 == NULL){

            for (int i = 0; i < num_reads; i++){

                if (main_tmp->data.visiting[i] <= SIMILAR_BOUND && tmp1->data.visiting[i] > SIMILAR_BOUND){
                    main_tmp->data.visiting[i] += SIMILAR_VISIT;
                }

            }

        }

        main_tmp = main_tmp->next;

    } 

    print_customer_list_array(customer_list, num_reads);
     
}

node_t *find_top_2(node_t *main_tmp, node_t *tmp, node_t *prev_tmp, int num_reads){

    int current_similarity, max_similarity = 0;
    node_t *top_tmp = NULL;

    while (tmp != NULL){
            if (strcmp(main_tmp->data.id,tmp->data.id) == 0){
                if (tmp->next == NULL){
                    break;
                }
                tmp = tmp->next;
            }

            current_similarity = similarity_calculation(main_tmp, tmp, num_reads);
            
            if(current_similarity > max_similarity && tmp != prev_tmp){
                max_similarity = current_similarity;
                top_tmp = tmp;
            }
            
            
            tmp = tmp->next;
        }



    return top_tmp;

}

int similarity_calculation(node_t *main_tmp, node_t *tmp, int num_reads){

    int similarity = 0;

    for (int i = 0; i < num_reads; i++){

        if (main_tmp->data.visiting[i] > 0 && tmp->data.visiting[i] > 0){
            similarity += main_tmp->data.visiting[i] * tmp->data.visiting[i];
        }

    }

    return similarity;
    
}
