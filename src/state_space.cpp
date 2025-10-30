#include <cmath>
#include <iostream>
#include <vector>
#include <shared_mutex>


#include "state_space.hpp"
#include "system.hpp"

/**************************** State Space ***************************/
//constructor
StateSpace::StateSpace(std::size_t dim)
 : dimension_{dim}, state_(dim, 0.0f), ddt_state_(dim, 0.0f)
{
    std::cout << "State: " << state_[0] << std::endl;
}
//functions
void StateSpace::state_space_equation() {
    return;
}
void StateSpace::state_space_observation(){
    return;
}

//accessor functions
const std::vector<float>& StateSpace::get_state() const{
    return state_;
}
const std::vector<float>& StateSpace::get_ddtState() const{
    return ddt_state_;
}
const std::vector<float>& StateSpace::get_observation() const{
    std::shared_lock<std::shared_timed_mutex> lock(observation_mutex_);
    return observation_;
}
//mutation functions
void StateSpace::set_system(SystemObjects* s){
    SystemObjects_ = s;
}
void StateSpace::set_state(float value, int index){
    if(index>state_.size()){
        throw std::invalid_argument("size mismatch.");
        return;
    }
    state_.at(index) = value;
}

void StateSpace::set_observation(float value, int index){
    if(index>observation_.size()){
        throw std::invalid_argument("Size mismatch.");
        return;
    }
    std::shared_lock<std::shared_timed_mutex> lock(observation_mutex_);
    observation_.at(index) = value;
}

// Other functions
void StateSpace::update_ddtState(){
    state_space_equation();
    state_space_observation();//segmentation fault because of this
}

void StateSpace::set_inititial_state(std::vector<float> initial_state){
    if(initial_state.size()!=static_cast<size_t>(dimension_)){
        throw std::invalid_argument("size mismatch.");
        return;
    }
    state_.assign(initial_state.begin(), initial_state.end());
}


/**************************** State Space: Pendulum ***************************/
//Pendulum constructor
StateSpacePendulum::StateSpacePendulum()
 : StateSpace(2)
 {
    observation_ = {0.0, 0.0};
 }

void StateSpacePendulum::state_space_equation(){
    ddt_state_[0] = state_[1];
    ddt_state_[1] = -(g/length_arm)*std::sin(state_[0]) - resistance_*state_[1];
}

void StateSpacePendulum::state_space_observation(){
    observation_[0] = length_arm*std::sin(state_[0]);
    observation_[1] = length_arm*std::cos(state_[0]);
}

void StateSpacePendulum::set_length_arm(float length){
    length_arm = length;
}