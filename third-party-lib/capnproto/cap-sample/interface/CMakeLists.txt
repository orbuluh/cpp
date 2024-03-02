capnp_generate_cpp(CapnProtoGenSrcs CapnProtoGenHdrs sample.capnp)

add_library(cap_sample_interface ${CapnProtoGenSrcs} ${CapnProtoGenHdrs})
