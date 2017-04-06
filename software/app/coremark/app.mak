APP_DIR = $(SRC_DIR)/$(APP)

app: kernel
	$(CC) $(CFLAGS) \
		$(APP_DIR)/core_list_join.c \
		$(APP_DIR)/core_main.c -DFLAGS_STR=\"'${CFLAGS}'\" \
		$(APP_DIR)/core_matrix.c \
		$(APP_DIR)/core_state.c \
		$(APP_DIR)/core_util.c \
		$(APP_DIR)/core_portme.c \
		$(APP_DIR)/coremark.c
