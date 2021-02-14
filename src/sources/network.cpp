// networklayer.cpp
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/version.hpp>
#include <boost/format.hpp>
#include <network.hpp>
#include <utility>

NetworkLayer::Network::Network(const ContextPtr &context_ptr, IPAddress connectionIP)
        : m_contextPtr(context_ptr), m_connectionIP(std::move(connectionIP)),
          m_stream(*m_contextPtr), m_ec() {}

bool NetworkLayer::Network::start() {
    tcp::resolver resolver(*m_contextPtr);
    auto const results = resolver.resolve(m_connectionIP.host, m_connectionIP.port);
    m_stream.connect(results, m_ec);
    return m_stream.socket().is_open();
}

namespace {
    constexpr int kHTTPVersion = 11;
    constexpr char kTargetTemplate[] = "/data/2.5/weather?q=%1%&appid=%2%&units=metric";
} // namespace

void NetworkLayer::Network::send(const http::request<http::string_body> &request) {
    if (m_stream.socket().is_open() && !m_ec) {
        http::write(m_stream, request, m_ec);
    }
}

void NetworkLayer::Network::send(const std::string &city_name, const std::string &token) {
    std::string target =
            (boost::format(kTargetTemplate) % city_name % token).str();
    http::request<http::string_body> req{http::verb::get, target, kHTTPVersion};
    req.set(http::field::host, m_connectionIP.host);
    req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
    send(req);
}

std::string NetworkLayer::Network::receive() {
    return m_ec ? m_ec.message() :
           beast::buffers_to_string(receiveFromServer().body().data());
}

NetworkLayer::http::response<NetworkLayer::http::dynamic_body>
NetworkLayer::Network::receiveFromServer() {
    http::response<http::dynamic_body> res;
    if (m_stream.socket().is_open() && !m_ec) {
        beast::flat_buffer buffer;
        read(m_stream, buffer, res, m_ec);
    }
    return res;
}

NetworkLayer::Network::~Network() {
    if (m_stream.socket().is_open()) {
        m_stream.socket().shutdown(tcp::socket::shutdown_both);
    }
}