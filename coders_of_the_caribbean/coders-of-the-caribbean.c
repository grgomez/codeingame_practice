#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

/**
 * Auto-generated code below aims at helping you parse
 * the standard input according to the problem statement.
 **/
 
#define MAX_BARREL_COUNT 26
#define MAX_MINE_COUNT 10
#define MAX_SHIP_COUNT 1

/* HEXAGONAL COORDINATE FUNCTIONS */

/* 3-D cube representation of a hexagonal corrdinate */
struct cube {
    int x, y, z;   
};

/* 2-D representation of odd-r offset representation */
struct hex {
    int col, row;
};

/* Convert cube coordinate to odd-r offset coordinate */
struct hex cube_to_oddr(struct cube c) {
       int col = c.x + (c.z - (c.z & 1)) / 2;
       int row = c.z;
       
       return (struct hex) { .col = col, .row = row };
}

/* Convert odd-r offset coordinate to cube coordinate */
struct cube oddr_to_cube(struct hex h) {
    int x = h.col - (h.row - (h.row & 1)) / 2;
    int z = h.row;
    int y = -x-z;
    
    return (struct cube) { .x = x, .y = y, .z = z};
}

/* Calculate cube coordinate distance between two hex odd-r coordinates */
int cube_distance(struct cube a, struct cube b) {
    return (abs(a.x - b.x) + abs(a.y - b.y) + abs(a.z + b.z)) / 2;
}

/* change in directions in cube coordinate */
/* this arrays´ indexes matches the enums of the ship's directions */
struct cube cube_directions[6] = {
    (struct cube) { .x = 1, .y = -1, .z = 0 }, (struct cube) { .x = 1, .y = 0, .z = -1 },
    (struct cube) { .x = 0, .y = -1, .z = 1 }, (struct cube) { .x = -1, .y = 1, .z = 0 },
    (struct cube) { .x = -1, .y = 0, .z = 1 }, (struct cube) { .x = 0, .y = -1, .z = 1 }
};

/* return the cube coordinate for each direction */
struct cube cube_direction(int direction) {
    return cube_directions[direction];   
}

/* add two cube's coordinates together */ 
struct cube cube_add(struct cube c, struct cube d) {
    return (struct cube) { 
                .x = c.x + d.x, 
                .y = c.y + d.y, 
                .z = c.z + d.z  };   
}

/* return the cube's neighbour in the specified direction */
struct cube cube_neighbour (struct cube c, int direction) {
    return cube_add(c, cube_direction(direction));   
}

/* QUEUE IMPLEMENTATION */
struct node {
    struct cube direction;
    struct node * next;
};
struct queue {
    struct node * head;
    struct node * tail;
};
void enqueue(struct queue * q, struct cube c) {
       
       struct node * p = malloc(sizeof(struct node));
       
       p->direction = c;
       
       if (!q->head && !q->tail) {
            q->head = q->tail = p;   
       } 
       else {
            q->tail->next = p;
            q->tail = p;
       }
}
int dequeue(struct queue * q, struct cube * c) {
    if (!q->head) return -1;
    
    c = &(q->head->direction);
    
    struct node * temp = q->head;
    q->head = q->head->next;
    free(temp);
    
    return 0;
}

/* HEXAGONAL COORDINATE FUNCTIONS END */

/* game structures */
/* Direction of the ship */
enum directions {
    EAST,
    NORTHEAST,
    NORTHWEST,
    WEST,
    SOUTHWEST,
    SOUTH,
    SOUTHEAST
};

/* Type of entities */
enum type {
    EMPTY,
    SHIP,
    BARREL,
    CANNONBALL,
    MINE
};

/* track entities in a map for pathfinding */
int map[23][21] = {
    {0, 0, 0, 0, 0, 0, 0, 0 , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},  
    {0, 0, 0, 0, 0, 0, 0, 0 , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},  
    {0, 0, 0, 0, 0, 0, 0, 0 , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},  
    {0, 0, 0, 0, 0, 0, 0, 0 , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},  
    {0, 0, 0, 0, 0, 0, 0, 0 , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},  
    {0, 0, 0, 0, 0, 0, 0, 0 , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},  
    {0, 0, 0, 0, 0, 0, 0, 0 , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},  
    {0, 0, 0, 0, 0, 0, 0, 0 , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},  
    {0, 0, 0, 0, 0, 0, 0, 0 , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},  
    {0, 0, 0, 0, 0, 0, 0, 0 , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},  
    {0, 0, 0, 0, 0, 0, 0, 0 , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},  
    {0, 0, 0, 0, 0, 0, 0, 0 , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},  
    {0, 0, 0, 0, 0, 0, 0, 0 , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},  
    {0, 0, 0, 0, 0, 0, 0, 0 , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},  
    {0, 0, 0, 0, 0, 0, 0, 0 , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},  
    {0, 0, 0, 0, 0, 0, 0, 0 , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},  
    {0, 0, 0, 0, 0, 0, 0, 0 , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},  
    {0, 0, 0, 0, 0, 0, 0, 0 , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},  
    {0, 0, 0, 0, 0, 0, 0, 0 , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},  
    {0, 0, 0, 0, 0, 0, 0, 0 , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},  
    {0, 0, 0, 0, 0, 0, 0, 0 , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
};

struct ship {
    struct hex location; /* hexagonal odd-r coordinate */
    int orientation; /* between 0 and 5 */
    int speed; /* between 0 and 2 */
    int rum_stock; /* curent rum units */
    int control; /* 1 if controlled by me 0 otherwise */
};

struct barrel {
    struct hex location; /* hexagonal odd-r coordinate */
    int amount; /* amount of rum in the barrel */   
};

struct mine {
    struct hex location; /* hexagonal odd-r coordinate */
};

/* array of ships */
struct ship * ships;

/* array of barrels */
struct barrel * barrels;

int main()
{
    ships = malloc(sizeof(struct ship));
    barrels = malloc(sizeof(struct barrel)*MAX_BARREL_COUNT);
    
    // game loop
    while (1) {
        // the number of remaining ships
        int my_ship_count;
        scanf("%d", &my_ship_count);
        
        
        /* Number of barrels in the current turn */
        int barrel_count = 0; 
        
        // the number of entities (e.g. ships, mines or cannonballs)
        int entity_count;
        
        scanf("%d", &entity_count);
        for (int i = 0; i < entity_count; i++) {
            int entity_id;
            char entity_type[11];
            int x;
            int y;
            int arg_1;
            int arg_2;
            int arg_3;
            int arg_4;
            scanf("%d%s%d%d%d%d%d%d", &entity_id, entity_type, &x, &y, &arg_1, &arg_2, &arg_3, &arg_4);
            
            if (!strcmp(entity_type, "SHIP") && 1 == arg_4) {
                /* for now there is only ONE ship so let's refer to it directly */
                ships[0].location = (struct hex) { .col = x, .row = y };
                ships[0].orientation = arg_1;
                ships[0].speed = arg_2;
                ships[0].rum_stock = arg_3;
            }
            if (!strcmp(entity_type, "BARREL")) {
                
                barrels[barrel_count].location = (struct hex) { .col = x, .row = y };
                barrels[barrel_count].amount = arg_1;
                barrel_count++;
            }
            if (!strcmp(entity_type, "MINE")) {
                fprintf(stderr, "MINE FOUND: %d %d", x, y);   
            }
        }
        for (int i = 0; i < my_ship_count; i++) {
            struct ship my_ship = ships[i];
            
            int barrel_index = 0;
            /* start with a ridiculous distance */
            int shortest_distance = 1000000000;
            /* let's calculate the closest barrel to my ship in this turn and get it */
            for (int j = 0; j < barrel_count; ++j) {
                struct barrel current_barrel = barrels[j];
                
                /* The coordinate is now a hex grid */
                int distance = cube_distance(
                                        oddr_to_cube(my_ship.location), 
                                        oddr_to_cube(current_barrel.location));
                
                if (distance < shortest_distance) {
                    shortest_distance = distance;
                    barrel_index = j;
                }
            }
            // Write an action using printf(). DON'T FORGET THE TRAILING \n
            // To debug: fprintf(stderr, "Debug messages...\n");
            
            // Any valid action, such as "WAIT" or "MOVE x y"
            printf("MOVE %d %d\n", 
                            barrels[barrel_index].location.col, 
                            barrels[barrel_index].location.row); 
        }
    }
    
    free(barrels);
    free(ships);
    
    return 0;
}
