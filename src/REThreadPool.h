#pragma once
#include "REInit.h"

// namespace RE {
//     class SpinLock {
//     private:
//         std::atomic_flag flag = ATOMIC_FLAG_INIT;

//     public:
//         SpinLock() {}

//         void lock() {
//             while (flag.test_and_set(std::memory_order_acquire)) {
//                 // 如果锁已经被其他线程占用，则自旋等待
//                 std::this_thread::yield();
//             }
//         }

//         void unlock() {
//             flag.clear(std::memory_order_release);
//         }

//         bool tryLocked() {
//             return flag.test_and_set(std::memory_order_acquire);
//         }
//     };

//     class primaryThread {
//     public:
//         primaryThread() :stop(false), empty(true) {
//             std::thread thr(mainloop, this);
//             thr.detach();
//         }
//         ~primaryThread() {
//             for (auto& i : workQue) {
//                 delete i;
//             }
//         }
//         bool tryAddTask(std::function<void()>* task) {
//             //REDebug << task;
//             if (lck.tryLocked()) {
//                 workQue.push_back(task);
//                 empty = false;
//                 lck.unlock();
//                 //REDebug << std::boolalpha << true;
//                 return true;
//             } else {
//                 //REDebug << std::boolalpha << false;
//                 return false;
//             }
//         }
//         void _stopThread() {
//             stop = true;
//         }
//     private:
//         bool stop;
//         bool empty;
//         std::deque<std::function<void()>*> workQue;
//         std::thread::id id;
//         SpinLock lck;

//         std::function<void()>* getTask() {
//             lck.lock();
//             std::function<void()>* out = workQue.front();
//             workQue.pop_front();
//             empty = workQue.empty();
//             lck.unlock();
//             return out;
//         };

//         static void mainloop(primaryThread* threadManager) {
//             threadManager->id = std::this_thread::get_id();
//             while (!threadManager->stop) {
//                 if (!threadManager->empty) {
//                     std::function<void()>* task = threadManager->getTask();
//                     try {
//                         (*task)();
//                         delete task;
//                     } catch (const std::runtime_error& e) {
//                         std::cerr << "Caught exception: " << e.what() << std::endl;
//                         REDebug << task;
//                     }
//                 } else {
//                     std::this_thread::yield();
//                 }
//             }
//             delete threadManager;
//         }
//     };


//     class ThreadPool {
//     public:
//         ThreadPool() {
//             int coreNum = getCoreNumber();
//             std::cout << "Number of CPU cores: " << coreNum << std::endl;

//             for (size_t i = 0; i < /*coreNum - 4*/6; i++) {
//                 createThread();
//             }
//         }
//         ~ThreadPool() {
//             for (auto& i : threadList) {
//                 i->_stopThread(); // 子线程终止会自动删除primaryThread对象
//             }
//         }

//         void submit(std::function<void()> task) {
//             size_t i = 0;
//             size_t max = threadList.size();
//             std::function<void()>* fptr = new std::function<void()>(task); //函数对象禁止拷贝

//             while (!threadList[i]->tryAddTask(fptr)) {
//                 i = (i < max - 1) ? (i + 1) : (0);
//             }
//         }
//     private:
//         std::vector<primaryThread*> threadList;

//         void createThread() {
//             auto newthread = new primaryThread();
//             threadList.push_back(newthread);
//         }

//         int getCoreNumber() {
//             return std::thread::hardware_concurrency();
//         }
//     }threadPool;
// }