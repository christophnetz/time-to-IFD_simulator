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

inline double intake(double n, double p) { return n / p; }

struct ind {

  ind() {}
  ind(int p, double a) : pos(p), act(a) {}

  void move(const vector<double>& landscape, vector<int>& presence, Param param_);

  double act;
  int pos;
};

//Individual movement to patch with highest potential intake 
void ind::move(const vector<double>& landscape, vector<int>& presence, Param param_) {

  double present_intake = intake(landscape[pos], static_cast<double> (presence[pos]));
  double potential_intake;
  int former_pos = pos;

  for (int i = 0; i < landscape.size(); ++i) {
    potential_intake = intake(landscape[i], (static_cast<double> (presence[i]) + 1.0));
    if (present_intake < potential_intake) {
      present_intake = potential_intake;
      pos = i;
    }
  }
  presence[pos] += 1;
  presence[former_pos] -= 1;
}

//Check whether IFD has been reached
bool check_IFD(const vector<ind>& pop, const vector < double>& landscape, const vector<int>& presence) {

  for (int p = 0; p < pop.size(); ++p) {
    double present_intake = intake(landscape[pop[p].pos], static_cast<double> (presence[pop[p].pos]));

    for (int i = 0; i < landscape.size(); ++i) {
      if (present_intake < intake(landscape[i], (static_cast<double> (presence[i]) + 1.0))) {
        return false;
      }
    }
  }
  return true;
}

//Count optimally distributed individuals
double count_IFD(const vector<ind>& pop, const vector < double>& landscape, const vector<int>& presence) {

  int count = pop.size();
  int p = 0;
label:
  for (; p < pop.size(); ++p) {
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

//Calculate intake variances
double intake_variance(const vector<ind>& pop, const vector < double>& landscape, const vector<int>& presence) {
  vector<double> intakes;
  int p = 0;
  for (; p < pop.size(); ++p) {
    intakes.push_back(intake(landscape[pop[p].pos], static_cast<double> (presence[pop[p].pos])));
  }

  double sum = std::accumulate(intakes.begin(), intakes.end(), 0.0);
  double m = sum / intakes.size();

  double accum = 0.0;
  std::for_each(intakes.begin(), intakes.end(), [&](const double d) {
    accum += (d - m) * (d - m);
    });

  return accum / static_cast<double>(intakes.size());
}

// Create a landscape vector with random values between 0.5 and 1.0
vector<double> landscape_setup(int size) {
  vector<double> landscape(size);
  for (int i = 0; i < size; ++i) {
    landscape[i] = uniform_real_distribution<double>(0.5, 1.0)(rnd::reng);
  }
  return landscape;
}

// Create a population with two morphs, in different proportions
vector<ind> population_setup(double a, int pop_size, int dims, double prop) {

  int morph1 = pop_size * prop;

  vector<ind> pop;
  auto pdist = std::uniform_int_distribution<int>(0, dims - 1);
  for (int i = 0; i < morph1; ++i) {
    pop.emplace_back(pdist(rnd::reng), a);
  }

  for (int i = 0; i < pop_size - morph1; ++i) {
    pop.emplace_back(pdist(rnd::reng), (1.0 - a));
  }

  return pop;
}


//Main simulation function
void simulation(const Param & param_) {
  
  //Write parameters to file
  std::ofstream ofs3(param_.outdir + "params.txt", std::ofstream::out);
  stream_parameter(ofs3, param_, "  ", ",\n", "c(", ")");
  ofs3.close();

  //Initialize output filestreams
  std::ofstream ofs1(param_.outdir + "ifd.txt", std::ofstream::out);
  std::ofstream ofs2(param_.outdir + "contin_ifd.txt", std::ofstream::out);
  ofs1 << "dim" << "\t" << "act" << "\t" << "prop" << "\t" << "pop_size" << "\t" << "iter" << "\t" << "time_to_IFD" << "\t" << "ifd_prop" << "\tvar" << "\n";
  ofs2 << "dim" << "\t" << "act" << "\t" << "pop_size" << "\t" << "iter" << "\t" << "time" << "\t" << "ifd_prop" << "\tvar" << "\n";


  //Loop over parameter vectors
  std::for_each(param_.v_dims.begin(), param_.v_dims.end(), [&](const int dims) {
    std::for_each(param_.v_popsize.begin(), param_.v_popsize.end(), [&](const int popsize) {
      std::for_each(param_.v_act.begin(), param_.v_act.end(), [&](const int act) {
        std::for_each(param_.v_prop.begin(), param_.v_prop.end(), [&](const int prop) {

          //Run different scenes
            for (int scenes = 0; scenes < param_.scenes; ++scenes) {
              
              //Population and landscape set up
              vector<ind> pop = population_setup(act, popsize, dims, prop);
              vector<double> landscape = landscape_setup(dims);

              vector<double> activities;
              vector<int> presence(dims, 0);
              for (int i = 0; i < pop.size(); ++i) {
                activities.push_back(pop[i].act);
                presence[pop[i].pos] += 1;
              }

              rndutils::mutable_discrete_distribution<int, rndutils::all_zero_policy_uni> rdist;
              rdist.mutate(activities.cbegin(), activities.cend());
              double total_act = std::accumulate(activities.begin(), activities.end(), 0.0);
              exponential_distribution<double> event_dist(total_act);



              double time = 0.0;
              double it_t = 0.0;
              double increment = 0.1;
              bool IFD_reached = false;
              double time_to_IFD = param_.t_scenes;

              //Event-based loop
              for (; time < param_.t_scenes; ) {
                time += event_dist(rnd::reng);

                if (!IFD_reached) {
                  pop[rdist(rnd::reng)].move(landscape, presence, param_);
                  if (time > it_t) {
                    IFD_reached = check_IFD(pop, landscape, presence);
                    time_to_IFD = time;
                  }
                }

                if (time > it_t) {
                  ofs2 << dims << "\t" << act << "\t" << popsize << "\t"
                    << scenes << "\t" << it_t << "\t" << intake_variance(pop, landscape, presence) << "\n";
                  it_t = floor(time / increment) * increment + increment;
                }
              }

              ofs1 << dims << "\t" << act << "\t" << prop << "\t"
                << popsize << "\t" << scenes << "\t" << time_to_IFD << "\t" << count_IFD(pop, landscape, presence)
                << "\t" << intake_variance(pop, landscape, presence) << "\n";
            }

          });
        std::cout << act << "\n";
        });
      std::cout << popsize << "\n";
      });
    });

  ofs1.close();
  ofs2.close();
  std::cout << "End";
}
