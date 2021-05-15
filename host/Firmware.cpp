//
// Created by genius on 5/15/21.
//

#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>

#include <iostream>
#include <cerrno>
#include <cstring>
#include <vector>
#include <algorithm>

#include "Firmware.h"

Firmware::Firmware()
{
    int ret = pthread_create(&main_thread, nullptr, main_thread_entry, this);
    if (ret != 0) {
        printf("main thread create failed\n");
        return;
    }
    pthread_join(main_thread, nullptr);
}

Firmware::~Firmware()
{

}

void *Firmware::main_thread_entry(void *arg)
{
    auto *This = static_cast<Firmware *>(arg);
    std::string firmware_path = "../firmware";
    while (true) {
        firmware_info_t firmware_info;
        int ret = Firmware::get_firmware_infos(firmware_path, firmware_info);
        if (ret == 0) {
            auto src_app_path = [firmware_path] (firmware_info_t firmware_info) {return firmware_path  + '/' + firmware_info.application_name;};
            auto dst_app_path = [firmware_path] (firmware_info_t firmware_info) {return firmware_path + "/current/" + firmware_info.application_name;};
            auto src_btl_path = [firmware_path] (firmware_info_t firmware_info) {return firmware_path + '/' + firmware_info.bootloader_name;};
            auto dst_btl_path = [firmware_path] (firmware_info_t firmware_info) {return firmware_path + "/current/" + firmware_info.bootloader_name;};
            auto src_ptb_path = [firmware_path] (firmware_info_t firmware_info) {return firmware_path + '/' + firmware_info.partition_name;};
            auto dst_ptb_path = [firmware_path] (firmware_info_t firmware_info) {return firmware_path + "/current/" + firmware_info.partition_name;};
            ret = Firmware::copy_firmware_file(dst_app_path(firmware_info), src_app_path(firmware_info));
            if (ret != 0) {
                std::cout << "failed";
            }
            ret = Firmware::copy_firmware_file(dst_btl_path(firmware_info), src_btl_path(firmware_info));
            if (ret != 0) {
                std::cout << "failed";
            }
            ret = Firmware::copy_firmware_file(dst_ptb_path(firmware_info), src_ptb_path(firmware_info));
            if (ret != 0) {
                std::cout << "failed";
            }
            std::cout << "copy successfully" << std::endl;
        }
        sleep(1);
    }
    return nullptr;
}

/**
 * Get firmware
 * @param path
 * @return
 *      0: successful
 *     -1: failed
 */
int Firmware::get_firmware_infos(std::string path, firmware_info_t &firmware_info)
{
    std::vector<std::string> firmware_vec;
    DIR *dir_stream = opendir("../firmware");
    if (dir_stream == nullptr) {
        printf("opendir error: %s", strerror(errno));
        exit(-1);
    }
    struct dirent *dir_item;
    while ((dir_item = readdir(dir_stream)) != nullptr) {
        std::string filename = dir_item->d_name;
        if (filename.find(".bin") != std::string::npos) {
            firmware_vec.push_back(dir_item->d_name);
        }
    }

    auto bl_it = std::find(firmware_vec.begin(), firmware_vec.end(), "bootloader.bin");
    auto pt_it = std::find(firmware_vec.begin(), firmware_vec.end(), "partition-table.bin");
    if (bl_it == firmware_vec.end() || pt_it == firmware_vec.end() || firmware_vec.size() != 3) {
        return -1;
    }

    firmware_info.bootloader_name = *bl_it;
    firmware_info.partition_name = *pt_it;
    for (const auto& firmware : firmware_vec) {
        if (firmware != *bl_it && firmware != *pt_it) {
            firmware_info.application_name = firmware;
        }
    }

    return 0;
}

int Firmware::copy_firmware_file(std::string dst_path, std::string src_path)
{
    int src_fd = open(src_path.c_str(), O_RDONLY);
    if (src_fd < 0) {
        perror("open failed");
        return src_fd;
    }

    int dst_fd = open(dst_path.c_str(), O_WRONLY | O_CREAT, 0644);
    if (dst_fd < 0) {
        perror("open failed");
        return dst_fd;
    }

    int len = 0;
    char read_buf[1024];
    while ((len = read(src_fd, read_buf, 1024)) > 0) {
        write(dst_fd, read_buf, len);
    }
    int ret = fsync(dst_fd);
    if (ret == -1) {
        perror("fsync failed");
        return ret;
    }
    close(src_fd);
    close(dst_fd);
    return 0;
}

int Firmware::update_firmware(const std::string &btl_path, const std::string &ptb_path, const std::string &app_path)
{
    
    return 0;
}
