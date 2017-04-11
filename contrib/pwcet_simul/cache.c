#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "pwcet-private.h"
#include "taskdata.h"
#include "cache.h"

void update_state(char *name, state_t *source)
{
	task_entry_t *entry = get_task_info(name);
	task_cache_t *ti = (task_cache_t*) entry->task_data;
	int i, t;
	int tid = entry->task_id;
	/* TODO to improve performance, update only blocks that are used in parametric computations */
	for (t = 0; t < NUM_TASKS; t++) {
		for (i = 0; i < ti->cblock_count; i++) {
			int age_exit = (tid == t) ? ti->dmg.max[i] : CACHE_WAY_COUNT;
			int idx = i*NUM_TASKS + t;
			int a = source->must[idx] + ti->dmg.max[i % CACHE_SET_COUNT];
			if (a > age_exit)
				a = age_exit;
			if (a > CACHE_WAY_COUNT) {
				a = CACHE_WAY_COUNT;
			}
			source->must[idx] = a;
		}
	} 
}

state_t *init_state()
{
	state_t *s = malloc(sizeof(struct state_s));
	int i;
	int t;
	for (t = 0; t < NUM_TASKS; t++) {
		for (i = 0; i < CBLOCK_ID_COUNT; i++)
			s->must[i*NUM_TASKS + t] = CACHE_WAY_COUNT;
	}
	return s;
}



void get_parameter_value(int id, param_value_t *pv, void *data) {
	param_eval_t *pe = data;
	lblockinfo_t *lbi = pe->lbi + id;
#ifdef DEBUG
	printf("Parametric LBlock %d (on set %d, ageing %d) ", id, lbi->set, lbi->ageing);
#endif	
	int idx = (lbi->cblock * CACHE_SET_COUNT + lbi->set)*NUM_TASKS + pe->tid;
	if (pe->state->must[idx] + lbi->ageing < CACHE_WAY_COUNT) {
		pv->ann.loop_id = -1;
		pv->ann.count = 0;
#ifdef DEBUG
		printf("was converted to AH (from: %s)\n", lbi->static_category == SC_NOT_CLASSIFIED ? "NC" : "FM");
#endif	
	} else {
		switch(lbi->static_category) {
			case SC_NOT_CLASSIFIED:
#ifdef DEBUG
				printf("stays NC\n");
#endif	
				pv->ann.loop_id = -1;
				pv->ann.count = -1;
				break;
			case SC_PERSISTENT:
#ifdef DEBUG
				printf("stays FM\n");
#endif	
				pv->ann.loop_id = lbi->loop_id;
				pv->ann.count = 1;
				break;
		}
	}
}

int compute_wcet(char *name, state_t * s)
{
	param_eval_t pe;
	pe.state = s;
	task_entry_t *entry = get_task_info(name);
	task_cache_t *ti = (task_cache_t*) entry->task_data;
#ifdef DEBUG
	if (ti) {
		printf("compute_wcet: Task %s found\n", name);
	} else {
		printf("compute_wcet: Task %s not found\n", name);
		abort();
	}
#endif
	pe.lbi = ti->lbi;
	pe.tid = entry->task_id;
	return evaluate(&ti->task.pwcet, &ti->task.li, get_parameter_value, &pe);
}
