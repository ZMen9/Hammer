#pragma once
#include "Hammer/Core/Base.h"
#ifdef HM_PLATFORM_WINDOWS

extern hammer::Application* hammer::CreateApplication();

int main(int argc, char** argv) {

  hammer::Log::Init();
  
  HM_PROFILE_BEGIN_SESSION("Startup", "HammerProfile-Startup.json");
	auto app = hammer::CreateApplication();
  HM_PROFILE_END_SESSION();

  HM_PROFILE_BEGIN_SESSION("Runtime", "HammerProfile-Runtime.json");
  app->Run();
  HM_PROFILE_END_SESSION();

  HM_PROFILE_BEGIN_SESSION("Shutdown", "HammerProfile-Shutdown.json");
  delete app;
  HM_PROFILE_END_SESSION();

  return 0;
}

#endif
