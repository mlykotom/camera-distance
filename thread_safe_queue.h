#ifndef THREAD_SAFE_QUEUE_H
#define THREAD_SAFE_QUEUE_H

#include <QQueue>
#include <QMutexLocker>

template <class T>
class ThreadSafeQueue{
private:
    QQueue<T> queue;
    QMutex mutex;
public:
    void enqueue(T &t){
        QMutexLocker locker(&mutex);
        queue.enqueue(t);
    }

    T dequeue(){
        QMutexLocker locker(&mutex);
        return queue.dequeue();
    }
    int length(){
        return queue.length();
    }

    bool empty(){
        return queue.empty();
    }
};

#endif // THREAD_SAFE_QUEUE_H
