// application.cpp
#include "application.hpp"
#include "client.hpp"
#include <boost/asio/io_context.hpp>
#include <boost/program_options.hpp>
#include <iostream>
#include <memory>

Application::Application(int argc, char **argv) : argc(argc), argv(argv) {}

namespace {
    constexpr char kParseCmdErrorHelp[] = "An exception was thrown during parsing "
                                          "of the arguments:\n";
    constexpr char kHelpDescription[] = "Show help";
    constexpr char kCityHelpDesc[] =
            "Enter city (If the city name is divided into several\n"
            " words - use _ instead of space)";
    constexpr char kTokenHelpDesc[] = "Enter token";
    constexpr char kDefaultHost[] = "api.openweathermap.org";
    constexpr char kDefaultPort[] = "80";
} // namespace

int Application::exec() {
    namespace po = boost::program_options;
    po::options_description description("Main Options");
    description.add_options()("help,h", kHelpDescription)
            ("city,c", po::value<std::string>(), kCityHelpDesc)
            ("token,t", po::value<std::string>(), kTokenHelpDesc);
    po::variables_map vm;
    try {
        po::store(po::parse_command_line(argc, argv, description), vm);
    } catch (const po::error& e){
        std::cout << kParseCmdErrorHelp << e.what() << "\n";
        return 0;
    }
    if (vm.count("help") || vm.empty()) {
        std::cout << description;
        return 0;
    }
    ClientLayer::Client client(std::make_shared<boost::asio::io_context>(),
                               kDefaultHost, kDefaultPort);
    if (vm.count("city")) {
        client.setCity(vm["city"].as<std::string>());
    }
    if (vm.count("token")) {
        client.setToken(vm["token"].as<std::string>());
    }
    client.process();
    return 0;
}