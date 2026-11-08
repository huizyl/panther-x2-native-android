// Temporary TF-only diagnostics. No policy, encryption, or network changes.
#pragma once

#include <fcntl.h>
#include <sched.h>
#include <sys/klog.h>
#include <sys/stat.h>
#include <unistd.h>

#include <cerrno>
#include <chrono>
#include <mutex>
#include <string>
#include <thread>
#include <utility>
#include <vector>

#include <android-base/file.h>
#include <android-base/logging.h>
#include <android-base/properties.h>
#include <android-base/unique_fd.h>

namespace android::init {

inline void StartPantherBootTrace() {
    // This header is only installed in the diagnostic build; additionally bind
    // execution to the TF experiment's existing boot identity.
    if (android::base::GetProperty("ro.boot.serialno", "") != "panther-x2-tf") return;
    static std::once_flag once;
    std::call_once(once, [] {
        std::thread([] {
            // PID 1 later uses setns(CLONE_NEWNS). A live thread sharing
            // fs_struct makes that call fail with EINVAL. Keep this observer's
            // filesystem state private, without changing its mount namespace.
            if (unshare(CLONE_FS) == -1) {
                PLOG(ERROR) << "Panther TF diagnostic fs isolation failed";
                return;
            }
            constexpr char kFinal[] = "panther-boot-kmsg.log";
            constexpr int kMaxKernelBytes = 4 * 1024 * 1024;
            std::string previous;
            // Bound lifetime and disk use. The PID 1 action queue never waits
            // for this thread; no logs are cleared from the kernel ring buffer.
            for (int sample = 0; sample < 180; ++sample) {
                struct stat cache_stat {}, root_stat {};
                if (stat("/cache", &cache_stat) == 0 && stat("/", &root_stat) == 0 &&
                    cache_stat.st_dev != root_stat.st_dev) {
                    android::base::unique_fd directory(
                            open("/cache", O_RDONLY | O_DIRECTORY | O_CLOEXEC | O_NOFOLLOW));
                    if (directory.ok()) {
                        std::string snapshot = "Panther TF bounded boot diagnostics\nRevision=4\n";
                        for (const char* name : {"ro.boot.serialno", "ro.product.device",
                                 "init.svc.servicemanager", "init.svc.vold",
                                 "init.svc.vendor.keymint-default", "init.svc.wait_for_optee",
                                 "init.svc.vendor.gatekeeper_software", "init.svc.apexd",
                                 "init.svc.keystore2", "init.svc.vendor.gralloc-v1",
                                 "init.svc.surfaceflinger", "init.svc.hwcomposer-3",
                                 "init.svc.logd", "apexd.status", "vold.decrypt",
                                 "init.svc.adbd", "ro.crypto.state", "sys.boot_completed"}) {
                            snapshot += std::string(name) + "=" +
                                        android::base::GetProperty(name, "<unset>") + "\n";
                        }
                        const int size = klogctl(10 /* SIZE_BUFFER */, nullptr, 0);
                        if (size > 0 && size <= kMaxKernelBytes) {
                            std::vector<char> buffer(size);
                            const int count = klogctl(3 /* READ_ALL, non-clearing */,
                                                      buffer.data(), size);
                            if (count >= 0) snapshot.append(buffer.data(), count);
                            else snapshot += "Kernel log read failed, errno=" + std::to_string(errno) + "\n";
                        } else {
                            snapshot += "Kernel log size unavailable/out of bound, errno=" +
                                        std::to_string(errno) + "\n";
                        }
                        if (snapshot != previous) {
                            // init may create/write cache_file but may not rename
                            // it under the existing policy. Check the inode before
                            // truncation; persist directly without changing policy.
                            auto persist = [&](const char* filename) {
                            android::base::unique_fd output(openat(directory.get(), filename,
                                    O_WRONLY | O_CREAT | O_CLOEXEC | O_NOFOLLOW, 0600));
                            struct stat file_stat {};
                            if (output.ok() && fstat(output.get(), &file_stat) == 0 &&
                                S_ISREG(file_stat.st_mode) && file_stat.st_nlink == 1 &&
                                file_stat.st_uid == 0 && fchmod(output.get(), 0600) == 0 &&
                                ftruncate(output.get(), 0) == 0 &&
                                android::base::WriteStringToFd(snapshot, output.get()) &&
                                fsync(output.get()) == 0) {
                                fsync(directory.get());
                                return true;
                            } else {
                                PLOG(ERROR) << "Panther TF diagnostic snapshot failed";
                                return false;
                            }
                            };
                            // Preserve the early failures before repeated service
                            // waits push them out of the finite kernel ring.
                            if (sample <= 10 && !persist("panther-boot-early.log")) return;
                            if (!persist(kFinal)) return;
                            previous = std::move(snapshot);
                        }
                    }
                }
                std::this_thread::sleep_for(std::chrono::seconds(2));
            }
        }).detach();
    });
}

}  // namespace android::init
