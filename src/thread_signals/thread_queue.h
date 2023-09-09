/*
 * thread_queue.h
 *
 *  Created on: Sep 1, 2023
 *      Author: root
 */

#ifndef SRC_THREAD_SIGNALS_THREAD_QUEUE_H_
#define SRC_THREAD_SIGNALS_THREAD_QUEUE_H_

#include <condition_variable>
#include <mutex>
#include <queue>

template <typename T>
class Thread_queue {
private:
    // Underlying queue
    std::queue<T> m_queue;

    // mutex for thread synchronization
    std::mutex m_mutex;

    // Condition variable for signaling
    std::condition_variable m_cond;

public:

    // Pushes an element to the queue
    void push(T item){

    	// Acquire lock
    	std::unique_lock<std::mutex> lock(m_mutex);

    	// Add item
    	m_queue.push(item);

    	// Notify one thread that
    	// is waiting
    	m_cond.notify_one();
    }

    // Pops an element off the queue
    T pop()
    {

    	// acquire lock
    	std::unique_lock<std::mutex> lock(m_mutex);

    	// wait until queue is not empty
    	m_cond.wait(lock, [this]() { return !m_queue.empty(); });

    	// retrieve item
    	T item = m_queue.front();
    	m_queue.pop();

    	// return item
    	return item;
    }

    T pop(int timeout)
    {

    	// acquire lock
    	std::unique_lock<std::mutex> lock(m_mutex);

    	// wait until queue is not empty
    	bool element = m_cond.wait_for(lock, std::chrono::milliseconds(timeout), [this]() { return !m_queue.empty(); });

    	// retrieve item
    	T item;
    	if(element){
    		item = m_queue.front();
    		m_queue.pop();
    	}else{
    		return -1;
    	}

    	// return item
    	return item;
    }

    T front()
    {

    	// acquire lock
    	std::unique_lock<std::mutex> lock(m_mutex);

    	// wait until queue is not empty
    	m_cond.wait(lock, [this]() { return !m_queue.empty(); });

    	// retrieve item
    	T item = m_queue.front();

    	// return item
    	return item;
    }

    T back()
    {

    	// acquire lock
    	std::unique_lock<std::mutex> lock(m_mutex);

    	// wait until queue is not empty
    	m_cond.wait(lock, [this]() { return !m_queue.empty(); });

    	// retrieve item
    	T item = m_queue.back();

    	// return item
    	return item;
    }

    T back_clear()
    {

    	// acquire lock
    	std::unique_lock<std::mutex> lock(m_mutex);

    	// wait until queue is not empty
    	m_cond.wait(lock, [this]() { return !m_queue.empty(); });

    	// retrieve item
    	T item = m_queue.back();

    	std::queue<T> empty;
    	m_queue.swap(empty);

    	// return item
    	return item;
    }

};


#endif /* SRC_THREAD_SIGNALS_THREAD_QUEUE_H_ */
