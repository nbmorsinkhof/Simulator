#ifndef STATE_SPACE_HPP
#define STATE_SPACE_HPP

#include <vector>
#include <shared_mutex>

class SystemObjects;

class StateSpace{
public:
    //constructor
    explicit StateSpace(std::size_t dim);

    //member functions
    void update_ddtState();

    //accessors
    const std::vector<float>& get_state() const;
    const std::vector<float>& get_ddtState() const;
    const std::vector<float>& get_observation() const;

    //transmutators
    void set_system(SystemObjects* s);

    void set_inititial_state(std::vector<float>);

    void set_state(float value, int index);

    void set_observation(float value, int index);

protected:
    //variables
    int dimension_;
    std::vector<float> state_;
    std::vector<float> ddt_state_;
    std::vector<float> observation_;

    // Internal computation
    virtual void state_space_equation();  // virtual for polymorphism
    virtual void state_space_observation();

private:
    SystemObjects* SystemObjects_ = nullptr;
    mutable std::shared_timed_mutex observation_mutex_;
};


/********************************* StateSpace::Pendulum**********************************************************/

// Derived class
class StateSpacePendulum: public StateSpace {
public:
    StateSpacePendulum();

    //member functions
    void set_length_arm(float length);
    void set_gravity(float gravity);
    

protected:
    void state_space_equation() override; // virtual for polymorphism
    void state_space_observation() override;
    float length_arm = 0.75;
    float g = 9.81;
    float resistance_ = 0.01;
};

#endif