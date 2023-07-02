#!/bin/bash

SAMPLE_GPIO_DRIVER="sample_gpio_driver"

display_tittle(){
	display_output="$1"
	display_width="$(tput cols)"

	echo -e "\e[33m"

	for ((i=0; i<display_width; i++)); do
		echo -n "="
	done

	echo -e "\n\t $display_output"

	for ((i=0; i<display_width; i++)); do
		echo -n "="
	done

	echo -e "\e[0m"
}

display_kernel_message() {
	display_tittle "Kernel Message (printk)"
	dmesg | tail -n $((1 + $(dmesg | wc -l) - $(dmesg | grep -n "$SAMPLE_GPIO_DRIVER" | cut -f 1 -d ':')))
}

display_tittle "Makefile"

make

display_tittle "Install Module"

if ! [[ -f "${SAMPLE_GPIO_DRIVER}.ko" ]]; then
	echo "Error: the kernel object file was not created"
	exit 1
fi

sudo insmod sample_gpio_driver.ko

display_tittle "List of Modules"

lsmod | grep "$SAMPLE_GPIO_DRIVER" -B 2 -A 2

display_tittle "Sample Gpio driver being installed"

display_kernel_message

display_tittle "Remove Module"

sudo rmmod sample_gpio_driver

display_tittle "List of Modules"

lsmod | grep "$SAMPLE_GPIO_DRIVER" -B 2 -A 2

display_kernel_message

display_tittle "Clean"

make clean
