picosystem_executable(
    3dDemo
    demo.cpp
    small3dlib.c
    S3L_port.c
    texture_model.c
)

pixel_double(3dDemo)
disable_startup_logo(3dDemo)
