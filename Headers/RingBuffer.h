/*
  ==============================================================================

    RingBuffer.h
    Created: 16 Apr 2025 4:02:00am
    Author:  Michael

  ==============================================================================
*/

#pragma once
/**
 * Single-producer, single-consumer lock-free ring buffer optimized for audio applications
 */
template <typename T>
class RingBuffer {
public:
    /**
     * Constructor
     * @param size Buffer size in samples (must be power of 2)
     */
    RingBuffer(size_t size) {
        // Ensure size is a power of 2
        size_t powerOfTwo = 1;
        while (powerOfTwo < size) {
            powerOfTwo <<= 1;
        }

        mSize = powerOfTwo;
        mSizeMask = mSize - 1;
        mBuffer = new T[mSize];
        memset(mBuffer, 0, mSize * sizeof(T));
        mWriteIndex = 0;
        mReadIndex = 0;
    }

    /**
     * Destructor
     */
    ~RingBuffer() {
        delete[] mBuffer;
    }

    /**
     * Reset the buffer to initial state
     */
    void reset() {
        mWriteIndex = 0;
        mReadIndex = 0;
        memset(mBuffer, 0, mSize * sizeof(T));
    }

    /**
     * Write data to the buffer
     * @param data Pointer to source data
     * @param numSamples Number of samples to write
     * @return Number of samples actually written
     */
    size_t write(const T* data, size_t numSamples) {
        size_t available = availableForWrite();

        // If there's not enough space, only write what we can
        size_t numToWrite = std::min(available, numSamples);

        if (numToWrite == 0)
            return 0;

        // First chunk (up to the end of the buffer)
        size_t firstChunk = std::min(numToWrite, mSize - (mWriteIndex & mSizeMask));
        memcpy(mBuffer + (mWriteIndex & mSizeMask), data, firstChunk * sizeof(T));

        // Second chunk (may wrap around to the beginning)
        if (firstChunk < numToWrite) {
            memcpy(mBuffer, data + firstChunk, (numToWrite - firstChunk) * sizeof(T));
        }

        // Update write index (atomic)
        mWriteIndex += numToWrite;

        return numToWrite;
    }

    /**
     * Read data from the buffer
     * @param data Pointer to destination buffer
     * @param numSamples Number of samples to read
     * @return Number of samples actually read
     */
    size_t read(T* data, size_t numSamples) {
        size_t available = availableForRead();

        // If there's not enough data, only read what we can
        size_t numToRead = std::min(available, numSamples);

        if (numToRead == 0)
            return 0;

        // First chunk (up to the end of the buffer)
        size_t firstChunk = std::min(numToRead, mSize - (mReadIndex & mSizeMask));
        memcpy(data, mBuffer + (mReadIndex & mSizeMask), firstChunk * sizeof(T));

        // Second chunk (may wrap around to the beginning)
        if (firstChunk < numToRead) {
            memcpy(data + firstChunk, mBuffer, (numToRead - firstChunk) * sizeof(T));
        }

        // Update read index (atomic)
        mReadIndex += numToRead;

        return numToRead;
    }

    /**
     * Get number of samples available for reading
     * @return Number of samples available
     */
    size_t availableForRead() const {
        return mWriteIndex - mReadIndex;
    }

    /**
     * Get number of samples that can be written
     * @return Space available for writing
     */
    size_t availableForWrite() const {
        return mSize - (mWriteIndex - mReadIndex);
    }

    /**
     * Check if the buffer is empty
     * @return True if empty
     */
    bool isEmpty() const {
        return mWriteIndex == mReadIndex;
    }

    /**
     * Check if the buffer is full
     * @return True if full
     */
    bool isFull() const {
        return availableForWrite() == 0;
    }

    /**
     * Get the size of the buffer
     * @return Buffer size in samples
     */
    size_t getSize() const {
        return mSize;
    }

private:
    T* mBuffer;                      // Actual buffer data
    size_t mSize;                    // Buffer size (power of 2)
    size_t mSizeMask;                // Bit mask for quick modulo operations
    std::atomic<size_t> mWriteIndex; // Current write position
    std::atomic<size_t> mReadIndex;  // Current read position

    // Prevent copying
    RingBuffer(const RingBuffer&) = delete;
    RingBuffer& operator=(const RingBuffer&) = delete;
};