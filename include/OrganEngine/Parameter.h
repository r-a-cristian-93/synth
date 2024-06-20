#ifndef PARAMETER_H
#define PARAMETER_H

#include <OrganEngine/Config.h>

struct Parameter
{
    double current_value = 1.0;
    double target_value = 1.0;

    const double increment_value = 0.1;
    const double max_value = 1.0;
    const double min_value = 0.0;
    const double update_rate = 20.0 / SAMPLE_RATE;

    Parameter& operator=(const Parameter& other)
    {
        if (this != &other)
        {
            current_value = other.current_value;
            target_value = other.target_value;
        }
        return *this;
    }


    void setValue(double value)
    {
        if (value <= max_value) target_value = value;
        else target_value = max_value;

        if (value >= min_value) target_value = value;
        else target_value = min_value;
    }

    void increaseValue()
    {
        if (target_value < max_value)
            target_value += increment_value;
    }

    void decreaseValue()
    {
        if (target_value > min_value)
            target_value -= increment_value;

        if (target_value < increment_value)
            target_value = min_value;
    }

    void update()
    {
        if (current_value < target_value)
        {
            current_value += update_rate;

            if (current_value > target_value)
                current_value = target_value;
        }

        if (current_value > target_value)
        {
            current_value -= update_rate;

            if (current_value < target_value)
                current_value = target_value;
        }
    }
};


#endif
