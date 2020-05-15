/******************************************************************************

                              Online C++ Compiler.
               Code, Compile, Run and Debug C++ program online.
Write your code in this editor and press "Run" button to compile and execute it.

*******************************************************************************/

#include <iostream>
#include <thread>
#include <atomic>
#include <condition_variable>
#include <mutex>
// using namespace std;

class thread_barrier {
private:
	std::mutex m;
	std::condition_variable cv;
	int threads;
	int waiting;
    thread_barrier(int nthreads) : threads{nthreads}, waiting{0} {};
public:
	void register() {
		std::unique_lock<std::mutex> lk(m);
		waiting++;
		if (waiting == threads) {
			waiting = 0;
			lk.unlock();
            cv.notify_all();
        }
        else {
		    cv.wait(lk, [this]{waiting < threads});
        }
    };
};

int main()
{
    cout<<"Hello World";

    return 0;
}
