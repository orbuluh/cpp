#include <unistd.h>

#include "sample.capnp.h"
#include "sample_client.h"

int main() {
  SampleClient client;
  client.start();
  sleep(1);
  while (client.is_running()) {
    client.initialize();
    sleep(1);
  }
  return 0;
}
