#ifndef __BSP_MODULE_H
#define __BSP_MODULE_H

#include <bsp_utils.h>

#define BSP_MODULE_NAME_LEN 64

enum bsp_module_state {
	BSP_MODULE_STATE_LIVE,
	BSP_MODULE_STATE_COMING,
	BSP_MODULE_STATE_GOING,
};

struct bsp_module {
	enum bsp_module_state state;
	sys_snode_t node;
	char name[BSP_MODULE_NAME_LEN];
	const char *version;
	void *descriptor;

	int (*setup)(void *);
	int (*desetup)(void *);
	void *(*prepare)(void *);
};

extern sys_slist_t bsp_module_list;

/**
 * @brief Iterate on all modules.
 * @param mod A pointer to peek each entry of the list
 */
#define BSP_MODULE_FOR_EACH(mod) SYS_SLIST_FOR_EACH_CONTAINER(&bsp_module_list, mod, node)

/**
 * @brief Add a module to the module list.
 * 
 * Only desetuped module can be added.
 * 
 * @param module Pointer to the BSP module to remove
 * @return Returns 0 on success, otherwise returns a negtaive error code
 */
static inline int bsp_module_append(struct bsp_module *module)
{
	if (module->state != BSP_MODULE_STATE_LIVE) {
		sys_slist_append(&bsp_module_list, &module->node);
	}
	return -EBUSY;
}

/**
 * @brief Remove a module from the module list.
 * 
 * Only desetuped module can be removed.
 * 
 * @param module Pointer to the BSP module to remove
 * @return Returns 0 on success, otherwise returns a negtaive error code
 */
static inline int bsp_module_remove(struct bsp_module *module)
{
	if (module->state != BSP_MODULE_STATE_LIVE) {
		sys_slist_find_and_remove(&bsp_module_list, &module->node);
		return 0;
	}
	return -EBUSY;
}

static inline void bsp_module_init(void)
{
	sys_slist_init(&bsp_module_list);
}

/**
 * @brief Find a bsp_module by name
 * @param name Name of the bsp_module to find
 * @return Pointer to the bsp_module if found, otherwise returns NULL
 */
static inline struct bsp_module *bsp_module_find(char *name)
{
	struct bsp_module *mod;
	BSP_MODULE_FOR_EACH(mod) {
		if (strncmp(mod->name, name, BSP_MODULE_NAME_LEN) == 0)
			return mod;
	}
	return NULL;
}

// static inline void bsp_module_prapare(struct bsp_module *module, void *params)
// {
// 	module->descriptor = CALL_IF_NOT_NULL_RET_PTR(module->prepare, params);
// 	module->state = BSP_MODULE_STATE_COMING;
// }

// static inline void bsp_module_setup(struct bsp_module *module, void *params)
// {
// 	module->descriptor = CALL_IF_NOT_NULL_RET_PTR(module->setup, params);
// 	module->state = BSP_MODULE_STATE_LIVE;
// }

// static inline void bsp_module_desetup(struct bsp_module *module, void *params)
// {
// 	module->descriptor = CALL_IF_NOT_NULL_RET_PTR(module->desetup, params);
// 	module->state = BSP_MODULE_STATE_GOING;
// }

static inline void bsp_module_prepare_all(void)
{
	struct bsp_module *mod;
	BSP_MODULE_FOR_EACH(mod) {
		void *tmp_des = CALL_IF_NOT_NULL_RET_PTR(mod->prepare, mod->descriptor);
		if (IS_ERR_OR_NULL(tmp_des)) {
			printf("[bsp_module_prepare][%s]: %s (%s) failed to get descriptor\n",
			       strerror(-PTR_ERR(tmp_des)), mod->name, mod->version);
			mod->state = BSP_MODULE_STATE_GOING;
		} else {
			mod->descriptor = tmp_des;
			mod->state = BSP_MODULE_STATE_COMING;
		}
	}
}

static inline void bsp_module_setup_all(void)
{
	struct bsp_module *mod;
	BSP_MODULE_FOR_EACH(mod) {
		if (mod->state != BSP_MODULE_STATE_COMING)
			continue;
		int res = CALL_IF_NOT_NULL_RET_INT(mod->setup, mod->descriptor);
		if (res) {
			printf("[bsp_module_setup][%s]: %s (%s) failed to get descriptor\n", strerror(-res), mod->name,
			       mod->version);
			mod->state = BSP_MODULE_STATE_GOING;
		} else {
			mod->state = BSP_MODULE_STATE_LIVE;
		}
	}
}

static inline void bsp_module_desetup_all(void)
{
	struct bsp_module *mod;
	BSP_MODULE_FOR_EACH(mod) {
		if (mod->state == BSP_MODULE_STATE_GOING)
			continue;
		int res = CALL_IF_NOT_NULL_RET_INT(mod->desetup, mod->descriptor);
		if (res) {
			printf("[bsp_module_desetup][%s]: %s (%s) failed to get descriptor\n", strerror(-res),
			       mod->name, mod->version);
		}
		mod->state = BSP_MODULE_STATE_GOING;
	}
}

#endif
