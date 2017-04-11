#ifndef CACHE_H
#define CACHE_H 1
#include "pwcet-private.h"
#include "taskdata.h"

struct lblockinfo_s {
	int cblock; /* OTAWA internal cache-block-id representatiopn (NOT equal to adress/blocksize) */
	int ageing;
#define SC_ALWAYS_MISS		1
#define SC_ALWAYS_HIT		2
#define SC_PERSISTENT		3
#define SC_NOT_CLASSIFIED 	4
	int static_category;
	int loop_id;
	int set; /* Set ID (address/blocksize % blocks_per_set) */
};
typedef struct lblockinfo_s lblockinfo_t;

struct param_eval_s {
	state_t *state;
	lblockinfo_t *lbi;
	int tid;
};
typedef struct param_eval_s param_eval_t;


struct state_s {
	int must[CBLOCK_ID_COUNT*NUM_TASKS];
};

struct damage_s {
	int *max;
	int *must;
};
typedef struct damage_s damage_t;

struct task_cache_s {
	int cblock_count;
	int lblock_count;
	damage_t dmg;
	lblockinfo_t *lbi;
	task_t task;
};
typedef struct task_cache_s task_cache_t;


void apply_damage(state_t *source, damage_t *dmg);

#endif
