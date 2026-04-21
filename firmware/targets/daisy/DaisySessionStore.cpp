#include "DaisySessionStore.h"

#include <filesystem>

namespace fantome {

std::string DaisyDefaultSessionPath()
{
  return (std::filesystem::temp_directory_path() / "fantome_iv_daisy_target_session.txt").string();
}

DaisySessionFileStoreStub::DaisySessionFileStoreStub(std::string session_path)
  : session_path_(std::move(session_path))
{
}

void DaisySessionFileStoreStub::Init()
{
  if (!session_path_.empty()) {
    return;
  }

  session_path_ = DaisyDefaultSessionPath();
}

std::string DaisySessionFileStoreStub::SessionPath() const
{
  return session_path_;
}

std::string DaisySessionFileStoreStub::BackendLabel() const
{
  return "file-stub";
}

void DaisySessionFileStoreStub::SetSessionPath(std::string session_path)
{
  session_path_ = std::move(session_path);
}

}  // namespace fantome
