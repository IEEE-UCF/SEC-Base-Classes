#ifndef BASECLASS_H
#define BASECLASS_H
#include <string>

namespace Classes {
class BaseSetup {
 public:
  BaseSetup() = default;
  BaseSetup(const std::string& _id) : id(_id) {}
  std::string getId() { return id; }

 private:
  std::string id = "undefined";
};
class BaseClass {
 public:
  /// @brief Default deconstructor
  virtual ~BaseClass() = default;

  /// @brief Constructor with id
  BaseClass(const BaseSetup& setup) : setup_(setup), initSuccess_(false) {}

  /// @brief Init, pure virtual function
  /// @return Init successful or not
  virtual bool init() = 0;

  /// @brief getInfo, pure virtual function
  virtual std::string getInfo() = 0;

  /// @brief  Get the id of the class
  /// @return ID of class
  std::string getId() const { return setup_.id; }

  /// @brief Get initialization success
  /// @return successful if true
  bool initSuccess() const { return initSuccess_; }

 protected:
  BaseSetup setup_;
  bool initSuccess_;  /// tracks if initalization is successful
};
}  // namespace Classes

#endif