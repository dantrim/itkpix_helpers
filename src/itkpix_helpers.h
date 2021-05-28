#ifndef RD53B_HELPERS_H
#define RD53B_HELPERS_H

// std/stl
#include <memory>  // shared_ptr
#include <string>

// json
//#include <nlohmann/json.hpp>
//#include "json.hpp"
//using json = nlohmann::json;
#include "storage.hpp"

// yarr
// class SpecController;
#include "SpecController.h"
// class Rd53b;
#include "Rd53b.h"

namespace itkpix {

namespace helpers {
std::unique_ptr<SpecController> spec_init(std::string config);
bool spec_init_trigger(std::unique_ptr<SpecController>& hw,
                       json trigger_config);
bool spec_trigger_loop(std::unique_ptr<SpecController>& hw);

std::unique_ptr<Rd53b> rd53b_init(std::unique_ptr<SpecController>& hw,
                                  std::string config, int chip_num = 0);

uint32_t read_efuses(std::unique_ptr<Rd53b>& fe, std::unique_ptr<SpecController>& hw);
std::pair<uint32_t, uint32_t> decode_register(uint32_t higher, uint32_t lower);
uint32_t read_register(std::unique_ptr<Rd53b>& fe, std::unique_ptr<SpecController>& hw, uint32_t register_address);
void rd53b_configure(std::unique_ptr<SpecController>& hw, std::unique_ptr<Rd53b>& fe);
void configure_init(std::unique_ptr<SpecController>& hw, std::unique_ptr<Rd53b>& fe);
void configure_global(std::unique_ptr<SpecController>& hw, std::unique_ptr<Rd53b>& fe);
void configure_pixels(std::unique_ptr<SpecController>& hw, std::unique_ptr<Rd53b>& fe);
bool rd53b_reset(std::unique_ptr<SpecController>& hw,
                 std::unique_ptr<Rd53b>& fe);

bool clear_tot_memories(std::unique_ptr<SpecController>& hw,
                        std::unique_ptr<Rd53b>& fe,
                        float pixel_fraction = 100.0);
bool disable_pixels(std::unique_ptr<Rd53b>& fe);
void set_core_columns(std::unique_ptr<Rd53b>& fe,
                      std::array<uint16_t, 4> cores);

};  // namespace helpers

};  // namespace itkpix

#endif


