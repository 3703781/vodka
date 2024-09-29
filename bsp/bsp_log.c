#include <bsp.h>
#include <bsp_log.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>

enum bsp_log_color {
	BSP_LOG_COLOR_NRM = 0,
	BSP_LOG_COLOR_RED,
	BSP_LOG_COLOR_GRN,
	BSP_LOG_COLOR_YEL,
	BSP_LOG_COLOR_BLU,
	BSP_LOG_COLOR_MAG,
	BSP_LOG_COLOR_CYN,
	BSP_LOG_COLOR_WHT,
	BSP_LOG_COLOR_NUM
};

static const char *lvl_name[BSP_LOG_LVL_NUM] = { "TRACE", "DEBUG", "INFO", "WARNING", "ERROR", "CRITICAL", "ALWAYS" };
static const enum bsp_log_color lvl_color[BSP_LOG_LVL_NUM] = { BSP_LOG_COLOR_NRM, BSP_LOG_COLOR_NRM, BSP_LOG_COLOR_NRM,
							       BSP_LOG_COLOR_YEL, BSP_LOG_COLOR_MAG, BSP_LOG_COLOR_RED,
							       BSP_LOG_COLOR_CYN };
static const char *color_str[BSP_LOG_COLOR_NUM] = { "\x1B[0m",	"\x1B[31m", "\x1B[32m", "\x1B[33m",
						    "\x1B[34m", "\x1B[35m", "\x1B[36m", "\x1B[37m" };

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
	SET_VAL_IF_NULL(int_des->_buf2, calloc(int_des->buf_size, 1));
	if (IS_ERR_OR_NULL(int_des->_buf))
		return ERR_PTR(-ENOMEM);
	sys_slist_init(int_des->_subscribers);
	SET_VAL_IF_NULL(int_des->_get_timestamp, bsp_debug_tsg_get);

	return int_des;
}

static void vlog(struct bsp_log_des *des, const char *fmt, va_list arg)
{
	struct bsp_log_subs *subs = NULL;
	des->_level_name = lvl_name[(size_t)des->_level];
	des->_time_stamp = des->get_timestamp();
	des->_color = color_str[(size_t)lvl_color[(size_t)des->_level]];

	des->_str_len = vsnprintf(des->_buf, des->buf_size, fmt, arg);
	if (des->_str_len <= 0)
		return;

	SYS_SLIST_FOR_EACH_CONTAINER(&des->_subscribers, subs, node) {
		if (des->_level >= subs->threshold)
			subs->message(des);
	}
}

void bsp_log_log(struct bsp_log_des *des, enum bsp_log_lvl level, const char *fmt, ...)
{
	va_list ap;
	des->_level = level;
	va_start(ap, fmt);
	vlog(des, fmt, ap);
	va_end(ap);
}

void bsp_log_trace(struct bsp_log_des *des, const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	des->_level = BSP_LOG_LVL_TRACE;
	vlog(des, fmt, ap);
	va_end(ap);
}

void bsp_log_debug(struct bsp_log_des *des, const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	des->_level = BSP_LOG_LVL_DEBUG;
	vlog(des, fmt, ap);
	va_end(ap);
}

void bsp_log_info(struct bsp_log_des *des, const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	des->_level = BSP_LOG_LVL_INFO;
	vlog(des, fmt, ap);
	va_end(ap);
}

void bsp_log_warning(struct bsp_log_des *des, const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	des->_level = BSP_LOG_LVL_WARNING;
	vlog(des, fmt, ap);
	va_end(ap);
}

void bsp_log_error(struct bsp_log_des *des, const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	des->_level = BSP_LOG_LVL_ERROR;
	vlog(des, fmt, ap);
	va_end(ap);
}

void bsp_log_critical(struct bsp_log_des *des, const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	des->_level = BSP_LOG_LVL_CRITICAL;
	vlog(des, fmt, ap);
	va_end(ap);
}

void bsp_log_always(struct bsp_log_des *des, const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	des->_level = BSP_LOG_LVL_ALWAYS;
	vlog(des, fmt, ap);
	va_end(ap);
}

static void tty1(struct bsp_log_des *des)
{
	static struct bsp_module *mod = NULL;
	const char *const level_name = (size_t)des->_level_name;
	if (mod == NULL)
		mod = bsp_module_find("TTY1");

	if (mod && mod->state == BSP_MODULE_STATE_LIVE) {
		struct bsp_tty_des *des = (struct bsp_tty_des *)mod->descriptor;
		vsnprintf(des->_buf2, des->buf_size,
			  "[%f] %s %s %s:", );
		des->ops.write(des, msg, strlen(msg));
		des->ops.write(des, msg, strlen(msg));
	}
}

static void itm_ch1(struct bsp_log_des *des)
{
	for (size_t i = 0; i < count; i++) {
		ITM_SendChar(msg[i], 1);
	}
}

int bsp_log_init(void *des)
{
	struct bsp_log_des *int_des = (struct bsp_log_des *)des;
	if (int_des == NULL)
		return -ENXIO;
	if (bsp_log_subscribe(int_des, tty1, BSP_LOG_LVL_INFO))
		return -EIO;
	if (bsp_log_subscribe(int_des, itm_ch1, BSP_LOG_LVL_INFO))
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
	free(int_des->_buf2);
	int_des->_buf2 = NULL;
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
	if (subs == NULL)
		return -ENOENT;
	sys_slist_find_and_remove(&des->_subscribers, subs);
	free(subs);

	return 0;
}
