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
    echo "            -g actually perform the update"
    echo "            -h show this help message"
    echo "-----------------------------------------------------------------------------"
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
    local dest_dir=$1

    # --------------------------------------------------------------------------
    # what does the low-resolution image dimensions look like ?
    local lowres_dim='320x256'

    # --------------------------------------------------------------------------
    # a high resolution image, f.e. 'image-foo.png', will be named
    # 'image-foo-lowres.png'.
    #
    # these images i.e. 'image-foo.png' and 'image-foo-lowres.png'
    # both need to be placed in '$PROJECT_ROOT/generated-images'
    # directory, and pushed into the repository.
    local lowres_img_suffix='lowres'

    ls *.ppm |
        while read img_fname_with_ext
        do
            echo " updating '"${img_fname_with_ext}"'"

            local img_fname_no_ext=$(echo "${img_fname_with_ext%.*}")

            # ------------------------------------------------------------------
            # perform the conversion
            magick ${img_fname_with_ext} ${img_fname_no_ext}.png
            magick ${img_fname_with_ext} -resize ${lowres_dim} ${img_fname_no_ext}-${lowres_img_suffix}.png

            # ------------------------------------------------------------------
            # and copy the generated targets
            cp ${img_fname_no_ext}.png ${dest_dir}
            cp ${img_fname_no_ext}-${lowres_img_suffix}.png ${dest_dir}

        done
}


# ------------------------------------------------------------------------------
# it's main(...) which ties all of this together
function main() {

    # --------------------------------------------------------------------------
    # lets go
    while getopts gd:h flag
    do
        case "${flag}" in
            d)
                dest_dir_flag=1
                dest_dir_value=${OPTARG}
                ;;

            g)
                do_update_flag=1
                do_update_value=1
                ;;

            h)
                echo "wow !"
                show_usage
                ;;
        esac
    done

    # --------------------------------------------------------------------------
    # update flag must be specified
    if [ "${do_update_value}" != "1" ]
    then
        error_printf "not sure what to do ?"
        show_usage
        return 254
    fi

    # --------------------------------------------------------------------------
    # null || bad-path is an error
    if [[ ( -z "${dest_dir_value}" ) || ( ! -w ${dest_dir_value} ) ]]
    then
        error_printf "bad dest-dir:'"${dest_dir_value}"'"
        show_usage
        return 255
    fi

    update_all_images ${dest_dir_value}
}

# oh boy, oh boy, oh boy !
main $*
