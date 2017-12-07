#ifndef MOXIE_MESSAGE_H
#define MOXIE_MESSAGE_H
#include <Buffer.h>

namespace moxie {

class Message {
public:
	int length() {
		return buffer_.readableBytes();
	}
	const char *data() {
		return buffer_.peek();
	}
private:
	Buffer buffer_;
};

}

#endif //MOXIE_MESSAGE_H
