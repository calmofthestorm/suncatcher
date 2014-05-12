// This file is part of Suncatcher
// Alex Roper <alex@aroper.net>
//
// Suncatcher is free software: you can redistribute it and/or modify it under
// the terms of version 3 of the GNU General Public License as published by the
// Free Software Foundation.
//
// Suncatcher is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
// more details.
//
// You should have received a copy of the GNU General Public License along with
// Suncatcher.  If not, see <http://www.gnu.org/licenses/>.
//
// Copyright 2014 Alex Roper

#ifndef POLYMORPHICEUCLIDEANSTATEINTERFACE_d8e5b2a7003a455aaf8171a8b9493afa
#define POLYMORPHICEUCLIDEANSTATEINTERFACE_d8e5b2a7003a455aaf8171a8b9493afa

#include "suncatcher/graph/EuclideanPatherState.hh"
#include "suncatcher/graph/EuclideanCoord.hh"

namespace suncatcher {
namespace graph {



class PolymorphicEuclideanStateInterface : public PatherStateInterface {
  public:
    PolymorphicEuclideanStateInterface(EuclideanCoord size);

    virtual bool get_expanded(EuclideanCoord cell) const override;
    virtual void set_expanded(EuclideanCoord cell, bool value) override;

    virtual float get_distance(EuclideanCoord cell) const override;
    virtual void set_distance(EuclideanCoord cell, float value) override;

    virtual EuclideanCoord get_previous(EuclideanCoord cell) const override;
    virtual void set_previous(EuclideanCoord cell, EuclideanCoord value) override;

    virtual void clear() override;


  private:
    EuclideanPatherState impl;
  
};



}  // namespace graph
}  // namespace suncatcher


#include "suncatcher/graph/PolymorphicEuclideanStateInterface-inl.hh"


#endif  /* POLYMORPHICEUCLIDEANSTATEINTERFACE_d8e5b2a7003a455aaf8171a8b9493afa */
