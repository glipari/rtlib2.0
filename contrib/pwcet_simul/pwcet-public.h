#ifndef PWCETPUBLIC_H
#define PWCETPUBLIC_H 1



/* #define DEBUG 1 */ 

#define MAX_TASK_NAME 64		/* Maximum task name (including \0) */


struct state_s;
typedef struct state_s state_t;

extern state_t *init_state();
extern void update_state(char *taskname, state_t * s);
extern int compute_wcet(char *taskname, state_t * s);
extern int computation_time(char *taskname);
extern void pwcet_library_init();
extern char* task_name(int i);

/* For testing parameter instantiation only */

/* Abstract WCET representation */
struct awcet_s {
#define LOOP_TOP 	-1
	int loop_id;
	int eta_count;
	int *eta;
	int others;
};
typedef struct awcet_s awcet_t;


/* Structure representing a context annotation */
struct annotation_s {
	int loop_id;
	int count;
};
typedef struct annotation_s annotation_t;

union param_value_u {
	int bound;
	annotation_t ann;
	awcet_t aw;
};
typedef union param_value_u param_value_t;

typedef void (param_valuation_t) (int param_id, param_value_t * param_val, void *data);

#endif
