# cpp-template
C++ template project.

## Dependencies
Dependencies are listed in [conanfile.py](./conanfile.py).

## How to build
* In one command:
```sh
conan build . -s build_type=Debug --build=missing
```

* Using `conan` and `cmake`:
```sh
conan install . -s build_type=Debug --build=missing
cmake --workflow --preset debug # Use multi-debug for multi-configs.
```
Check contents of [CMakePresets.json](./CMakePresets.json) for more info.

## How to run
```sh
./build/Debug/bin/cpp-template
```
