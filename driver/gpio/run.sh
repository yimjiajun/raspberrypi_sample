#!/bin/bash

SAMPLE_GPIO_DRIVER="sample_gpio_driver"
SAMPLE_GPIO_PROC="sample-gpio"
dmesg_prev_line="$(dmesg | wc -l)"

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
	dmesg | tail -n $(($(dmesg | wc -l) - $dmesg_prev_line))
	dmesg_prev_line="$(dmesg | wc -l)"
}

display_tittle "Makefile"

make

display_tittle "Install Module"

if ! [[ -f "${SAMPLE_GPIO_DRIVER}.ko" ]]; then
	echo "Error: the kernel object file was not created"
	exit 1
fi

if [[ $(lsmod | grep -c "$SAMPLE_GPIO_DRIVER") -ne 0 ]]; then
	sudo rmmod $SAMPLE_GPIO_DRIVER
fi

sudo insmod ${SAMPLE_GPIO_DRIVER}.ko

display_tittle "List of Modules"

lsmod | grep "$SAMPLE_GPIO_DRIVER" -B 2 -A 2

display_tittle "List of procfs files"

ls -l /proc | grep "$SAMPLE_GPIO_PROC" -B 1 -A 1

display_kernel_message

display_tittle "Write procfs file"

WR_PROC_MSG="Hello World"
echo $WR_PROC_MSG ; echo $WR_PROC_MSG > /proc/"$SAMPLE_GPIO_PROC"

display_tittle "Read procfs file"

cat /proc/"$SAMPLE_GPIO_PROC"

display_tittle "Remove Module"

sudo rmmod $SAMPLE_GPIO_DRIVER

lsmod | grep "$SAMPLE_GPIO_DRIVER" -B 2 -A 2

display_kernel_message

display_tittle "Clean"

make clean
