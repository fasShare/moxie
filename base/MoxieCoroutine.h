#ifndef MOXIE_COROUTINE_H
#define MOXIE_COROUTINE_H
#include <Log.h>

#include <boost/context/continuation.hpp>
#include <boost/function.hpp>

namespace moxie {

class MoixeException {
public:
	MoixeException (const std::string msg) :
		msg_(msg) {
	}
	const std::string what() const {
		return msg_;
	}

private:
	std::string msg_;
};

class MoxieCoroutine {
public:
	MoxieCoroutine(const boost::function<void ()>& cofunc) : cofunc_(cofunc){
	}
	void run() {
		continue_ = boost::context::callcc([this](boost::context::continuation&& sink){
			this->sink_ = std::move(sink);
			try {
				this->cofunc_();
			} catch (const MoixeException &ex) {
				LOGGER_WARN("Exception:" << ex.what());
			}
			// 返回被调用的协程，交给callcc，切换回本协程的调用者
			return std::move(this->sink_);
		});  
	}
	bool vaild() const {
		return static_cast<bool>(continue_);
	}
	void resume() {
		continue_ = continue_.resume();
	}
	void yeild() {
		if (!this->sink_) {
			return;
		}
		sink_ = sink_.resume();
	}
private:
	boost::function<void ()> cofunc_;
	boost::context::continuation continue_;
	boost::context::continuation sink_;
};

}

#endif //MOXIE_COROUTINE_H
