#ifndef RUN_AND_FORGET__HPP
#define RUN_AND_FORGET__HPP

#include <thread>
#include <atomic>
#include <map>

class ThreadsOwner {
private:
	struct ThreadsOwnerData {
	
		std::atomic<unsigned int> id{0};
		std::atomic<bool> done{false};
		std::map<unsigned int, std::thread> threads;
		std::mutex mutex;

		template <typename CB, typename... T> void run(CB && cb, T && ... args)
		{
			const unsigned int id{this->id++};
			std::lock_guard<std::mutex> lock{mutex};
			
			threads.emplace(id, std::thread([this,id](CB && cb, T && ... args){
				try {
					cb(std::forward<T>(args)...);
				}
				catch (...) { }
				this->remove(id);
			}, std::forward<CB>(cb), std::forward<T>(args)...));
		}
		void remove(unsigned int id) {
			if (!done.load()) {
				std::lock_guard<std::mutex> lock{mutex};
				auto it = threads.find(id);
				if (it != threads.end()) {
					it->second.detach();
					threads.erase(it);
					//std::cout << "erasing thread #"<<id<<" prematurely.\n";
				}
			}
		}
		~ThreadsOwnerData() {
			done.store(true);
			std::lock_guard<std::mutex> lock{mutex};
			//std::cout << threads.size() << " to erase.\n";
			while (threads.size()) {
				auto it = threads.begin();
				if (it->second.joinable()) it->second.join();
				//std::cout << "erasing thread #"<<it->first<<" at end.\n";
				threads.erase(it);
			}
		}
	};
	std::unique_ptr<ThreadsOwnerData> data;
public:
	ThreadsOwner(): data{std::make_unique<ThreadsOwnerData>()} { }
	ThreadsOwner(const ThreadsOwner &) = delete;
	ThreadsOwner(ThreadsOwner &&) = default;
	~ThreadsOwner() = default;
	template <typename... T> void run(T && ... args) {
		data->run(std::forward<T>(args)...);
	}
};


#endif
