#pragma once

#ifdef HM_PLATFORM_WINDOWS

extern hammer::Application* hammer::CreateApplication();

int main(int argc, char** argv) {

  hammer::Log::Init();
  HM_CORE_WARN("Initialized Log!");
  int a = 5;
  HM_INFO("Hello Client={0}!", a);

	auto app = hammer::CreateApplication();
  app->Run();
  delete app;
  return 0;
}

#endif