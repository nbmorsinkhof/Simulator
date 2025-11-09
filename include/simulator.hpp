#ifndef SIMULATOR_HPP
#define SIMULATOR_HPP

#include <vector>

#include "state_space.hpp"

struct SystemObjects;
class StateSpacePendulum;

class Simulator{
public:
    //constructor
    explicit Simulator();

    //accessors
    std::vector<float> get_observation();

    //member functions
    void set_system(SystemObjects* s);

    void update_state();
    void simulate();
    void run();
protected:
    //member variables
    float dt_ = 0.005;

    //functions

    float forward_euler(float dt, float ddt_x, float x_prev);

    //float runge_kutta(float dt, float ddt_x, float x_prev);

private:
    SystemObjects* SystemObjects_ = nullptr;
};


#endif