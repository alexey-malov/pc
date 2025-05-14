#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/beast/core/flat_buffer.hpp>
#include <boost/beast/http/empty_body.hpp>
#include <boost/beast/http/message.hpp>
#include <boost/beast/http/read.hpp>
#include <boost/beast/http/string_body.hpp>
#include <boost/beast/http/write.hpp>
#include <boost/cobalt/gather.hpp>
#include <boost/cobalt/main.hpp>
#include <boost/cobalt/promise.hpp>
#include <boost/cobalt/task.hpp>
#include <cassert>
#include <chrono>
#include <iostream>
#include <string>

using namespace std::chrono_literals;

namespace asio = boost::asio;
namespace cobalt = boost::cobalt;
namespace ssl = asio::ssl;
namespace beast = boost::beast;
namespace http = beast::http;

using tcp = asio::ip::tcp;
using SteadyClock = std::chrono::steady_clock;
using Seconds = std::chrono::duration<double>;

// Это исполнитель по умолчанию, если явным образом не задан
using UseOpCobaltExecutor = cobalt::use_op_t::executor_with_default<cobalt::executor>;
// Это тип сокета, который будет использоваться в корутинах
using TCPSocket = typename tcp::socket::rebind_executor<UseOpCobaltExecutor>::other;
// Это тип SSL-сокета, который будет использоваться в корутинах
using SSLSocket = ssl::stream<TCPSocket>;

cobalt::promise<SSLSocket> Connect(std::string_view host, ssl::context& sslContext)
{
	auto thisThreadExecutor = cobalt::this_thread::get_executor();
	tcp::resolver resolver{ thisThreadExecutor };

	// Определяем endpoint по имени хоста (фактически, это запрос к DNS-серверу)
	std::cout << "Resolving host " << host << " address\n";
	auto endpoints = co_await resolver.async_resolve(host, "https", cobalt::use_op);
	assert(!endpoints.empty());
	std::cout << "Host address resolved to " << endpoints.begin()->endpoint() << "\n";

	SSLSocket socket{ thisThreadExecutor, sslContext };

	std::cout << "Connecting\n";
	co_await socket.next_layer().async_connect(*endpoints.begin());
	std::cout << "Connected\n";

	std::cout << "Handshaking\n";
	co_await socket.async_handshake(ssl::stream_base::client);
	std::cout << "Handshake complete\n";

	co_return socket;
}

cobalt::promise<http::response<http::string_body>> SendHTTPSRequest(std::string_view host, std::string_view target)
{
	ssl::context sslContext{ ssl::context::tls_client };
	auto conn = co_await Connect(host, sslContext);

	// GET /index.html 1.1
	http::request<http::empty_body> req{ http::verb::get, target, /*version=*/11 };
	req.set(http::field::host, host);

	std::cout << "Send request " << req << "\n";
	co_await http::async_write(conn, req, cobalt::use_op);
	std::cout << "Request sent\n";

	beast::flat_buffer buf;
	http::response<http::string_body> response;

	std::cout << "Read response\n";
	co_await http::async_read(conn, buf, response, cobalt::use_op);
	std::cout << "Response received\n";

	co_return response;
}

cobalt::main co_main(int /*argc*/, char* /*argv*/[])
{
	try
	{
		auto response = co_await SendHTTPSRequest("www.boost.org", "/index.html");
		
		std::cout << response << std::endl;
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		co_return EXIT_FAILURE;
	}
	co_return EXIT_SUCCESS;
}
