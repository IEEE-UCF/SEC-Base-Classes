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

  /// @brief Update, pure virtual function
  virtual void update() = 0;

  /// @brief Begin, must contain code to move the subsystem to an active state.
  virtual void begin() = 0;

  /// @brief Pause, must contain code to move the subsystem to a paused state.
  virtual void pause() = 0;

  /// @brief Reset, must contain code to move the subsystem to a reset state.
  virtual void reset() = 0;
};
}  // namespace Classes

#endif