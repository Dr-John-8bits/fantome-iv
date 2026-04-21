#pragma once

#include <string>

namespace fantome {

class DaisySessionStore {
 public:
  virtual ~DaisySessionStore() = default;

  virtual void Init() = 0;
  virtual std::string SessionPath() const = 0;
  virtual std::string BackendLabel() const = 0;
};

class DaisySessionFileStoreStub final : public DaisySessionStore {
 public:
  DaisySessionFileStoreStub() = default;
  explicit DaisySessionFileStoreStub(std::string session_path);

  void Init() override;
  std::string SessionPath() const override;
  std::string BackendLabel() const override;

  void SetSessionPath(std::string session_path);

 private:
  std::string session_path_ {};
};

}  // namespace fantome
