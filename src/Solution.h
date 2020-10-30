/**
 * Warehouse manager
 *
 * @file    Solution.h
 * @date    10/23/2020
 * @author  Filip Kocica
 * @brief   Struct representing optimization problem solution
 */

#ifdef WHM_OPT

#pragma once

namespace whm
{
    struct Solution_t
    {
        double fitness{ 0.0 };           //< How good solution is (objective function)
        int32_t trialValue{ 0 };         //< Counter of iterations fitness did not improve
        std::vector<int32_t> genes;      //< Solution dimensions

        bool operator==(const Solution_t& rhs) const
        {
            return genes      == rhs.genes   &&
                   fitness    == rhs.fitness &&
                   trialValue == rhs.trialValue;
        }

        bool operator>(const Solution_t& rhs) const
        {
            return fitness > rhs.fitness;
        }

        bool operator>=(const Solution_t& rhs) const
        {
            return fitness >= rhs.fitness;
        }

        bool operator<(const Solution_t& rhs) const
        {
            return fitness < rhs.fitness;
        }

        bool operator<=(const Solution_t& rhs) const
        {
            return fitness <= rhs.fitness;
        }
    };
}

#endif
