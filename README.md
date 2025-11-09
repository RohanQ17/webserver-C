# 🌐 HTTP Server in C

A lightweight, fast HTTP webserver implementation written in pure C. Built for learning and understanding the fundamentals of web servers and network programming.

## ✨ Features

- 🚀 **High Performance** - Minimal overhead with raw C implementation
- 📡 **HTTP/1.1 Support** - Handles standard HTTP requests and responses
- 🔧 **Lightweight** - No external dependencies, pure C
- 📁 **Static File Serving** - Serves HTML, CSS, JS, and other static files
- 🛡️ **Error Handling** - Robust error handling and logging
- ⚡ **Concurrent Connections** - Multi-threaded request handling

## 🚀 Quick Start

### Prerequisites

- GCC compiler
- POSIX-compliant system (Linux/macOS)
- Basic knowledge of C programming

### Installation

```bash
# Clone the repository
git clone https://github.com/yourusername/http-server-c.git
cd http-server-c

# Compile the server
make

# Or compile manually
gcc -o server server.c -lpthread
```

## 🔧 Configuration

The server can be configured through command-line arguments:

| Parameter | Description | Default |
|-----------|-------------|---------|
| Port | Server port number | 8080 |
| Document Root | Web files directory | `./public` |
| Max Connections | Maximum concurrent connections | 100 |

## 🛠️ Development

### Building

```bash
# Debug build
make debug

# Release build
make release

# Clean build files
make clean
```

### Testing

```bash
# Run basic tests
make test

# Test with curl
curl -v http://localhost:8080/
```

## 📊 Performance

- **Requests/sec**: ~10,000 on modern hardware
- **Memory usage**: < 5MB base footprint
- **Concurrent connections**: Up to 1000 (configurable)

## 🤝 Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

1. Fork the project
2. Create your feature branch (`git checkout -b feature/AmazingFeature`)
3. Commit your changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

## 📝 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

##  Acknowledgments

- Inspired by the need to understand HTTP fundamentals
- Built with ❤️ for the C programming community
- Special thanks to my mates.

---

⭐ **Star this repository if you found it helpful!** ⭐
