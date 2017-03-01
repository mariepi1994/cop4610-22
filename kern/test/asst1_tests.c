/*
 * Thread test code.
 */
#include <types.h>
#include <lib.h>
#include <thread.h>
#include <synch.h>
#include <test.h>

//Test function used to test thread join_test
//Simple looper
static void testFunction(void * junk, unsigned long a)
{        
        (void)a;        
        (void)junk;

	int b;
        for(b = 0; b < 1000000; b++);
}

//Tests the thread_join function
static int join_test(int nargs, char **args)
{
        char name[15];
        (void)nargs;
        (void)args;
        
        int i;

	//Arbitrary numberof points to non-created threads
        struct thread * myThreads[8];

	//For each thread pointer, create a instance of thread
	//in the custom thread fork which returns the pointer 
	//To the newly created thread
        for(i = 0; i < 8; i++)
		myThreads[i] = thread_fork_join(name, NULL, testFunction, NULL, i);

	//Join each of the threads, when a thread has successfully
	//joined, its thread ID is returned and printed.
	//If thread_join fails, then a negative value "-1" indicates failure.
        for(i = 0; i < 8; i++)
        {
		int num;
		num = thread_join(myThreads[i]);
		kprintf("%x \n", num);	//Print the returned thread ID, in hex!
        }

	return 0;
}


//Value that is to be taken ONLY if it is non-zero
int val_to_take;
struct lock * testlockASST;
struct cv * testcvASST;
struct semaphore * testsemASST;

//Take 
static void lockcv_take(void * junk, unsigned long a)
{
        (void)junk;
        (void)a;

	lock_acquire(testlockASST);
	while(val_to_take == 0)
		cv_wait(testcvASST, testlockASST);
	val_to_take = 0;
	lock_release(testlockASST);
	
}

//Give the integer a value
static void lockcv_give(void * junk, unsigned long a)
{
        (void)junk;
        (void)a;
	
	lock_acquire(testlockASST);
	val_to_take = 1;
	cv_signal(testcvASST, testlockASST);
	lock_release(testlockASST);
	kprintf("CV Test Passed!\n");
	V(testsemASST);
}

//CV Test
static int lockcv_test(int nargs, char **args)
{
        (void)nargs;
        (void)args;

	//Set this to zero so that cv_take has to wait
	val_to_take = 0;
	
	testlockASST = lock_create("testlockASST");
	testcvASST = cv_create("testcvASST");
	testsemASST = sem_create("testsemASST", 0);

	thread_fork("takeThread", NULL, lockcv_take, NULL, 0);
	thread_fork("giveThread", NULL, lockcv_give, NULL, 0);
	P(testsemASST);

	sem_destroy(testsemASST);
	cv_destroy(testcvASST);
	lock_destroy(testlockASST);

	return 0;
}

int asst1_tests(int nargs, char **args)
{
	(void)nargs;
	(void)args;
	
	//Test Thread Join
	join_test(0, NULL);

	//Test the user built CVs
	lockcv_test(0, NULL);

	return 0;
}
