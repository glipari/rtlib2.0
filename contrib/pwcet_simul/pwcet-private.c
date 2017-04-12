#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "pwcet-private.h"
#include "taskdata.h"

int my_strcmp(char *str1, char *str2)
{
	int i;
	for (i = 0; i < MAX_TASK_NAME; i++) {
		if (str1[i] != str2[i])
			return 1;
		if (str1[i] == 0)
			break;
	}
	return 0;
}

task_entry_t *get_task_info(char *name)
{
	int i;
	for (i = 0; (i < NUM_TASKS); i++) {
		if (!my_strcmp(taskdata_tasktab[i]->name, name))
			return taskdata_tasktab[i];
	}
	return NULL;
}

char *task_name(int i) {
	return taskdata_tasktab[i]->name;
}


int computation_time(char *name)
{
	//printf("pas implem\n");
	return 0;
}

void compute_node(evalctx_t * ctx, formula_t * f)
{
	int i;
	switch (f->kind) {
		case KIND_SEQ:
#ifdef DEBUG
			printf("compute_node: start processing SEQ node\n");
#endif
			for (i = 0; i < f->opdata.children_count; i++)
				compute_node(ctx, &f->children[i]);
			awcet_seq(ctx, f->opdata.children_count, f->children, &f->aw);
#ifdef DEBUG
			printf("compute_node: end processing SEQ node\n");
#endif
			break;
		case KIND_ALT:
#ifdef DEBUG
			printf("compute_node: start processing ALT node\n");
#endif
			for (i = 0; i < f->opdata.children_count; i++)
				compute_node(ctx, &f->children[i]);
			awcet_alt(ctx, f->opdata.children_count, f->children, &f->aw);
#ifdef DEBUG
			printf("compute_node: end processing ALT node\n");
#endif
			break;
		case KIND_LOOP:
#ifdef DEBUG
			printf("compute_node: start processing LOOP node\n");
#endif
			compute_node(ctx, &f->children[0]);
			awcet_loop(ctx, &f->children->aw, f);
#ifdef DEBUG
			printf("compute_node: end processing LOOP node\n");
#endif
			break;
		case KIND_ANN:
#ifdef DEBUG
			printf("compute_node: start processing ANN node\n");
#endif
			compute_node(ctx, &f->children[0]);
			awcet_ann(ctx, &f->children->aw, f);
#ifdef DEBUG
			printf("compute_node: end processing ANN node\n");
#endif
			break;
		case KIND_AWCET:
#ifdef DEBUG
			printf("compute_node: processing AWCET node\n");
#endif
			ctx->param_valuation(f->param_id, (union param_value_u*)&f->aw, ctx->pv_data);
			break;
		case KIND_CONST:
#ifdef DEBUG
			printf("compute_node: processing CONST node\n");
#endif
			break;
#ifdef DEBUG
		default:
			printf("compute_node: unknown node type %d\n", f->kind);
			abort();
#endif
	}
#ifdef DEBUG
	printf("compute_node: loop=%d, eta[]={", f->aw.loop_id);
	for (i = 0; i < f->aw.eta_count; i++)
		printf("%d, ", f->aw.eta[i]);
	printf("}, others=%d\n", f->aw.others);

#endif
}

int evaluate(formula_t *f, loopinfo_t *li, param_valuation_t pv, void *data)
{
	evalctx_t ctx;
	ctx.li = li;
	ctx.param_valuation = pv;
	ctx.pv_data = data;
	compute_node(&ctx, f);
	if (f->aw.eta_count == 0) {
		return f->aw.others;
	} else {
		return f->aw.eta[0];
	}
}

static int loop_inner(loopinfo_t * li, int inner_id, int outer_id)
{
	if ((inner_id == outer_id) || (inner_id == -1))
		return 0;
	if (outer_id == -1)
		return 1;
	return (li->hier) (inner_id, outer_id);
}

static int loop_bound(loopinfo_t * li, int loop_id)
{
	return (li->bnd) (loop_id);
}

void awcet_seq(evalctx_t * ctx, int source_count, formula_t * source,
			   awcet_t * dest)
{
	int inner_loop = -1;
	int i, j;
	dest->others = 0;
	for (i = 0; i < source_count; i++) {
		if ((inner_loop == -1)
			|| loop_inner(ctx->li, source[i].aw.loop_id, inner_loop))
			inner_loop = source[i].aw.loop_id;
		dest->others += source[i].aw.others;
		if (dest->eta_count < source[i].aw.eta_count)
			dest->eta_count = source[i].aw.eta_count;
	}
	dest->loop_id = inner_loop;
	memset(dest->eta, 0, sizeof(int) * dest->eta_count);
	for (j = 0; j < dest->eta_count; j++) {
		int *plop = &dest->eta[j];
		for (i = 0; i < source_count; i++) {
			*plop +=
				(source[i].aw.eta_count >
				 j) ? source[i].aw.eta[j] : source[i].aw.others;
		}
	}
}

void awcet_alt(evalctx_t * ctx, int source_count, formula_t * source,
			   awcet_t * dest)
{
	int i, temp_max_source, dest_idx;
	int temp_max;
	int inner_loop = -1;
	static int tab[ALT_MAX];
	dest->others = -1;
	dest->eta_count = 0;
	for (i = 0; i < source_count; i++) {
		if ((inner_loop == -1)
			|| loop_inner(ctx->li, source[i].aw.loop_id, inner_loop))
			inner_loop = source[i].aw.loop_id;
		if (dest->others < source[i].aw.others)
			dest->others = source[i].aw.others;
	}
	dest->loop_id = inner_loop;

	dest_idx = 0;
	memset(tab, 0, sizeof(tab));

	/* Loop iteration count bounded by sum[i=1..source_count](source[i].eta_count) */
	while (1) {
		temp_max = -1;
		for (i = 0; i < source_count; i++) {
			if (tab[i] == source[i].aw.eta_count)
				continue;
			if (source[i].aw.eta[tab[i]] <= dest->others) {
				continue;
			}
			if (source[i].aw.eta[tab[i]] > temp_max) {
				temp_max = source[i].aw.eta[tab[i]];
				temp_max_source = i;
			}
		}
		if (temp_max == -1)
			break;
		dest->eta[dest_idx] = temp_max;
		dest_idx++;
		tab[temp_max_source]++;
		dest->eta_count++;
	}
}

void awcet_loop(evalctx_t * ctx, awcet_t * source, formula_t * dest)
{
	int i, j;
	int bound;
	int loop_wcet = 0;
	param_value_t pv;
	if (dest->param_id != IDENT_NONE) {
		ctx->param_valuation(dest->param_id, &pv, ctx->pv_data);
		bound = pv.bound;
	} else {
		bound = loop_bound(ctx->li, dest->opdata.loop_id);
	}
	if (source->loop_id == dest->opdata.loop_id) {
		for (i = 0; (i < bound) && (i < source->eta_count); i++)
			loop_wcet += source->eta[i];
		if (i < bound)
			loop_wcet += (bound - i) * source->others;
		dest->aw.others = loop_wcet;
		dest->aw.eta_count = 0;
		dest->aw.loop_id = LOOP_TOP;
	} else {
		dest->aw.loop_id = source->loop_id;
		dest->aw.eta_count = source->eta_count / bound;
		dest->aw.others = source->others * bound;
		if (source->eta_count % bound)
			dest->aw.eta_count++;

		for (i = 0; i < dest->aw.eta_count; i++) {
			loop_wcet = 0;
			for (j = 0; j < bound; j++) {
				if ((j + i * bound) < source->eta_count) {
					loop_wcet += source->eta[j + i * bound];
				} else
					loop_wcet += source->others;
			}
			dest->aw.eta[i] = loop_wcet;
		}

	}
}

void awcet_ann(evalctx_t * ctx, awcet_t * source, formula_t * dest)
{
	int i;
	int inner_ann_takeover = 0;
	param_value_t pv;
	annotation_t *ann;

	if (dest->param_id != IDENT_NONE) {
		ctx->param_valuation(dest->param_id, &pv, ctx->pv_data);
		ann = &pv.ann;
	} else {
		ann = &dest->opdata.ann;
	}

	if (ann->count == -1) { 
		memcpy(&dest->aw, source, sizeof(awcet_t));
		return;
	}

	if ((source->eta_count != 0)
		&& loop_inner(ctx->li, ann->loop_id, source->loop_id)) {
		if ((ANN_CONFLICT_PRIORITY == ANN_INNER)
			&& (ann->count < source->eta_count)) {
			inner_ann_takeover = 1;
		} else {
			memcpy(&dest->aw, source, sizeof(awcet_t));
			return;
		}
	}

	dest->aw.eta_count = ann->count;
	for (i = 0; i < dest->aw.eta_count; i++) {
		if (i < source->eta_count) {
			dest->aw.eta[i] = source->eta[i];
		} else {
			dest->aw.eta[i] = source->others;
		}
	}
	dest->aw.others = 0;
	if ((source->eta_count == 0) || (inner_ann_takeover == 1)) {
		dest->aw.loop_id = ann->loop_id;
	} else
		dest->aw.loop_id = source->loop_id;
}

void partial_eval(formula_t * f)
{
}

int awcet_is_equal(awcet_t * s1, awcet_t * s2)
{
	int i;

	if (s1->eta_count != s2->eta_count)
		return 0;
	if (s1->loop_id != s2->loop_id)
		return 0;
	for (i = 0; i < s1->eta_count; i++) {
		if (s1->eta[i] != s2->eta[i])
			return 0;
	}
	return s1->others == s2->others;

}

void pwcet_library_init() {
	int i;
	for (i = 0; i < NUM_TASKS; i++) {
		taskdata_tasktab[i]->task_id = i;
	}
}
