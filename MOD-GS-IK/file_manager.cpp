#include "file_manager.h"

#include <regex>
#include <string>
#include <cstdlib>

#include <QFileInfo>

#include "global_consts.h"

FileManager::FileManager()
{

}

bool FileManager::Empty() const
{
    return pos_to_file_.empty();
}

void FileManager::Clear()
{
    pos_to_file_.clear();
}

bool FileManager::AddFiles(const QStringList& list)
{
    if (list.size() < 3) {
        Print("Minimum 3 files required");
        return false;
    }

    FileDataBase tmp_base;
    for (const auto& file_path : list) {
        QFile tmp_file (file_path);
        QFileInfo file_info(tmp_file.fileName());
        std::string filename(file_info.fileName().toLocal8Bit());
        std::smatch match;
        if (std::regex_match(filename, match, kFileFormat)) {
            tmp_base.emplace(std::atof(match[1].str().data()), file_path);
        } else {
            Print("Invalid file name: " + file_path + ". No files were added");
            return false;
        }
    }

    Clear();
    pos_to_file_ = std::move(tmp_base);
    PrinFileList();

    return true;
}

//void FileManager::AddParsedFile(double pos, QFile file) {
//    pos_to_file_.emplace(pos, file);
//}


std::optional<QFile*> FileManager::GetFileInPosition(double position)
{
    auto file = pos_to_file_.find(position);
    if (file != pos_to_file_.end()) {
        return &file->second;
    }
    return std::nullopt;
}

FileDataBase::iterator FileManager::begin()
{
    return pos_to_file_.begin();
}

FileDataBase::iterator FileManager::end()
{
    return pos_to_file_.end();
}

const FileDataBase::const_iterator FileManager::cbegin() const
{
    return pos_to_file_.cbegin();
}

const FileDataBase::const_iterator FileManager::cend() const
{
    return pos_to_file_.cend();
}

const FileDataBase::const_iterator FileManager::begin() const
{
    return pos_to_file_.cbegin();
}

const FileDataBase::const_iterator FileManager::end() const
{
    return pos_to_file_.cend();
}

void FileManager::SetOutput(QTextBrowser *browser)
{
    browser_ = browser;
}

const QTextBrowser* FileManager::GetOutput() const
{
    return browser_;
}

void FileManager::Print(const QString& text) const {
    if (browser_) {
        browser_->append(text);
    }
}

void FileManager::PrinFileList() const
{
    Print(QString::number(pos_to_file_.size()) + " files in list:");
    for (const auto& [pos, name] : pos_to_file_) {
        Print("Position " + QString::number(pos) + ": " + name.fileName());
    }
}

