#include "chunk.h"

#include "../system/paths.h"

Chunk::Chunk()
{
  InitChunkKeys();
}

Chunk::~Chunk()
{
  ClearChunk(&curChunkDecompressed);
  ClearChunk(&nextChunkDecompressed);
  ClearChunkCache(&chunkCache);
}

u8* Chunk::OodleDecompress(u8* buffer, i32 size, i32 uncompressedSize, i32* decompressCount) {
  u8* buffer_decompress = nullptr;

  if (Oodle::OodleLZ_Decompress) {
    buffer_decompress = (u8*)malloc(sizeof(u8) * uncompressedSize);
    *decompressCount = Oodle::OodleLZ_Decompress(
      buffer, size, buffer_decompress, uncompressedSize,
      OodleLZ_FuzzSafe::OodleLZ_FuzzSafe_No,
      OodleLZ_CheckCRC::OodleLZ_CheckCRC_Default,
      OodleLZ_Verbosity::OodleLZ_Verbosity_Default,
      nullptr, 0,
      nullptr, nullptr,
      nullptr, 0,
      OodleLZ_Decode_ThreadPhase::OodleLZ_Decode_Default
    );
  }

  return buffer_decompress;
}

QList<FileNode*> Chunk::AnalyseChunk(const QString& inputFile, QList<FileNode*> inputFileNodeList)
{
  inputFilePath = inputFile;
  QFileInfo inputFileInfo(inputFilePath);
  qInfo() << "Now analysing: " << inputFile;

  QList<FileNode*> fileNodeList = inputFileNodeList;
  QString NamePKG = inputFileInfo.baseName() + ".pkg";
  inputFileHandle.setFileName(inputFilePath);
  bool opened = inputFileHandle.open(QIODevice::ReadOnly);
  reader.setDevice(&inputFileHandle);
  reader.setByteOrder(QDataStream::LittleEndian);

  u32 magic = 0;
  reader >> magic;
  // Read header
  if (magic != MagicChunk)
  {
    inputFileHandle.close();
    return inputFileNodeList;
  }
  reader.device()->seek(4);
  int ChunkCount = 0;
  reader >> ChunkCount;
  qInfo() << ChunkCount << "subchunks detected.";

  // Read file list
  dictCount = 0;
  i64 totalChunkSize = 0;
  for (int i = 0; i < ChunkCount; i++)
  {
    // Process file size
    u64 chunkPack = 0; reader >> chunkPack;
    u64 chunkSize = (((chunkPack & 0xFF) >> 4) & 0xFF) | (0xFFFF00 & chunkPack);
    chunkSize = (chunkSize >> 4) + (chunkSize & 0xF);
    totalChunkSize += chunkSize;

    u64 chunkOffset = (chunkPack & 0xFFFFFFFFFF000000) >> 24;

    metaChunk.insert(chunkOffset, chunkSize);
    chunkOffsetDict.insert(i, chunkOffset);
    dictCount = i + 1;
  }

  curIndex = 0;
  i64 curOffset = chunkOffsetDict[curIndex];
  i64 curSize = metaChunk[curOffset];
  curChunkDecompressed = GetDecompressedChunk(curOffset, curSize, &reader, curIndex);
  nextChunkDecompressed = nullptr;
  if (curIndex + 1 < dictCount)
  {
    nextChunkDecompressed = GetDecompressedChunk(chunkOffsetDict[curIndex + 1], metaChunk[chunkOffsetDict[curIndex + 1]], &reader, curIndex + 1);
  }
  curPointer = 0x0C;
  int totalParentCount = *(int*)(&curChunkDecompressed[curPointer]);
  curPointer += 4;
  int totalChildrenCount = *(int*)(&curChunkDecompressed[curPointer]);
  curPointer = 0x100;
  FileNode* rootNode = nullptr;
  for (int i = 0; i < totalParentCount; i++)
  {
    QString stringNameParent = GetASCIIString(0x3C).replace('\\', '/');
    i64 fileSize = GetInt64();
    i64 fileOffset = GetInt64();
    i32 entryType = GetInt32();
    i32 countChildren = GetInt32();

    if (fileNodeList.size() == 0)
    {
      rootNode = new FileNode(stringNameParent, false, inputFile);
      rootNode->EntireName = rootNode->Name;
      fileNodeList.append(rootNode);
    }
    else
    {
      rootNode = fileNodeList[0];
      rootNode->FromChunk = inputFilePath;
    }
    for (int j = 0; j < countChildren; j++)
    {
      int originPointer = curPointer;
      int originIndex = curIndex;
      if (!chunkCache.contains(curIndex)) chunkCache.insert(curIndex, curChunkDecompressed);
      if (!chunkCache.contains(curIndex + 1)) chunkCache.insert(curIndex + 1, nextChunkDecompressed);

      QString stringNameChild = GetASCIIString(0xA0).replace('\\', '/');
      fileSize = GetInt64();
      fileOffset = GetInt64();
      entryType = GetInt32();
      int unknown = GetInt32();

      if (entryType == 0x02)
      {
        curPointer = originPointer;
        if (curIndex != originIndex)
        {
          curIndex = originIndex;
          curChunkDecompressed = chunkCache[curIndex];
          nextChunkDecompressed = chunkCache[curIndex + 1];
          chunkCache.remove(curIndex);
          chunkCache.remove(curIndex + 1);
        }
        stringNameChild = GetASCIIString(0x50).replace('\\', '/');
        GetByLength(0x68, nullptr, 0);
      }
      QStringList fatherNodes = stringNameChild.split('/');
      bool isFile = false;
      if (entryType == 0x02 || entryType == 0x00) isFile = true;
      FileNode* childNode = new FileNode(fatherNodes[fatherNodes.size() - 1], isFile, inputFile);
      if (isFile)
      {
        childNode->Size = fileSize;
        childNode->Offset = fileOffset;
        childNode->ChunkIndex = (int)(fileOffset / 0x40000);
        childNode->ChunkPointer = (int)(fileOffset % 0x40000);
      }

      childNode->EntireName = stringNameChild;
      FileNode* targetNode = rootNode;
      foreach(const QString & nodeName, fatherNodes)
      {
        if (nodeName.isEmpty()) continue;

        foreach(FileNode * node, targetNode->Children)
        {
          if (node->Name == nodeName)
          {
            if (node->Name == childNode->Name) { break; }
            targetNode = node;
            break;
          }
        }
      }
      bool needAdd = true;
      foreach(FileNode * tmpFileNode, targetNode->Children)
      {
        if (tmpFileNode->Name == childNode->Name)
        {
          if (childNode->IsFile) {
            targetNode->Children.removeOne(tmpFileNode);
            delete tmpFileNode;
          }
          else
          {
            tmpFileNode->FromChunk = childNode->FromChunk;
            tmpFileNode->FromChunkName = childNode->FromChunkName;
            delete childNode;
            needAdd = false;
          }
          break;
        }
      }
      if (needAdd) targetNode->Children.append(childNode);
    }
    // qInfo() << i + 1 << totalParentCount;
  }

  ClearChunkCache(&chunkCache);
  for (int i = 0; i < fileNodeList.size(); i++)
  {
    fileNodeList[i]->getSize();
  }

  return fileNodeList;
}

int Chunk::ExtractSelected(QList<FileNode*> itemlist, const QString& BaseLocation)
{
  int failed = 0;

  QDir nodeDir;
  nodeDir.mkpath(BaseLocation);

  foreach(FileNode * node, itemlist)
  {
    node->IsSelected = true;
    if (node->Children.size() > 0)
    {
      failed += ExtractSelected(node->Children, BaseLocation);
    }
    else if (node->IsSelected)
    {
      QString nodePath = BaseLocation + node->EntireName;

      if (node->IsFile) {
        qInfo().noquote() << "Writing node:" << node->EntireName;

        QFileInfo fileInfo(nodePath);
        nodeDir.mkpath(fileInfo.absolutePath());
        u32 nodeSize = node->Size;
        u32 chunkSize = 0;
        u8* nodeData = GetFileData(node);
        // FileUtils::WriteFile(nodePath, nodeData, nodeSize);
        free(nodeData);
      }
      else {
        nodeDir.mkpath(nodePath);
      }
    }
  }
  return failed;
}

void Chunk::InitChunkKeys()
{
  if (chunkKeyPattern.size() != 0) {
    qWarning("Keys already initialised.");
    return;
  }

  QFile keyFile(Paths::GetResourcesPath("chunk.key"));
  keyFile.open(QIODevice::ReadOnly);
  QDataStream keyReader(&keyFile);
  keyReader.setByteOrder(QDataStream::LittleEndian);
  i32 keyIndexCount = 0;
  keyReader >> keyIndexCount;
  chunkKeyPattern.resize(keyIndexCount);
  for (int keyIterator = 0; keyIterator < keyIndexCount; keyIterator++)
  {
    u8 curKey = 0;
    keyReader >> curKey;
    chunkKeyPattern[keyIterator] = curKey;
  }
  keyFile.close();
}

u8* Chunk::GetDecompressedChunk(i64 offset, i64 size, QDataStream* reader, i32 chunkNum)
{
  if (size)
  {
    QIODevice* device = reader->device();
    device->seek(offset);
    u8* buffer = (u8*)malloc(size * sizeof(u8));
    u32 readBytes = reader->readRawData((char*)buffer, (int)size);
    u32 decompressSize = 0;
    u8* decompressed = OodleDecompress(buffer, readBytes, 0x40000, (i32*)&decompressSize);
    free(buffer);
    u8* chunk_key = GetChunkKey(chunkNum);
    return DecryptChunk(decompressed, decompressSize, chunk_key);
  }
  else
  {
    reader->device()->seek(offset);
    u8* buffer = (u8*)malloc(0x40000 * sizeof(u8));
    u32 readBytes = reader->readRawData((char*)buffer, 0x40000);
    u32 key_size = 0;
    u8* chunk_key = GetChunkKey(chunkNum);
    return DecryptChunk(buffer, readBytes, chunk_key);
  }
}

u8* Chunk::GetByLength(i64 targetlength, u8* tmp, i64 startAddr)
{
  QList<u8*> cached = chunkCache.values();
  u8* currSub = curChunkDecompressed;
  u8* nextSub = nextChunkDecompressed;

  while (curPointer + targetlength >= 0x40000) {
    i32 tmpCanReadLength = 0x40000 - curPointer;
    i64 tmpRemainNeededLength = targetlength - tmpCanReadLength;
    memcpy(&tmp[startAddr], &currSub[curPointer], tmpCanReadLength * sizeof(u8));
    curPointer = 0;
    if (currSub != curChunkDecompressed && currSub != nextChunkDecompressed) {
      free(currSub);
      currSub = nullptr;
    }
    currSub = nextSub;
    curIndex += 1;
    if (curIndex + 1 < dictCount) {
      nextSub = GetDecompressedChunk(
        chunkOffsetDict[curIndex + 1],
        metaChunk[chunkOffsetDict[curIndex + 1]],
        &reader, curIndex + 1);
    }
    else
    {
      nextSub = nullptr;
    }

    targetlength = tmpRemainNeededLength;
    startAddr += tmpCanReadLength;
  }
  memcpy(&tmp[startAddr], &currSub[curPointer], targetlength * sizeof(u8));
  curPointer += (int)targetlength;

  if (!cached.contains(curChunkDecompressed) && curChunkDecompressed != currSub && curChunkDecompressed != nextSub) {
    free(curChunkDecompressed);
    curChunkDecompressed = nullptr;
  }

  curChunkDecompressed = currSub;
  nextChunkDecompressed = nextSub;

  return tmp;
}

QString Chunk::GetASCIIString(int targetlength)
{
  u8* buffer = (u8*)malloc(targetlength * sizeof(u8));

  GetByLength(targetlength, buffer, 0);
  QString str = QString::fromUtf8(buffer);
  free(buffer);

  return str;
}

i32 Chunk::GetInt32()
{
  i32 result = 0;
  GetByLength(4, (u8*)&result, 0);
  return result;
}

i64 Chunk::GetInt64()
{
  i64 result = 0;
  GetByLength(8, (u8*)&result, 0);
  return result;
}

void Chunk::ClearChunk(u8** chunk)
{
  QList<u8*> cached = chunkCache.values();

  if (*chunk && !cached.contains(*chunk)) {
    free(*chunk);
    *chunk = nullptr;
  }
}

void Chunk::ClearChunkCache(QMap<i32, u8*>* cache)
{
  QMapIterator<i32, u8*> chunkIterator(*cache);
  while (chunkIterator.hasNext()) {
    chunkIterator.next();

    i32 index = chunkIterator.key();
    u8* chunk = chunkIterator.value();
    if (chunk != curChunkDecompressed && chunk != nextChunkDecompressed)
      free(chunk);
  }
  cache->clear();
}

u8* Chunk::GetFileData(FileNode* node)
{
  Chunk* curNodeChunk = GetChunk(node->FromChunk);
  curNodeChunk->curIndex = node->ChunkIndex;
  curNodeChunk->curPointer = node->ChunkPointer;
  i64 filesize = node->Size;

  ClearChunk(&curNodeChunk->curChunkDecompressed);
  ClearChunk(&curNodeChunk->nextChunkDecompressed);
  if (curNodeChunk->chunkCache.contains(curNodeChunk->curIndex))
  {
    curNodeChunk->curChunkDecompressed = curNodeChunk->chunkCache[curNodeChunk->curIndex];
  }
  else
  {
    if (curNodeChunk->chunkCache.size() > 20) ClearChunkCache(&chunkCache);

    i64 curChunkOffset = curNodeChunk->chunkOffsetDict[curNodeChunk->curIndex];
    i64 curChunkMeta = curNodeChunk->metaChunk[curChunkOffset];
    curNodeChunk->curChunkDecompressed = curNodeChunk->GetDecompressedChunk(
      curChunkOffset, curChunkMeta, &curNodeChunk->reader,
      curNodeChunk->curIndex
    );

    curNodeChunk->chunkCache.insert(curNodeChunk->curIndex, curNodeChunk->curChunkDecompressed);
  }

  if (curNodeChunk->chunkCache.contains(curNodeChunk->curIndex + 1))
  {
    curNodeChunk->nextChunkDecompressed = curNodeChunk->chunkCache[curNodeChunk->curIndex + 1];
  }
  else
  {
    if (curNodeChunk->chunkCache.size() > 20) ClearChunkCache(&chunkCache);

    if (curNodeChunk->curIndex + 1 < curNodeChunk->dictCount) {
      i64 nextChunkOffset = curNodeChunk->chunkOffsetDict[curNodeChunk->curIndex + 1];
      i64 nextChunkMeta = curNodeChunk->metaChunk[nextChunkOffset];
      curNodeChunk->nextChunkDecompressed = curNodeChunk->GetDecompressedChunk(
        nextChunkOffset,
        nextChunkMeta,
        &curNodeChunk->reader,
        curNodeChunk->curIndex + 1);
    }
    else {
      curNodeChunk->nextChunkDecompressed = nullptr;
    }
    curNodeChunk->chunkCache.insert(curNodeChunk->curIndex + 1, curNodeChunk->nextChunkDecompressed);
  }
  u8* buffer = (u8*)malloc(filesize * sizeof(u8));
  if (buffer) buffer = curNodeChunk->GetByLength(filesize, buffer, 0);
  else qWarning() << "Could not allocate memory.";
  return buffer;
}

u8* Chunk::GetChunkKey(int i)
{
  int keyPos = chunkKeyPattern[i];
  if (keyPos > 0xF) keyPos = 0;
  u8* chunkKey = (u8*)chunkKeys[keyPos];
  return chunkKey;
}

u8* Chunk::DecryptChunk(u8* data, u32 size, u8* chunkKey)
{
  for (int i = 0; i < size; i++)
  {
    data[i] = chunkKey[i % 16] ^ data[i];
  }
  return data;
}

Chunk* Chunk::GetChunk(const QString& chunk)
{
  return this;
}
