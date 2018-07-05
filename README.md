# NetSocket

Cross-platform C++11 Network Socket Interface

### Dependencies

* [CMake](https://cmake.org/)

### Third Party Software (included)

* [Asio](http://think-async.com/Asio/) (non-Boost)

### API

See example directory for sample server and client applications.

### TLS Certificate and Key Generation

* Generate keys:
    * `openssl req -new -newkey rsa:2048 -nodes -out ca.csr -keyout ca.key`
    * `openssl x509 -trustout -signkey ca.key -days 365 -req -in ca.csr -out ca.crt`
    * `cat ca.crt ca.key > server_crt_key.pem`
* Generate dh file:
    * `openssl dhparam -out dh.pem 2048`

### Interfacing with Other Libraries / Languages

NetSocket send and receive methods use a simple custom protocol. A 5-byte header is prepended to all messages with information about the data type (string vs binary) and message size.

##### Header

* Byte 0: data type (0: string, 1: binary)
* Bytes 1-4: message size (unsigned 32-bit integer in network byte order)

##### Data

* Untyped byte array with length equal to the message size specified in the header. String messages should be null terminated
