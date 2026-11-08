// Transient RGA test: two tiny DMA32 buffers, hardware fill/copy, pixel readback.
#include <fcntl.h>
#include <linux/dma-buf.h>
#include <linux/dma-heap.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include "im2d.h"

constexpr int kWidth = 64;
constexpr int kHeight = 64;
constexpr size_t kBytes = kWidth * kHeight * sizeof(uint32_t);
struct Buffer {
    int fd = -1;
    void* memory = MAP_FAILED;
    rga_buffer_handle_t handle = 0;
    ~Buffer() {
        if (handle) releasebuffer_handle(handle);
        if (memory != MAP_FAILED) munmap(memory, kBytes);
        if (fd >= 0) close(fd);
    }
    bool sync(uint64_t flags) {
        dma_buf_sync request = {};
        request.flags = flags;
        return ioctl(fd, DMA_BUF_IOCTL_SYNC, &request) == 0;
    }
    bool allocate(unsigned char initial) {
        const int heap = open("/dev/dma_heap/system-uncached-dma32", O_RDWR | O_CLOEXEC);
        if (heap < 0) return false;
        dma_heap_allocation_data request = {};
        request.len = kBytes;
        request.fd_flags = O_RDWR | O_CLOEXEC;
        const int status = ioctl(heap, DMA_HEAP_IOCTL_ALLOC, &request);
        close(heap);
        if (status < 0) return false;
        fd = static_cast<int>(request.fd);
        memory = mmap(nullptr, kBytes, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
        if (memory == MAP_FAILED || !sync(DMA_BUF_SYNC_START | DMA_BUF_SYNC_RW)) return false;
        std::memset(memory, initial, kBytes);
        if (!sync(DMA_BUF_SYNC_END | DMA_BUF_SYNC_RW)) return false;
        handle = importbuffer_fd(fd, static_cast<int>(kBytes));
        return handle != 0;
    }
    rga_buffer_t image() {
        return wrapbuffer_handle_t(handle, kWidth, kHeight, kWidth, kHeight, RK_FORMAT_RGBA_8888);
    }
};
int main() {
    Buffer source, destination;
    if (!source.allocate(0x33) || !destination.allocate(0x77)) {
        std::puts("{\"passed\":false,\"stage\":\"allocate_or_import\"}");
        return 2;
    }
    const im_rect rectangle = {8, 8, 32, 32};
    const IM_STATUS fill = imfill(source.image(), rectangle, static_cast<int>(0xff00ff00u));
    if (fill != IM_STATUS_SUCCESS || !source.sync(DMA_BUF_SYNC_START | DMA_BUF_SYNC_RW)) {
        std::printf("{\"passed\":false,\"stage\":\"fill\",\"status\":%d}\n", fill);
        return 2;
    }
    const auto* pixels = static_cast<const uint32_t*>(source.memory);
    int verified = 0;
    for (int y = 0; y < kHeight; ++y) {
        for (int x = 0; x < kWidth; ++x) {
            const uint32_t expected = x >= 8 && x < 40 && y >= 8 && y < 40 ? 0xff00ff00u : 0x33333333u;
            if (pixels[y * kWidth + x] == expected) ++verified;
        }
    }
    if (!source.sync(DMA_BUF_SYNC_END | DMA_BUF_SYNC_RW)) return 2;
    const IM_STATUS copy = imcopy(source.image(), destination.image());
    bool equal = false;
    if (copy == IM_STATUS_SUCCESS && source.sync(DMA_BUF_SYNC_START | DMA_BUF_SYNC_RW) &&
        destination.sync(DMA_BUF_SYNC_START | DMA_BUF_SYNC_RW)) {
        equal = std::memcmp(source.memory, destination.memory, kBytes) == 0;
        source.sync(DMA_BUF_SYNC_END | DMA_BUF_SYNC_RW);
        destination.sync(DMA_BUF_SYNC_END | DMA_BUF_SYNC_RW);
    }
    const bool passed = verified == kWidth * kHeight && equal;
    std::printf("{\"passed\":%s,\"fill_status\":%d,\"copy_status\":%d,"
                "\"verified_pixels\":%d,\"expected_pixels\":4096,\"copy_matches\":%s,"
                "\"total_dma_bytes\":32768}\n",
                passed ? "true" : "false", fill, copy, verified, equal ? "true" : "false");
    return passed ? 0 : 2;
}
