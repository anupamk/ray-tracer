#pragma once


namespace raytracer
{
        /*
         * this defines a 'badge' and allows for finer grained access to public
         * api's without too much performance overhead
         **/
        template <typename T>
        class the_badge
        {
                friend T;

                the_badge()
                {
                }

                // clang-format off
                the_badge(const the_badge&)            = delete;
                the_badge& operator=(const the_badge&) = delete;
                the_badge(the_badge&&)		       = delete;
                the_badge& operator=(the_badge&&)      = delete;
                // clang-format on
        };
} // namespace raytracer


