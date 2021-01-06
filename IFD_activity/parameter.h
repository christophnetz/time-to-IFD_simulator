#ifndef CINE2_PARAMETER_H_INCLUDED
#define CINE2_PARAMETER_H_INCLUDED

#include <string>
#include "cmd_line.h"
#include <array>

namespace cine2 {

  //const int dims = 20;
  //const int pop_size = 10000;
  //const int Gmax = 80000;
  //const int run_time = 10;//100
  //double mutation_rate = 0.01; //0.001
  //double mutation_shape = 0.0100;//0.1
  //const int num_scenes = 10;//10
  //const double fcost = 0.0001;
  //const string ID_run = "03_29-11";


  struct Param
  {
    int scenes;                // time ticks per generation
    int t_scenes;                // time ticks per generation

    //std::array<int, 7> v_popsize;
    int dims;
    double resource_min;
    double resource_max;
    int functional_response;

    //std::array<double, 6> v_act;

    double a;
    double h;
    double q1;
    double q2;
    double m1;
    double m2;
    double m3;
    double r;


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
