#ifndef BASECLASS_H
#define BASECLASS_H

namespace Classes {
class BaseSetup {
 public:
  ~BaseSetup() = default;
  BaseSetup() = default;
  BaseSetup(const char* _id) : id(_id) {}
  const char* getId() const { return id; }

 private:
  const char* id;
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
  virtual const char* getInfo() = 0;

  /// @brief Get initialization success
  /// @return successful if true
  bool initSuccess() const { return initSuccess_; }

 protected:
  BaseSetup setup_;
  bool initSuccess_;  /// tracks if initalization is successful
};
}  // namespace Classes

#endif