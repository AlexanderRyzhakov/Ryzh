#ifndef FILE_READ_WRITE_H
#define FILE_READ_WRITE_H

#include <fstream>
#include <string>

void SaveLib();

void OpenLib(const std::string& file_name);

void Open_ALI_APP_v_0_1(std::ifstream& file);

void Save_ALI_APP_v_0_1();

#endif // FILE_READ_WRITE_H
