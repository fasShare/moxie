#ifndef MOXIE_MESSAGE_H
#define MOXIE_MESSAGE_H
#include <Buffer.h>

namespace moxie {

class Message {
public:
	size_t length() {
		return buffer_->readableBytes();
	}
	const char *data() {
		return buffer_->peek();
	}
    void append(const char *data, size_t len) {
        buffer_->append(data, len);
    }
    Message (size_t size) {
        buffer_ = new Buffer(size);
    }
private:
	Buffer *buffer_;
};

}

#endif //MOXIE_MESSAGE_H
