#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "pwcet-public.h"
#include "taskdata.h"


void paramvaluation(int id, param_value_t * v)
{
	v->bound = 1000;
	return;
}


state_t *s;

int do_task(char *name) {
	int wcet;
/*	printf("Execute task: %s...", name); */
	wcet = compute_wcet(name, s);
	update_state(name, s);
/*	printf("WCET: %d \n", wcet); */ 
	return wcet;
}

void test_task(char *tname) {
	int i;
	int max=-1;
	int min=99999999;
	int w;
	for (i = 0; i < 10; i++) {
		w = do_task(tname);
		if (w > max)
			max = w;
		if (w < min)
			min = w;
	}
	printf("Task: %s, min=%d, max=%d, diff=%d\n", tname, min, max, (max-min));
}

int main(void)
{
	int i;
	unsigned long delta = 0;
	struct timespec ts, ts2;
	
	pwcet_library_init();


	s = init_state();
	for (i = 0; i < NUM_TASKS; i++) {
		test_task(task_name(i));
	}
	

	free(s);

	return 0;

}

