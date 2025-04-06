#pragma once

#include <cstddef>
#include <string>
#include <string_view>
#include <sys/stat.h>

namespace platform_utils
{
        /*
         * this class implements 'infrastructure' for facilitating parsing input
         * files. specifically, this makes it easier for mmapped based file parsing.
         *
         * parsing content of files is very format specific, however, some portion of
         * that work is fairly routine, and this 'automates' that.
         **/
        class mmapped_file_reader final
        {
            private:
                /// --------------------------------------------------------------------
                /// name, descriptor and information about the open file.
                std::string file_name_;
                int fd_;
                struct stat file_info_;

                /// --------------------------------------------------------------------
                /// the mmapped address
                void* mmapped_addr_;

            public:
                mmapped_file_reader(std::string file_name);
                ~mmapped_file_reader();

            public:
                /// ------------------------------------------------------------
                /// name of associated file
                std::string file_name() const;

                /// --------------------------------------------------------------------
                /// get the start of first byte of data
                void* data() const;

                /// --------------------------------------------------------------------
                /// get size (in bytes) of mapped file
                size_t size() const;

            private:
                /// ------------------------------------------------------------
                /// return fd associated with opening 'file_name' for read-only
                /// operations
                ///
                /// runtime fatal error if file cannot be opened for some
                /// reason.
                int open_file() const;

                /// ------------------------------------------------------------
                /// fstat a descriptor to gather more info about the file.
                ///
                /// runtime fatal error if 'fstat(...)' fails for any reason.
                struct stat fstat_descriptor() const;

                /// ------------------------------------------------------------
                /// mmap a descriptor.
                ///
                /// runtime fatal error if mmap(...) fails for any reason.
                void* mmap_descriptor() const;
        };

        /// --------------------------------------------------------------------
        /// fill a file with data, and if successful, it returns the name of the
        /// file where the input data is stored.
        std::string fill_file_with_data(std::string_view);

} // namespace platform_utils
