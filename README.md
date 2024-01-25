# libtlvcpp

`libtlvcpp` is a modern C++ ASN.1 tree manipulating & encoding/decoding library with a focus on a clean interface and automatic memory management. The library consists of three main components: `tree_node`, `tlv`, and `tlv_tree_node`. The `tree_node` represents a generic tree structure, `tlv` represents a TLV element, and `tlv_tree_node` is a specialized tree node for TLV elements.

## Features

- **Clean Interface**: Provides a simple and intuitive interface for working with TLV data structures.
- **Automatic Memory Management**: Utilizes modern C++ features to handle memory management automatically.
- **Flexible Tree Structure**: The `tree_node` class allows you to create a flexible tree structure for organizing TLV elements.
- **Works on**: Windows, Linux, macOS, ESP-IDF, and Webassembly using Emscripten.

## Usage

### Including the Library

```cpp
#include "tlv_tree.h"
```

### Creating a TLV Tree

```cpp
// Create a TLV tree node
tlvcpp::tlv_tree_node root;

// Add TLV elements to the tree
root.add_child(1, "Value1");
auto& childNode = root.add_child(2, "Value2");

// Add child elements to a node
childNode.add_child(3, "Value3");

// Dump the tree structure
root.dump();
```

### Serializing and Deserializing

```cpp
// Serialize the TLV tree to a vector
std::vector<uint8_t> buffer;
root.serialize(buffer);

// Deserialize the TLV tree from a vector
tlvcpp::tlv_tree_node newRoot;
newRoot.deserialize(buffer);
```

### Finding Elements

```cpp
// Find a TLV element by tag
tlvcpp::tlv* foundElement = root.find(2);

// Find an immediate child TLV element by tag
tlvcpp::tlv* foundImmediateElement = root.find_immediate(2);
```

## Build Process

`libtlvcpp` uses CMake for its build process. Follow these steps to build the library:

1. Create a build directory:

   ```bash
   mkdir build
   cd build
   ```

2. Generate build files with CMake:

   ```bash
   cmake ..
   ```

3. Build the library:

   ```bash
   cmake --build .
   ```

## License

This library is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Contributing

Feel free to contribute by opening issues or pull requests on the [GitHub repository](https://github.com/your-username/libtlvcpp).

Happy coding!
