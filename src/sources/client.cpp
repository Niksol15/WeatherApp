// client.cpp
#include "client.hpp"
#include <boost/format.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <iostream>

namespace {
    constexpr char kCityNotEnteredHelp[] = "Enter the name of the city, please";
    constexpr char kUnableToReadTokenHelp[] =
            "Unable to read token from file, please enter token as program argument,\n"
            "or create \"token.txt\" file with token";
    constexpr char kUnableToConnectHelp[] = "Something wrong, unable to connect to server";
    constexpr char kResultTemplate[] = "City: %1%\n"
                                       "Temperature: %2%°C\n"
                                       "Wind's speed: %3% meter/sec\n"
                                       "Wind's direction: %4%°\n";
    constexpr char kParsingError[] =
            "An error occurred while parsing the response:\n ";
} // namespace

ClientLayer::Client::Client(const NetworkLayer::ContextPtr &context_ptr,
                            std::string host, std::string port)
        : m_connection(context_ptr, {std::move(host), std::move(port)}), m_city(), m_token() {}

void ClientLayer::Client::setCity(const std::string &city_name) {
    m_city = city_name;
}

void ClientLayer::Client::setToken(const std::string &input_token) {
    m_token = input_token;
}

bool ClientLayer::Client::readTokenFromFile() {
    std::ifstream input("token.txt");
    if (input) {
        input >> m_token;
    }
    return !m_token.empty();
}

void ClientLayer::Client::process() {
    if (m_city.empty()) {
        std::cout << kCityNotEnteredHelp << "\n";
        return;
    }
    if (m_token.empty() && !readTokenFromFile()) {
        std::cout << kUnableToReadTokenHelp << "\n";
        return;
    }

    if (!m_connection.start()) {
        std::cout << kUnableToConnectHelp << "\n";
        return;
    }
    m_connection.send(m_city, m_token);
    std::cout << parseResponse(m_connection.receive()) << "\n";
}

std::string ClientLayer::Client::parseResponse(const std::string &response) {
    namespace pt = boost::property_tree;
    pt::ptree propertyTree;
    std::stringstream is(response);
    try {
        pt::read_json(is, propertyTree);
        std::string cityName = propertyTree.get<std::string>("name");
        int temp = propertyTree.get<double>("main.temp");
        int windsSpeed = propertyTree.get<double>("wind.speed");
        int windsDirection = propertyTree.get<int>("wind.deg");
        return (boost::format(kResultTemplate) % cityName % temp % windsSpeed %
                windsDirection).str();
    } catch (const pt::ptree_error &e) {
        return kParsingError + response;
    }
}