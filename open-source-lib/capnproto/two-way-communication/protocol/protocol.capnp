@0xf0db9d3189f09c3f;

struct FooData {
  fooString @0 :Text;
}

struct BarData {
  barString @0 :Text;
}

enum DataType {
  fooData @0;
  barData @1;
}

interface DataSubscriberHandle(T) {
  # Note:
  # DataSubscriberHandle::Service is data subscriber
  # DataSubscriberHandle::Client is data provider

  onSubscribedData @0 (dataFromPublisher: T) -> ();
}

interface DataProviderInterface {
  # Note:
  # DataProviderInterface::Service is data provider
  # DataProviderInterface::Client is data subscriber

  makeSubscription @0 [T] (dataType: DataType,
                           subscriberHandle: DataSubscriberHandle(T)) -> ();
}