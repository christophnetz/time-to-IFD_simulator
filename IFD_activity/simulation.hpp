#pragma once
#include "parameter.h"
#include <iostream>
#include <vector>
#include <random>
#include <cmath>
#include <chrono>
#include <numeric>
#include <fstream>
#include <algorithm>
#include <string>

#include "rnd.hpp"

using namespace cine2;
using namespace std;

inline double intake(double n, double p) {


  //return max(intake, 0.0);
  return n / p;

}

struct ind {

  ind() {}
  ind(int p, double a) : pos(p), act(a) {}



  void move(const vector<double>& landscape, vector<int>& presence, Param param_);

  double act;
  int pos;
};

void ind::move(const vector<double>& landscape, vector<int>& presence, Param param_) {

  //double present_intake = landscape[xpos][ypos] / static_cast<double> (presence[xpos][ypos]);
  double present_intake = intake(landscape[pos], static_cast<double> (presence[pos]));
  double potential_intake;
  int former_pos = pos;

  for (int i = 0; i < landscape.size(); ++i) {
      //potential_intake = landscape[i][j] / (static_cast<double> (presence[i][j]) + 1.0);
      potential_intake = intake(landscape[i], (static_cast<double> (presence[i]) + 1.0));
      if (present_intake < potential_intake) {
        present_intake = potential_intake;
        pos = i;        
      }
    
  }

  presence[pos] += 1;
  presence[former_pos] -= 1;


}

bool check_IFD(const vector<ind>& pop, const vector < double>& landscape, const vector<int>& presence, Param param_) {


  for (int p = 0; p < pop.size(); ++p) {
    //double present_intake = landscape[pop[p].xpos][pop[p].ypos] / static_cast<double> (presence[pop[p].xpos][pop[p].ypos]);
    double present_intake = intake(landscape[pop[p].pos], static_cast<double> (presence[pop[p].pos]));

    for (int i = 0; i < landscape.size(); ++i) {
        if (present_intake < intake(landscape[i], (static_cast<double> (presence[i]) + 1.0))) {
          return false;
        }
      
    }


  }
  return true;
}

double count_IFD(const vector<ind>& pop, const vector < double>& landscape, const vector<int>& presence, Param param_) {

  int count = pop.size();
  int p = 0;
label:
  for (; p < pop.size(); ++p) {
    //double present_intake = landscape[pop[p].xpos][pop[p].ypos] / static_cast<double> (presence[pop[p].xpos][pop[p].ypos]);
    double present_intake = intake(landscape[pop[p].pos], static_cast<double> (presence[pop[p].pos]));

    for (int i = 0; i < landscape.size(); ++i) {
        if (present_intake < intake(landscape[i], (static_cast<double> (presence[i]) + 1.0))) {
          --count;
          ++p;
          goto label;
        }
    }


  }
  return static_cast<double>(count) / pop.size();
}

double intake_variance(const vector<ind>& pop, const vector < double>& landscape, const vector<int>& presence, Param param_) {
  vector<double> intakes;
  int p = 0;
  for (; p < pop.size(); ++p) {
    //intakes.push_back(landscape[pop[p].xpos][pop[p].ypos] / static_cast<double> (presence[pop[p].xpos][pop[p].ypos]));
    intakes.push_back(intake(landscape[pop[p].pos], static_cast<double> (presence[pop[p].pos])));
  }

  double sum = std::accumulate(intakes.begin(), intakes.end(), 0.0);
  double m = sum / intakes.size();

  double accum = 0.0;
  std::for_each(intakes.begin(), intakes.end(), [&](const double d) {
    accum += (d - m) * (d - m);
    });

  double stdev = sqrt(accum / (intakes.size()));

  return stdev;
}


void landscape_setup(vector<double> & landscape) {
  for (int i = 0; i < landscape.size(); ++i) {
      landscape[i] = uniform_real_distribution<double>(0.5, 1.0)(rnd::reng);
  }
}

vector<ind> population_setup(double a, int pop_size, int dims, double prop) {

  int morph1 = pop_size * prop;
  //uniform_real_distribution<double> a_dist(0.5 - a, 0.5 + a);

  vector<ind> pop;
  auto pdist = std::uniform_int_distribution<int>(0, dims - 1);
  for (int i = 0; i < morph1; ++i) {
    pop.emplace_back(pdist(rnd::reng), a);
  }

  for (int i = 0; i < pop_size - morph1; ++i) {
    pop.emplace_back(pdist(rnd::reng), (1.0-a));
  }

  return pop;

}



void simulation(const Param & param_) {

  std::vector<double> v_act(param_.v_act.begin(), param_.v_act.end());
  //std::vector<int> v_popsize = { 50,200,400,1000,2000,4000,8000 };
  std::vector<int> v_popsize(param_.v_popsize.begin(), param_.v_popsize.end());

  std::vector<int> v_dims(param_.v_dims.begin(), param_.v_dims.end());



  std::ofstream ofs1(param_.outdir + "ifd.txt", std::ofstream::out);
  std::ofstream ofs2(param_.outdir + "contin_ifd.txt", std::ofstream::out);
  ofs1 << "dim" << "\t" << "act" << "\t" << "prop" << "\t" << "pop_size" << "\t" << "iter" << "\t" << "time_to_IFD" << "\t" << "ifd_prop" << "\tstddev" << "\n";
  ofs2 << "dim" << "\t" << "act" << "\t" << "pop_size" << "\t" << "iter" << "\t" << "time" << "\t" << "ifd_prop" << "\tstddev" << "\n";

  //std::ofstream ofs2("IDF2.txt", std::ofstream::out);
  //ofs2 << "G" << "\t" << "prop_ifd" << "\t" << "avg_ttifd" << "\t" << endl;

  std::ofstream ofs3(param_.outdir + "params.txt", std::ofstream::out);
  stream_parameter(ofs3, param_, "  ", ",\n", "c(", ")");
  ofs3.close();

  for (int idim = 0; idim < v_dims.size(); idim++) {
    vector<double> landscape(v_dims[idim], 1.0);

    //}
    //landscape initialization

    for (int psize = 0; psize < v_popsize.size(); ++psize) {
      //int psize = idim;
      for (int iact = 0; iact < v_act.size(); ++iact) {
        for(int iprop = 0; iprop < param_.v_prop.size(); ++iprop){
        for (int scenes = 0; scenes < param_.scenes; ++scenes) {


          vector<ind> pop = population_setup(v_act[iact], v_popsize[psize], landscape.size(), param_.v_prop[iprop]);

          vector<double> activities;
          vector<int> presence(v_dims[idim], 0);
          for (int i = 0; i < pop.size(); ++i) {
            activities.push_back(pop[i].act);
            presence[pop[i].pos] += 1;
          }

          rndutils::mutable_discrete_distribution<int, rndutils::all_zero_policy_uni> rdist;
          rdist.mutate(activities.cbegin(), activities.cend());
          double total_act = std::accumulate(activities.begin(), activities.end(), 0.0);
          exponential_distribution<double> event_dist(total_act);

          double ifd_prop;
          double stdev;

          //Landscape set up
          landscape_setup(landscape);

          double time = 0.0;
          int id;
          double it_t = 0.0;
          double increment = 0.1;
          bool IFD_reached = false;
          double time_to_IFD = param_.t_scenes;

          for (; time < param_.t_scenes; ) {
            //cout << time << "\n";
            time += event_dist(rnd::reng);



            if (!IFD_reached) {
              id = rdist(rnd::reng);
              pop[id].move(landscape, presence, param_);
              if (time > it_t) {
                IFD_reached = check_IFD(pop, landscape, presence, param_);
                time_to_IFD = time;
              }
            }

            if (time > it_t) {
              ofs2 << v_dims[idim] << "\t" << v_act[iact] << "\t" << v_popsize[psize] << "\t"
                << scenes << "\t" << it_t /*<< "\t" << count_IFD(pop, landscape, presence, param_)*/
                << "\t" << intake_variance(pop, landscape, presence, param_) << "\n";
              it_t = floor(time / increment) * increment + increment;
            }




            //cout << time << "\t" << IFD_reached << "\t" << endl;
          }
          //prop idf fulfilled
          ifd_prop = count_IFD(pop, landscape, presence, param_);
          stdev = intake_variance(pop, landscape, presence, param_);


          ofs1 << v_dims[idim] << "\t" << v_act[iact] << "\t" << param_.v_prop[iprop] << "\t" << v_popsize[psize] << "\t" << scenes << "\t" << time_to_IFD << "\t" << ifd_prop << "\t" << stdev << "\n";
        }
        }
        std::cout << v_act[iact] << "\n";
      }
      std::cout << v_popsize[psize] << "\n";

    }
  }
  ofs1.close();
  ofs2.close();
  std::cout << "End";



}
