#include "mmapped_file_reader.hpp"

#include <cerrno>
#include <cstdio>
#include <cstring>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>

/// our includes
#include "common/include/assert_utils.h"

/*
 * this is the implementation of platform_utils on the darwin platform.
 **/
namespace platform_utils
{
        /// --------------------------------------------------------------------
        /// it is a fatal runtime error to pass a non-existent or unreadable
        /// file here
        mmapped_file_reader::mmapped_file_reader(std::string file_name)
            : file_name_(file_name)
            , fd_(open_file())
            , file_info_(fstat_descriptor())
            , mmapped_addr_(mmap_descriptor())
        {
        }

        /// --------------------------------------------------------------------
        /// cleanup the resources
        mmapped_file_reader::~mmapped_file_reader()
        {
                munmap(this->mmapped_addr_, this->file_info_.st_size);
                close(this->fd_);
        }

        /// --------------------------------------------------------------------
        /// return some useful file information
        std::string mmapped_file_reader::file_name() const
        {
                return this->file_name_;
        }

        void* mmapped_file_reader::data() const
        {
                return this->mmapped_addr_;
        }

        size_t mmapped_file_reader::size() const
        {
                return this->file_info_.st_size;
        }

        /*
         * only private functions from this point onward
         **/

        /// --------------------------------------------------------------------
        /// this function is called to open a file (identified by 'file_name_')
        /// and return an associated descriptor when the file is opened
        /// successfully in read-only mode.
        int mmapped_file_reader::open_file() const
        {
                errno = 0;

                int fd = open(this->file_name().c_str(), O_RDONLY, S_IRUSR);
                if (fd == -1) {
                        auto open_err_str = strerror(errno);

                        constexpr int err_buf_size = 2048;
                        char err_buf[err_buf_size];
                        snprintf(err_buf, err_buf_size, "open('%s', ...) failed, reason:%s",
                                 this->file_name().c_str(), open_err_str);

                        ASSERT_FAIL(err_buf);
                }

                return fd;
        }

        /// --------------------------------------------------------------------
        /// this function is called to collect more information about a file
        /// using the canonical 'fstat(...)'
        struct stat mmapped_file_reader::fstat_descriptor() const
        {
                struct stat file_info;
                errno = 0;

                if (fstat(this->fd_, &file_info) == -1) {
                        auto fstat_err_str = strerror(errno);

                        constexpr int err_buf_size = 2048;
                        char err_buf[err_buf_size];
                        snprintf(err_buf, err_buf_size, "fstat('%d', ...) failed, reason:%s", this->fd_,
                                 fstat_err_str);

                        ASSERT_FAIL(err_buf);
                }

                return file_info;
        }

        /// --------------------------------------------------------------------
        /// this function is called to mmap(...) the descriptor into this
        /// processes's address space.
        ///
        /// it is a runtime fatal error if mmap(...) fails for any reason.
        void* mmapped_file_reader::mmap_descriptor() const
        {
                void* mmapped_addr     = nullptr;
                errno                  = 0;
                size_t const file_size = this->file_info_.st_size;
                int const MMAP_FLAGS   = (MAP_PRIVATE); /// private c.o.w mapping

                mmapped_addr = mmap(nullptr,    /// kernel-decides-map-addr
                                    file_size,  /// number-of-bytes-to-map
                                    PROT_READ,  /// for reading
                                    MMAP_FLAGS, /// see above
                                    this->fd_,  /// fd-to-map
                                    0);         /// start-filemap-offset

                if (mmapped_addr == MAP_FAILED) {
                        auto mmap_err_str = strerror(errno);

                        constexpr int err_buf_size = 2048;
                        char err_buf[err_buf_size];
                        snprintf(err_buf, err_buf_size, "mmap(...,'%d',... ) failed, reason:%s", this->fd_,
                                 mmap_err_str);

                        /// ----------------------------------------------------
                        /// crash-n-burn
                        ASSERT_FAIL(err_buf);
                }

                /*
                 * on platforms lacking 'MAP_POPULATE' mmap-flag, augmenting
                 * mmap invocation with 'madvise(...)' is sufficient to get the
                 * same same behavior.
                 *
                 * lets do just that.
                 **/

                /// ------------------------------------------------------------
                /// the 'MADV_WILLNEED' flag tells the os, that the application
                /// expects to access the mmaped address range pretty soon.
                errno                 = 0;
                int const madvise_ret = madvise(mmapped_addr, file_size, MADV_WILLNEED);
                if ((errno != 0) || (madvise_ret == -1)) {
                        auto madv_err_str = strerror(errno);

                        constexpr int err_buf_size = 2048;
                        char err_buf[err_buf_size];
                        snprintf(err_buf, err_buf_size, "madvise(%p, %ld, MADV_WILLNEED) failed, reason:%s",
                                 mmapped_addr, file_size, madv_err_str);

                        /// ----------------------------------------------------
                        /// crash-n-burn
                        ASSERT_FAIL(err_buf);
                }

                return mmapped_addr;
        }

        /// ----------------------------------------------------------------------------
        /// this function is called to fill a file with data, and if successful, it
        /// returns the name of file where the data was written
        std::string fill_file_with_data(std::string_view file_data)
        {
                char ret_fname[] = "/tmp/data-XXXXXX";
                int fd           = mkstemp(ret_fname);

                if (fd == -1) {
                        return std::string{};
                }

                int const data_bytes    = file_data.size();
                int const written_bytes = write(fd, file_data.data(), data_bytes);

                if (written_bytes != data_bytes) {
                        close(fd);
                        return std::string{};
                }

                close(fd);
                return std::string{ret_fname};
        }

} // namespace platform_utils
