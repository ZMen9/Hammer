#pragma once

#ifdef HM_PLATFORM_WINDOWS

extern Hammer::Application* Hammer::CreateApplication();

int main(int argc, char** argv) {
	auto app = Hammer::CreateApplication();
  app->Run();
  delete app;
  return 0;
}

#endif