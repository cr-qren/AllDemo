# Raspberry-Qt

## Introduction

This repository is a basic template for a Qt5 project for a customized camera system. The core structure of the project is as below:

```
.
├── include
|   └── {customized_lib_header_files}
├── src
|   ├── {customized_lib_source_files}
|   ├── CMakeLists.txt
|   └── tools
|       ├── {main_files}
|       └── CMakeLists.txt
├── data
|   └── {test_data_etc}
├── CMakeLists.txt
├── .gitignore
└── README.md
```

Directory [include/](./include) contains all the header files needed for the project.

Directory [src/](./src) contains all the source files needed for the project.

Subdirectory [tools/](./src/tools) of [src/](./src) contains the main files for execution.

Directory [data/](./data) contains all the testing files/data needed for the project.

## Quick start

Download the reporitory and do the following steps:

1. cd {your_repo_root}
2. mkdir build
3. cd build
4. cmake ..
5. make
6. ./bin/demo
