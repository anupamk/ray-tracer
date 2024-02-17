#!/usr/bin/env zsh

# ------------------------------------------------------------------------------
# debugging is enabled
ENABLE_DEBUG="1"

# ------------------------------------------------------------------------------
# what are we called ?
PROGRAM_NAME=`basename "$0"`

# ------------------------------------------------------------------------------
# this function is called to display the usage information for this
# program.
function show_usage() {
    echo "-----------------------------------------------------------------------------"
    echo "About : "
    echo "       '"${PROGRAM_NAME}"' run all executables, and generate images"
    echo "-----------------------------------------------------------------------------"

    exit
}

# ------------------------------------------------------------------------------
# when debugging is enabled, use the system logger to dump information
# into the system log file.
function varlog_debug_printf__() {
    if [ $ENABLE_DEBUG = "1" ]; then
        logger "$*"
    fi
}

# ------------------------------------------------------------------------------
# always log into the system log file.
function varlog_error_printf__() {
    logger "$*"
}

# ------------------------------------------------------------------------------
# debug log via the system-logger
function debug_printf() {
    local log_msg=$(printf "%s[%s]: %s" ${BASH_SOURCE[1]##*/} ${BASH_LINENO[0]} "${@}")
    varlog_debug_printf__ ${log_msg}
}


# ------------------------------------------------------------------------------
# error log via the system-logger
function error_printf() {
    local log_msg=$(printf "error %s[%s]: %s" ${BASH_SOURCE[1]##*/} ${BASH_LINENO[0]} "${@}")
    varlog_error_printf__ ${log_msg}
}

# ------------------------------------------------------------------------------
# generate all the images
function generate_all_images() {
    local all_executable_names=(                                 \
                                 render_virtual_cannon           \
                                 render_clock_dial               \
                                 render_sphere_silhouette        \
                                 render_phong_sphere             \
                                 render_world_main               \
                                 render_with_plane               \
                                 render_with_patterns            \
                                 render_with_textures            \
                                 render_nasa_blue_earth          \
                                 render_with_reflections         \
                                 render_with_t3r                 \
                                 render_nested_glass_spheres     \
                                 render_align_check_plane        \
                                 render_checkered_cubes          \
                                 render_skybox                   \
                                 render_skybox_movie             \
                                 render_cube_scene               \
                                 render_cube_light_scene         \
                                 render_cylinder_scene           \
                                 render_spheres_with_decorations \
                                 render_cylinder_texture_scene   \
                                 render_cone_scene               \
                                 render_hexagon_scene            \
                                 render_teapot                   \
                                 render_cessna                   \
                                 render_newtons_cradle           \
                                 render_csg_dice                 \
                                 render_chess_pawn

                                 # add more executable names ↑
                                 # ---------------------------------------------
    )

    # --------------------------------------------------------------------------
    # log all output to a file
    local render_logfname=$(mktemp /tmp/render-all-images-XXXXXX.log)
    touch ${render_logfname}

    echo "--------------------------------------------------------------------------------------------------------------------------------"
    echo "*** NOTICE NOTICE NOTICE (1/3) : all output is redirected to: '"${render_logfname}"' NOTICE NOTICE NOTICE ***"
    echo "*** NOTICE NOTICE NOTICE (2/3) : all output is redirected to: '"${render_logfname}"' NOTICE NOTICE NOTICE ***"
    echo "*** NOTICE NOTICE NOTICE (3/3) : all output is redirected to: '"${render_logfname}"' NOTICE NOTICE NOTICE ***"
    echo "--------------------------------------------------------------------------------------------------------------------------------"

    # --------------------------------------------------------------------------
    # depending on the machine, this takes a while ! go watch the
    # star-wars trilogy (ha !) or something.
    for exec_name in ${all_executable_names}
    do
        echo "generating images using:'"${exec_name}"'" | tee -a ${render_logfname}
        ./${exec_name} 2>&1 | tee -a ${render_logfname}
        echo "--------------------------------------------------------------------------------------------------------------------------------" | tee -a ${render_logfname}
    done
}


# ------------------------------------------------------------------------------
# it's main(...) which ties all of this together
function main() {

    # --------------------------------------------------------------------------
    # usage is messed up
    if [ $# -lt 1 ]
    then
        show_usage
    fi

    # --------------------------------------------------------------------------
    # lets go
    case "$1" in
        "-g")
            generate_all_images
        ;;

        "-h" | *)
            show_usage
            ;;
    esac
}

# oh boy, oh boy, oh boy !
main $*
