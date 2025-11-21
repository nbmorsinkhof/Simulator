#ifndef SIMULATOR_HPP
#define SIMULATOR_HPP

#include <vector>

#include "state_space.hpp"
#include "CsvLogger.hpp"

struct SystemObjects;
class StateSpacePendulum;


struct LogData {
    float time;
    std::vector<float> state;
    std::vector<float> observation;
    std::vector<float> control_input;
};

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

    std::vector<float> f_state_space(std::vector<float> state);
    
    //member variables
    float dt_ = 0.005; //seconds

    //functions

    float forward_euler(float dt, float ddt_x, float x_prev);

    std::vector<float> runge_kutta(float dt, std::vector<float> x_prev);

    static void stateFormatter(std::ostream& os, const LogData& data);

    void log_data();
    //float runge_kutta(float dt, float ddt_x, float x_prev);

private:
    SystemObjects* SystemObjects_ = nullptr;
    LogData logdata_; 
    CsvLogger<LogData> logger_;
    
};


#endif