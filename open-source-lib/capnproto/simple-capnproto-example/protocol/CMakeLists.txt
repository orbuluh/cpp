capnp_generate_cpp(CapnProtoGenSrcs CapnProtoGenHdrs protocol.capnp)

add_library(protocol ${CapnProtoGenSrcs} ${CapnProtoGenHdrs})
