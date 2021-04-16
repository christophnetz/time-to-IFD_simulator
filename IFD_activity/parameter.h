#ifndef CINE2_PARAMETER_H_INCLUDED
#define CINE2_PARAMETER_H_INCLUDED

#include <string>
#include "cmd_line.h"
#include <array>
#include <vector>

namespace cine2 {



  struct Param
  {
    int scenes;                // time ticks per generation
    int t_scenes;                // time ticks per generation

    std::vector<int> v_popsize;
    double resource_min;
    double resource_max;
    double act_a;
    double act_b;

    std::vector<double> v_prop;
    std::vector<int> v_cells;



    std::string outdir;   // output folder
    




  };


  Param parse_parameter(cmd::cmd_line_parser& clp);
  cmd::cmd_line_parser config_file_parser(const std::string& config);

  // write as textfile
  std::ostream& stream_parameter(std::ostream& os,
    const Param& param,
    const char* prefix,
    const char* postfix,
    const char* lb,
    const char* rb);


}
#endif
