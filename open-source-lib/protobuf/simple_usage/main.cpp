#include "proto/message.pb.h"

#include <google/protobuf/util/time_util.h>

#include <iostream>

using google::protobuf::util::TimeUtil;

int main() {
  // Verify that the version of the library that we linked against is
  // compatible with the version of the headers we compiled against.
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  simple_demo::PersonDetail pd;
  pd.set_name("abc");
  pd.set_id(123);
  pd.set_email("abc@aa.com");

  for (int i = 0; i < 3; ++i) {
    simple_demo::PersonDetail::PhoneNumber* phoneNumber  = pd.add_phones();
    phoneNumber->set_number("3345678" + std::to_string(i));
    phoneNumber->set_type(static_cast<simple_demo::PersonDetail::PhoneType>(i % 3));
  }

  *(pd.mutable_last_updated()) = TimeUtil::SecondsToTimestamp(time(NULL));

  std::cout << pd.DebugString() << std::endl;
  return 0;
}