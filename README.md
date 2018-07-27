# CtrlrFx-0.9

----------

**NOTE**: This project is obsolete in the face of the C++11 standard. Much of the library deals with wrapping OS features like threading, signaling, date, and time in a portable fashion, but those features are covered better now in the standard library. Some features like allocation-free collections, binary streams, and networking classes are still useful, but should be upgraded to use move semantics and other new language capabilities.

----------

The Controller Framework is an object-oriented C++ class library for implementing real-time and embedded systems, with an emphasis on rapid application development, efficiency, and portability. The Framework includes classes for data manipulation, inter-processor communication, general I/O, PID control, and wrappers for several popular real-time operating systems, particularly low-cost, open-source, and royalty-free systems, such as POSIX (Embedded Linux), eCos, and uC/OS-II. The Framework also includes generic interfaces for common hardware classes, including A/D & D/A converters, digital I/O ports, motors, Pulse Width Modulators, and hardware timers.

The Framework is written almost entirely in C++ and has been ported to a number of platforms. 
