#ifndef PWCET_PRIVATE_H
#define PWCET_PRIVATE_H 1
#include "pwcet-public.h"

#define ANN_INNER 0
#define ANN_OUTER 1
/*
 * Keep annotation refering to INNER loop, or OUTER loop in case of conflict? 
 * Both cases produce safe WCET, but the choice may affect pessimism. 
 */
#define ANN_CONFLICT_PRIORITY ANN_INNER


struct formula_s;
typedef struct formula_s formula_t;

/* WCET formula operator type */
#define KIND_ALT 	0
#define KIND_SEQ 	1
#define KIND_LOOP 	2
#define KIND_ANN 	3
#define KIND_CONST	4
#define KIND_AWCET  5

#define IDENT_NONE -1


/* Operator data (interpretation depends on operator type) */
union opdata_u {
	int children_count;			/* Number of children for Alt and Seq operators */
	int loop_id;				/* Loop bound for Loop operators */
	annotation_t ann;			/* Annotation for Ann operators */
};
typedef union opdata_u opdata_t;

/* WCET formula representation */
struct formula_s {
	int kind;
	/* 
	 * Identifier for parameters, set to IDENT_NONE if unused.
	 * If param_id != IDENT_NONE, then the parameter type depends on the kind field:
	 * - KIND_AWCET: The parameter represents a full abstract WCET
	 * - KIND_LOOP: The parameter represents a parametric loop bound
	 * - KIND_ANN: The parameter represents a parametric annotation
	 */
	int param_id;
	opdata_t opdata;
	awcet_t aw;
	formula_t *children;
};

/* Represent loop structure of a task */
typedef int (loophierarchy_t) (int l1, int l2);
typedef int (loopbounds_t) (int l1);
struct loopinfo_s {
	loophierarchy_t *hier;
	loopbounds_t *bnd;
};
typedef struct loopinfo_s loopinfo_t;


int evaluate(formula_t *f, loopinfo_t *li, param_valuation_t pv, void *data);
struct evalctx_s {
	loopinfo_t *li;
	param_valuation_t *param_valuation;
	void *pv_data;
};
typedef struct evalctx_s evalctx_t;

/* Task information struct */
struct task_s {
	loopinfo_t li; 
	formula_t pwcet;
};
typedef struct task_s task_t;

struct task_entry_s {
	char *name;
	int task_id;
	void *task_data;
};
typedef struct task_entry_s task_entry_t;



extern int taskdata_taskcount;
extern task_entry_t *taskdata_tasktab[];

void awcet_seq(evalctx_t * ctx, int source_count, formula_t * source,
			   awcet_t * dest);
void awcet_alt(evalctx_t * ctx, int source_count, formula_t * source,
			   awcet_t * dest);
void awcet_loop(evalctx_t * ctx, awcet_t * source, formula_t * dest);
void awcet_ann(evalctx_t * ctx, awcet_t * source, formula_t * dest);
int awcet_is_equal(awcet_t * s1, awcet_t * s2);
void compute_node(evalctx_t * ctx, formula_t * f);
void partial_eval(formula_t * f);
task_entry_t *get_task_info(char *name);

#endif
