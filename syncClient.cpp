#include <boost/asio.hpp>
#include <iostream>

using namespace boost::asio::ip;
using namespace std;
//using namespace boost;
const int MAX_LENGTH = 1024;//规定发送和接收的最大字节数

int main() {
	try {
		boost::asio::io_context ioc;//创建上下文服务
		//构造端点
		tcp::endpoint remote_ep(address::from_string("127.0.0.1"), 10086);
		//创建socket
		tcp::socket sock(ioc, remote_ep.protocol());
		boost::system::error_code error = boost::asio::error::host_not_found; 
		sock.connect(remote_ep,error);//连接对端的端点
		if(error) {
			cout << "connect failed" << error.value() << "error message" << error.what();
			return 0;
		}

		cout << "enter some message";
		char request[MAX_LENGTH];
		std::cin.getline(request, MAX_LENGTH);
		size_t request_length = strlen(request);
		boost::asio::write(sock, boost::asio::buffer(request,request_length));


		char reply[MAX_LENGTH];
		size_t reply_length = boost::asio::read(sock, boost::asio::buffer(reply, request_length));
		std::cout << "reply is" << endl;
		std::cout.write(reply, reply_length);
		std::cout << "\n";



	}catch (std::exception& e) {
		std::cout << "failed" << e.what();
	}

	return 0;
}