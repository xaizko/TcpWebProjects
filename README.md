# 🌐 Simple Web Server in C

This project is a minimalist web server written in C. It handles HTTP `GET` requests, serves static HTML and image files, and demonstrates the fundamental building blocks of how web servers work.  

---

## 🚀 Features

- Handles multiple clients using `fork()`
- Parses basic HTTP GET requests
- Serves HTML and image files
- Sends valid HTTP responses with proper headers
- Simple routing for paths like `/app/webpage` and `/img/filename.png`
  
---

## ⏰ Future Improvements

- Upload complete hmtl, css, and js files
- Convert to a library for reusability as a self-hosting website 
 
---

## 🧠 What I Learned

### 🔌 Socket Programming Basics
Learned how to:
- Create sockets with `socket()`
- Bind to a port using `bind()`
- Listen for incoming connections via `listen()`
- Accept client connections with `accept()`

### 🗣 Handling Client Communication
- Read HTTP requests using `read()`
- Write responses using `write()`
- Understood how request-response cycles work at a low level

### 🧾 Parsing HTTP Requests
- Built a custom parser to extract HTTP method and URL
- Reinforced understanding of HTTP as plain text communication

### 💡 Web Servers Just Send Text!
- Discovered that serving a website is as simple as sending formatted text
- Browsers interpret HTML, CSS, and other data based on headers—not magic

### 🖼 Serving Static Files
- Implemented basic file I/O using `open()`, `read()`, and `realloc()`
- Returned binary image data over a socket by setting correct headers

### 🧵 Handling Concurrent Clients
- Used `fork()` to handle each connection in a child process
- Learned about process-based concurrency for simple use cases

### 🧪 Respecting HTTP Standards
- Learned the importance of proper header formatting
  - e.g. `Content-Type`, `Content-Length`, and newlines (`\r\n\r\n`)
- Ensured compatibility with browsers and tools like `curl`

### 🌍 The Internet is Simple at Its Core
- Understood how basic text exchange forms the foundation of modern web communication
- Gained confidence in building foundational backend systems from scratch

---

## 🧪 Try It Out

1. **Compile the server and run it:**
   ```sh
   gcc -o webserver webserver.c
   ./webserver.c

   
2. **Test in your browser:**
   ```sh
   http://localhost:<port>/app/webpage
   http://localhost:8282/img/<name_of_photo.png>

  This will serve the raw image from your local file system.
