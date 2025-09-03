#pragma once

#include <vector>
#include <fstream>
#include <type_traits>
#include <stdexcept>
#include <cstring>

namespace lumos
{

    template <typename T>
    void saveBinaryVector(const std::vector<T> &data, const std::string &filename)
    {
        static_assert(std::is_trivially_copyable_v<T>,
                      "Type T must be trivially copyable for binary serialization");

        std::ofstream file(filename, std::ios::binary);
        if (!file.is_open())
        {
            throw std::runtime_error("Failed to open file for writing: " + filename);
        }

        // Write type information
        const char *type_name = typeid(T).name();
        size_t type_name_length = std::strlen(type_name);
        file.write(reinterpret_cast<const char *>(&type_name_length), sizeof(type_name_length));
        file.write(type_name, type_name_length);

        // Write element size
        size_t element_size = sizeof(T);
        file.write(reinterpret_cast<const char *>(&element_size), sizeof(element_size));

        // Write vector size
        size_t vector_size = data.size();
        file.write(reinterpret_cast<const char *>(&vector_size), sizeof(vector_size));

        // Write vector data
        if (!data.empty())
        {
            file.write(reinterpret_cast<const char *>(data.data()), vector_size * sizeof(T));
        }

        if (!file.good())
        {
            throw std::runtime_error("Error writing to file: " + filename);
        }
    }

    template <typename T>
    std::vector<T> loadBinaryVector(const std::string &filename)
    {
        static_assert(std::is_trivially_copyable_v<T>,
                      "Type T must be trivially copyable for binary deserialization");

        std::ifstream file(filename, std::ios::binary);
        if (!file.is_open())
        {
            throw std::runtime_error("Failed to open file for reading: " + filename);
        }

        // Read and verify type information
        size_t type_name_length;
        file.read(reinterpret_cast<char *>(&type_name_length), sizeof(type_name_length));

        std::vector<char> stored_type_name(type_name_length + 1, '\0');
        file.read(stored_type_name.data(), type_name_length);

        if (std::strcmp(stored_type_name.data(), typeid(T).name()) != 0)
        {
            throw std::runtime_error("Type mismatch: file contains " +
                                     std::string(stored_type_name.data()) +
                                     ", requested " + std::string(typeid(T).name()));
        }

        // Read and verify element size
        size_t stored_element_size;
        file.read(reinterpret_cast<char *>(&stored_element_size), sizeof(stored_element_size));

        if (stored_element_size != sizeof(T))
        {
            throw std::runtime_error("Element size mismatch");
        }

        // Read vector size
        size_t vector_size;
        file.read(reinterpret_cast<char *>(&vector_size), sizeof(vector_size));

        // Read vector data
        std::vector<T> result(vector_size);
        if (vector_size > 0)
        {
            file.read(reinterpret_cast<char *>(result.data()), vector_size * sizeof(T));
        }

        if (!file.good())
        {
            throw std::runtime_error("Error reading from file: " + filename);
        }

        return result;
    }

}
