#include <stdio.h>
#include <stdlib.h>
#include "pwcet-private.h"

void display_awcet(awcet_t * aw)
{
	int i;
	printf("loop=%d, eta[]={", aw->loop_id);
	for (i = 0; i < aw->eta_count; i++)
		printf("%d, ", aw->eta[i]);
	printf("}, others=%d", aw->others);
}

#define TESTS_COUNT 15


formula_t tests[TESTS_COUNT] = {
	{							/* Test 0 */
	 KIND_ALT, -1,
	 {2}
	 ,							/* Children count */
	 {							/* awcet (result placeholder) */
	  0,						/* loop id */
	  7,						/* eta count */
	  (int[7]) {},				/* eta values */
	  0							/* others */
	  },
	 (formula_t[2]) {
					 {			/* Child 1 */

					  KIND_CONST, -1,
					  {0},
					  {
					   1,
					   3,
					   (int[3]) {30, 25, 20},
					   10,
					   },
					  NULL},
					 {			/* Child 2 */

					  KIND_CONST, -1,
					  {0},
					  {
					   1,
					   4,
					   (int[4]) {60, 50, 40, 18},
					   15,
					   },
					  NULL},
					 },			/* children nodes */
	 },
	{							/* Test 1 */
	 KIND_ALT, -1,
	 {2},						/* Children count */
	 {							/* awcet (result placeholder) */
	  0,						/* loop id */
	  7,						/* eta count */
	  (int[7]) {},				/* eta values */
	  0							/* others */
	  },
	 (formula_t[2]) {
					 {			/* Child 1 */

					  KIND_CONST, -1,
					  {0},
					  {
					   1,
					   3,
					   (int[3]) {30, 25, 20},
					   10,
					   },
					  NULL},
					 {			/* Child 2 */

					  KIND_CONST, -1,
					  {0},
					  {
					   2,
					   4,
					   (int[4]) {60, 50, 40, 18},
					   15,
					   },
					  NULL},
					 },			/* children nodes */
	 },
	{							/* Test 2 */
	 KIND_SEQ, -1,
	 {2},						/* Children count */
	 {							/* awcet (result placeholder) */
	  0,						/* loop id */
	  4,						/* eta count */
	  (int[4]) {},				/* eta values */
	  0							/* others */
	  },
	 (formula_t[2]) {
					 {			/* Child 1 */

					  KIND_CONST, -1,
					  {0},
					  {
					   1,
					   3,
					   (int[3]) {30, 25, 20},
					   10,
					   },
					  NULL},
					 {			/* Child 2 */

					  KIND_CONST, -1,
					  {0},
					  {
					   1,
					   4,
					   (int[4]) {60, 50, 40, 18},
					   15,
					   },
					  NULL},
					 },			/* children nodes */
	 },
	{							/* Test 3 */
	 KIND_SEQ, -1,
	 {2},						/* Children count */
	 {							/* awcet (result placeholder) */
	  0,						/* loop id */
	  4,						/* eta count */
	  (int[4]) {},				/* eta values */
	  0							/* others */
	  },
	 (formula_t[2]) {
					 {			/* Child 1 */

					  KIND_CONST, -1,
					  {0},
					  {
					   1,
					   3,
					   (int[3]) {30, 25, 20},
					   10,
					   },
					  NULL},
					 {			/* Child 2 */

					  KIND_CONST, -1,
					  {0},
					  {
					   2,
					   4,
					   (int[4]) {60, 50, 40, 18},
					   15,
					   },
					  NULL},
					 },			/* children nodes */
	 },
	{							/* Test 4 */
	 KIND_LOOP, -1,
	 {2},						/* Loop id */
	 {							/* awcet (result placeholder) */
	  0,						/* loop id */
	  0,						/* eta count */
	  (int[1]) {},				/* eta values */
	  0							/* others */
	  },
	 (formula_t[1]) {
					 {			/* Child 1 */

					  KIND_CONST, -1,
					  {0},
					  {
					   2,		/* loop id */
					   3,
					   (int[3]) {30, 25, 20},
					   10,
					   },
					  NULL},
					 },			/* children nodes */
	 },
	{							/* Test 5 */
	 KIND_LOOP, -1,
	 {2},						/* Loop id */
	 {							/* awcet (result placeholder) */
	  0,						/* loop id */
	  0,						/* eta count */
	  (int[1]) {},				/* eta values */
	  0							/* others */
	  },
	 (formula_t[1]) {
					 {			/* Child 1 */

					  KIND_CONST, -1,
					  {0},
					  {
					   1,		/* loop id */
					   3,
					   (int[3]) {30, 25, 20},
					   10,
					   },
					  NULL},
					 },			/* children nodes */
	 },
	{							/* Test 6 */
	 KIND_LOOP, -1,
	 {2},						/* Loop id */
	 {							/* awcet (result placeholder) */
	  0,						/* loop id */
	  0,						/* eta count */
	  (int[1]) {},				/* eta values */
	  0							/* others */
	  },
	 (formula_t[1]) {
					 {			/* Child 1 */

					  KIND_CONST, -1,
					  {0},
					  {
					   1,		/* loop id */
					   6,
					   (int[6]) {30, 25, 20, 18, 15, 12},
					   10,
					   },
					  NULL},
					 },			/* children nodes */
	 },
	{							/* Test 7 */
	 KIND_LOOP, -1,
	 {2},						/* Loop id */

	 {							/* awcet (result placeholder) */
	  0,						/* loop id */
	  0,						/* eta count */
	  (int[1]) {},				/* eta values */
	  0							/* others */
	  },
	 (formula_t[1]) {
					 {			/* Child 1 */

					  KIND_CONST, -1,
					  {0},
					  {
					   1,		/* loop id */
					   5,
					   (int[6]) {30, 25, 20, 18, 15,},
					   10,
					   },
					  NULL},
					 },			/* children nodes */
	 },
	{							/* Test 8 */
	 KIND_LOOP, -1,
	 {2},						/* Loop id */
	 {							/* awcet (result placeholder) */
	  0,						/* loop id */
	  0,						/* eta count */
	  (int[1]) {},				/* eta values */
	  0							/* others */
	  },
	 (formula_t[1]) {
					 {			/* Child 1 */

					  KIND_CONST, -1,
					  {0},
					  {
					   1,		/* loop id */
					   2,
					   (int[3]) {30, 25,},
					   10,
					   },
					  NULL},
					 },			/* children nodes */
	 },
	{							/* Test 9 */
	 KIND_LOOP, -1,
	 {2},						/* Loop id */
	 {							/* awcet (result placeholder) */
	  0,						/* loop id */
	  0,						/* eta count */
	  (int[1]) {},				/* eta values */
	  0							/* others */
	  },
	 (formula_t[1]) {
					 {			/* Child 1 */

					  KIND_CONST, -1,
					  {0},
					  {
					   2,		/* loop id */
					   4,
					   (int[4]) {30, 25, 20, 15},
					   10,
					   },
					  NULL},
					 },			/* children nodes */
	 },
	{							/* Test 10 */
	 KIND_LOOP, -1,
	 {2},						/* Loop id */
	 {							/* awcet (result placeholder) */
	  0,						/* loop id */
	  0,						/* eta count */
	  (int[1]) {},				/* eta values */
	  0							/* others */
	  },
	 (formula_t[1]) {
					 {			/* Child 1 */

					  KIND_CONST, -1,
					  {0},
					  {
					   2,		/* loop id */
					   2,
					   (int[2]) {30, 25,},
					   10,
					   },
					  NULL},
					 },			/* children nodes */
	 },
	{							/* Test 11 */
	 KIND_ANN, -1,
	 {.ann = {1, 3}},			/* Loop id, Exec Limit */
	 {							/* awcet (result placeholder) */
	  0,						/* loop id */
	  0,						/* eta count */
	  (int[3]) {},				/* eta values */
	  0							/* others */
	  },
	 (formula_t[1]) {
					 {			/* Child 1 */

					  KIND_CONST, -1,
					  {0},
					  {
					   -1,		/* loop id */
					   0,
					   (int[1]) {},
					   10,
					   },
					  NULL},
					 },			/* children nodes */
	 },
	{							/* Test 12 */
	 KIND_ANN, -1,
	 {.ann = {1, 3}},			/* Loop id, Exec Limit */
	 {							/* awcet (result placeholder) */
	  0,						/* loop id */
	  0,						/* eta count */
	  (int[3]) {},				/* eta values */
	  0							/* others */
	  },
	 (formula_t[1]) {
					 {			/* Child 1 */

					  KIND_CONST, -1,
					  {0},
					  {
					   2,		/* loop id */
					   5,
					   (int[5]) {15, 14, 13, 12, 11},
					   0,
					   },
					  NULL},
					 },			/* children nodes */
	 },
	{							/* Test 13 */
	 KIND_ANN, -1,
	 {.ann = {2, 3}},			/* Loop id, Exec Limit */
	 {							/* awcet (result placeholder) */
	  0,						/* loop id */
	  0,						/* eta count */
	  (int[3]) {},				/* eta values */
	  0							/* others */
	  },
	 (formula_t[1]) {
					 {			/* Child 1 */

					  KIND_CONST, -1,
					  {0},
					  {
					   1,		/* loop id */
					   5,
					   (int[5]) {15, 14, 13, 12, 11},
					   0,
					   },
					  NULL},
					 },			/* children nodes */
	 },
	{							/* Test 14 */
	 KIND_ANN, -1,
	 {.ann = {2, 3}},			/* Loop id, Exec Limit */
	 {							/* awcet (result placeholder) */
	  0,						/* loop id */
	  0,						/* eta count */
	  (int[3]) {},				/* eta values */
	  0							/* others */
	  },
	 (formula_t[1]) {
					 {			/* Child 1 */

					  KIND_CONST, -1,
					  {0},
					  {
					   1,		/* loop id */
					   2,
					   (int[5]) {15, 14,},
					   0,
					   },
					  NULL},
					 },			/* children nodes */
	 },
};

awcet_t results[TESTS_COUNT] = {
	{							/* Expected test result 0 */
	 1,							/* loop id */
	 7,							/* eta count */
	 (int[]) {60, 50, 40, 30, 25, 20, 18},	/* eta values */
	 15,						/* others */
	 },
	{							/* Expected test result 1 */
	 2,							/* loop id */
	 7,							/* eta count */
	 (int[]) {60, 50, 40, 30, 25, 20, 18},	/* eta values */
	 15,						/* others */
	 },
	{							/* Expected test result 2 */
	 1,							/* loop id */
	 4,							/* eta count */
	 (int[]) {90, 75, 60, 28,},	/* eta values */
	 25,						/* others */
	 },
	{							/* Expected test result 3 */
	 2,							/* loop id */
	 4,							/* eta count */
	 (int[]) {90, 75, 60, 28,},	/* eta values */
	 25,						/* others */
	 },
	{							/* Expected test result 4 */
	 -1,						/* loop id */
	 0,							/* eta count */
	 (int[1]) {0},				/* eta values */
	 75,						/* others */
	 },
	{							/* Expected test result 5 */
	 1,							/* loop id */
	 1,							/* eta count */
	 (int[1]) {75,},			/* eta values */
	 30,						/* others */
	 },
	{							/* Expected test result 6 */
	 1,							/* loop id */
	 2,							/* eta count */
	 (int[2]) {75, 45,},		/* eta values */
	 30,						/* others */
	 },
	{							/* Expected test result 7 */
	 1,							/* loop id */
	 2,							/* eta count */
	 (int[2]) {75, 43,},		/* eta values */
	 30,						/* others */
	 },
	{							/* Expected test result 8 */
	 1,							/* loop id */
	 1,							/* eta count */
	 (int[1]) {65,},			/* eta values */
	 30,						/* others */
	 },
	{							/* Expected test result 9 */
	 -1,						/* loop id */
	 0,							/* eta count */
	 (int[1]) {},				/* eta values */
	 75,						/* others */
	 },
	{							/* Expected test result 10 */
	 -1,						/* loop id */
	 0,							/* eta count */
	 (int[1]) {},				/* eta values */
	 65,						/* others */
	 },
	{							/* Expected test result 11 */
	 1,							/* loop id */
	 3,							/* eta count */
	 (int[3]) {10, 10, 10},		/* eta values */
	 0,							/* others */
	 },
	{							/* Expected test result 12 */
	 2,							/* loop id */
	 3,							/* eta count */
	 (int[3]) {15, 14, 13},		/* eta values */
	 0,							/* others */
	 },
#if ANN_CONFLICT_PRIORITY == ANN_OUTER
	{							/* Expected test result 13 */
	 1,							/* loop id */
	 5,							/* eta count */
	 (int[5]) {15, 14, 13, 12, 11},	/* eta values */
	 0,							/* others */
	 },
#else
	{							/* Expected test result 13 */
	 2,							/* loop id */
	 3,							/* eta count */
	 (int[3]) {15, 14, 13,},	/* eta values */
	 0,							/* others */
	 },
#endif
	{							/* Expected test result 14 */
	 1,							/* loop id */
	 2,							/* eta count */
	 (int[2]) {15, 14,},		/* eta values */
	 0,							/* others */
	 },
};

char *testname[TESTS_COUNT] = {
	"ALT same loop",
	"ALT different loop",
	"SEQ same loop",
	"SEQ different loop",
	"LOOP same loop (eta_count = loop_bound)",
	"LOOP different loop (eta_count = loop_bound)",
	"LOOP different loop (eta_count > loop_bound, multiple)",
	"LOOP different loop (eta_count > loop_bound, not multiple)",
	"LOOP different loop (eta_count < loop_bound)",
	"LOOP same loop (eta_count > loop_bound)",
	"LOOP same loop (eta_count < loop_bound)",
	"ANN without previous annotation",
	"ANN with previous inner loop annotation",
	"ANN with previous outer loop annotation, ann.count < source->eta_count",
	"ANN with previous outer loop annotation, ann.count >= source->eta_count",
};


/* Loop1 ==> Loop2 */
int loophierarchy(int l1, int l2)
{
	if ((l1 == 1) && (l2 == 1))
		return 1;
	if ((l1 == 2) && (l2 == 2))
		return 1;
	if ((l1 == 2) && (l2 == 1))
		return 1;
	if ((l2 == 2) && (l1 == 1))
		return 0;
	abort();
}

int loopbounds(int l1)
{
	switch (l1) {
		case 1:
			return 10;
		case 2:
			return 3;
		default:
			abort();
	}
}


loopinfo_t li = {
	loophierarchy,
	loopbounds,
};

int main(void)
{
	int i;
	int failed = 0;
	evalctx_t ctx;
	ctx.li = &li;


	printf("Running %d unit tests...\n", TESTS_COUNT);
	for (i = 0; i < TESTS_COUNT; i++) {
		printf("Running unit test: %s (%u) ... ", testname[i], i);
		compute_node(&ctx, &tests[i]);
		if (awcet_is_equal(&tests[i].aw, &results[i])) {
			printf("passed!\n");
		} else {
			printf("failed!\n\nTest failed: %s (%u):\n  Computed result: ",
				   testname[i], i);
			display_awcet(&tests[i].aw);
			printf("\n");
			printf("  Expected result: ");
			display_awcet(&results[i]);
			printf("\n\n");
			failed++;
		}
	}
	if (!failed) {
		printf("All tests passed.\n");
		return 0;
	} else {
		printf("%d tests (of %d) failed\n", failed, TESTS_COUNT);
		return 1;
	}
}
