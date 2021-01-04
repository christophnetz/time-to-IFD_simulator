#include <iostream>
#include <vector>
#include <random>
#include <cmath>
#include <chrono>
#include <numeric>
#include <fstream>
#include <algorithm>
#include <string>
#include "rndutils.hpp"

using namespace std;
/// Parameters
const int dims = 20;
//const int pop_size = 1000;
//const int Gmax = 80000;
const int run_time = 100;//100
//double mutation_rate = 0.01; //0.001
//double mutation_shape = 0.0100;//0.1
const int num_scenes = 30;//10
//const double fcost = 0.005;
const string ID_run = "17-11-1";

const vector<int> v_popsize = { 50, 200, 400, 1000, 2000, 4000, 8000 };
const vector<double> def_act = { 0.05, 0.1, 0.2, 0.3, 0.4, 0.5 };

std::mt19937_64 rng;

struct ind {

  ind() {
    food = 0.0;
    act = uniform_real_distribution<double>(0.15, 0.19)(rng);
    xpos = uniform_int_distribution<int>(0, dims - 1)(rng);
    ypos = uniform_int_distribution<int>(0, dims - 1)(rng);

  }
  ind(double a) {
    food = 0.0;
    act = a;
    xpos = uniform_int_distribution<int>(0, dims - 1)(rng);
    ypos = uniform_int_distribution<int>(0, dims - 1)(rng);

  }
  void move(const vector<vector<double>>& landscape, vector<vector<int>>& presence);

  double food;
  double act;
  int xpos;
  int ypos;
};

void ind::move(const vector<vector<double>>& landscape, vector<vector<int>>& presence) {

  double present_intake = landscape[xpos][ypos] / static_cast<double> (presence[xpos][ypos]);
  double potential_intake;
  int former_xpos = xpos;
  int former_ypos = ypos;

  for (int i = 0; i < dims; ++i) {
    for (int j = 0; j < dims; ++j) {
      potential_intake = landscape[i][j] / (static_cast<double> (presence[i][j]) + 1.0);
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

bool check_IFD(const vector<ind>& pop, const vector < vector<double>>& landscape, const vector<vector<int>>& presence) {


  for (int p = 0; p < pop.size(); ++p) {
    double present_intake = landscape[pop[p].xpos][pop[p].ypos] / static_cast<double> (presence[pop[p].xpos][pop[p].ypos]);

    for (int i = 0; i < dims; ++i) {
      for (int j = 0; j < dims; ++j) {
        if (present_intake < landscape[i][j] / (static_cast<double> (presence[i][j]) + 1.0)) {
          return false;
        }
      }
    }


  }
  return true;
}

double count_IFD(const vector<ind>& pop, const vector < vector<double>>& landscape, const vector<vector<int>>& presence) {

  int count = pop.size();
  int p = 0;
label:
  for (; p < pop.size(); ++p) {
    double present_intake = landscape[pop[p].xpos][pop[p].ypos] / static_cast<double> (presence[pop[p].xpos][pop[p].ypos]);

    for (int i = 0; i < dims; ++i) {
      for (int j = 0; j < dims; ++j) {
        if (present_intake < landscape[i][j] / (static_cast<double> (presence[i][j]) + 1.0)) {
          --count;
          ++p;
          goto label;
        }
      }
    }


  }
  return static_cast<double>(count) / pop.size();
}

double intake_variance(const vector<ind>& pop, const vector < vector<double>>& landscape, const vector<vector<int>>& presence) {
  vector<double> intakes;
  int p = 0;
  for (; p < pop.size(); ++p) {
    intakes.push_back(landscape[pop[p].xpos][pop[p].ypos] / static_cast<double> (presence[pop[p].xpos][pop[p].ypos]));
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


void landscape_setup(vector<vector<double>>& landscape, const int popsize) {
  for (int i = 0; i < dims; ++i) {
    for (int j = 0; j < dims; ++j) {
      landscape[i][j] = uniform_real_distribution<double>(0.5 * popsize / 1000.0, 1.0 * popsize / 1000.0)(rng);
    }
  }
}

vector<ind> population_setup(double a, int pop_size) {

  vector<ind> pop(pop_size);

  //uniform_real_distribution<double> a_dist(0.5 - a, 0.5 + a);

  for (int i = 0; i < pop_size; ++i) {
    if (i % 2) {
      pop[i].act = a;
    }
    else {
      pop[i].act = 1.0 - a;

    }

    //pop[i].act = a_dist(rng);

  }

  return pop;

}




int main() {

  unsigned seed =
    static_cast<unsigned>(std::chrono::high_resolution_clock::now().time_since_epoch().count());
  std::clog << "random_seed : " << seed << '\n';
  rng.seed(seed);

  std::ofstream ofs1(ID_run + "ifd.txt", std::ofstream::out);
  std::ofstream ofs2(ID_run + "contin_ifd.txt", std::ofstream::out);
  ofs1 << "act" << "\t" << "pop_size" << "\t" << "iter" << "\t" << "time_to_IFD" << "\t" << "ifd_prop" << "\tstddev"<< "\n";
  ofs2 << "act" << "\t" << "pop_size" << "\t" << "iter" << "\t" << "time" << "\t" << "ifd_prop" << "\n";

  //std::ofstream ofs2("IDF2.txt", std::ofstream::out);
  //ofs2 << "G" << "\t" << "prop_ifd" << "\t" << "avg_ttifd" << "\t" << endl;

  std::ofstream ofs3(ID_run + "params.txt", std::ofstream::out);
  ofs3 << "dims" << "\t" << dims << "\n"
    << "run_time" << "\t" << run_time << "\n"
    << "num_scenes" << "\t" << num_scenes << "\n";

  ofs3.close();


  //landscape initialization
  vector<vector<double>> landscape(dims, vector<double>(dims, 1.0));

  for (int psize = 0; psize < v_popsize.size(); ++psize) {

    for (int iact = 0; iact < def_act.size(); ++iact) {

      for (int scenes = 0; scenes < num_scenes; ++scenes) {


        vector<ind> pop = population_setup(def_act[iact], v_popsize[psize]);

        vector<double> activities;
        vector<vector<int>> presence(dims, vector<int>(dims, 0));
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
        landscape_setup(landscape, v_popsize[psize]);

        double time = 0.0;
        int id;
        double it_t = 0.0;
        double increment = 0.01;
        bool IFD_reached = false;
        double time_to_IFD = run_time;

        for (; time < run_time; ) {
          //cout << time << "\n";
          time += event_dist(rng);


          if (time > it_t) {
            ofs2 << def_act[iact] << "\t" << v_popsize[psize] << "\t" << scenes  << "\t" << it_t <<"\t" << count_IFD(pop, landscape, presence) << "\n";
            it_t = floor(time / increment) * increment + increment;
          }

          if (!IFD_reached) {
            id = rdist(rng);
            pop[id].move(landscape, presence);
            IFD_reached = check_IFD(pop, landscape, presence);
          }
          else {
            time_to_IFD = time;
            break;
          }



          //cout << time << "\t" << IFD_reached << "\t" << endl;
        }
        //prop idf fulfilled
        ifd_prop = count_IFD(pop, landscape, presence);
        stdev = intake_variance(pop, landscape, presence);
        

        ofs1 << def_act[iact] << "\t" << v_popsize[psize] << "\t" << scenes << "\t" << time_to_IFD << "\t" << ifd_prop << "\t" << stdev << "\n";

      }
      cout << def_act[iact] << "\n";
    }
    cout << v_popsize[psize] << "\n";

  }
  ofs1.close();
  ofs2.close();
  cout << "End";

  return 0;
}