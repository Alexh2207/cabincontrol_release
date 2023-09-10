/**
  ******************************************************************************
  * @file   thread_flag.h
  * @author Alejandro Hontanilla Belinchón (a.hontanillab@alumnos.upm.es)
  * @brief  Thread-safe flag that provides synchronization
  *
  * @note   End-of-degree work.
  ******************************************************************************
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

	/**
	* @brief Sets the flag to true and notifies a thread
	*/
    void set(){

    	// Acquire lock
    	std::unique_lock<std::mutex> lock(flag_mutex);

    	flag = true;
    	// Notify one thread that
    	// is waiting
    	flag_cond.notify_one();
    }

	/**
	* @brief Waits for the flag to be set forever, without a timeout. When flag is set it clears it automatically
	*
	* @return True when flag is set.
	*/
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

	/**
	* @brief Waits for the flag to be set for the timeout specified in milliseconds.
	* 		 When flag is set it clears it automatically.
	*
	* @param[in] timeout time to wait for the flag to be set in ms.
	*
	* @return True if flag set before or while thread is waiting. False if timeout reached.
	*/
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
