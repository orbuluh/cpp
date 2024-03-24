@0xf0db9d3189f09c3f;

struct SomeEvent {
  someString @0 :Text;
}

interface ClientHandle {
  onServiceEvent @0 (event :SomeEvent) -> ();
}

interface ServiceCommunication {
  makeSubscription @0 (clientHandle :ClientHandle) -> ();
}