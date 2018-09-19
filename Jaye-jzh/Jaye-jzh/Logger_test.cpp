#include "Logger.hpp"


int main(int argc, char* argv[])
{
	LOG_SET_THREAD_NAME("Main");
	LOG_DEUBG << "Starting the application大家放假";


	std::thread t ([](){
		LOG_SET_THREAD_NAME("Thread1");

		int i = 0;
		for (;;) {
			std::this_thread::sleep_for(std::chrono::seconds(1));
			LOG_DEUBG("The value i is ", i++);
		}
		LOG_WARN("The loop is over.");
	});



	for (short i = 0; i < 10; ++i) {
		LOG_ERROR << "The value mi is " << i;
	}

	LOG_WARN << "The Main loop is over.";
	
	if (t.joinable()) {
		t.join();
	}
	std::ofstream fxxx("xxxx.log", std::ios_base::binary | std::ios_base::out);
	//fxxx << "什么" << std::endl << std::flush;
	//fxxx << "什么" << std::endl << std::flush;
	std::string what = "什么";
	fxxx.write(what.c_str(), what.size());
	fxxx.write(what.c_str(), what.size());

	for (;;) {
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}

	return 0;
}