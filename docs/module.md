# Modules

The library is organised in module. Even the core of the library is a module.

Each module extend the library functionality and you can decide do activate/deactivate modules you consider you may
need or not. Each module can have another module as dependency or external dependency.

## Directory Structure

```
MyFencyModule
│   CMakeLists.txt
│
├───public/
│   └───netero/
│           (...)
│
├───private/
│       (...)
│
└───tests/
        CMakeLists.txt
```

 * CMakeLists.txt  
 The root CMakeLists is the main project file of the module.

 * public/netero  
 The public directory contains file exposed to client application and other modules. It contains a netero/
 subdirectory for header files.

 * private/  
 The private directory contains file that are not exposed to client application and module likely sources files,
 you are free to
