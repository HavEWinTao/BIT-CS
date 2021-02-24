cmake_minimum_required(VERSION 3.15)
project(File-OS)

set(CMAKE_CXX_STANDARD 11)

add_executable(FileSystem
        main.cpp
        utils.h
        utils.cpp
        cd.h
        cd.cpp
        close.h
        close.cpp
        create.h
        create.cpp
        delete.h
        delete.cpp
        exit.h
        exit.cpp
        ls.h
        ls.cpp
        mkdir.h
        mkdir.cpp
        new.h
        new.cpp
        open.h
        open.cpp
        read.h
        read.cpp
        rmdir.h
        rmdir.cpp
        sfs.h
        sfs.cpp
        write.h
        write.cpp
        )