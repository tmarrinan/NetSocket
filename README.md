# NetSocket

C++11 Network Socket Interface

### Dependencies

* [CMake](https://cmake.org/)

### Third Party Software (included)

* [Asio](http://think-async.com/Asio/) (non-Boost)

### API

See example directory for sample server and client applications.

### Interfacing with Other Libraries / Languages

The NetSocket send and receive methods use a simple custom protocol. A 5-byte header is prepended to all messages with information about the data type (string vs binary) and message size.

##### Header

* Byte 0: data type (0: string, 1: binary)
* Bytes 1-4: message size (unsigned 32-bit integer in network byte order)

##### Data

* Untyped byte array with length equal to the message size specified in the header
