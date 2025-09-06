#include <algorithm>
#include <iostream> // For error messages (optional)

#include "FileSystem.hpp"
#include "FileAlreadyExistsException.hpp"
#include "FileMissingException.hpp"
#include "NoAvailableInodeException.hpp"
#include "UnformattedFilesystemException.hpp"
#include "NoFreeBlockAvailableException.hpp"

namespace cse4733
{

    FileSystem::FileSystem(size_t diskSize, size_t blockSize)
        : diskSize(diskSize), blockSize(blockSize),
          blockManager(diskSize / blockSize), inodeTable(diskSize / (blockSize * 10))
    {
        // Initialize the filesystem with a root directory and empty inode table.
    }

    FileSystem::~FileSystem()
    {
        // Destructor - no special cleanup needed for this in-memory filesystem
    }

    bool FileSystem::createFile(const std::string &filename)
    {
        if (!isFormatted) {
            throw UnformattedFilesystemException();
        }

        try 
        {
            int inodeIndex = allocateInode();
            rootDirectory.addFile(filename, inodeIndex);
            return true;
        }
        catch(const cse4733::NoAvailableInodeException &e) 
        {
            return false;
        }
    }

    bool FileSystem::deleteFile(const std::string &filename)
    {
        if (!isFormatted) {
            throw UnformattedFilesystemException();
        }

        try 
        {
            int inodeIndex = findInode(filename);
            Inode &inode = inodeTable[inodeIndex];
            for (auto i: inode.directBlocks) {
                blockManager.freeBlock(i);
            }
            releaseInode(inodeIndex);
            rootDirectory.removeFile(filename);
            return true;
        }
        catch(const cse4733::FileMissingException &e)
        {
            return false;
        }
    }

    bool FileSystem::writeFile(const std::string &filename, const std::string &data)
    {

        if (!isFormatted) {
            throw UnformattedFilesystemException();
        }

        int inodeIndex = findInode(filename);
        Inode &inode = inodeTable[inodeIndex];
        for (auto i: inode.directBlocks) {
            blockManager.freeBlock(i);
        }
        std::vector<int> blockIndexes = writeDataToBlocks(data);
        inode.allocate(data.size(), blockIndexes);
        
        return true;
    }

    std::string FileSystem::readFile(const std::string &filename)
    {
        if (!isFormatted) {
            throw UnformattedFilesystemException();
        }

        try
        {
            int inodeIndex = findInode(filename);
            Inode &inode = inodeTable[inodeIndex];
            std::string data = readDataFromBlocks(inode.directBlocks);
            return data;
        }
        catch(const cse4733::FileMissingException &e)
        {
            return {};
        }
    }

    std::vector<std::string> FileSystem::listFiles()
    {
        if (!isFormatted)
        {
            throw UnformattedFilesystemException(); // Return empty if the filesystem has not been formatted
        }

        return rootDirectory.listFiles();
    }

    bool FileSystem::format()
    {   
        for (auto inode: inodeTable) {
            inode.deallocate();
        }

        rootDirectory = Directory();
        blockManager = BlockManager(diskSize / blockSize );
        isFormatted = true;
        return true;
        
    
    }

    int FileSystem::findInode(const std::string &filename)
    {
        if (!isFormatted)
        {
            throw UnformattedFilesystemException(); // Filesystem has not been formatted
        }
        return rootDirectory.getInodeIndex(filename);
    }

    unsigned int FileSystem::allocateInode()
    {   
        if (!isFormatted) {
            throw UnformattedFilesystemException();
        }

        auto it = std::find_if(inodeTable.begin(), inodeTable.end(), [](const Inode &inode) { return !inode.isAllocated; });
        if (it != inodeTable.end()) {
            it->allocate(0, {});
            return std::distance(inodeTable.begin(), it);
        } else {
            throw NoAvailableInodeException();
        }
    }

    void FileSystem::releaseInode(int inodeIndex)
    {
        if (inodeIndex >= 0 && static_cast<size_t>(inodeIndex) < inodeTable.size())
        {
            inodeTable[inodeIndex].deallocate();
        }
    }

    std::vector<int> FileSystem::writeDataToBlocks(const std::string &data)
    {
        std::vector<int> blockIndexes;
        size_t blocksNeeded = (data.size() + blockSize - 1) / blockSize;
        for (size_t i = 0; i < blocksNeeded; i++) {
            try
            {
                int blockIndex = blockManager.allocateBlock();
                size_t offset = i * blockSize;
                size_t length = std::min(blockSize, data.size() - offset);
                blockManager.writeBlock(blockIndex, data.substr(offset, length));
                blockIndexes.push_back(blockIndex);
            }
            catch(const cse4733::NoFreeBlockAvailableException& e)
            {
                for (int index: blockIndexes) {
                    blockManager.freeBlock(index);
                }
                return {};
            }
        }
        return blockIndexes;
    }

    std::string FileSystem::readDataFromBlocks(const std::vector<int> &blockIndexes)
    {
        std::string data;
        for (int i: blockIndexes) {
            std::string blockData = blockManager.readBlock(i);
            data += blockData;
        }
        return data;
    }

    size_t FileSystem::getFreeBlockCount() const
    {
        return blockManager.getFreeBlockCount(); // Retrieve the count of free blocks from BlockManager
    }

    size_t FileSystem::getTotalBlockCount() const
    {
        return blockManager.getTotalBlocks(); // Retrieve the total count of blocks from BlockManager
    }

} // namespace cse4733
