#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

#include <QFile>
#include <QString>
#include <QStringList>
#include <QTextBrowser>

#include <map>
#include <optional>

using FileDataBase = std::map<double, QFile>;

class FileManager
{
public:
    FileManager();

    bool Empty() const;

    void Clear();

    bool AddFiles(const QStringList& list);

//    void AddParsedFile(double pos, QFile file);

    std::optional<QFile*> GetFileInPosition(double position);

    FileDataBase::iterator begin();

    FileDataBase::iterator end();

    const FileDataBase::const_iterator cbegin() const;

    const FileDataBase::const_iterator cend() const;

    const FileDataBase::const_iterator begin() const;

    const FileDataBase::const_iterator end() const;

    void SetOutput(QTextBrowser *browser);

    const QTextBrowser* GetOutput() const;

    void PrinFileList() const;

private:
    void Print(const QString& text) const;

private:
FileDataBase pos_to_file_;
QTextBrowser *browser_;
};

#endif // FILE_MANAGER_H
