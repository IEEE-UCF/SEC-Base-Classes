#ifndef BASECLASS_H
#define BASECLASS_H
#include <string>

namespace Classes {
struct BaseSetup {
  std::string id = "undefined";
  explicit BaseSetup(const std::string& _id) : id(_id) {}
};
class BaseClass {
 public:
  /// @brief Default deconstructor
  virtual ~BaseClass() = default;

  /// @brief Constructor with id
  BaseClass(const BaseSetup& setup) : setup_(setup) {}

  /// @brief Init, pure virtual function
  virtual void init() = 0;

  /// @brief getInfo, pure virtual function
  virtual std::string getInfo() = 0;

  /// @brief  Get the id of the class
  /// @return ID of class
  std::string getId() const { return setup_.id; }

 protected:
  BaseSetup setup_;
};
}  // namespace Classes

#endif