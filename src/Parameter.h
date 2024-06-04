#ifndef PARAMETER_H
#define PARAMETER_H

#include <cmath>

#include "Common.h"

struct Parameter
{
    double current_value = 1.0;
    double target_value = 1.0;

    const double increment_value = 0.1;
    const double max_value = 1.0;
    const double min_value = 0.0;
    const double update_rate = 1.0;

    double change_time_start = 0.0;

    Parameter& operator=(const Parameter& other)
    {
        if (this != &other)
        {
            current_value = other.current_value;
            target_value = other.target_value;
            change_time_start = other.change_time_start;
        }
        return *this;
    }


    void setValue(double value)
    {
        change_time_start = g_time;

        if (value <= max_value) target_value = value;
        else target_value = max_value;

        if (value >= min_value) target_value = value;
        else target_value = min_value;
    }

    void increaseValue()
    {
        change_time_start = g_time;

        if (target_value < max_value)
            target_value += increment_value;
    }

    void decreaseValue()
    {
        change_time_start = g_time;

        if (target_value > min_value)
            target_value -= increment_value;

        if (target_value < increment_value)
            target_value = min_value;
    }

    void update()
    {
        if (target_value != current_value)
        {
            double step_value = pow(g_time - change_time_start, 2) * update_rate;

            if (step_value < PARAM_LOWEST_VALUE) step_value = 0;

            if (target_value < current_value)
            {
                current_value -= step_value;

                if (current_value < target_value)
                    current_value = target_value;
            }

            if (target_value > current_value)
            {
                current_value += step_value;

                if (current_value > target_value)
                    current_value = target_value;
            }
        }
    }
};


#endif
