#ifndef BASEDRIVER_H
#define BASEDRIVER_H
#include "BaseClass.h"

namespace Classes {
class BaseDriver : public BaseClass {
 public:
  /// @brief Default deconstructor
  virtual ~BaseDriver() = default;

  /// @brief Constructor with id
  BaseDriver(const BaseSetup& setup) : BaseClass(setup) {}

  /// @brief Update, pure virtual function
  virtual void update() = 0;
};
}  // namespace Classes

#endif