struct list {
	void *elem;
	struct list *next;
};

struct list *list_init(void);
int32_t list_append(struct list *lst, void *item);
int32_t list_insert(struct list *lst, void *item, int32_t pos);
int32_t list_remove(struct list *lst, int32_t pos);
void *list_get(struct list *lst, int32_t pos);
int32_t list_set(struct list *lst, void *item, int32_t pos);
int32_t list_count(struct list *lst);
