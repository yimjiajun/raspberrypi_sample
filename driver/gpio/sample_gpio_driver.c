#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/proc_fs.h>

#define LLL_MAX_USER_SIZE 1024

static struct proc_dir_entry *lll_proc = NULL;
static char data_buffer[LLL_MAX_USER_SIZE+1] = {0};
static size_t data_size = 0;

ssize_t lll_read(struct file *file, char __user *user, size_t size, loff_t *off)
{
	size_t len = data_size;

	if (len == 0)
		return 0;

	data_size = 0;

	return copy_to_user(user, data_buffer, len) ? 0 : len;
}

ssize_t lll_write(struct file *file, const char __user *user, size_t size, loff_t *off)
{
	memset(data_buffer, 0x0, sizeof(data_buffer));

	if (size > LLL_MAX_USER_SIZE)
		size = LLL_MAX_USER_SIZE;

	if (copy_from_user(data_buffer, user, size))
		return 0;

	data_size = size;
	printk("Data buffer: %s", data_buffer);

	return size;
}

static const struct proc_ops lll_proc_fops =
{
	.proc_read = lll_read,
	.proc_write = lll_write,
};

static int __init gpio_driver_init(void)
{
	printk("Welcome to my driver!\n");

	lll_proc = proc_create("sample-gpio", 0666, NULL, &lll_proc_fops);

	if (lll_proc == NULL)
		return -1;

	return 0;
}

static void __exit gpio_driver_exit(void)
{
	printk("Leaving my driver!\n");
	proc_remove(lll_proc);

	return;
}

module_init(gpio_driver_init);
module_exit(gpio_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Richard Yim");
MODULE_DESCRIPTION("Test of writing drivers for RASPI");
MODULE_VERSION("1.0");
