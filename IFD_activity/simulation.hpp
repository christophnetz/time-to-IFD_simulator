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

double intake(double n, double p, Param param_) {
  double intake = 0.0;
  switch (param_.functional_response) {
  case 1:
    intake = param_.a * n / p;
    break;

  case 2:
    intake = param_.a * n / (1.0 + param_.a * param_.h * n + param_.q1 * p);
    break;

  case 3:
    intake = param_.a * n / (1.0 + param_.a * param_.h * n + param_.q1 * p / (1.0 + param_.a * param_.h * n));
    break;

  case 4:
    intake = param_.a * pow((p / param_.r), -param_.m2) * n / (1.0 + param_.a * pow((p / param_.r), -param_.m2) * param_.h * n);
    break;

  case 5:
    intake = pow((p / param_.r), -param_.m1) * param_.a * n / (1.0 + param_.a * param_.h * n);
    break;

  case 6:
    intake = (1.0 - param_.m3 * log(p / param_.r)) * param_.a * n / (1.0 + param_.a * param_.h * n);
    break;
  case 7:
    intake = (1.0 - param_.q2 * p) * param_.a * n / (1.0 + param_.a * param_.h * n);
    break;

  }

  return max(intake, 0.0);
}

struct ind {

  ind() {}
  ind(int x, int y, double a) : xpos(x), ypos(y), act(a), food(0.0) {}



  void move(const vector<vector<double>>& landscape, vector<vector<int>>& presence, Param param_);

  double food;
  double act;
  int xpos;
  int ypos;
};

void ind::move(const vector<vector<double>>& landscape, vector<vector<int>>& presence, Param param_) {

  //double present_intake = landscape[xpos][ypos] / static_cast<double> (presence[xpos][ypos]);
  double present_intake = intake(landscape[xpos][ypos], static_cast<double> (presence[xpos][ypos]), param_);
  double potential_intake;
  int former_xpos = xpos;
  int former_ypos = ypos;

  for (int i = 0; i < landscape.size(); ++i) {
    for (int j = 0; j < landscape[i].size(); ++j) {
      //potential_intake = landscape[i][j] / (static_cast<double> (presence[i][j]) + 1.0);
      potential_intake = intake(landscape[i][j] , (static_cast<double> (presence[i][j]) + 1.0), param_);
      if (present_intake < potential_intake) {
        present_intake = potential_intake;
        xpos = i;
        ypos = j;
      }
    }
  }

  presence[xpos][ypos] += 1;
  presence[former_xpos][former_ypos] -= 1;
}

bool check_IFD(const vector<ind>& pop, const vector < vector<double>>& landscape, const vector<vector<int>>& presence, Param param_) {


  for (int p = 0; p < pop.size(); ++p) {
    //double present_intake = landscape[pop[p].xpos][pop[p].ypos] / static_cast<double> (presence[pop[p].xpos][pop[p].ypos]);
    double present_intake = intake(landscape[pop[p].xpos][pop[p].ypos], static_cast<double> (presence[pop[p].xpos][pop[p].ypos]), param_);

    for (int i = 0; i < landscape.size(); ++i) {
      for (int j = 0; j < landscape[i].size(); ++j) {
        if (present_intake < intake(landscape[i][j], (static_cast<double> (presence[i][j]) + 1.0), param_)) {
          return false;
        }
      }
    }


  }
  return true;
}

double count_IFD(const vector<ind>& pop, const vector < vector<double>>& landscape, const vector<vector<int>>& presence, Param param_) {

  int count = pop.size();
  int p = 0;
label:
  for (; p < pop.size(); ++p) {
    //double present_intake = landscape[pop[p].xpos][pop[p].ypos] / static_cast<double> (presence[pop[p].xpos][pop[p].ypos]);
    double present_intake = intake(landscape[pop[p].xpos][pop[p].ypos], static_cast<double> (presence[pop[p].xpos][pop[p].ypos]), param_);

    for (int i = 0; i < landscape.size(); ++i) {
      for (int j = 0; j < landscape[i].size(); ++j) {
        if (present_intake < intake(landscape[i][j], (static_cast<double> (presence[i][j]) + 1.0), param_)) {
          --count;
          ++p;
          goto label;
        }
      }
    }


  }
  return static_cast<double>(count) / pop.size();
}

double intake_variance(const vector<ind>& pop, const vector < vector<double>>& landscape, const vector<vector<int>>& presence, Param param_) {
  vector<double> intakes;
  int p = 0;
  for (; p < pop.size(); ++p) {
    //intakes.push_back(landscape[pop[p].xpos][pop[p].ypos] / static_cast<double> (presence[pop[p].xpos][pop[p].ypos]));
    intakes.push_back(intake(landscape[pop[p].xpos][pop[p].ypos], static_cast<double> (presence[pop[p].xpos][pop[p].ypos]), param_));
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


void landscape_setup(vector<vector<double>> & landscape) {
  for (int i = 0; i < landscape.size(); ++i) {
    for (int j = 0; j < landscape[i].size(); ++j) {
      landscape[i][j] = uniform_real_distribution<double>(0.5, 1.0)(rnd::reng);
    }
  }
}

vector<ind> population_setup(double a, int pop_size, int dims) {


  //uniform_real_distribution<double> a_dist(0.5 - a, 0.5 + a);

  vector<ind> pop;
  auto pdist = std::uniform_int_distribution<int>(0, dims - 1);
  for (int i = 0; i < pop_size; ++i) {
    pop.emplace_back(pdist(rnd::reng), pdist(rnd::reng), 0.5);
  }

  for (int i = 0; i < pop_size; ++i) {
    if (i % 2) {
      pop[i].act = a;
    }
    else {
      pop[i].act = 1.0 - a;

    }
  }

  return pop;

}



void simulation(const Param& param_) {

  std::vector<double> v_act = { 0.05,0.1,0.2,0.3,0.4,0.5 };
  std::vector<int> v_popsize = { 50,200,400,1000,2000,4000,8000 };




  std::ofstream ofs1(param_.outdir + "ifd.txt", std::ofstream::out);
  std::ofstream ofs2(param_.outdir + "contin_ifd.txt", std::ofstream::out);
  ofs1 << "act" << "\t" << "pop_size" << "\t" << "iter" << "\t" << "time_to_IFD" << "\t" << "ifd_prop" << "\tstddev" << "\n";
  ofs2 << "act" << "\t" << "pop_size" << "\t" << "iter" << "\t" << "time" << "\t" << "ifd_prop" << "\tstddev" << "\n";

  //std::ofstream ofs2("IDF2.txt", std::ofstream::out);
  //ofs2 << "G" << "\t" << "prop_ifd" << "\t" << "avg_ttifd" << "\t" << endl;

  std::ofstream ofs3(param_.outdir + "params.txt", std::ofstream::out);
  ofs3 << "dims" << "\t" << param_.outdir << "\n"
    << "run_time" << "\t" << param_.outdir << "\n"
    << "num_scenes" << "\t" << param_.outdir << "\n";

  ofs3.close();


  //landscape initialization
  vector<vector<double>> landscape(param_.dims, vector<double>(param_.dims, 1.0));

  for (int psize = 0; psize < v_popsize.size(); ++psize) {

    for (int iact = 0; iact < v_act.size(); ++iact) {

      for (int scenes = 0; scenes < param_.scenes; ++scenes) {


        vector<ind> pop = population_setup(v_act[iact], v_popsize[psize], param_.dims);

        vector<double> activities;
        vector<vector<int>> presence(param_.dims, vector<int>(param_.dims, 0));
        for (int i = 0; i < pop.size(); ++i) {
          activities.push_back(pop[i].act);
          presence[pop[i].xpos][pop[i].ypos] += 1;
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
        double increment = 0.01;
        bool IFD_reached = false;
        double time_to_IFD = param_.t_scenes;

        for (; time < param_.t_scenes; ) {
          //cout << time << "\n";
          time += event_dist(rnd::reng);


          if (time > it_t) {
            ofs2 << v_act[iact] << "\t" << v_popsize[psize] << "\t" 
              << scenes << "\t" << it_t << "\t" << count_IFD(pop, landscape, presence, param_)
              << "\t" << intake_variance(pop, landscape, presence, param_) << "\n";
            it_t = floor(time / increment) * increment + increment;
          }

          if (!IFD_reached) {
            id = rdist(rnd::reng);
            pop[id].move(landscape, presence, param_);
            IFD_reached = check_IFD(pop, landscape, presence, param_);
          }
          else {
            time_to_IFD = time;
          }



          //cout << time << "\t" << IFD_reached << "\t" << endl;
        }
        //prop idf fulfilled
        ifd_prop = count_IFD(pop, landscape, presence, param_);
        stdev = intake_variance(pop, landscape, presence, param_);


        ofs1 << v_act[iact] << "\t" << v_popsize[psize] << "\t" << scenes << "\t" << time_to_IFD << "\t" << ifd_prop << "\t" << stdev << "\n";

      }
      cout << v_act[iact] << "\n";
    }
    cout << v_popsize[psize] << "\n";

  }
  ofs1.close();
  ofs2.close();
  cout << "End";



}
