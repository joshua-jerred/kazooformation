/// @author Joshua Jerred
/// @copyright Copyright (c) 2025
/// @file main.cpp
/// @date 2025-03-14

#include <array>
#include <iostream>
#include <random>
#include <vector>

#include "model_tester.hpp"

#include <ktl/models/k1_model.hpp>
#include <ktl/models/k2/k2_model.hpp>

double k1Test() {
  kazoo::model::K1Model::Model symbol_model_ = kazoo::model::K1Model::Model{};
  kazoo::model::K1Model::Stream symbol_stream_{symbol_model_};
  kazoo::model::K1Model::Transcoder symbol_transcoder_{symbol_model_};

  kazoo::ModelTester model_tester{symbol_model_, symbol_stream_,
                                  symbol_transcoder_};
  model_tester.runAllTests();

  return model_tester.getScore();
}

double k2Test() {
  kazoo::model::K2PeakModel::Model symbol_model_ =
      kazoo::model::K2PeakModel::Model{};
  kazoo::model::K2PeakModel::Stream symbol_stream_{symbol_model_};
  kazoo::model::K2PeakModel::Transcoder symbol_transcoder_{symbol_model_};

  kazoo::ModelTester model_tester{symbol_model_, symbol_stream_,
                                  symbol_transcoder_};
  model_tester.runAllTests();

  return model_tester.getScore();
}

int main() {
  std::cout << "Scoring Models" << std::endl;
  double score = 0;

  score = k1Test();
  std::cout << "K1 Model Score: " << score << std::endl;
  score = k2Test();
  std::cout << "K2 Peak Model Score: " << score << std::endl;

  return 0;
}