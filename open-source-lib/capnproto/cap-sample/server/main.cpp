#include <atomic>

#include "sample_server.h"

int main() {
  std::atomic<bool> keep_run = true;
  auto server = kj::refcounted<SampleServer>();
  kj::Thread another_thread([&]() {
    while (keep_run) {
      server->broadcastEvents();
      sleep(1);
    }
  });
  server->start("unix:sample.sock");
  keep_run = false;
  return 0;
}
