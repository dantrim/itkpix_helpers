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
                                {"do-imux", no_argument, NULL, 'a'},
                                {"val", required_argument, NULL, 'v'},
                                {0,0,0,0}};

int main(int argc, char* argv[]) {

    std::string connectivity_config_filename = "";
    std::string hw_config_filename = "";
    int chip_id = -1;
    uint32_t value = 0;
    bool do_imux = false;
    int c = 0;
    while ((c = getopt_long(argc, argv, "r:c:i:v:a", longopts_t, NULL)) != -1) {
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
            case 'v' :
                value = static_cast<uint32_t>(std::stoi(optarg));
                break;
            case 'a' :
                do_imux = true;
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

    for(size_t i = 0; i < 4; i++) {
        if(chip_id >= 0 && i != chip_id) continue;
        auto fe = pix::rd53b_init(hw, connectivity_config_filename, i);
        if(!fe) {
            std::cout << "ERROR: Initialized fe chip " << i << " is nullptr" << std::endl;
            return 1;
        }
        auto cfg = dynamic_cast<Rd53bCfg*>(fe.get());
        hw->setCmdEnable(cfg->getTxChannel());
        hw->setTrigEnable(0);
        if(chip_id < 0) {
            fe->writeNamedRegister("MonitorEnable", 1);
            // do all chips
            if(do_imux) {
                fe->writeNamedRegister("MonitorV", 1);
                fe->writeNamedRegister("MonitorI", value);
            } else {
                fe->writeNamedRegister("MonitorV", value);
            }
        } else if (i == chip_id) {
            fe->writeNamedRegister("MonitorEnable", 1);
            if(do_imux) {
                fe->writeNamedRegister("MonitorV", 1);
                fe->writeNamedRegister("MonitorI", value);
            } else {
                fe->writeNamedRegister("MonitorV", value);
            }
            
        }
        //if(i != chip_id) {
        //    fe->writeNamedRegister("MonitorV", 63);
        //    fe->writeNamedRegister("MonitorI", 63);
        //    fe->writeNamedRegister("MonitorEnable", 0);
        //} else {
        //    fe->writeNamedRegister("MonitorEnable", 1);
        //    if(do_imux) {
        //        fe->writeNamedRegister("MonitorV", 1);
        //        fe->writeNamedRegister("MonitorI", value);
        //    } else {
        //        fe->writeNamedRegister("MonitorV", value);
        //        fe->writeNamedRegister("MonitorI", 63);
        //    }
        //}
    }
    return 0;
}
