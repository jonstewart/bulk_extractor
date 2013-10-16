#include "findopts.h"

bool FindOpts::empty() const {
  return Files.empty() && Patterns.empty();
}

FindOpts& FindOpts::get() {
  static FindOpts singleton;
  return singleton;
}
