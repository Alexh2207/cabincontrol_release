/*
 * thread_flag.h
 *
 *  Created on: Sep 2, 2023
 *      Author: root
 */

#ifndef SRC_THREAD_SIGNALS_THREAD_FLAG_H_
#define SRC_THREAD_SIGNALS_THREAD_FLAG_H_


#include <condition_variable>
#include <mutex>
#include <queue>

class Thread_flag {
private:
	//flag value
	bool flag;

    // mutex for thread synchronization
    std::mutex flag_mutex;

    // Condition variable for signaling
    std::condition_variable flag_cond;

public:

    Thread_flag(){
    	flag = false;
    }

    // Pushes an element to the queue
    void set(){

    	// Acquire lock
    	std::unique_lock<std::mutex> lock(flag_mutex);

    	flag = true;
    	// Notify one thread that
    	// is waiting
    	flag_cond.notify_one();
    }

    // Pops an element off the queue
    bool wait()
    {

    	// acquire lock
    	std::unique_lock<std::mutex> lock(flag_mutex);

    	// wait until queue is not empty
    	flag_cond.wait(lock, [this]() { return flag; });

    	flag = false;

    	// return item
    	return true;
    }

    bool wait(int timeout)
    {

    	// acquire lock
    	std::unique_lock<std::mutex> lock(flag_mutex);

    	// wait until flag
    	bool timeout_reached = flag_cond.wait_for(lock, std::chrono::milliseconds(timeout), [this]() { return flag; });

    	if(timeout_reached){
    		flag = false;
    	}

    	// return item
    	return flag;
    }

};





#endif /* SRC_THREAD_SIGNALS_THREAD_FLAG_H_ */
