#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <random>
#include <filesystem>
#include <algorithm>

namespace fs = std::filesystem;

#ifndef VAULTT_USER
#define VAULTT_USER "default"
#endif

struct PasswordEntry
{
    std::string site;
    std::string username;
    std::string password;
};

class VaulttEngine
{
private:
    std::string storage_file_path = "/home/" + std::string(VAULTT_USER) + "/vaultt/passwords.txt";

public:
    VaulttEngine()
    {
        ensure_storage_exists();
    }

    void ensure_storage_exists()
    {
        fs::path p(storage_file_path);
        if (!p.parent_path().empty() && !fs::exists(p.parent_path()))
        {
            fs::create_directories(p.parent_path());
        }
    }

    std::string generate_secure_password(int length)
    {
        const std::string chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*()-_=+[]{}|;:,.<>?";
        std::random_device rd;
        std::mt19937 generator(rd());
        std::uniform_int_distribution<> distribution(0, static_cast<int>(chars.size() - 1));

        std::string password = "";
        for (int i = 0; i < length; ++i)
        {
            password += chars[distribution(generator)];
        }
        return password;
    }

    std::vector<PasswordEntry> load_entries()
    {
        std::vector<PasswordEntry> entries;
        if (!fs::exists(storage_file_path))
        {
            return entries;
        }

        std::ifstream file(storage_file_path);
        std::string line;
        while (std::getline(file, line))
        {
            size_t first_delim = line.find('|');
            size_t second_delim = line.find('|', first_delim + 1);

            if (first_delim != std::string::npos && second_delim != std::string::npos)
            {
                PasswordEntry entry;
                entry.site = line.substr(0, first_delim);
                entry.username = line.substr(first_delim + 1, second_delim - first_delim - 1);
                entry.password = line.substr(second_delim + 1);
                entries.push_back(entry);
            }
        }
        file.close();
        return entries;
    }

    bool save_entries(const std::vector<PasswordEntry>& entries)
    {
        std::ofstream file(storage_file_path, std::ios::trunc);
        if (!file.is_open())
        {
            return false;
        }

        for (const auto& entry : entries)
        {
            file << entry.site << "|" << entry.username << "|" << entry.password << "\n";
        }
        file.close();
        return true;
    }

    bool add_entry(const PasswordEntry& new_entry)
    {
        auto entries = load_entries();
        entries.push_back(new_entry);
        return save_entries(entries);
    }

    bool edit_entry_by_index(size_t index, const std::string& new_password)
    {
        auto entries = load_entries();
        if (index >= entries.size())
        {
            return false;
        }
        entries[index].password = new_password;
        return save_entries(entries);
    }

    bool remove_entry_by_index(size_t index)
    {
        auto entries = load_entries();
        if (index >= entries.size())
        {
            return false;
        }
        entries.erase(entries.begin() + index);
        return save_entries(entries);
    }
};

void print_help()
{
    std::cout << "Vaultt - Secure Password Manager CLI\n\n"
              << "Usage:\n"
              << "  vaultt                         Interactive mode to add a new password entry\n"
              << "  vaultt --help                  Display this help menu\n"
              << "  vaultt --version               Display application version and logo\n"
              << "  vaultt --list-passes           List all saved credentials with indices\n"
              << "  vaultt --edit pass=ID          Edit a specific password by index\n"
              << "  vaultt --remove pass=ID        Delete a specific password by index\n";
}

void print_version()
{
    std::cout << "____   ____           .__   __   __   \n"
              << "\\   \\ /   /____  __ __|  |_/  |__/  |_ \n"
              << " \\   Y   /\\__  \\|  |  \\  |\\   __\\   __\\\n"
              << "  \\     /  / __ \\|  |  /  |_|  |  |  |  \n"
              << "   \\___/  (____  /____/|____/__|  |__|  \n"
              << "               \\/                       \n"
              << "Vaultt version 2.0.0\n";
}

int main(int argc, char* argv[])
{
    VaulttEngine engine;

    if (argc == 1)
    {
        std::cout << "--- Vaultt: Create New Entry ---\n";
        PasswordEntry entry;

        std::cout << "Enter website/app name: ";
        std::getline(std::cin, entry.site);

        std::cout << "Enter username: ";
        std::getline(std::cin, entry.username);

        std::cout << "Do you want to auto-generate a secure password? (y/n): ";
        std::string choice;
        std::getline(std::cin, choice);

        if (choice == "y" || choice == "Y" || choice == "yes" || choice == "Yes")
        {
            std::cout << "Enter desired password length: ";
            std::string len_str;
            std::getline(std::cin, len_str);
            int length = 16;
            try
            {
                length = std::stoi(len_str);
            }
            catch (...)
            {
                length = 16;
            }

            entry.password = engine.generate_secure_password(length);
            std::cout << "Generated Secure Password: " << entry.password << "\n";
        }
        else
        {
            std::cout << "Enter your custom password: ";
            std::getline(std::cin, entry.password);
        }

        if (engine.add_entry(entry))
        {
            std::cout << "Successfully saved to storage.\n";
        }
        else
        {
            std::cerr << "Error: Failed to save credentials. Are you running as root or using sudo?\n";
        }
        return 0;
    }

    std::string flag = argv[1];

    if (flag == "--help")
    {
        print_help();
        return 0;
    }

    if (flag == "--version")
    {
        print_version();
        return 0;
    }

    if (flag == "--list-passes")
    {
        auto entries = engine.load_entries();
        if (entries.empty())
        {
            std::cout << "No credentials found in vault database.\n";
            return 0;
        }

        std::cout << "--- Saved Accounts ---\n";
        for (size_t i = 0; i < entries.size(); ++i)
        {
            std::cout << "[" << i << "] Site: " << entries[i].site 
                      << " | User: " << entries[i].username 
                      << " | Pass: " << entries[i].password << "\n";
        }
        return 0;
    }

    if (flag == "--edit" && argc == 3)
    {
        std::string arg = argv[2];
        if (arg.rfind("pass=", 0) != 0)
        {
            std::cerr << "Error: Invalid argument. Use: vaultt --edit pass=0\n";
            return 1;
        }

        size_t index = std::stoull(arg.substr(5));
        std::cout << "Enter new password for entry [" << index << "]: ";
        std::string new_password;
        std::getline(std::cin, new_password);

        if (engine.edit_entry_by_index(index, new_password))
        {
            std::cout << "Record updated successfully.\n";
        }
        else
        {
            std::cerr << "Error: Invalid index or file write permission error.\n";
        }
        return 0;
    }

    if (flag == "--remove" && argc == 3)
    {
        std::string arg = argv[2];
        if (arg.rfind("pass=", 0) != 0)
        {
            std::cerr << "Error: Invalid argument. Use: vaultt --remove pass=0\n";
            return 1;
        }

        size_t index = std::stoull(arg.substr(5));
        if (engine.remove_entry_by_index(index))
        {
            std::cout << "Record removed successfully.\n";
        }
        else
        {
            std::cerr << "Error: Invalid index or file write permission error.\n";
        }
        return 0;
    }

    std::cerr << "Unknown parameter or missing target arguments.\n";
    print_help();
    return 1;
}

