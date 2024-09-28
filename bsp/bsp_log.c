#include <bsp.h>
#include <bsp_log.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>

int bsp_log_init(void *des);
int bsp_log_uninit(void *des);

struct bsp_module bsp_log_mod = { .name = "LOG",
				  .state = BSP_MODULE_STATE_COMING,
				  .prepare = set_default_des,
				  .setup = bsp_log_init,
				  .desetup = bsp_log_uninit,
				  .descriptor = NULL,
				  .version = "0.1" };

/**
 * @brief Initialize uninitialized members in the log descriptor with default values.
 * 
 * @param des log descriptor, set to NULL to alloc the memory of the descriptor automatically.
 * @return struct bsp_log_des * Pointer to descriptor on success, otherwise negative error code.
 */
static void *set_default_des(void *des)
{
	struct bsp_log_des *int_des = (struct bsp_log_des *)des;

	if (int_des == NULL) {
		int_des = (struct bsp_log_des *)calloc(sizeof(struct bsp_log_des), 1);
		if (IS_ERR_OR_NULL(int_des))
			return ERR_PTR(-ENOMEM);
	}

	/* set to default value */
	SET_VAL_IF_ZERO(int_des->buf_size, 4096);
	SET_VAL_IF_NULL(int_des->_buf, calloc(int_des->buf_size, 1));
	if (IS_ERR_OR_NULL(int_des->_buf))
		return ERR_PTR(-ENOMEM);
	sys_slist_init(int_des->_subscribers);

	return int_des;
}

int bsp_log_init(void *des)
{
	return 0;
}

int bsp_log_uninit(void *des)
{
	struct bsp_log_des *int_des = (struct bsp_log_des *)des;
	free(int_des->_buf);
	int_des->_buf = NULL;
	sys_snode_t *node;
	struct bsp_log_subs *subs;
	while (node = sys_slist_get(&int_des->_subscribers)) {
		free(SYS_SLIST_CONTAINER(node, subs, node));
	}

	return 0;
}

/**
 * @brief Search the subscribers table to install or update fn
 */
int bsp_log_subscribe(struct bsp_log_des *des, bsp_log_msg_fn fn, enum bsp_log_lvl threshold)
{
	struct bsp_log_subs *subscriber = malloc(sizeof(struct bsp_log_subs));
	subscriber->write = fn;
	subscriber->threshold = threshold;
	sys_slist_append(&des->_subscribers, &subscriber->node);

	return 0;
}

/**
 * @brief search the subscribers table to remove
 */
int bsp_log_unsubscribe(struct bsp_log_des *des, bsp_log_function_t fn)
{
	int i;
	struct bsp_log_subs *subs = NULL;
	SYS_SLIST_FOR_EACH_CONTAINER(&des->_subscribers, subs, node) {
		if (subs->write == fn)
			break;
	}
	if (subs == MULL)
		return -ENOENT;
	sys_slist_find_and_remove(&des->_subscribers, subs);
	free(subs);

	return 0;
}

static const char *lvl_name[BSP_LOG_LVL_NUM] = { "TRACE", "DEBUG", "INFO", "WARNING", "ERROR", "CRITICAL", "ALWAYS" };

void bsp_log_printf(struct bsp_log_des *des, enum bsp_log_lvl level, const char *fmt, ...)
{
	va_list ap;
	struct bsp_log_subs *subs = NULL;
	va_start(ap, fmt);
	vsnprintf(des->_buf, des->buf_size, fmt, ap);
	va_end(ap);

	SYS_SLIST_FOR_EACH_CONTAINER(&des->_subscribers, subs, node) {
		if (level >= subs->threshold)
			subs->message(des, level, get_lvl_name(level));
	}
}
