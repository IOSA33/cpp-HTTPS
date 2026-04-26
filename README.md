# HTTPS Library over TLS handshake for your Use
- Using OpenSSL for TLS handshake secure connection
- Using wepoll and threadpool for handling over 100k concurrent connections
- Works currently only on WinSock2
- You can create custom middleware functions
- Sending for example HTML files to the client
- Recieve and send json
- Set headers and body for response
- Different USE middlewares

# How to Start TLS
## Step 1
### In main folder
```
mkdir keys
- You need to generate yours own OpenSSL keys (Private/Public-Key.pem)
- Also (Certificate.pem)
```
## Step 2
### In main folder
```
mkdir build
cd build
cmake ..
ninja
./app.exe ../keys/my_certificate.pem ../keys/my_private_key.pem
```

# Examples
- Every function you can find in ```main.cpp``` file