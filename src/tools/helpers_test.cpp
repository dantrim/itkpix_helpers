//std/stl
#include <getopt.h>
#include <iostream>
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;

//yarr
#include "Rd53b.h"
#include "SpecController.h"

// helpers
#include "itkpix_helpers.h"

struct option longopts_t[] = {{"hw", required_argument, NULL, 'r'},
                                {"chip", required_argument, NULL, 'c'},
                                {0,0,0,0}};

int main(int argc, char* argv[]) {

    std::cout << "hello, world!" << std::endl;

    std::string chip_config_filename = "";
    std::string hw_config_filename = "";
    int c = 0;
    while ((c = getopt_long(argc, argv, "r:c:", longopts_t, NULL)) != -1) {
        switch (c) {
            case 'r' :
                hw_config_filename = optarg;
                break;
            case 'c' :
                chip_config_filename = optarg;
                break;
            case '?' :
            default :
                std::cout << "ERROR: Invalid command-line argument provided: " << char(c) << std::endl;
                return 1;
                break;
        } // switch
    } // while

    fs::path hw_config_path(hw_config_filename);
    fs::path chip_config_path(chip_config_filename);
    if(!fs::exists(hw_config_path)) {
        std::cout << "ERROR: hw config file does not exist" << std::endl;
        return 1;
    }

    if(!fs::exists(chip_config_path)) {
        std::cout << "ERROR: chip config file does not exist" << std::endl;
        return 1;
    }

    namespace pix = itkpix::helpers;
    auto hw = pix::spec_init(hw_config_filename);
    auto fe = pix::rd53b_init(hw, chip_config_filename);

    bool ok = true;
    if(hw == nullptr) {
        std::cout << "ERROR: hw is nullptr!" << std::endl;
        ok = false;
    }
    if(fe == nullptr) {
        std::cout << "ERROR: fe is nullptr!" << std::endl;
        ok = false;
    }
    if(!ok) {
        return 1;
    }

    return 0;
}
