#pragma once


namespace raytracer
{
        /*
         * this defines a 'badge' and allows for finer grained access to public
         * api's without too much performance overhead
         **/
        template <typename T>
        class the_badge final
        {
                friend T;

                the_badge()
                {
                }

                the_badge(the_badge const&)            = delete;
                the_badge& operator=(the_badge const&) = delete;
                the_badge(the_badge&&)                 = delete;
                the_badge& operator=(the_badge&&)      = delete;
        };

} // namespace raytracer
