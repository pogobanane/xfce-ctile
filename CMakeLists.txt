cmake_minimum_required(VERSION 3.0)
project(ctile)

find_package(PkgConfig REQUIRED)
pkg_check_modules(X11 REQUIRED x11)
pkg_check_modules(libwnck REQUIRED libwnck-3.0)
add_executable(ctile main.c winstate.c)
target_link_libraries(ctile ${X11_LIBRARIES} ${libwnck_LIBRARIES})
target_include_directories(ctile PUBLIC ${X11_INCLUDE_DIRS} ${libwnck_INCLUDE_DIRS})
target_compile_options(ctile PUBLIC ${X11_CFLAGS_OTHER} ${libwnck_CFLAGS_OTHER} "-DWNCK_I_KNOW_THIS_IS_UNSTABLE")

