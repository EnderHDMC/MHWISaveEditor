#pragma once

#include <QString>
#include <QList>
#include <QFileInfo>

#include "../../types/types.h"

class FileNode
{
public:
  QString Name;
  QString EntireName;
  QString FromChunk;
  QString FromChunkName;

  i32 ChunkIndex = 0;
  i32 ChunkPointer = 0;
  i64 Size = 0;
  i64 Offset = 0;

  bool IsFile;
  QList<FileNode*> Children;

  bool IsSelected = false;
  bool Visible = false;

  FileNode(const QString& name, bool isFile, const QString& fromChunk)
  {
    Name = name;
    IsFile = isFile;
    IsSelected = false;
    FromChunk = fromChunk;
    Visible = true;

    QFileInfo info(fromChunk);
    FromChunkName = info.baseName();
  }

  ~FileNode() {
    if (!IsFile)
    {
      DeleteList(Children);
    }
  }

  i64 getSize() {
    if (IsFile)
    {
      return Size;
    }
    else
    {
      long _size = 0;
      foreach(FileNode * child, Children)
      {
        _size += child->getSize();
      }
      Size = _size;
      return _size;
    }
  }

  static void DeleteList(QList<FileNode*> nodes) {
    foreach(FileNode * child, nodes)
    {
      delete child;
    }
  }
};
