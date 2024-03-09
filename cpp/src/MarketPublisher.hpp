#include <stdio.h>
#include <pthread.h>
#include <iostream>

namespace optquote {

/**
 * Reference:
 * https://qiita.com/kasei-san/items/c06ea092dfdaff2d98c1
 **/
class MarketPublisher {
	private:
		pthread_t thread_handler; // thread handler
		pthread_mutex_t mutex; // mutex
		int data;

	public:
		MarketPublisher();
		~MarketPublisher(){
			pthread_cancel(this->thread_handler);
			pthread_join(this->thread_handler, NULL);
		}

		static void* executeLauncher(void* args){
			reinterpret_cast<ThreadTest*>(args)->execute();
			return (void*) NULL;
		}

		void threadStart(){
			if((this->thread_handler) == NULL){
				pthread_mutex_init(&(this->mutex), NULL);
				pthread_create(
					&(this->thread_handler),
					NULL,
					&MarketPublisher::executeLauncher,
					this
				);
			}
		}

		void execute(){
			thread_testcancel(); // check cancel request
			pthread_mutex_lock(&(this->mutex));
			// do something
			pthread_mutex_unlock(&(this->mutex));
		}

}

}
