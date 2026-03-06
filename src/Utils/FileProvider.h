#pragma once

#include <iostream>
#include <string>
#include <filesystem>
#include <fstream>
#include <exception>
#include "AppPaths.h"

namespace fs = std::filesystem;

namespace G4Worker::Utils
{
    class FileProvider
    {
    public:
        static bool CreateDirectory(const std::string &name)
        {
            try
            {
                fs::path directory = name;

                if (fs::create_directory(directory))
                    return true;

                throw std::runtime_error("Canot create directory: " + name);
            }
            catch (std::exception ex)
            {

                return false;
            }
        }

        static bool CreateFile(const std::string &filePath)
        {
            try
            {
                if (fs::exists(filePath))
                    throw std::runtime_error("File existing: " + filePath);

                std::ofstream file(filePath);
                return file.good();
            }
            catch (std::exception ex)
            {
                return false;
            }
        }

        static bool CreateExperimentFile(const std::string &fileName)
        {
            try
            {
                fs::path dirPath = Utils::Constants::AppPaths::__EXPERIMENTS_DIR_NAME;
                fs::path filePath = dirPath / fileName;

                if (!fs::exists(dirPath))
                {
                    fs::create_directories(dirPath);
                }

                if (fs::exists(filePath))
                {
                    return false;
                }

                std::ofstream file(filePath);

                return file.good();
            }
            catch (const std::exception &ex)
            {
                return false;
            }
        }
    };
}