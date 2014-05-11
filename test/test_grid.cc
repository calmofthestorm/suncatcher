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

#include <vector>

#include "suncatcher/test/common.hh"

#include "suncatcher/util/Grid.hh"

using suncatcher::util::Grid;



// ****************************************************************************
// *** Single mutations, as simple as possible, affect the map.
// ****************************************************************************


TEST(GridTest, RowVector) {
  Grid<int> foo({10, 1, 1}, -27);
  std::vector<int> bar(10, -27);
  for (uint16_t i = 0; i < 10; ++i) {
    ASSERT_EQ(bar.at(i), foo.at({i, 0, 0}));
  }
  for (uint16_t i = 0; i < 10; ++i) {
    bar.at(i) = foo.at({i, 0, 0}) = i * 10;
  }
  for (uint16_t i = 0; i < 10; ++i) {
    ASSERT_EQ(bar.at(i), foo.at({i, 0, 0}));
  }
}


TEST(GridTest, ColVector) {
  Grid<int> foo({1, 10, 1}, -27);
  std::vector<int> bar(10, -27);
  for (uint16_t i = 0; i < 10; ++i) {
    ASSERT_EQ(bar.at(i), foo.at({0, i, 0}));
  }
  for (uint16_t i = 0; i < 10; ++i) {
    bar.at(i) = foo.at({0, i, 0}) = i * 10;
  }
  for (uint16_t i = 0; i < 10; ++i) {
    ASSERT_EQ(bar.at(i), foo.at({0, i, 0}));
  }
}


TEST(GridTest, LayerVector) {
  Grid<int> foo({1, 1, 10}, -27);
  std::vector<int> bar(10, -27);
  for (uint16_t i = 0; i < 10; ++i) {
    ASSERT_EQ(bar.at(i), foo.at({0, 0, i}));
  }
  for (uint16_t i = 0; i < 10; ++i) {
    bar.at(i) = foo.at({0, 0, i}) = i * 10;
  }
  for (uint16_t i = 0; i < 10; ++i) {
    ASSERT_EQ(bar.at(i), foo.at({0, 0, i}));
  }
}


TEST(GridTest, RowColSquare1) {
  Grid<int> foo({10, 10, 1}, 63);
  std::vector<std::vector<std::vector<int>>> bar(10, std::vector<std::vector<int>>(10, std::vector<int>(1, 63)));
  for (uint16_t i = 0; i < 10; ++i) {
    for (uint16_t j = 0; j < 10; ++j) {
      ASSERT_EQ(foo.at({i, j, 0}), bar.at(i).at(j).at(0));
    }
  }
  for (uint16_t i = 0; i < 10; ++i) {
    for (uint16_t j = 0; j < 10; ++j) {
      foo.at({i, j, 0}) = bar.at(i).at(j).at(0) = i * j * (2 * ((i * j * j) % 2) - 1);
    }
  }
  for (uint16_t i = 0; i < 10; ++i) {
    for (uint16_t j = 0; j < 10; ++j) {
      ASSERT_EQ(foo.at({i, j, 0}), bar.at(i).at(j).at(0));
    }
  }
}


TEST(GridTest, RowColSquare2) {
  Grid<int> foo({10, 10, 1}, 63);
  std::vector<std::vector<std::vector<int>>> bar(10, std::vector<std::vector<int>>(10, std::vector<int>(1, 63)));
  for (uint16_t j = 0; j < 10; ++j) {
    for (uint16_t i = 0; i < 10; ++i) {
      ASSERT_EQ(foo.at({i, j, 0}), bar.at(i).at(j).at(0));
    }
  }
  for (uint16_t j = 0; j < 10; ++j) {
    for (uint16_t i = 0; i < 10; ++i) {
      foo.at({i, j, 0}) = bar.at(i).at(j).at(0) = i * j * (2 * ((i * j * j) % 2) - 1);
    }
  }
  for (uint16_t j = 0; j < 10; ++j) {
    for (uint16_t i = 0; i < 10; ++i) {
      ASSERT_EQ(foo.at({i, j, 0}), bar.at(i).at(j).at(0));
    }
  }
}


TEST(GridTest, RowLayerSquare1) {
  Grid<int> foo({10, 1, 10}, 63);
  std::vector<std::vector<std::vector<int>>> bar(10, std::vector<std::vector<int>>(1, std::vector<int>(10, 63)));
  for (uint16_t i = 0; i < 10; ++i) {
    for (uint16_t j = 0; j < 10; ++j) {
      ASSERT_EQ(foo.at({i, 0, j}), bar.at(i).at(0).at(j));
    }
  }
  for (uint16_t i = 0; i < 10; ++i) {
    for (uint16_t j = 0; j < 10; ++j) {
      foo.at({i, 0, j}) = bar.at(i).at(0).at(j) = i * j * (2 * ((i * j * j) % 2) - 1);
    }
  }
  for (uint16_t i = 0; i < 10; ++i) {
    for (uint16_t j = 0; j < 10; ++j) {
      ASSERT_EQ(foo.at({i, 0, j}), bar.at(i).at(0).at(j));
    }
  }
}


TEST(GridTest, RowLayerSquare2) {
  Grid<int> foo({10, 1, 10}, 63);
  std::vector<std::vector<std::vector<int>>> bar(10, std::vector<std::vector<int>>(1, std::vector<int>(10, 63)));
  for (uint16_t j = 0; j < 10; ++j) {
    for (uint16_t i = 0; i < 10; ++i) {
      ASSERT_EQ(foo.at({i, 0, j}), bar.at(i).at(0).at(j));
    }
  }
  for (uint16_t j = 0; j < 10; ++j) {
    for (uint16_t i = 0; i < 10; ++i) {
      foo.at({i, 0, j}) = bar.at(i).at(0).at(j) = i * j * (2 * ((i * j * j) % 2) - 1);
    }
  }
  for (uint16_t j = 0; j < 10; ++j) {
    for (uint16_t i = 0; i < 10; ++i) {
      ASSERT_EQ(foo.at({i, 0, j}), bar.at(i).at(0).at(j));
    }
  }
}


TEST(GridTest, ColLayerSquare1) {
  Grid<int> foo({1, 10, 10}, 63);
  std::vector<std::vector<std::vector<int>>> bar(1, std::vector<std::vector<int>>(10, std::vector<int>(10, 63)));
  for (uint16_t i = 0; i < 10; ++i) {
    for (uint16_t j = 0; j < 10; ++j) {
      ASSERT_EQ(foo.at({0, i, j}), bar.at(0).at(i).at(j));
    }
  }
  for (uint16_t i = 0; i < 10; ++i) {
    for (uint16_t j = 0; j < 10; ++j) {
      foo.at({0, i, j}) = bar.at(0).at(i).at(j) = i * j * (2 * ((i * j * j) % 2) - 1);
    }
  }
  for (uint16_t i = 0; i < 10; ++i) {
    for (uint16_t j = 0; j < 10; ++j) {
      ASSERT_EQ(foo.at({0, i, j}), bar.at(0).at(i).at(j));
    }
  }
}


TEST(GridTest, ColLayerSquare2) {
  Grid<int> foo({1, 10, 10}, 63);
  std::vector<std::vector<std::vector<int>>> bar(1, std::vector<std::vector<int>>(10, std::vector<int>(10, 63)));
  for (uint16_t j = 0; j < 10; ++j) {
    for (uint16_t i = 0; i < 10; ++i) {
      ASSERT_EQ(foo.at({0, i, j}), bar.at(0).at(i).at(j));
    }
  }
  for (uint16_t j = 0; j < 10; ++j) {
    for (uint16_t i = 0; i < 10; ++i) {
      foo.at({0, i, j}) = bar.at(0).at(i).at(j) = i * j * (2 * ((i * j * j) % 2) - 1);
    }
  }
  for (uint16_t j = 0; j < 10; ++j) {
    for (uint16_t i = 0; i < 10; ++i) {
      ASSERT_EQ(foo.at({0, i, j}), bar.at(0).at(i).at(j));
    }
  }
}


class SolidGrid : public ::testing::Test {
  public:
    void check(uint16_t rows, uint16_t cols, uint16_t layers) {
      Grid<int> foo({rows, cols, layers}, 98);
      std::vector<std::vector<std::vector<int>>> bar(rows, std::vector<std::vector<int>>(cols, std::vector<int>(layers, 98)));
      for (uint16_t k = 0; k < layers; ++k) {
        for (uint16_t j = 0; j < rows; ++j) {
          for (uint16_t i = 0; i < cols; ++i) {
            ASSERT_EQ(foo.at({j, i, k}), bar.at(j).at(i).at(k));
          }
        }
      }

      uint16_t seed = 0;
      for (uint16_t k = 0; k < layers; ++k) {
        for (uint16_t j = 0; j < rows; ++j) {
          for (uint16_t i = 0; i < cols; ++i) {
            foo.at({j, i, k}) = bar.at(j).at(i).at(k) = seed++;
          }
        }
      }

      for (uint16_t k = 0; k < layers; ++k) {
        for (uint16_t j = 0; j < rows; ++j) {
          for (uint16_t i = 0; i < cols; ++i) {
            ASSERT_EQ(foo.at({j, i, k}), bar.at(j).at(i).at(k));
          }
        }
      }
    }
};


TEST_F(SolidGrid, ScalarTest) {
  check(1, 1, 1);
}


TEST_F(SolidGrid, Cube) {
  check(10, 10, 10);
  check(5, 5, 5);
}


TEST_F(SolidGrid, ThickSquare1) {
  check(10, 10, 2);
  check(10, 2, 10);
  check(2, 10, 10);
}


TEST_F(SolidGrid, ThickSquare2) {
  check(10, 10, 5);
  check(10, 5, 10);
  check(5, 10, 10);
}


TEST_F(SolidGrid, Rod1) {
  check(2, 2, 5);
  check(2, 5, 2);
  check(5, 2, 2);
}


TEST_F(SolidGrid, Weird1) {
  check(12, 7, 19);
}


TEST_F(SolidGrid, Weird2) {
  check(1, 2, 3);
  check(2, 1, 3);
  check(3, 1, 2);
}


TEST_F(SolidGrid, Weird3) {
  check(11, 13, 8);
}
