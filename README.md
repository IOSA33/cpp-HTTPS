# HTTPS Library over TLS handshake for your Use
- Using OpenSSL for TLS handshake secure connection
- Using wepoll and threadpool for handling over 100k concurrent connections
- Works currently only on WinSock2
- You can create custom middleware functions
- Sending for example HTML files to the client
- Recieve and send json
- Set headers and body for response
- Different USE middlewares

# How to Start
```
cd TCP
mkdir build
cd build
cmake ..
ninja
./app.exe
```

# Examples
- Every function you can find in ```main.cpp``` file