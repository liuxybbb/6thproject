#include <linux/kobject.h>
#include <linux/string.h>
#include <linux/sysfs.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/timer.h>
#include <linux/delay.h>


struct timer_list timer;

static long delay;

static void timer_function(unsigned long data)
{
    printk(KERN_INFO "Hello, World!\n");
    mod_timer(&timer, jiffies + delay * HZ);
    if (delay == 0) {
      del_timer(&timer);
    }
}

static ssize_t sys_show(struct kobject *kobj, struct kobj_attribute *attr,
      char *buf)
{
  return sprintf(buf, "%ld\n", delay);
}

static ssize_t sys_store(struct kobject *kobj, struct kobj_attribute *attr,
       const char *buf, size_t count)
{

  kstrtol(buf, 10, &delay);
  if (delay == 0) {
      del_timer(&timer);
    }
  init_timer(&timer);
  timer.expires = jiffies;
  timer.data = 0;
  timer.function = timer_function;
  add_timer(&timer);
  return count;
}

static struct kobj_attribute sys_attribute =
  __ATTR(delay, 0664, sys_show, sys_store);

static struct attribute *attrs[] = {
  &sys_attribute.attr,
  NULL,
};

// group of attributes for create group function
static struct attribute_group attr_group = {
  .attrs = attrs,
};

static struct kobject *sys_kobj;

static int sys_init(void)
{
  int retval;

  sys_kobj = kobject_create_and_add("Tom", kernel_kobj);
  if (!sys_kobj)
    return -ENOMEM;

  retval = sysfs_create_group(sys_kobj, &attr_group);
  if (retval)
    kobject_put(sys_kobj);

  return retval;
}

static void sys_exit(void)
{
    del_timer(&timer);
    kobject_put(sys_kobj);
}

module_init(sys_init);
module_exit(sys_exit);
MODULE_LICENSE("GPL");
