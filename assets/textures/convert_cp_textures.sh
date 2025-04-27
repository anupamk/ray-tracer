#!/usr/bin/env zsh

# ------------------------------------------------------------------------------
# what are we called ?
PROGRAM_NAME=`basename "$0"`

# ------------------------------------------------------------------------------
# this function is called to display the usage information for this
# program.
function show_usage() {
    echo "-----------------------------------------------------------------------------"
    echo "About : "
    echo "       '"${PROGRAM_NAME}"' convert and copy all jpg images from source-dir "
    echo "       to ascii ppm images in dst-dir"
    echo "-----------------------------------------------------------------------------"

    exit
}

# ------------------------------------------------------------------------------
# show a spinner, to just let the user know *something* is going on.
function show_spinner() {
    local spin=("-" "\\" "|" "/")

    for i in "${spin[@]}"
    do
        echo -ne "\b$i"
        sleep 0.2
    done
}

# ------------------------------------------------------------------------------
# convert textures in src-dir from jpg -> ascii-ppm, and copy to
# destination directory
function convert_cp_textures() {
    local convert_exe=$1
    local src_dirname=$2
    local dst_dirname=$3

    find ${src_dirname} -name '*.jpg' |
        while read jpg_fname
        do
            # ------------------------------------------------------------------
            # something is going on
            show_spinner

            local base_jpg_fname=$(basename $(echo ${jpg_fname}))
            local dir_jpg_fname=$(dirname $(echo ${jpg_fname}))

            # ------------------------------------------------------------------
            # ok, so we want to recreate the same directory structure
            # at the destination as it is at the source. now, things
            # look like this:
            #
            #   src-dir: '.../assets/textures'
            #   dst-dir: '.../build/assets/textures'
            #
            # textures are organized like so:
            #
            #           .
            #            `-- skybox
            #                `-- chapel
            #
            # this implies that all textures within 'skybox/chapel/'
            # should be copied into: '${dst-dir}/skybox/chapel'
            #
            # therefore, from the ${jpg_fname} we need to strip the
            # ${src-dir} to get to ${ppm-dirname}
            local ppm_fname=$(echo ${base_jpg_fname%.*}).ppm
            local ppm_dirname=${dst_dirname}$(echo ${${dir_jpg_fname}#"${src_dirname}"})

            # ------------------------------------------------------------------
            # create the directory, and copy the file.
            mkdir -p ${ppm_dirname}
            ${convert_exe} -compress none ${jpg_fname} ${ppm_dirname}/${ppm_fname}

            # ------------------------------------------------------------------
            # something is going on
            show_spinner
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
    local src_dirname=""
    local dst_dirname=""

    while [[ $# -gt 0 ]]
    do
        case "$1" in
            "-s")
                src_dirname=$2
                shift
                ;;

            "-d")
                dst_dirname=$2
                shift
                ;;

            "-h" | *)
                show_usage
                ;;
        esac
        shift
    done

    # --------------------------------------------------------------------------
    # is imagemagick installed ?
    local convert_exe_fname=$(which convert)
    if ! [ -x ${convert_exe_fname} ]
    then
        echo "runtime error : 'ImageMagick' is not installed !"
        echo "url: 'https://github.com/ImageMagick/ImageMagick'"
        echo "please install it, and then run this script once again"
        exit 255
    fi

    # --------------------------------------------------------------------------
    # convert all jpg files in ${src_dirname} to ascii-ppm files and
    # copy them over to ${dst_dirname}
    convert_cp_textures ${convert_exe_fname} ${src_dirname} ${dst_dirname}
}

# oh boy, oh boy, oh boy !
main $*
