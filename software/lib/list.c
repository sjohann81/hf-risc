#include <hf-risc.h>
#include <list.h>

struct list *list_init(void)
{
	struct list *lst;
	
	lst = (struct list *)malloc(sizeof(struct list));
	
	if (lst){
		lst->next = NULL;
		lst->elem = NULL;
	}

	return lst;
}

int32_t list_append(struct list *lst, void *item)
{
	struct list *t1, *t2;

	t1 = (struct list *)malloc(sizeof(struct list));
	if (t1){
		t1->elem = item;
		t1->next = NULL;
		t2 = lst;

		while (t2->next)
			t2 = t2->next;

		t2->next = t1;

		return 0;
	}else{
		return -1;
	}
}

int32_t list_insert(struct list *lst, void *item, int32_t pos)
{
	struct list *t1, *t2;
	int32_t i = 0;

	t1 = (struct list *)malloc(sizeof(struct list));
	if (t1){
		t1->elem = item;
		t1->next = NULL;
		t2 = lst;

		while (t2->next){
			t2 = t2->next;
			if (++i == pos) break;
		}

		t1->next = t2->next;
		t2->next = t1;

		return 0;
	}else{
		return -1;
	}
}

int32_t list_remove(struct list *lst, int32_t pos)
{
	struct list *t1, *t2;
	int32_t i = 0;

	t1 = lst;
	t2 = t1;
	while ((t1 = t1->next)){
		if (i++ == pos){
			t2->next = t1->next;
			free(t1);
			return 0;
		}
		t2 = t1;
	}
	
	return -1;
}

void *list_get(struct list *lst, int32_t pos)
{
	struct list *t1;
	int32_t i = 0;

	t1 = lst;
	while ((t1 = t1->next)){
		if (i++ == pos)
			return (void *)t1->elem;
	}
	
	return 0;
}

int32_t list_set(struct list *lst, void *item, int32_t pos)
{
	struct list *t1;
	int32_t i = 0;

	t1 = lst;
	while ((t1 = t1->next)){
		if (i++ == pos){
			t1->elem = item;
			return 0;
		}
	}
	
	return -1;
}

int32_t list_count(struct list *lst)
{
	struct list *t1;
	int32_t i = 0;

	t1 = lst;
	while ((t1 = t1->next))
		i++;

	return i;
}
