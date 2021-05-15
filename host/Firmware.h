//
// Created by genius on 5/15/21.
//

#ifndef HOST_FIRMWARE_H
#define HOST_FIRMWARE_H

#include <pthread.h>
#include <string>

class Firmware {
public:
    Firmware();
    ~Firmware();

private:
    struct firmware_info_s {
        std::string bootloader_name;
        std::string partition_name;
        std::string application_name;
    };
    typedef struct firmware_info_s firmware_info_t;

    pthread_t main_thread;
    firmware_info_t current_firmware_info;

    static void *main_thread_entry(void *arg);
    static int get_firmware_infos(std::string path, firmware_info_t &firmware_info);
    static int copy_firmware_file(std::string dst_path, std::string src_path);
    static int update_firmware(const std::string &btl_path, const std::string &ptb_path, const std::string &app_path);
};


#endif //HOST_FIRMWARE_H
