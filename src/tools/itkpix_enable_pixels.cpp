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
                                {"cur", required_argument, NULL, 'x'},
                                {0,0,0,0}};

bool applyMask(unsigned col, unsigned row, unsigned cur = 0) {
    unsigned core_row = row / 8;
    unsigned serial = (core_row*64)+((col+(core_row%8))%8)*8+row%8;
    return ((serial % 64) == cur);
}

int main(int argc, char* argv[]) {

    std::string connectivity_config_filename = "";
    std::string hw_config_filename = "";
    int chip_id = -1;
    bool disable = false;
    bool en_only = false;
    bool inj_only = false;
    bool hitbus_only = false;
    bool single_mask = false;
    unsigned m_cur = 0;
    int c = 0;
    while ((c = getopt_long(argc, argv, "r:c:i:dabhsx:", longopts_t, NULL)) != -1) {
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
            case 'd' :
                disable = true;
                break;
            case 'a' :
                en_only = true;
                break;
            case 'b' :
                inj_only = true;
                break;
            case 'h' :
                hitbus_only = true;
                break;
            case 's' :
                single_mask = true;
                break;
            case 'x' :
                m_cur = std::stoi(optarg);
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

    unsigned nCol = 400;
    unsigned nRow = 384;

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
        unsigned set_value = (disable ? 0 : 1);

        std::vector<std::pair<unsigned, unsigned>> modPixels;

        unsigned n_pix_en = 0;
        unsigned n_pix_inj = 0;
        unsigned n_pix_hit = 0;
        if(chip_id < 0) {
            for(unsigned col = 0; col < nCol; col++) {
                for(unsigned row = 0; row < nRow; row++) {
                    bool modified = false;

                    if(single_mask) {
                        if(!applyMask(col, row, m_cur)) continue;
                    }

                    if(!(inj_only || hitbus_only)) {
                        fe->setEn(col, row, set_value);
                        modified = true;
                        n_pix_en++;
                    }

                    if(!(en_only || hitbus_only)) {
                        fe->setInjEn(col, row, set_value);
                        modified = true;
                        n_pix_inj++;
                    }

                    if(!(en_only || inj_only)) {
                        fe->setHitbus(col, row, set_value);
                        modified = true;
                        n_pix_hit++;
                    }

                    if(modified) modPixels.push_back(std::make_pair(col, row));
                } // row
            } // col
        } else if (i == chip_id) {
            for(unsigned col = 0; col < nCol; col++) {
                for(unsigned row = 0; row < nRow; row++) {
                    bool modified = false;
                    if(single_mask) {
                        if(!applyMask(col, row, m_cur)) continue;
                    }

                    if(!(inj_only || hitbus_only)) {
                        fe->setEn(col, row, set_value);
                        modified = true;
                        n_pix_en++;
                    }

                    if(!(en_only || hitbus_only)) {
                        fe->setInjEn(col, row, set_value);
                        modified = true;
                        n_pix_inj++;
                    }

                    if(!(en_only || inj_only)) {
                        fe->setHitbus(col, row, set_value);
                        modified = true;
                        n_pix_hit++;
                    }
                    if(modified) modPixels.push_back(std::make_pair(col, row));

                } // row
            } // col

            fe->configurePixels(modPixels);
        }

        std::cout << "Chip " << chip_id << ": " << n_pix_en << " " << (set_value == 1 ? "enabled" : "disabled") << ", " << n_pix_inj << " inj. " << (set_value == 1 ? "enabled" : "disabled") << n_pix_hit << " hitbus " << (set_value == 1 ? "enabled" : "disabled") << std::endl;
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
