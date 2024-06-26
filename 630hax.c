#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/list.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/pagemap.h>
#include <linux/pid_namespace.h>
#include <linux/proc_fs.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/version.h>
#include <linux/vmalloc.h>
#include <asm/mman.h>

MODULE_LICENSE("Dual BSD/GPL");

#define RUNNING_TOTAL_PROC_NAME "running_total"
#define SORTED_LIST_PROC_NAME "sorted_list"
#define MY_PID_PROC_NAME "my_piddo"

/* Some suggested state/data structures. */
static spinlock_t state_lock;
static int64_t total = 0;
static struct list_head sorted_list_head;
struct sorted_node {
  int64_t val;
  struct list_head list;
};

static struct proc_dir_entry * running_total_entry;
static struct proc_dir_entry * sorted_list_entry;
static struct proc_dir_entry * my_pid_entry;


/**
 * Hook to read the running total.
 *
 * This function should copy the value of the running total into the
 * user-level buffer provided.
 *
 * It retuns the number of bytes written into the buffer.
 *
 * It should also adjust *offp to reflect the offset into the file.
 *
 * @filp is the file handle.  You probably won't need to use this.
 * @buffer is the user-level buffer.
 * @count is the size of the user-level buffer.  You will never copy more
 *        than count bytes into bufffer.
 * @offp is a pointer to an unsigned int that stores the offset into the "file"
 */
static ssize_t proc_running_total_read(struct file *filp, char __user *buffer,
				       size_t count, loff_t *offp)
{
	int ret = 0;

	// Signal end-of-file (EOF) if the offset is not zero
	// This is a little hacky, since we are assuming we will always
	// be able to return the entire contents of the "file" in one read()
	// system call.  This is true for a single number, but may not be
	// true in general.
	if (*offp) return 0;

	// Exercise 1: Your code here

	// First, you should write the total into a kernel buffer,
	// as a string - not the binary value of the integer.
	// Don't forget to use a lock to protect the count!
	// Finally, you may find kfree() and kmalloc() helpful.

	// Then, use copy_to_user to copy the contents of
	// the buffer to the user-provided buffer.  In general,
	// kernel code ought not work directly on user memory,
	// which could be a bad pointer or changed by another user thread.

	return ret;
}

/**
 * Hook to update the running total.
 *
 * The user writes an integer (as a string) into the
 * proc file, which should be converted to an integer and added to the running total.
 *
 * It retuns the number of bytes "written" to the integer, or, the
 * number of bytes "consumed" in the conversion to an integer.
 *
 * It should also adjust *offp to reflect the offset into the file.
 *
 * For this exercise, one call should consume all of the bytes in the
 * common case.
 *
 * Finally, you will probably get subsequent calls with a non-zero offset;
 * you can just return zero when this happens (i.e., EOF).
 *
 * @filp is the file handle.  You probably won't need to use this.
 * @buffer is the user-level buffer.
 * @count is the size of the user-level buffer.  You will never copy more
 *        than count bytes from the bufffer.
 * @offp is a pointer to an unsigned int that stores the offset into the "file"
 */
static ssize_t proc_running_total_write(struct file *file,
					const char __user *buffer,
					size_t count, loff_t *offp)
{
	// Signal EOF if the offset is not zero
	if (*offp) return 0;

	// Exercise 1: Your code here:
	//
	// As with the read hook, allocate a kernel buffer, and copy
	// the data from userspace before trying to convert to an
	// integer.  Then add the integer to the running total.
	//
	// Hints: copy_from_user and sscanf may be helpful.

	// Here we can assume that, in the absence of errors,
	// we consume all the bytes written in one call.
	return count;
}

/**
 * Hook to read the sorted list
 *
 * This function should copy the value of each entry in the list (with a newline)
 * into the user-level buffer provided, in sorted order.
 *
 * It retuns the number of bytes written into the buffer.
 *
 * It should also adjust *offp to reflect the offset into the file.
 *
 * For simplicity, we can assume everything will be read in one call
 * (i.e., you don't need to worry about a read that doesn't return the
 * entire list - assume that if count bytes are filled, you have
 * returned the whole list.  You should do bounds checks on any buffers,
 * however - if your list is longer than buffer can hold, do not overflow
 * any buffers.
 *
 * @filp is the file handle.  You probably won't need to use this.
 * @buffer is the user-level buffer.
 * @count is the size of the user-level buffer.  You will never copy more
 *        than count bytes into bufffer.
 * @offp is a pointer to an unsigned int that stores the offset into the "file"
 */
static ssize_t proc_sorted_list_read(struct file *filp, char __user *buffer,
				     size_t count, loff_t *offp)
{
	size_t my_count = 0;

	// Signal EOF if the offset is not zero
	if (*offp) return 0;

	// Exercise 2: Your code here.
	//
	// Much of the code from proc_running_total_read() can be
	// copied here.  Instead of writing a single integer into the
	// user provided buffer, you will write a series of integers.
	//
	// You may find the helper functions in include/linux/list.h helpful,
	// such as list_for_each_entry().
	//
	// As before, be cognizant of concurrency and use a lock

	return my_count;
}

/**
 * Hook to add a number to the sorted list.
 *
 * The user writes an integer (as a string) into the
 * proc file, which should be converted to an integer and added to the
 * list of integers in the right place.
 *
 * Duplicates are allowed.
 *
 * It retuns the number of bytes "written" to the integer, or, the
 * number of bytes "consumed" in the conversion to an integer.
 *
 * It should also adjust *offp to reflect the offset into the file.
 *
 * For this exercise, one call should consume all of the bytes in the
 * common case.
 *
 * Finally, you will probably get subsequent calls with a non-zero offset;
 * you can just return zero when this happens (i.e., EOF).
 *
 * @filp is the file handle.  You probably won't need to use this.
 * @buffer is the user-level buffer.
 * @count is the size of the user-level buffer.  You will never copy more
 *        than count bytes from the bufffer.
 * @offp is a pointer to an unsigned int that stores the offset into the "file"
 */
static ssize_t proc_sorted_list_write(struct file *file,
				      const char __user *buffer,
				      size_t count, loff_t *offp)
{
	// Signal EOF if the offset is not zero
	if (*offp) return 0;

	// Exercise 2: Your code here.
	//
	// Much of the code from proc_running_total_write() can be
	// copied here.  Just like before, you will read a single integer in,
	// but rather than add it to a total, you will place it in the list,
	// in the correct position in the list.
	//
	// You may find the helper functions in include/linux/list.h helpful,
	// such as list_for_each_entry() and list_add_tail().
	//
	// As before, be cognizant of concurrency and use a lock

	return count;
}

/* Helper code borrowed form Linux because it is not exported as a
 * symbol; this is a brittle practice and could break on a different kernel version. */
static void set_pid_nr_ns(struct pid *pid, struct pid_namespace *ns, pid_t new_pid)
{
	struct upid *upid;

	if (pid && ns->level <= pid->level) {
		upid = &pid->numbers[ns->level];
		if (upid->ns == ns)
		  upid->nr = new_pid;
	} else {
	  printk(KERN_ERR "Failed to set pid...\n");
	}
}

/* Also stolen from Linux */
static struct pid **task_pid_ptr(struct task_struct *task, enum pid_type type)
{
	return (type == PIDTYPE_PID) ?
		&task->thread_pid :
		&task->signal->pids[type];
}

/* Also heavily inspired by linux code */
static void my_change_pid(struct task_struct *task, pid_t new_pid)
{
	struct pid **pid_ptr;
	struct pid_namespace *ns;
	rcu_read_lock();
	ns = task_active_pid_ns(current);
	pid_ptr = task_pid_ptr(task, PIDTYPE_TGID);
	set_pid_nr_ns(rcu_dereference(*pid_ptr), ns, new_pid);
	rcu_read_unlock();
}

/**
 * User writes an integer in; set the uid field of the task struct to
 * this value.  This is not secure in any way - it basically bypasses
 * permission/password/authentication checks for su/login/ssh.
 *
 * The user writes an integer (as a string) into the
 * proc file, which should be converted to an integer.  We provide
 * helper functions to modify the task struct; you will use my_change_pid().
 *
 * It retuns the number of bytes "written", storing the integer, or, the
 * number of bytes "consumed" in the conversion to an integer.
 *
 * It should also adjust *offp to reflect the offset into the file.
 *
 * For this exercise, one call should consume all of the bytes in the
 * common case.
 *
 * Finally, you will probably get subsequent calls with a non-zero offset;
 * you can just return zero when this happens (i.e., EOF).
 *
 * @filp is the file handle.  You probably won't need to use this.
 * @buffer is the user-level buffer.
 * @count is the size of the user-level buffer.  You will never copy more
 *        than count bytes from the bufffer.
 * @offp is a pointer to an unsigned int that stores the offset into the "file"
 */
static ssize_t proc_my_pid_write(struct file *file,
				 const char __user *buffer,
				 size_t count, loff_t *offp)
{
	// Signal EOF if the offset is not zero
	if (*offp) return 0;

	// Execise 3: Your code here.  Again, this can be largely copied and pasted.
	// The main difference is using my_change_pid().

	// Silence compiler warnings;  eventually delete this line
	my_change_pid(current, current->tgid);

	return count;
}


const struct proc_ops running_total_proc_ops = {
	.proc_read = proc_running_total_read,
	.proc_write = proc_running_total_write,
};


const struct proc_ops sorted_list_proc_ops = {
	.proc_read = proc_sorted_list_read,
	.proc_write = proc_sorted_list_write,
};

const struct proc_ops my_pid_proc_ops = {
	.proc_write = proc_my_pid_write,
};


/*
 * If an init function is provided, an exit function must also be provided
 * to allow module unload.
 */
static int init_630hax(void)
{
	int rv = 0;

	spin_lock_init(&state_lock);
	total = 0;
	INIT_LIST_HEAD(&sorted_list_head);
	/* create proc file */
	running_total_entry = proc_create(RUNNING_TOTAL_PROC_NAME, 0666, NULL,
					  &running_total_proc_ops);
	if (running_total_entry == NULL) {
		remove_proc_entry(RUNNING_TOTAL_PROC_NAME, NULL);
		printk(KERN_ALERT "Failed to initialize running total procfile: %s\n", RUNNING_TOTAL_PROC_NAME);
		return -ENOMEM;
	}
	printk(KERN_ALERT "running total procfs entry created\n");

	/* create proc file */
	sorted_list_entry = proc_create(SORTED_LIST_PROC_NAME, 0666, NULL,
					&sorted_list_proc_ops);
	if (sorted_list_entry == NULL) {
		remove_proc_entry(SORTED_LIST_PROC_NAME, NULL);
		printk(KERN_ALERT "Failed to initialize sorted list procfile: %s\n", RUNNING_TOTAL_PROC_NAME);
		return -ENOMEM;
	}
	printk(KERN_ALERT "sorted list procfs entry created\n");

	my_pid_entry = proc_create(MY_PID_PROC_NAME, 0222, NULL,
					&my_pid_proc_ops);
	if (my_pid_entry == NULL) {
		remove_proc_entry(MY_PID_PROC_NAME, NULL);
		printk(KERN_ALERT "Failed to initialize sorted list procfile: %s\n", RUNNING_TOTAL_PROC_NAME);
		return -ENOMEM;
	}
	printk(KERN_ALERT "my uid procfs entry created\n");


	printk(KERN_ALERT "Hello, comp 630 world\n");
	return rv;
}

static void exit_630hax(void)
{
	printk(KERN_ALERT "Goodbye, comp 630 world\n");

	/* remove proc files */
	remove_proc_entry(RUNNING_TOTAL_PROC_NAME, NULL);
	printk(KERN_ALERT "running total procfs entry removed\n");

	remove_proc_entry(SORTED_LIST_PROC_NAME, NULL);
	printk(KERN_ALERT "sorted list procfs entry removed\n");

	remove_proc_entry(MY_PID_PROC_NAME, NULL);
	printk(KERN_ALERT "my uid procfs entry removed\n");

	// Exercise 2: Your code here: Free list nodes
}

module_init(init_630hax);
module_exit(exit_630hax);
