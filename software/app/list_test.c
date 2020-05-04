#include <hf-risc.h>
#include <list.h>

char mem_pool[8192];

void show_list(struct list *l)
{
	int32_t i;
	int32_t *p;
	
	printf("\nshowing the list...");
	for (i = 0; i < list_count(l); i++){
		p = list_get(l, i);
		if (p)
			printf("\nnode %d: %d", i, *p);
		else
			printf("\nnode %d: (null)", i);
	}
}

int main(void)
{
	int32_t i, a, b, c, d;
	struct list *l;
	
	heap_init((uint32_t *)&mem_pool, sizeof(mem_pool));
	
	l = list_init();
	printf("\nadding 20 nodes to a list...");
	for (i = 0; i < 20; i++)
		if(list_append(l, NULL)) printf("FAIL!");
	printf("\nthe list has %d nodes", list_count(l));
	a = 10;
	b = 15;
	c = 2;
	d = 45;
	printf("\nfilling the 4th, 5th and 17th nodes");
	if (list_set(l, &a, 3)) printf("FAIL");
	if (list_set(l, &b, 4)) printf("FAIL");
	if (list_set(l, &c, 16)) printf("FAIL");
	show_list(l);
	printf("\ninserting a node on the 7th position");
	if (list_insert(l, &d, 6)) printf("FAIL");
	show_list(l);
	printf("\nremoving the 6th, 10th, 11th and 17th nodes");
	if (list_remove(l, 5)) printf("FAIL");
	if (list_remove(l, 9)) printf("FAIL");
	if (list_remove(l, 10)) printf("FAIL");
	if (list_remove(l, 16)) printf("FAIL");
	show_list(l);

	return 0;
}
