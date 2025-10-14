#ifndef BASESUBSYSTEM_H
#define BASESUBSYSTEM_H
#include "BaseClass.h"

namespace Classes {
class BaseSubsystem : public BaseClass {
 public:
  /// @brief Default deconstructor
  virtual ~BaseSubsystem() = default;

  /// @brief Constructor with id
  BaseSubsystem(const BaseSetup& setup) : BaseClass(setup) {}

  /// @brief Sets pwm value
  virtual void setPWM(uint16_t value) = 0;

  /// @brief Writes pwm value
  virtual void writePWM() = 0;
};
}  // namespace Classes

#endif