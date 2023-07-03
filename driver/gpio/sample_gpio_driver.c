#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <asm/io.h>

#define LLL_MAX_USER_SIZE 1024
#define BCM2837_GPIO_ADDRESS 0x3F200000
#define BCM2711_GPIO_ADDRESS 0xfe200000

static struct proc_dir_entry *lll_proc = NULL;
static char data_buffer[LLL_MAX_USER_SIZE+1] = {0};
static size_t data_size = 0;
static unsigned int *gpio_registers = NULL;

static void gpio_pin_on(unsigned int pin)
{
	unsigned int fsel_index = pin/10;
	unsigned int fsel_bitpos = pin%10;
	unsigned int* gpio_fsel = gpio_registers + fsel_index;
	unsigned int* gpio_on_register = (unsigned int*)((char*)gpio_registers + 0x1c);
	unsigned int* gpio_pin_level_reg = (unsigned int*)((char*)gpio_registers + 0x34);

	*gpio_fsel &= ~(7 << (fsel_bitpos*3));
	*gpio_fsel |= (1 << (fsel_bitpos*3));
	unsigned int value = (1<<pin);
	printk("%s: gpio-%02d lvl: %d (0x%0x) -> %x",
			__FUNCTION__, pin, (*gpio_pin_level_reg & (1 << pin))?1:0, *gpio_pin_level_reg, value);
	*gpio_on_register |= value;
	printk("%s: gpio-%02d lvl: %d (0x%0x)",
			__FUNCTION__, pin, (*gpio_pin_level_reg & (1 << pin))?1:0, *gpio_pin_level_reg);
	return;
}

static void gpio_pin_off(unsigned int pin)
{
	unsigned int *gpio_off_register = (unsigned int*)((char*)gpio_registers + 0x28);
	unsigned int* gpio_pin_level_reg = (unsigned int*)((char*)gpio_registers + 0x34);
	printk("%s: gpio-%02d lvl: %d (0x%0x)",
			__FUNCTION__, pin, (*gpio_pin_level_reg & (1 << pin))?1:0, *gpio_pin_level_reg);
	*gpio_off_register |= (1 << pin);
	printk("%s: gpio-%02d lvl: %d (0x%0x)",
			__FUNCTION__, pin, (*gpio_pin_level_reg & (1 << pin))?1:0, *gpio_pin_level_reg);
	return;
}

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
	unsigned int pin = UINT_MAX;
	unsigned int value = UINT_MAX;

	memset(data_buffer, 0x0, sizeof(data_buffer));

	if (size > LLL_MAX_USER_SIZE)
		size = LLL_MAX_USER_SIZE;

	if (copy_from_user(data_buffer, user, size))
		return 0;

	data_size = size;
	printk("Data buffer: %s", data_buffer);

	if (sscanf(data_buffer, "%d,%d", &pin, &value) != 2) {
		printk("Inproper data format submitted\n");
		return size;
	}

	if (pin > 27 || pin < 0) {
		printk("Invalid pin number submitted\n");
		return size;
	}

	if (value != 0 && value != 1) {
		printk("Invalid on/off value\n");
		return size;
	}

	printk("You said pin %d, value %d", pin, value);

	if (value == 1)
		gpio_pin_on(pin);
	else if (value == 0)
		gpio_pin_off(pin);

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

	gpio_registers = (int*)ioremap(BCM2711_GPIO_ADDRESS, PAGE_SIZE);

	printk("gpio_base_address %p", (unsigned int *)gpio_registers);

	if (gpio_registers == NULL) {
		printk("Failed to map GPIO memory to driver\n");
		return -1;
	}

	printk("Successfully mapped in GPIO memory\n");

	lll_proc = proc_create("sample-gpio", 0666, NULL, &lll_proc_fops);

	if (lll_proc == NULL)
		return -1;

	return 0;
}

static void __exit gpio_driver_exit(void)
{
	printk("Leaving my driver!\n");
	iounmap(gpio_registers);
	proc_remove(lll_proc);

	return;
}

module_init(gpio_driver_init);
module_exit(gpio_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Richard Yim");
MODULE_DESCRIPTION("Test of writing drivers for RASPI");
MODULE_VERSION("1.0");
