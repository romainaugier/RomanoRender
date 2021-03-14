# Adding our executable
add_executable(${PROJECT_NAME} Main.cpp)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)

