#include <linux/module.h>
#include <linux/init.h>
#include <linux/gpio.h>
#include <linux/delay.h>
#include <linux/kthread.h>
#include <linux/sched.h>
#include <linux/timer.h>
#include <linux/ftrace.h> 
#include <linux/mm.h>
#include <asm/pgtable.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

#define GPIO_LED 26
static struct task_struct *blink_thread;
static struct timer_list page_fault_timer;

static void tracing_mark_write(const char *fmt, ...) {
    va_list args;
    char buf[256];
    int len;
    struct file *filp;
    loff_t pos = 0;

    va_start(args, fmt);
    len = vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);

    filp = filp_open("/sys/kernel/debug/tracing/trace_marker", O_WRONLY | O_APPEND, 0);
    if (!IS_ERR(filp)) {
        kernel_write(filp, buf, len, &pos);
        filp_close(filp, NULL);
    }
}

/* Callback function to simulate a page fault */
static void page_fault_callback(struct timer_list *t) {
    volatile int *ptr = (int *)0xdeadbeef;
    
    // Log to ftrace before triggering the fault
    trace_printk("GPIO Ftrace: Triggering a page fault by accessing an invalid address.\n");
    
    // This will cause a page fault
    *ptr = 1;
}

static int gpio_thread(void *data) {
    int value = 0;
    while (!kthread_should_stop()) {
        value = !value;
        gpio_set_value(GPIO_LED, value);

        // Use trace_printk to log directly to ftrace
        trace_printk("GPIO_THREAD: GPIO %d set to %d\n", GPIO_LED, value);

        // Use trace_marker to create a special event marker
        tracing_mark_write("GPIO %d", GPIO_LED);
        tracing_mark_write("STATE %d", value);

        msleep(500); // 500ms delay
    }
    return 0;
}

static int __init gpio_ftrace_init(void) {
    int ret;
    pr_info("GPIO Ftrace Demo: module loaded\n");

    ret = gpio_request(GPIO_LED, "gpio_ftrace_led");
    if (ret) {
        pr_err("Failed to request GPIO %d\n", GPIO_LED);
        return ret;
    }
    gpio_direction_output(GPIO_LED, 0);

    blink_thread = kthread_run(gpio_thread, NULL, "gpio_blink_kthread");
    if (IS_ERR(blink_thread)) {
        pr_err("Failed to create kthread\n");
        gpio_free(GPIO_LED);
        return PTR_ERR(blink_thread);
    }
    
    // Initialize timer to trigger a page fault after 5 seconds
    timer_setup(&page_fault_timer, page_fault_callback, 0);
    mod_timer(&page_fault_timer, jiffies + msecs_to_jiffies(5000));
    
    return 0;
}

static void __exit gpio_ftrace_exit(void) {
    kthread_stop(blink_thread);
    del_timer(&page_fault_timer);
    gpio_set_value(GPIO_LED, 0);
    gpio_free(GPIO_LED);
    pr_info("GPIO Ftrace Demo: module unloaded\n");
}

module_init(gpio_ftrace_init);
module_exit(gpio_ftrace_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("GPIO Ftrace Demo Module");