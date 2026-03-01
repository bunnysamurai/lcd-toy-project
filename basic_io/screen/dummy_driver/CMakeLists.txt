add_library(dummy_driver screen_impl.cpp)
target_compile_features(dummy_driver PUBLIC cxx_std_20)
target_compile_options(dummy_driver PUBLIC -O3)
target_link_options(dummy_driver PUBLIC -flto)