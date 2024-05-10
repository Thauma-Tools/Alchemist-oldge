#include <Alchemist/Application.h>
#include <Alchemist/Defines.h>
#include <Alchemist/Main.h>

#include <iostream>

int main(int argc, char **argv) {
  tt::Application *app = new tt::Application;
  app->set_window_title("Alchemist Sandbox");
  app->set_window_width(1920);
  app->set_window_height(1080);

  std::cout << "Hello from Sanbox!" << std::endl;

  return 0;
}
