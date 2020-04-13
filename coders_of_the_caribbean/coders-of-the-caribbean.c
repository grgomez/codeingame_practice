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
    return (abs(a.x - b.x) + abs(a.y - b.y) + abs(a.z - b.z)) / 2;
}
/* Calculate offset distance between to odd-r coordinates */
int offset_distance(struct hex a, struct hex b) {
    struct cube ac = oddr_to_cube(a);
    struct cube bc = oddr_to_cube(b);
    return cube_distance(ac, bc);
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
/* Directions of the ship */
enum directions {
    EAST,
    NORTHEAST,
    NORTHWEST,
    WEST,
    SOUTHWEST,
    SOUTHEAST
};
/* find all the neighbours of a cube */
void get_cube_neighbours(struct cube c, struct cube neighbours[6]) {
    neighbours[EAST] = cube_neighbour(c, EAST);
    neighbours[NORTHEAST] = cube_neighbour(c, NORTHEAST);
    neighbours[NORTHWEST] = cube_neighbour(c, NORTHWEST);
    neighbours[WEST] = cube_neighbour(c, WEST);
    neighbours[SOUTHWEST] = cube_neighbour(c, SOUTHWEST);
    neighbours[SOUTHEAST] = cube_neighbour(c, SOUTHEAST);
}
/* HEXAGONAL COORDINATE FUNCTIONS END */
/* PRIORITY QUEUE IMPLEMENTATION */
struct node {
    struct cube direction;
    int priority;
    struct node * next;
};
struct queue {
    struct node * head;
    size_t length;
};
void enqueue(struct queue * q, struct cube c, int priority) {
       
       struct node * p = malloc(sizeof(struct node));
       
       p->direction = c;
       p->next = NULL;
       p->priority = priority;
       
       /* the queue is uninitialized */
       if (!q->head) {
           q->head = p;
           q->length = 1;
           
           return;
       }
       
       q->length++;
       
       /* add node at the head */
       if (q->head->priority > priority) {
            p->next = q->head;
            q->head = p;
       }
       else {
           struct node * temp = q->head;
           while(!temp->next && temp->next->priority < priority) {
                temp = temp->next;   
           }
           
           p->next = temp->next;
           temp->next = p;
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
/* PRIORITY QUEUE IMPLEMENTATION END */

/* game structures */
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
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},  
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},  
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},  
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},  
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},  
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},  
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},  
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},  
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},  
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},  
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},  
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},  
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},  
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},  
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},  
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},  
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},  
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},  
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},  
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},  
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
};
struct ship {
    struct hex location; 
    int orientation; /* between 0 and 5 */
    int speed; /* between 0 and 2 */
    int rum_stock; /* current rum units */
    int control; /* 1 if controlled by me 0 otherwise */
};
/* NOTE: MAYBE WE DO NOT NEED THESE STRUCTS 
    AFTER IMPLEMENTING PATH FINDING
    RATHER KEEP TRACK OF THEM AS COSTS
    IN THE MAP AND FIND THE BEST PATH ACCORDINGLY */
struct barrel {
    struct hex location;
    int amount; /* amount of rum in the barrel */   
    int entity_id;
};
struct mine {
    struct hex location;
};
/* array of ships */
struct ship * ships;
/* array of barrels */
struct barrel * barrels;
/* PATHFINDING ALGORITHM IMPLEMENTATION 
int heuristic (struct direction a, struct direction b, int entity_type) {
    
    int distance = cube_distance(a.cube_location, b.cube_location);
    switch(entity_type) {
        case CANNONBALL:
            distance += CANNONBALL;
            break;
        case MINE:
            distance += MINE;
            break;
    }
    
    return distance;
}
*/
/*
 BFS TO FIND CLOSEST BARREL 
void bfs_find_barrel(struct direction start, int ** map) {
    
    odd-r representation of visited tiles 
    int visited[23][21] = {
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},  
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},  
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},  
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},  
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},  
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},  
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},  
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},  
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},  
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},  
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},  
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},  
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},  
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},  
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},  
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},  
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},  
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},  
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},  
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},  
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}};
    visited[start.hex_location.col][start.hex_location.row] = 1;
    struct queue * frontier;
    enqueue(frontier, start.cube_location, 0);
    
    while (frontier->length > 1) {
        struct cube current;
        dequeue(frontier, &current);
        struct cube neighbours[6] = {
            (struct cube) { .x = 1, .y = -1, .z = 0 }, (struct cube) { .x = 1, .y = 0, .z = -1 },
            (struct cube) { .x = 0, .y = -1, .z = 1 }, (struct cube) { .x = -1, .y = 1, .z = 0 },
            (struct cube) { .x = -1, .y = 0, .z = 1 }, (struct cube) { .x = 0, .y = -1, .z = 1 }};
        get_cube_neighbours(current, neighbours);
        
        for (int i = 0; i < 6; ++i) {
            struct cube next = neighbours[i];
            /*if (visited[next.
        }
    }
}
*/
/* MAIN EXECUTION */
int main()
{
    ships = malloc(sizeof(struct ship));
    barrels = malloc(sizeof(struct barrel)*MAX_BARREL_COUNT);
    
    struct barrel target = (struct barrel) { 
        .location = (struct hex) { .col = 0, .row = 0},
        .amount = -1,
        .entity_id = -1
    };
    
    /* for now only one enemy */
    struct ship enemy;
    
    // game loop
    while (1) {
        // the number of remaining ships
        int my_ship_count;
        scanf("%d", &my_ship_count);
        
        /* Number of barrels in the current turn */
        int barrel_count = 0; 
        
        // the number of entities (e.g. ships, mines or cannonballs)
        int entity_count;
        
        /* get data for each entity */
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
            
            if (!strcmp(entity_type, "SHIP")) {
                if (1 == arg_4) {
                    /* for now there is only ONE ship so let's refer to it directly */
                    ships[0].location = (struct hex) { .col = x, .row = y };
                    ships[0].orientation = arg_1;
                    ships[0].speed = arg_2;
                    ships[0].rum_stock = arg_3;
                } else {
                    enemy.location = (struct hex) { .col = x, .row = y };
                    enemy.orientation = arg_1;
                    enemy.speed = arg_2;
                    enemy.rum_stock = arg_3;
                }
            }
            if (!strcmp(entity_type, "BARREL")) {
                barrels[barrel_count].location= (struct hex) { .col = x, .row = y };
                barrels[barrel_count].amount = arg_1;
                barrels[barrel_count].entity_id = entity_id;
                barrel_count++;
            }
            if (!strcmp(entity_type, "MINE")) {
                map[x][y] = MINE;
            }
            if (!strcmp(entity_type, "CANNONBALL")) { 
                map[x][y] = CANNONBALL;
            }
        }
        /* process everything */
        for (int i = 0; i < my_ship_count; i++) {
            struct ship my_ship = ships[i];
            
            int barrel_index = 0;
            /* start with a ridiculous distance */
            int shortest_distance = 1000000000;
            /* let's calculate the closest barrel to my ship in this turn and get it */
            for (int j = 0; j < barrel_count; ++j) {
                struct barrel current_barrel = barrels[j];
                
                /* our target is still in the map */
                if (target.entity_id == current_barrel.entity_id) break;
                
                /* The coordinate is now a hex grid */
                int distance = offset_distance(
                    my_ship.location, 
                    current_barrel.location); 
                
                if (distance < shortest_distance) {
                    shortest_distance = distance;
                    //barrel_index = j;
                    target = current_barrel;
                }
            }
            
            /* let's determine the enemy's location and shoot em up */
            int enemy_distance = offset_distance(
                    my_ship.location, 
                    enemy.location); 
            
            fprintf(stderr, "enemy distance: %d\n", enemy_distance);                  
            if (enemy_distance <= 5) {
                   printf("FIRE %d %d\n", 
                            enemy.location.col, 
                            enemy.location.row); 
            } else {
                printf("MOVE %d %d\n", 
                            target.location.col, 
                            target.location.row); 
            }
        }
    }
    
    free(barrels);
    free(ships);
    
    return 0;
}
