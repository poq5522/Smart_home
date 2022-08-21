#include <linux/build-salt.h>
#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

BUILD_SALT;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(.gnu.linkonce.this_module) = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef CONFIG_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif

static const struct modversion_info ____versions[]
__used __section(__versions) = {
	{ 0x3e549f1d, "module_layout" },
	{ 0xb013481f, "platform_driver_unregister" },
	{ 0xd7362d1, "__platform_driver_register" },
	{ 0xd0f859e2, "device_destroy" },
	{ 0xd840a947, "gpiod_put" },
	{ 0x4202ea9c, "class_destroy" },
	{ 0x37a0cba, "kfree" },
	{ 0x6091b333, "unregister_chrdev_region" },
	{ 0x644b2a6e, "cdev_del" },
	{ 0xf1a8b4a9, "device_create" },
	{ 0xb47e05b2, "__class_create" },
	{ 0x24ade49f, "cdev_add" },
	{ 0xe3ec2f2b, "alloc_chrdev_region" },
	{ 0xf5787540, "cdev_init" },
	{ 0x1a8bcf24, "cdev_alloc" },
	{ 0xe96d9d62, "gpiod_get_from_of_node" },
	{ 0x71af8f1, "gpiod_set_value" },
	{ 0xefd6cf06, "__aeabi_unwind_cpp_pr0" },
	{ 0xc5850110, "printk" },
};

MODULE_INFO(depends, "");

