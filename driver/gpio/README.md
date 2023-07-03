# GPIO Driver

# Register

---

In 5.2. Register View from [raspi-4b-bcm2711](https://datasheets.raspberrypi.com/bcm2711/bcm2711-peripherals.pdf)

The GPIO has the following registers. All accesses are assumed to be 32-bit. The GPIO register base address is **0x7e200000**.

# Experiment

----

<br>

1. Install module

        sudo insmod sample_gpio_driver.ko

    * the function will being run after installed module. in _sample\_gpio\_driver.c_:

            module_init(gpio_driver_init);

    * the module init function will run and display kernel message from **printk**

            dmesg | tail -n 3

    * at the same time, create a proc file for this module name as **sample-gpio** in _sample\gpio\_driver.c_:

            lll_proc = proc_create("sample-gpio", 0666, NULL, &lll_proc_fops);

<br>

2. check installed module. the module named as **sample\_gpio\_driver** from installed kernel object.

        lsmod | grep sample_gpio_driver

<br>

3. write the message into module. the proc file name as `sample-gpio` from function **gpio\_driver\_init** in **sample\_gpio\_driver.c**:

        echo "Hello World" > /proc/sample-gpio

    * check kernel message will found the **printk("Data buffer: %s", data_buffer);** display message from write proc file.

            dmesg | tail -n 2

<br>

4. read the message from module

        cat /proc/sample-gpio

<br>

5. remove module

        sudo rmmod sample_gpio_driver
