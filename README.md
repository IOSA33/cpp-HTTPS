# HTTPS Library over TLS handshake for your Use
- Using OpenSSL for TLS handshake secure connection
- Using wepoll and threadpool for handling over 100k concurrent connections
- Works on Windows and Linux
- You can create custom middleware functions
- Sending for example HTML files to the client
- Recieve and send json
- Set headers and body for response
- Different USE middlewares

# How to Start TLS/TCP Server
## Requirements 
- c++23 and above
- g++14 compiler and above
- nlohmann/json libary
- openssl libary

## Step 1 - Generate Keys
### In main folder
```
mkdir keys
```
- In keys folder You need to Generate Cerificate.pem/Private-Key.pem

## Step 2 - Start the Server
### In main folder
```
mkdir build
cd build
cmake -G Ninja ..
ninja
./app.exe ../keys/my_certificate.pem ../keys/my_private_key.pem
```

# Examples
- Every function you can find in ```main.cpp``` file