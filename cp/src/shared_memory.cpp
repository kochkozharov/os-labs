#include "shared_memory.h"

#include <errno.h>
#include <sys/mman.h>
#include <unistd.h>

#include <cstdio>
#include <cstdlib>

WeakSharedMemory::WeakSharedMemory(std::string_view name, std::size_t size)
    : _name(name), _size(size) {

    _wSemPtr = sem_open((_name + "W").c_str(), O_CREAT, S_IRUSR | S_IWUSR, 1);
    _rSemPtr = sem_open((_name + "R").c_str(), O_CREAT, S_IRUSR | S_IWUSR, 0);
    _FD = shm_open(_name.c_str(), O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    if (_FD < 0) {
        switch (errno) {
            case EACCES:
                throw SharedMemoryException("Permission Exception");
                break;
            case EINVAL:
                throw SharedMemoryException(
                    "Invalid shared memory name passed");
                break;
            case EMFILE:
                throw SharedMemoryException(
                    "The process already has the maximum number of files "
                    "open");
                break;
            case ENAMETOOLONG:
                throw SharedMemoryException(
                    "The length of name exceeds PATH_MAX");
                break;
            case ENFILE:
                throw SharedMemoryException(
                    "The limit on the total number of files open on the system "
                    "has been reached");
                break;
            default:
                throw SharedMemoryException(
                    "Invalid exception occurred in shared memory creation");
                break;
        }
    }
    ftruncate(_FD, _size);
    _ptr = mmap(nullptr, _size, O_RDWR, MAP_SHARED, _FD, 0);
    if (_ptr == nullptr) {
        throw SharedMemoryException(
            "Exception in attaching the shared memory region");
    }
}

void WeakSharedMemory::writeLock() { sem_wait(_wSemPtr); }

void WeakSharedMemory::writeUnlock() { sem_post(_wSemPtr); }

void WeakSharedMemory::readLock() { sem_wait(_rSemPtr); }

void WeakSharedMemory::readUnlock() { sem_post(_rSemPtr); }

WeakSharedMemory::~WeakSharedMemory() {
    if (sem_close(_rSemPtr) < 0) {
        std::perror("sem_close");
        std::abort();
    }
    if (sem_close(_wSemPtr) < 0) {
        std::perror("sem_close");
        std::abort();
    }
    if (munmap(_ptr, _size) < 0) {
        std::perror("munmap");
        std::abort();
    }
}

SharedMemory::~SharedMemory() {
    if (shm_unlink(getName().c_str()) < 0) {
        std::perror("shm_unlink");
        std::abort();
    }
    if (sem_unlink(( getName() + "W").c_str()) < 0) {
        std::perror("sem_unlink");
        std::abort();
    }
    if (sem_unlink((getName() + "R").c_str()) < 0) {
        std::perror("sem_unlink");
        std::abort();
    }
}