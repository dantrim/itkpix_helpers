//std/stl
#include <getopt.h>
#include <iostream>
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
#include <thread>

//yarr
#include "Rd53b.h"
#include "SpecController.h"

//helpers
#include "itkpix_helpers.h"

struct option longopts_t[] = {{"hw", required_argument, NULL, 'r'},
                                {"connectivity", required_argument, NULL, 'c'},
                                {"chip-id", required_argument, NULL, 'i'},
                                {0,0,0,0}};

int main(int argc, char* argv[]) {

    std::string connectivity_config_filename = "";
    std::string hw_config_filename = "";
    int chip_id = -1;
    int c = 0;
    while ((c = getopt_long(argc, argv, "r:c:i:", longopts_t, NULL)) != -1) {
        switch(c) {
            case 'r' :
                hw_config_filename = optarg;
                break;
            case 'c' :
                connectivity_config_filename = optarg;
                break;
            case 'i' :
                chip_id = std::stoi(optarg);
                break;
            case '?' :
            default :
                std::cout << "ERROR: Invalid command-line argument provided: " << char(c) << std::endl;
                return 1;
                break;
        } // switch
    } // while

    fs::path hw_config_path(hw_config_filename);
    if(!fs::exists(hw_config_path)) {
        std::cout << "ERROR: Provided hw config file (=" << hw_config_filename << ") does not exist" << std::endl;
        return 1;
    }

    fs::path chip_config_path(connectivity_config_filename);
    if(!fs::exists(chip_config_path)) {
        std::cout << "ERROR: Provided chip config file (=" << connectivity_config_filename << ") does not exist" << std::endl;
        return 1;
    }

    namespace pix = itkpix::helpers;
    auto hw = pix::spec_init(hw_config_filename);

    if(!hw) {
        std::cout << "ERROR: Initialized hw controller is nullptr" << std::endl;
        return 1;
    }
    hw->setTrigEnable(0);

    std::stringstream efuses_data;
    for(size_t i = 0; i < 4; i++) {
        auto fe = pix::rd53b_init(hw, connectivity_config_filename, i);
        if(!fe) {
            std::cout << "ERROR: Initialized fe chip " << i << " is nullptr" << std::endl;
            return 1;
        }

        if (fe->ServiceBlockEn.read() == 0) {
            std::cout << "ERROR: RD53B register messages are not enabled" << std::endl;
            return 1;
        }
        hw->flushBuffer();
        auto cfg = dynamic_cast<Rd53bCfg*>(fe.get());
        hw->setCmdEnable(cfg->getTxChannel());
        hw->setRxEnable(cfg->getRxChannel());
        if(chip_id < 0) {
            uint32_t efuse_data = pix::read_efuses(fe, hw);
            std::stringstream efuse;
            efuse << "EFUSE:0x" << std::hex << efuse_data;
            efuses_data << " " << efuse.str();
        } else if (i == chip_id) {
            uint32_t efuse_data = pix::read_efuses(fe, hw);
            std::stringstream efuse;
            efuse << "EFUSE:0x" << std::hex << efuse_data;
            efuses_data << " " << efuse.str();
        }
    }
    std::cout << efuses_data.str() << std::endl;
    return 0;
}
