#include <file_config.h>
#include <bp_strlen.h>
#include <user_config.h>
/**
  * 1=BEECOM_SN
  */


int ConfigRead(uint8_t config_id, char * buf, uint32 size)
{
    int fd, read_count;
    const char * config_file_name;

    if(!buf || size < 1) {
        return -1;
    }

    config_file_name = GetConfigFileName(config_id);
    if(!config_file_name) {
        return -2;
    }

    fd = open(config_file_name, O_RDONLY);
    if(fd <= 0) {
        return -3;
    }

    read_count = read(fd, buf, size);
    if(read_count < 0) {
        close(fd);
        return -4;
    }

    close(fd);

    /* there is always some end dilimiter delimiter CR/LF('\r'/'\n') when files created by the FS */
    if('\r' == buf[read_count-1] || '\n' == buf[read_count-1]) {
        buf[read_count-1] = '\0';
    }
    return 0;
}

int ConfigWrite(uint8_t config_id, const char * buf)
{
    int fd, write_count;
    const char * config_file_name;

    if(!buf) {
        return -1;
    }

    config_file_name = GetConfigFileName(config_id);
    if(!config_file_name) {
        return -2;
    }

    if(!buf) {
        return -1;
    }
    fd = open(config_file_name, O_TRUNC | O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    if(fd <= 0) {
        return -3;
    }

    write_count = write(fd, buf, strlen_bp(buf)+1);
    if(write_count < 0) {
        close(fd);
        return -4;
    }

    close(fd);


    return 0;
}

const char * GetConfigFileName(uint8_t config_id)
{
    switch(config_id) {
        case EN_CONFIG_ID_SN:
            return CONFIG_FILE_SN;
        case EN_CONFIG_ID_KEY:
            return CONFIG_FILE_KEY;
        case EN_CONFIG_ID_ADMIN:
            return CONFIG_FILE_ADMIN;
        case EN_CONFIG_ID_ALIVE_TIME:
            return CONFIG_FILE_ALIVE_TIME;
        case EN_CONFIG_ID_NET_MODE:
            return CONFIG_FILE_NET_MODE;
        default:
            return NULL;
    }
}
