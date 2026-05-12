/**
 * @file ThreadedSubsystem.h
 * @author Aldem Pido
 * @date 5/8/26
 * @brief Abstract base class wrapping a FreeRTOS task pinned to a core.
 *
 * Derived classes implement `initRaw()` and `updateRaw()`; the base class
 * owns the task lifecycle, init flags, and an optional logger.
 *
 * @code
 *   class MySubsystem : public Subsystem::ThreadedSubsystem {
 *    public:
 *     using Subsystem::ThreadedSubsystem::ThreadedSubsystem;
 *    protected:
 *     bool initRaw() override { ...; return true; }
 *     void updateRaw() override { ... }
 *   };
 *
 *   MySubsystem sub({"my", 4096, 1, 1, 10});
 *   sub.startTask();
 * @endcode
 */
#pragma once
#include <Arduino.h>

#include <atomic>
#include <functional>
#include <string>

namespace Subsystem {

/**
 * @brief Configuration for a ThreadedSubsystem.
 *
 * Copied into the instance, so the caller's object need not outlive it.
 */
struct BaseSetup {
  /// Task name; also used as the FreeRTOS task name.
  std::string id;
  /// Stack size in bytes (ESP-IDF FreeRTOS convention).
  uint32_t usStackDepth;
  /// FreeRTOS task priority; higher value means higher priority.
  uint8_t uxPriority;
  /// Core to pin to (0, 1, or `tskNO_AFFINITY`).
  uint8_t xCoreID;
  /// Period between `updateRaw()` calls in milliseconds; avoid 0.
  uint32_t delayMs;

  /**
   * @brief Construct a fully-specified setup.
   * @param id            Subsystem and task name.
   * @param usStackDepth  Stack size in bytes.
   * @param uxPriority    FreeRTOS priority.
   * @param xCoreID       Core to pin to.
   * @param delayMs       Period between `updateRaw()` calls in ms.
   */
  BaseSetup(const std::string& id, const uint32_t usStackDepth,
            const uint8_t uxPriority, const uint8_t xCoreID, uint32_t delayMs)
      : id(id),
        usStackDepth(usStackDepth),
        uxPriority(uxPriority),
        xCoreID(xCoreID),
        delayMs(delayMs) {}
};

/**
 * @brief Log severity for `log()`.
 *
 * Scoped to avoid clashing with macros like `ERROR` or `DEBUG`.
 */
enum class LoggerLevel {
  Communication = 0,
  Error = 1,
  Warning,
  Info,
  Debug,
  Verbose
};

/**
 * @brief Abstract FreeRTOS-backed subsystem.
 *
 * Construct with a `BaseSetup`, optionally call `setLogger()` and `init()`,
 * then call `startTask()` to spawn the loop.
 */
class ThreadedSubsystem {
 public:
  virtual ~ThreadedSubsystem() = default;

  /**
   * @brief Construct the subsystem with the given configuration.
   * @param setup  Configuration; copied into the instance.
   */
  ThreadedSubsystem(const BaseSetup& setup)
      : _setup(setup),
        _initSuccess(false),
        _initRan(false),
        _taskSuccess(false) {}

  /**
   * @brief Run one-time initialization via `initRaw()`.
   * @return `true` if `initRaw()` succeeded.
   */
  bool init() {
    _initSuccess = initRaw();
    _initRan = true;
    return _initSuccess;
  }

  /**
   * @brief Spawn the FreeRTOS task; calls `init()` on first run if needed.
   * @return Result of `xTaskCreatePinnedToCore`; `pdPASS` on success.
   */
  BaseType_t startTask() {
    BaseType_t xReturned;
    xReturned = xTaskCreatePinnedToCore(
        taskFunction, _setup.id.c_str(), _setup.usStackDepth, this,
        _setup.uxPriority, &pvTaskCode, _setup.xCoreID);
    _taskSuccess = xReturned == pdPASS;
    return xReturned;
  }

  /**
   * @brief Get the FreeRTOS task handle.
   * @return The handle, or `nullptr` if the task hasn't started successfully.
   */
  TaskHandle_t getTaskHandle() {
    if (_taskSuccess) {
      return pvTaskCode;
    } else {
      return nullptr;
    }
  }

  /**
   * @brief Install or replace the logger callback.
   * @param fn  Callback, or empty `std::function` to disable.
   */
  void setLogger(
      std::function<void(LoggerLevel, const std::string&, const std::string&)>
          fn) {
    _logger = std::move(fn);
  }

  /**
   * @brief Emit a log message via the installed logger; no-op if unset.
   * @param level      Severity level.
   * @param subsystem  Origin tag.
   * @param message    Message text.
   */
  void log(LoggerLevel level, const std::string& subsystem,
           const std::string& message) {
    if (_logger) _logger(level, subsystem, message);
  }

 protected:
  /// Owned copy of the configuration.
  BaseSetup _setup;
  /// `true` once `initRaw()` has succeeded; gates the update loop.
  std::atomic<bool> _initSuccess;
  /// `true` once `init()` has run; lets the task skip a redundant call.
  std::atomic<bool> _initRan;
  /// `true` once `startTask()` has returned `pdPASS`.
  std::atomic<bool> _taskSuccess;
  /// Optional logger callback; invoke via `log()`.
  std::function<void(LoggerLevel, const std::string&, const std::string&)>
      _logger;

  /**
   * @brief One-time setup hook implemented by derived classes.
   * @return `true` to allow the update loop to start.
   */
  virtual bool initRaw() = 0;

  /// @brief Per-tick work hook; should be non-blocking and bounded.
  virtual void updateRaw() = 0;

 private:
  /// FreeRTOS task handle, written by `startTask()`.
  TaskHandle_t pvTaskCode = nullptr;

  /// @brief Guards `updateRaw()` against a failed init.
  void update() {
    if (!_initSuccess) return;
    updateRaw();
  }

  /**
   * @brief FreeRTOS task entry point.
   * @param pvParameters  Pointer to the owning subsystem.
   */
  static void taskFunction(void* pvParameters) {
    ThreadedSubsystem* self = static_cast<ThreadedSubsystem*>(pvParameters);
    if (!self) return;
    if (!self->_initRan) {
      self->init();
    }
    if (!self->_initSuccess) return;
    while (true) {
      self->update();
      vTaskDelay(pdMS_TO_TICKS(self->_setup.delayMs));
    }
  }
};

}  // namespace Subsystem
