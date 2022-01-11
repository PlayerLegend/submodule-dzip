# dzip

This is a submodule providing dzip inflate and deflate functionality, as well as compatibility with convert streams defined in the [convert submodule](../submodule-convert/)

# Dependencies

The C standard library
sys/time.h  
[submodule-range](submodule-range)  
[submodule-window](submodule-window)  
[submodule-convert](submodule-convert)  
[submodule-vluint](submodule-vluint)  
[submodule-log](submodule-log)

# Description

dzip compression is designed to be simple and also to integrate well with my other projects.
For simplicity, it is currently only a deduplicating algorithm, but it does allow for extensions.

