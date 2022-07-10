/*
 * implement the raytracer canvas ppm reader
 **/

/// c includes
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

/// c++ includes
#include <algorithm>
#include <cerrno>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <memory>
#include <sstream>
#include <string>
#include <typeindex>
#include <utility>

/// our includes
#include "canvas.hpp"
#include "color.hpp"
#include "common/include/string_utils.hpp"
#include "execution_profiler.hpp"
#include "utils.hpp"

namespace raytracer
{
        /// --------------------------------------------------------------------
        /// this represents an instance of ppm-image header
        class ppm_header
        {
                /// --------------------------------------------------------------------
                /// a simle valid/invalid type for all header parameters
                template <typename T>
                struct is_valid_t {
                        bool is_valid{false};
                        T data{};

                        bool operator()() const
                        {
                                return is_valid;
                        }
                };

            public:
                static constexpr auto PPM_ASCII_MAGIC  = "P3";
                static constexpr auto PPM_BINARY_MAGIC = "P6";

            public:
                is_valid_t<std::string> magic{};
                is_valid_t<uint32_t> width{};
                is_valid_t<uint32_t> height{};
                is_valid_t<uint16_t> color_scale{};

            public:
                void set_image_magic(std::string val)
                {
                        magic.data     = val;
                        magic.is_valid = ((val == PPM_ASCII_MAGIC) || (val == PPM_BINARY_MAGIC));
                }

                bool magic_is_valid() const
                {
                        return magic.is_valid;
                }

                bool is_ascii() const
                {
                        return (magic.data == PPM_ASCII_MAGIC);
                }

                void set_image_width(uint32_t val)
                {
                        width.data     = val;
                        width.is_valid = true;
                }

                void set_image_height(uint32_t val)
                {
                        height.data     = val;
                        height.is_valid = true;
                }

                void set_image_color_scale(uint16_t val)
                {
                        if (val == 0 || val > 255) {
                                return;
                        }

                        color_scale.data     = val;
                        color_scale.is_valid = true;
                }

                bool is_valid() const
                {
                        return (magic() && width() && height() && color_scale());
                }

                /// --------------------------------------------------------------------
                /// stringified representation of image header
                std::string stringify() const
                {
                        std::stringstream ss("");

                        ss << "{"
                           << "type: " << magic.data << ", "
                           << "width: " << width.data << ", "
                           << "height: " << height.data << ", "
                           << "color-scale: " << color_scale.data << "}";

                        return ss.str();
                }
        };

        /// ----------------------------------------------------------------------------
        /// a token from the ppm file
        struct token_context_t {
                bool is_valid  = false;
                bool bad_token = false;

                /// token-start and token-end index. index exists within the
                /// stream of data that we read)
                size_t ts_index = 0;
                size_t te_index = 0;

                /// token
                std::string_view token_value{};

                bool token_is_valid() const
                {
                        return is_valid;
                }
        };

        /// --------------------------------------------------------------------
        /// mmap un-mapper : this allows cleanups on mmapped regions on exit
        class mmap_unmapper
        {
            private:
                int mmapped_fd          = -1;
                void* mmapped_data      = nullptr;
                long long mmapped_bytes = 0;

            public:
                mmap_unmapper(int file_fd, void* mmap_data, long long num_bytes)
                    : mmapped_fd(file_fd)
                    , mmapped_data(mmap_data)
                    , mmapped_bytes(num_bytes)
                {
                        close(mmapped_fd);
                }

                ~mmap_unmapper()
                {
                        munmap(mmapped_data, mmapped_bytes);
                }
        };

        /// --------------------------------------------------------------------
        /// this function is called to parse a ppm-formatted file and return a
        /// new canvas instance.
        std::optional<canvas> canvas::load_from_file(std::string file_name)
        {
                PROFILE_SCOPE;

                /// ------------------------------------------------------------
                /// we are using mmap(...) and friends for reading data from the
                /// file. why ? because we can.
                errno = 0;

                /// --------------------------------------------------------------------
                /// open the file for reading
                int ppm_fd = open(file_name.c_str(), O_RDONLY, S_IRUSR);
                {
                        if (ppm_fd == -1) {
                                auto open_err_str = strerror(errno);

                                close(ppm_fd);
                                LOG_ERROR("%s(...) open(%s, ...) failed, reason:%s", __PRETTY_FUNCTION__,
                                          file_name.c_str(), open_err_str);

                                return std::nullopt;
                        }
                }

                /// --------------------------------------------------------------------
                /// get file size
                struct stat ppm_file_info = {};
                {
                        errno = 0;
                        if (fstat(ppm_fd, &ppm_file_info) == -1) {
                                auto fstat_err_str = strerror(errno);

                                close(ppm_fd);
                                LOG_ERROR("%s(...) fstat(%d, ...) failed, reason:%s", __PRETTY_FUNCTION__,
                                          ppm_fd, fstat_err_str);

                                return std::nullopt;
                        }
                }

                /// --------------------------------------------------------------------
                /// mmap(...) the file
                void* mmapped_addr = nullptr;
                {
                        errno = 0;

                        int const MMAP_FLAGS = (MAP_POPULATE | /// populate page tables for mapping
                                                MAP_PRIVATE);  /// private c.o.w mapping

                        mmapped_addr = mmap(nullptr,               /// kernel-decides-map-addr
                                            ppm_file_info.st_size, /// number-of-bytes-to-map
                                            PROT_READ,             /// for reading
                                            MMAP_FLAGS,            /// see above
                                            ppm_fd,                /// fd-to-map
                                            0);                    /// start-filemap-offset

                        if (mmapped_addr == MAP_FAILED) {
                                auto mmap_err_str = strerror(errno);

                                close(ppm_fd);
                                LOG_ERROR("%s(...) mmap(...) failed, reason:%s", __PRETTY_FUNCTION__,
                                          mmap_err_str);

                                return std::nullopt;
                        }
                }

                /// let the mmap_unmapper do resource cleanups...
                mmap_unmapper file_unmapper(ppm_fd, mmapped_addr, ppm_file_info.st_size);

                /// --------------------------------------------------------------------
                /// lets go spelunking
                char const* ppm_file_data = reinterpret_cast<char const*>(mmapped_addr);
                size_t ci                 = 0;                     /// current-index (into raw-data)
                size_t const ei           = ppm_file_info.st_size; /// end-index (of raw-data)
                token_context_t ppm_token = {};

                /// --------------------------------------------------------------------
                /// this is the 'tokenizer' creating ppm tokens (actually just pointers
                /// to indexes (start, end)) from the data-stream. once a token is
                /// created, it is available for the next stage of processing in the
                /// `ppm_token`
                ///
                /// ppm-tokens are simply character sequences sperated by whitespace.
                auto get_next_token = [&]() {
                        ppm_token = token_context_t{};

                        /// get token start
                        while (ci < ei) {
                                uint8_t tok   = ppm_file_data[ci];
                                bool get_next = false;

                                switch (tok) {
                                case ' ':
                                case '\t':
                                case '\n':
                                case '\r':
                                case '\f':
                                        get_next = true;
                                        break;

                                case '#':
                                        /// --------------------------------------------
                                        /// ignore till end of line
                                        while (ppm_file_data[ci] != '\n')
                                                ci += 1;

                                        get_next = true;
                                        break;

                                default:
                                        break;
                                }

                                if (get_next) {
                                        ci += 1;
                                        continue;
                                }

                                ppm_token.ts_index = ci;
                                ci += 1;
                                break;
                        }

                        /// get token end
                        while (ci < ei) {
                                uint8_t tok   = ppm_file_data[ci];
                                bool get_next = false;

                                switch (tok) {
                                default:
                                        get_next = true;
                                        break;

                                case ' ':
                                case '\t':
                                case '\n':
                                case '\r':
                                        ppm_token.te_index = ci;
                                        ppm_token.is_valid = true;
                                        break;

                                case '\f':
                                case '#':
                                        ppm_token.bad_token = true;
                                        ppm_token.is_valid  = false;

                                        return;
                                        break; /// unreached
                                }

                                if (get_next) {
                                        ci += 1;
                                        continue;
                                }

                                ci += 1;
                                break;
                        }

                        /// string-view of data : still no copying is going on :)
                        if (ppm_token.is_valid) {
                                ppm_token.token_value =
                                        std::string_view(&ppm_file_data[ppm_token.ts_index],
                                                         ppm_token.te_index - ppm_token.ts_index);
                        }

                        return;
                };

                /// --------------------------------------------------------------------
                /// our needs are spartan, there is no point in using atoi(...) /
                /// stoi(...) we roll our own
                auto conv2uint = [](uint32_t& val, std::string_view str) -> bool {
                        val = 0;
                        for (auto itr = str.begin(); itr != str.end(); ++itr) {
                                /// bogus characters in input
                                if ((*itr < '0') || (*itr > '9')) {
                                        return false;
                                }

                                val = val * 10 + *itr - '0';
                        }

                        return true;
                };

#ifndef READ_NEXT_UINT_TOKEN
#define READ_NEXT_UINT_TOKEN(err_msg)                                                                        \
        ({                                                                                                   \
                uint32_t dst;                                                                                \
                                                                                                             \
                get_next_token();                                                                            \
                if (unlikely(!ppm_token.token_is_valid()) ||                                                 \
                    unlikely(!conv2uint(dst, ppm_token.token_value))) {                                      \
                        return std::nullopt;                                                                 \
                }                                                                                            \
                                                                                                             \
                dst;                                                                                         \
        })
#endif /// READ_NEXT_UINT_TOKEN(...)

                /// --------------------------------------------------------------------
                /// step-01: image-header-parsing
                ///   01. parse magic i.e. first 2 bytes of the stream must be either
                ///       "P3" || "P6"
                ppm_header img_header;
                img_header.set_image_magic(std::string{ppm_file_data, 2});
                ci += 2;

                /// --------------------------------------------------------------------
                /// step-01: image-header-parsing
                ///   02. parse image width, height and color-scale
                uint32_t const image_width = READ_NEXT_UINT_TOKEN("image-width");
                img_header.set_image_width(image_width);

                uint32_t const image_height = READ_NEXT_UINT_TOKEN("image-height");
                img_header.set_image_height(image_height);

                uint32_t const color_scale = READ_NEXT_UINT_TOKEN("image-color-scale");
                img_header.set_image_color_scale(color_scale);

                /// hopefully, we have valid image header by now
                if (!img_header.is_valid()) {
                        return std::nullopt;
                }

                /// --------------------------------------------------------------------
                /// step-02: process image-raster
                if (img_header.is_ascii()) {
                        canvas ppm_img_retval =
                                canvas::create_ascii(img_header.width.data, img_header.height.data);

                        for (size_t y = 0; y < img_header.height.data; y++) {
                                for (size_t x = 0; x < img_header.width.data; x++) {
                                        float const r_value = (1.0 * READ_NEXT_UINT_TOKEN("R pixel")) /
                                                              (1.0 * img_header.color_scale.data);

                                        float const g_value = (1.0 * READ_NEXT_UINT_TOKEN("G pixel")) /
                                                              (1.0 * img_header.color_scale.data);

                                        float const b_value = (1.0 * READ_NEXT_UINT_TOKEN("B pixel")) /
                                                              (1.0 * img_header.color_scale.data);

                                        color const pixel_color{r_value, g_value, b_value};
                                        ppm_img_retval.write_pixel(x, y, pixel_color);
                                }
                        }

                        return ppm_img_retval;
                }

                return std::nullopt;
        }

} // namespace raytracer
