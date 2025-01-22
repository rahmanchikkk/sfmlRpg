#pragma once
#include <unordered_map>
#include <iostream>
#include <sstream>
#include <fstream>
#include "Utilities.h"

template <typename Derived, typename T>
class ResourceManager {
public:
    ResourceManager(const std::string& l_pathsFile) {
        LoadPaths(l_pathsFile);
    };

    ~ResourceManager() { PurgeResources(); };

    T* GetResource(const std::string& l_id) {
        auto itr = Find(l_id);
        return (itr ? itr->first : nullptr);
    };

    std::string GetPath(const std::string& l_id) {
        auto itr = m_paths.find(l_id);
        return (itr != m_paths.end() ? itr->second : "");
    };

    bool RequireResource(const std::string& l_id) {
        auto itr = Find(l_id);
        if (itr) {
            ++itr->second;
            return true;
        }
        auto path = m_paths.find(l_id);
        if (path == m_paths.end()) return false;
        T* res = Load(path);
        if (!res) return false;
        m_resources.emplace(l_id, std::make_pair(res, 1));
        return true;
    };

    bool ReleaseResource(const std::string& l_id) {
        auto itr = m_resources.find(l_id);
        if (itr == m_resources.end()) return false;
        --itr->second.second;
        if (!itr->second.second) Unload(l_id);
        return true;
    };

    void PurgeResources() {
        while (m_resources.begin() != m_resources.end()) {
            std::cout << "Purging resource: " << m_resources.begin()->first << std::endl;
            delete m_resources.begin()->second.first;
            m_resources.erase(m_resources.begin());
        }
        std::cout << "Purging finished." << std::endl;
    };
protected:
    T* Load(const std::string& l_path) {
        return static_cast<Derived*>(this)->Load(l_path);
    };
private:
    std::pair<T*, unsigned int>* Find(const std::string& l_id) {
        auto itr = m_resources.find(l_id);
        return (itr == m_resources.end() ? nullptr : &itr->second);
    };

    bool Unload(const std::string& l_id) {
        auto itr = m_resources.find(l_id);
        if (itr == m_resources.end()) return false;
        delete itr->second.first;
        m_resources.erase(itr);
        return true;
    };

    void LoadPaths(const std::string& l_pathsFile) {
        std::ifstream file;
        file.open(Utils::GetWorkingDirectory() + l_pathsFile);
        if (!file.is_open()) {
            std::cout << "Failed opening paths file: " << l_pathsFile << std::endl;
            return;
        }
        std::string line;
        while (std::getline(file, line)) {
            std::stringstream keystream(line);
            std::string pathName;
            std::string pathFile;
            keystream >> pathName;
            keystream >> pathFile;
            m_paths.emplace(pathName, pathFile);
        }
        file.close();
    };
    
    std::unordered_map<std::string, std::pair<T*, unsigned int>> m_resources;
    std::unordered_map<std::string, std::string> m_paths;
};