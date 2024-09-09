#include <boost/asio.hpp>
#include <iostream>
#include <set>//存储线程
#include <memory>
using boost::asio::ip::tcp;
const int MAX_LENGTH = 1024;
typedef std::shared_ptr<tcp::socket> socket_ptr;
std::set<std::shared_ptr<std::thread>> thread_set;
using namespace std;

void session(socket_ptr sock) {
    try {
        for (; ; ) {
            char data[MAX_LENGTH];
            memset(data,'\0',MAX_LENGTH);//将char数组清零
            boost::system::error_code error;
            //size_t length = boost::asio::read(sock, boost::asio::buffer(data,MAX_LENGTH),error);//使用read，就要等到1024个字节再返回
            //假如客户端只发送了7个字节，服务器端会一直等待
            //使用read_some()的方式
            size_t length = sock->read_some(boost::asio::buffer(data, MAX_LENGTH), error);
            if (error == boost::asio::error::eof) {
                cout << "connection closed by peer" << endl;
                break;
            }
            else if(error){
                throw boost::system::system_error(error);
            }

            cout << "receive from " << sock->remote_endpoint().address().to_string() << endl;
            cout << "receive message is: " << data << endl;
            //将收到的数据回传给对方
            boost::asio::write(*sock, boost::asio::buffer(data, length));
        }
    }
    catch (exception& e) {
        cerr << "exception in thread: " << e.what() << "\n" << endl;//打印线程中出现的问题(如果有的话)
    }
}

//接收客户端的连接

void server(boost::asio::io_context& ioc, unsigned short port_num) {
    tcp::acceptor a(ioc, tcp::endpoint(tcp::v4(), port_num));//服务器用来接收客户端的连接
    for ( ; ;) {
        //每来一个请求，都生成一个socket
        socket_ptr socket(new tcp::socket(ioc));
        a.accept(*socket);
        //创建一个线程，此线程做session这样的工作，session的参数是socket，socket用于客户端发送过来的数据的读写
        auto t = std::make_shared<std::thread>(session, socket);
        //保证线程不会被回收
        thread_set.insert(t);
    }

}

int main()
{
    //在主线程中，等待子线程结束以后再退出主线程
    try {
        boost::asio::io_context ioc;
        server(ioc,10086);
        for (auto& t : thread_set) {
            t->join();
        }

    }
    catch (std::exception& e) {
        std::cerr << "Exception " << e.what() << "\n";
    }

    return 0;
}
