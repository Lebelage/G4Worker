#pragma once

#include <string>
#include <filesystem>
#include <exception>

namespace fs = std::filesystem;

namespace G4Worker::Utils
{
    class FileProvider
    {
    public:
        static bool CreateDirectory(std::string &name)
        {
            try
            {
                fs::path directory = name;

                if (fs::create_directory(directory))
                    return true;

                throw std::runtime_error("Canot create directory:" + name);
            }
            catch (std::exception ex)
            {

                return false;
            }
        }
    };
}