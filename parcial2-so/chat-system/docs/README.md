# Chat System Documentation

## Overview
This project implements a chat system using System V message queues in Linux. It consists of a server and multiple clients that can communicate with each other through defined chat rooms. The system allows users to send and receive messages, join chat rooms, and manage user interactions.

## Project Structure
```
chat-system
├── src
│   ├── servidor.c          # Server implementation
│   ├── cliente.c           # Client implementation
│   ├── common.h            # Common definitions and structures
│   └── utils
│       ├── message_queue.c  # Message queue utility functions
│       └── message_queue.h  # Message queue utility function declarations
├── include
│   └── chat_protocol.h      # Communication protocol definitions
├── build                    # Compiled files and build artifacts
├── docs
│   └── README.md            # Project documentation
├── tests
│   ├── test_server.c        # Unit tests for server functionality
│   └── test_client.c        # Unit tests for client functionality
├── scripts
│   ├── compile.sh           # Compilation script
│   └── cleanup.sh           # Cleanup script
├── Makefile                 # Build instructions
└── README.md                # Project overview and usage instructions
```

## Compilation and Execution
To compile the project, navigate to the root directory of the project and run the following command:

```bash
make
```

This will generate the executables for both the server and the client in the `build` directory.

To run the server, execute:

```bash
./build/servidor
```

To run the client, execute:

```bash
./build/cliente
```

## Usage
1. Start the server first.
2. Then, start one or more clients.
3. Clients can join chat rooms and start sending messages.

## Testing
Unit tests for both the server and client can be found in the `tests` directory. To run the tests, you can compile the test files and execute them:

```bash
gcc -o test_server tests/test_server.c src/servidor.c src/utils/message_queue.c
./test_server

gcc -o test_client tests/test_client.c src/cliente.c src/utils/message_queue.c
./test_client
```

## Scripts
- `scripts/compile.sh`: Automates the compilation process.
- `scripts/cleanup.sh`: Cleans up resources created during execution, such as message queues and temporary files.

## License
This project is licensed under the MIT License. See the LICENSE file for more details.

## Acknowledgments
Thanks to all contributors and libraries that made this project possible.