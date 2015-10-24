#include "include/helpers.h"

// ------- Config Helper ----------

config_helper::config_helper() {
    config_path = "";

    std::vector<std::string> possible_paths = {
      "config",
      "../config",
      "../CGI/config"
    };
    for(auto path : possible_paths) {
        std::ifstream f(path);
        if(f.good()) {
            config_path = path;
            break;
        }
        f.close();
    }

    if(config_path == "") {
        throw std::runtime_error("Config file not found!");
    }

    std::ifstream config_file;
    config_file.open(config_path);

    std::string param;

    while(std::getline(config_file, param)) {
        if(param[0] != '#' && param != "") {
            std::string key = param.substr(0, param.find_first_of(':'));
            std::string val = param.substr(param.find_first_of(':')+1);
            parameters[key] = val;
        }
    }

    config_file.close();
}

std::string config_helper::get_param(std::string name) {
    return parameters.at(name);
}

int config_helper::get_iparam(std::string name) {
    return std::stoi(parameters.at(name));
}

config_helper::~config_helper() {
}

// ------ File Helper -------

file_helper::file_helper(std::string path) {
    root_dir = path;
}

file_helper::~file_helper() {
}

// ------- Message Helper ---------



// --------- Error Helper ----------

error_helper::error_helper() {

}
