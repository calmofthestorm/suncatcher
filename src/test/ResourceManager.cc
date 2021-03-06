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

#include "suncatcher/test/ResourceManager.hh"

#include <fstream>

#include "suncatcher/test/DeltaMap.hh"
#include "suncatcher/MapBuilder.hh"
#include "suncatcher/MapView.hh"
#include "suncatcher/graph/EuclideanGraphBuilder.hh"
#include "suncatcher/graph/PolymorphicEuclideanGraph.hh"

namespace suncatcher {
namespace test {

namespace {

pathfinder::MapView view_from_stream(std::istream& is) {
  using graph::PolymorphicEuclideanGraph;
  assert(is);
  auto graph_builder = graph::EuclideanGraphBuilder(is);
  #ifdef POLYMORPHIC_API
  auto builder = pathfinder::MapBuilder(std::move(graph_builder));
  #else
  auto builder = pathfinder::MapBuilder(std::move(graph_builder));
  #endif
  return pathfinder::MapView(std::move(builder));
}

}  // anonymous namespace

const DeltaMap& ResourceManager::get_map(std::string fn) {
  if (get_me().map_cache.find(fn) == get_me().map_cache.end()) {
    std::ifstream is(std::string("maps/" + fn + ".txt"));
    if (!is) {
      std::cerr << "Unable to find maps file. Run tests from project root." << std::endl;
      exit(-1);
    }
    get_me().map_cache[fn] = DeltaMap(view_from_stream(is));
  }
  return get_me().map_cache[fn];
}


ResourceManager& ResourceManager::get_me() {
  static ResourceManager self;
  return self;
}


ResourceManager::ResourceManager() { }

}  // namespace test
}  // namespace suncatcher
