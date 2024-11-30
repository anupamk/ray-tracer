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
    echo "       '"${PROGRAM_NAME}"' update generated images for README.org"
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
# update all images.
#
# this function is called to convert all '.ppm' images to '.png'
# format, and also creates a 'low-resolution' version of the same
# image as well.
function update_all_images() {
    # --------------------------------------------------------------------------
    # what does the low-resolution image dimensions look like ?
    local lowres_dim='320x256'

    # --------------------------------------------------------------------------
    # low resolution images will be named as 'image-foo-lowres.png'
    # for a high resolution image called 'image-foo.png'
    local lowres_img_suffix='lowres'

    ls *.ppm |
        while read img_fname_with_ext
        do
            local img_fname_no_ext=$(echo "${img_fname_with_ext%.*}")

            # ------------------------------------------------------------------
            # perform the conversion
            magick ${img_fname_with_ext} ${img_fname_no_ext}.png
            magick ${img_fname_with_ext} -resize ${lowres_dim} ${img_fname_no_ext}-${lowres_img_suffix}.png
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
            update_all_images
        ;;

        "-h" | *)
            show_usage
            ;;
    esac
}

# oh boy, oh boy, oh boy !
main $*
