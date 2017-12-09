#ifndef MOXIE_TCPCLIENT_H
#define MOXIE_TCPCLIENT_H
#include <Events.h>
#include <Socket.h>
#include <Message.h>
#include <NetAddress.h>
#include <Timestamp.h>
#include <TcpConnection.h>

#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>

namespace moxie {

class ServiceClient;

class DataTransfer {
public:
	enum RET {
		DATA_OK,
		DATA_INCOMPLETE,
		DATA_ERROR,
		DATA_FATAL,
	};
	virtual RET DataCheck(boost::shared_ptr<TcpConnection> conn, size_t& length) = 0;
	virtual bool DataFetch(boost::shared_ptr<TcpConnection> conn, size_t length_from_check,
					boost::shared_ptr<Message>, boost::shared_ptr<Message>) = 0;
};

class TcpClient {
public:
	TcpClient(const NetAddress& addr);
	using TalkDone = boost::function<void (boost::shared_ptr<Message>, boost::shared_ptr<Message>)>;

	static void HasData(boost::shared_ptr<TcpConnection> conn, Timestamp time, boost::shared_ptr<TcpClient> client);
	static void WriteDone(boost::shared_ptr<TcpConnection> conn, Timestamp time, boost::shared_ptr<TcpClient> client);
	static void WillBeClose(boost::shared_ptr<TcpConnection> conn, Timestamp time, boost::shared_ptr<TcpClient> client);
	void SetDataTransfer(DataTransfer *transfer);

    void setIndex(long index);
    long getIndex() const;
    void setService(ServiceClient *service);
        
    static bool RemoveFromServiceClient(boost::shared_ptr<TcpClient> client);

    bool connectToServer();
    bool initWithEvent(boost::shared_ptr<Events> event);
	static bool Talk(boost::shared_ptr<TcpClient> client, boost::shared_ptr<Message> request, 
					boost::shared_ptr<Message> response, TalkDone done);
private:
	boost::shared_ptr<Message> response;
	boost::shared_ptr<Message> request;
	boost::shared_ptr<TcpConnection> conn_;
	DataTransfer *transfer_;
    NetAddress addr_;
    Socket sock_;
	TalkDone done;
    long index_;
    ServiceClient *service_;
};

}

#endif //MOXIE_TCPCLIENT_H
