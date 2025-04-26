#include <iostream>
#include <fstream>
#include <thread>
#include <filesystem>

#include <boost/program_options.hpp>
#include <boost/lexical_cast.hpp>
#include <nlohmann/json.hpp>

#include "config.h"
#include "app.h"



namespace po = boost::program_options;


template<typename T>
void set_value(T& v, std::string const& s)
{
    v = boost::lexical_cast<T>(s);
}

std::string help_schema(std::string const& name, nlohmann::json::object_t const& sch)
{
    std::ostringstream oss;
    oss << name << ":" << sch.at("type").get<std::string>();
    oss << " - " << sch.at("title").get<std::string>() << "\n";
    if (sch.count("enum"))
        oss << "  - values: one of " << sch.at("enum") << "\n";
    return oss.str();
}

int info_options(int argc, const char** argv)
{
    po::variables_map args;
    po::options_description info_options("Info");
    info_options.add_options()
        ("help,h", "Produce this message")
        ("config_list,H", "Show config variables")
        ("config_schema,S", "Show config schema");

    try {
        po::store(po::parse_command_line(argc, argv, info_options), args);
        po::notify(args);
    }
    catch (std::exception &e) {
        return 0;
    }

    bool at_least_one = false;
    if (args.count("help")) {
        std::cerr << info_options << std::endl;
    }
    if (args.count("config_list")) {
        auto schema = nlohmann::json::parse(Config::_schema());
        mini_json::_private::for_sequence(std::make_index_sequence<std::tuple_size<decltype(Config::json_properties())>::value>{},
                [&](auto i) {
                    constexpr auto property = std::get<i>(Config::json_properties());
                    auto item = schema.get<nlohmann::json::object_t>().at(property.name);
                    std::cerr << help_schema(property.name, item);
                });
        std::cerr << std::endl;
        at_least_one = true;
    }
    if (args.count("config_schema")) {
        auto schema = Config::_schema();
        std::cerr << schema << std::endl;
        at_least_one = true;
    }
    return at_least_one;
}

int options(int argc, const char** argv, po::variables_map& args)
{
    po::options_description options("Options");
    options.add_options()
        ("help,h", "Produce this message")
        ("config,c", po::value<std::string>()->required(), "Config file")
        ("variable,D", po::value<std::vector<std::string>>()->multitoken(), "Set config variable");

    try {
        po::store(po::parse_command_line(argc, argv, options), args);
        po::notify(args);
    }
    catch (std::exception &e) {
        std::cerr << "Error: " << e.what() << '\n';
        std::cerr << options << '\n';
        return 1;
    }
    return 0;
}


int main(int argc, const char** argv)
{
    po::variables_map args;
    if (info_options(argc, argv))
        return 1;
    if (options(argc, argv, args))
        return 1;

    Config config;
    {
        auto config_path = args.at("config").as<std::string>();
        if (not std::filesystem::exists(config_path)) {
            std::ofstream ofl(config_path);
            if (ofl.is_open()) {
                mini_json::serialize(config, ofl);
            }
        }
        std::ifstream ifl(config_path);
        if (not ifl.is_open()) {
            std::cerr << "Error open '" << config_path << "': " << strerror(errno) << std::endl;
            return 1;
        }

        std::string s;
        ifl.seekg(0, std::ios::end);
        s.resize(ifl.tellg());
        ifl.seekg(0, std::ios::beg);
        ifl.read(&s[0], s.size());
        config = mini_json::parse<Config>(begin(s), end(s));

        if (not args["variable"].empty())
        for (auto& opt : args["variable"].as<std::vector<std::string>>()){
            std::string k = opt.substr(0, opt.find('='));
            std::string v = opt.substr(opt.find('=') != opt.npos ? opt.find('=') + 1 : opt.size() - 1);
            if (v.empty()) {
                std::cerr << "Error: required -D<key>=<value>" << std::endl;
                return 1;
            }
            bool found = false;
            try {
                mini_json::_private::for_sequence(std::make_index_sequence<std::tuple_size<decltype(Config::json_properties())>::value>{},
                        [&](auto i) {
                            constexpr auto property = std::get<i>(Config::json_properties());
                            if (k == property.name) {
                                found = true;
                                try {
                                    set_value(config.*(property.member), v);
                                }
                                catch (std::exception const& e) {
                                    std::cerr << "Error set " << k << "=cast<" << typeid(config.*(property.member)).name() << ">(" << v << ")" << std::endl;
                                    throw;
                                }
                            }
                        });
            }
            catch (std::exception const& e) {
                std::cerr << "Error occured: " << e.what() << std::endl;
                return 1;
            }
            if (not found) {
                std::cerr << "Error: key '" << k << "' not found in config" << std::endl;
                return 1;
            }
        }
    }

    App app(config);
    app.run();

    return 0;
}
