#include <bsp.h>
#include <bsp_log.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>

static const char *lvl_name[BSP_LOG_LVL_NUM] = { "TRACE", "DEBUG", "INFO", "WARNING", "ERROR", "CRITICAL", "ALWAYS" };

int bsp_log_init(void *des);
int bsp_log_uninit(void *des);

struct bsp_module bsp_log_mod = { .name = "LOG",
				  .state = BSP_MODULE_STATE_COMING,
				  .prepare = set_default_des,
				  .setup = bsp_log_init,
				  .desetup = bsp_log_uninit,
				  .descriptor = NULL,
				  .version = "0.1" };

static inline void log(struct bsp_log_des *des, enum bsp_log_lvl level, const char *fmt, ...);
static inline void trace(struct bsp_log_des *des, const char *fmt, ...);
static inline void debug(struct bsp_log_des *des, const char *fmt, ...);
static inline void info(struct bsp_log_des *des, const char *fmt, ...);
static inline void warning(struct bsp_log_des *des, const char *fmt, ...);
static inline void error(struct bsp_log_des *des, const char *fmt, ...);
static inline void critical(struct bsp_log_des *des, const char *fmt, ...);
static inline void always(struct bsp_log_des *des, const char *fmt, ...);

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
	SET_VAL_IF_NULL(int_des->log, log);
	SET_VAL_IF_NULL(int_des->trace, trace);
	SET_VAL_IF_NULL(int_des->debug, debug);
	SET_VAL_IF_NULL(int_des->info, info);
	SET_VAL_IF_NULL(int_des->warning, warning);
	SET_VAL_IF_NULL(int_des->error, error);
	SET_VAL_IF_NULL(int_des->critical, critical);
	SET_VAL_IF_NULL(int_des->always, always);
	SET_VAL_IF_NULL(int_des->get_timestamp, bsp_debug_tsg_get);

	return int_des;
}

static void vlog(struct bsp_log_des *des, enum bsp_log_lvl level, const char *fmt, va_list arg)
{
	struct bsp_log_subs *subs = NULL;

	vsnprintf(des->_buf, des->buf_size, fmt, arg);

	SYS_SLIST_FOR_EACH_CONTAINER(&des->_subscribers, subs, node) {
		if (level >= subs->threshold)
			subs->message(des, level, lvl_name[(size_t)level]);
	}
}

static inline void log(struct bsp_log_des *des, enum bsp_log_lvl level, const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	vlog(des, level, fmt, ap);
	va_end(ap);
}

static inline void trace(struct bsp_log_des *des, const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	vlog(des, BSP_LOG_LVL_TRACE, fmt);
	va_end(ap);
}

static inline void debug(struct bsp_log_des *des, const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	vlog(des, BSP_LOG_LVL_DEBUG, fmt);
	va_end(ap);
}

static inline void info(struct bsp_log_des *des, const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	vlog(des, BSP_LOG_LVL_INFO, fmt);
	va_end(ap);
}

static inline void warning(struct bsp_log_des *des, const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	vlog(des, BSP_LOG_LVL_WARNING, fmt);
	va_end(ap);
}

static inline void error(struct bsp_log_des *des, const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	vlog(des, BSP_LOG_LVL_ERROR, fmt);
	va_end(ap);
}

static inline void critical(struct bsp_log_des *des, const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	vlog(des, BSP_LOG_LVL_CRITICAL, fmt);
	va_end(ap);
}

static inline void always(struct bsp_log_des *des, const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	vlog(des, BSP_LOG_LVL_ALWAYS, fmt);
	va_end(ap);
}

static void message(enum bsp_log_lvl level, char *str_level, char *msg)
{
	static struct bsp_module * mod = NULL;
	(void)level;
	if (mod == NULL)
		mod = bsp_module_find("TTY1");

	if (mod && mod->state == BSP_MODULE_STATE_LIVE) {
		struct bsp_tty_des *des = (struct bsp_tty_des *)mod->descriptor;
		des->ops.write(des, msg, strlen(msg));
	}
}

int bsp_log_init(void *des)
{	#define KNRM  "\x1B[0m"
	#define KRED  "\x1B[31m"
	#define KGRN  "\x1B[32m"
	#define KYEL  "\x1B[33m"
	#define KBLU  "\x1B[34m"
	#define KMAG  "\x1B[35m"
	#define KCYN  "\x1B[36m"
	#define KWHT  "\x1B[37m"
	struct bsp_log_des *int_des = (struct bsp_log_des *)des;
	if (int_des == NULL)
		return -ENXIO;
	if (bsp_log_subscribe(int_des, message, BSP_LOG_LVL_INFO))
		return -EIO;

	return 0;
}

/**
 * @brief Uninitialize log module
 * @param des 
 * @return 
 */
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
	free(des);

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
	if (subs == NULL)
		return -ENOENT;
	sys_slist_find_and_remove(&des->_subscribers, subs);
	free(subs);

	return 0;
}

void bsp_log_printf(struct bsp_log_des *des, enum bsp_log_lvl level, const char *fmt, ...)
{
}
